#include <Vector2F.h>
#include "proxyVizNode.h"
#include <maya/MFnMatrixAttribute.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MVectorArray.h>
#include <maya/MFnVectorArrayData.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnPointArrayData.h>
#include <maya/MFnDoubleArrayData.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MIntArray.h>
#include <maya/MPointArray.h>
#include <maya/MFnMeshData.h>
#include <PseudoNoise.h>
#include <EnvVar.h>
#include <AHelper.h>
#include <ExampVox.h>

#ifdef WIN32
#include <gExtension.h>
#endif	

namespace aphid {

MTypeId ProxyViz::id( 0x95a19e );
MObject ProxyViz::abboxminx;
MObject ProxyViz::abboxminy;
MObject ProxyViz::abboxminz;
MObject ProxyViz::abboxmaxx;
MObject ProxyViz::abboxmaxy;
MObject ProxyViz::abboxmaxz;
MObject ProxyViz::aradiusMult;
MObject ProxyViz::outPositionPP;
MObject ProxyViz::outScalePP;
MObject ProxyViz::outRotationPP;
MObject ProxyViz::outValue;
MObject ProxyViz::acachename;
MObject ProxyViz::acameraspace;
MObject ProxyViz::ahapeture;
MObject ProxyViz::avapeture;
MObject ProxyViz::afocallength;
MObject ProxyViz::alodgatehigh;
MObject ProxyViz::alodgatelow;
MObject ProxyViz::axmultiplier;
MObject ProxyViz::aymultiplier;
MObject ProxyViz::azmultiplier;
MObject ProxyViz::agroupcount;
MObject ProxyViz::ainstanceId;
MObject ProxyViz::ainmesh;
MObject ProxyViz::astandinNames;
MObject ProxyViz::aconvertPercentage;
MObject ProxyViz::agroundMesh;
MObject ProxyViz::agroundSpace;
MObject ProxyViz::aplantTransformCache;
MObject ProxyViz::aplantIdCache;
MObject ProxyViz::aplantTriangleIdCache;
MObject ProxyViz::aplantTriangleCoordCache;
MObject ProxyViz::ainexamp;
MObject ProxyViz::adisplayVox;
MObject ProxyViz::acheckDepth;
MObject ProxyViz::ainoverscan;
MObject ProxyViz::aactivated;
MObject ProxyViz::outValue1;

ProxyViz::ProxyViz() : _firstLoad(1), fHasView(0),
m_toSetGrid(true),
m_toCheckVisibility(false),
m_enableCompute(true),
m_hasParticle(false)
{ attachSceneCallbacks(); }

ProxyViz::~ProxyViz() 
{ detachSceneCallbacks(); }

MStatus ProxyViz::compute( const MPlug& plug, MDataBlock& block )
{
	if(!m_enableCompute) return MS::kSuccess;
	if( plug == outValue ) {
		
		updateWorldSpace();
		
		MStatus status;

		ExampVox * defBox = plantExample(0);
		
		defBox->setGeomSizeMult(block.inputValue(aradiusMult).asFloat() );
		
		defBox->setGeomBox(block.inputValue(abboxminx).asFloat(),
			block.inputValue(abboxminy).asFloat(), 
			block.inputValue(abboxminz).asFloat(), 
			block.inputValue(abboxmaxx).asFloat(), 
			block.inputValue(abboxmaxy).asFloat(), 
			block.inputValue(abboxmaxz).asFloat());
		
		const float grdsz = defBox->geomExtent() * 31.f ;
		if(grdsz < 1e-1f) {
			AHelper::Info(" ProxyViz input box is too small", grdsz);
		}
		
		if(m_toSetGrid) {
			m_toSetGrid = false;
			resetGrid(grdsz);
		}
		
		if(_firstLoad) {
/// internal cache has the priority
			if(!loadInternal(block) ) {
				MString filename =  block.inputValue( acachename ).asString();
				if(filename != "") {
					loadExternal(replaceEnvVar(filename).c_str());
				}
			}
			_firstLoad = 0;
		}
        
		if(!m_toCheckVisibility) {
			MArrayDataHandle groundMeshArray = block.inputArrayValue(agroundMesh );
			MArrayDataHandle groundSpaceArray = block.inputArrayValue(agroundSpace );
/// in case no ground is connected
            if(updateGround(groundMeshArray, groundSpaceArray )) {
                moveWithGround();
                AHelper::Info<std::string>(" ProxyViz ground ", groundBuildLog() );
            }
		}
		
		if(!m_hasParticle) {
			block.setClean(plug);
            return MS::kSuccess;
		}
		
		MDataHandle hdata = block.inputValue(outPositionPP, &status);
        MFnVectorArrayData farray(hdata.data(), &status);
        if(!status) {
            MGlobal::displayInfo("proxy viz is not properly connected to a particle system");
			block.setClean(plug);
            return MS::kSuccess;
        }
    
        MDataHandle scaledata = block.inputValue(outScalePP, &status);
        MFnVectorArrayData scalearray(scaledata.data(), &status);
        if(!status) {
            MGlobal::displayInfo("proxy viz is not properly connected to a particle system");
			block.setClean(plug);
            return MS::kSuccess;
        }
		
		MDataHandle rotatedata = block.inputValue(outRotationPP, &status);
        MFnVectorArrayData rotatearray(rotatedata.data(), &status);
        if(!status) {
            MGlobal::displayInfo("proxy viz is not properly connected to a particle system");
			block.setClean(plug);
            return MS::kSuccess;
        }
		
		MVectorArray outPosArray = farray.array();	
        MVectorArray outScaleArray = scalearray.array();
		MVectorArray outRotateArray = rotatearray.array();
		
		if( outPosArray.length() < 1) {
			block.setClean(plug);
			return MS::kSuccess;
		}
		
		MGlobal::displayInfo(MString("proxy viz computes particle attributes"));
        
		saveParticles(outPosArray, outRotateArray, outScaleArray);

        float result = outPosArray.length();

		MDataHandle outputHandle = block.outputValue( outValue );
		outputHandle.set( result );
		block.setClean(plug);
    }
	if(plug == outValue1) {
		
		MArrayDataHandle hArray = block.inputArrayValue(ainexamp);
		updateExamples(hArray);

		float result = 91.f;

		MDataHandle outputHandle = block.outputValue( outValue1 );
		outputHandle.set( result );
		block.setClean(plug);
	}

	return MS::kSuccess;
}

void ProxyViz::draw( M3dView & view, const MDagPath & path, 
							 M3dView::DisplayStyle style,
							 M3dView::DisplayStatus status )
{ 	
	if(!m_enableCompute) return;
	updateWorldSpace();
	MObject thisNode = thisMObject();
	
	MPlug mutxplug( thisNode, axmultiplier);
	MPlug mutyplug( thisNode, aymultiplier);
	MPlug mutzplug( thisNode, azmultiplier);
	setScaleMuliplier(mutxplug.asFloat(), 
						mutyplug.asFloat(),
						mutzplug.asFloat() );	
                        
    MPlug svtPlug(thisNode, adisplayVox);
    setShowVoxLodThresold(svtPlug.asFloat() );
	
	MDagPath cameraPath;
	view.getCamera(cameraPath);
	if(hasView() ) updateViewFrustum(thisNode);
	else updateViewFrustum(cameraPath);
	
	setViewportAspect(view.portWidth(), view.portHeight() );
	
	MPlug actp(thisNode, aactivated);
	if(actp.asBool()) setWireColor(.125f, .1925f, .1725f);
    else setWireColor(.0675f, .0675f, .0675f);

	_viewport = view;
	fHasView = 1;
	
	view.beginGL();
	
	double mm[16];
	matrix_as_array(_worldInverseSpace, mm);
	
	glPushMatrix();
	glMultMatrixd(mm);	
	
	ExampVox * defBox = plantExample(0);
	updateGeomBox(defBox, thisNode);
	drawWireBox(defBox->geomCenterV(), defBox->geomScale() );
	Matrix44F mat;
	mat.setFrontOrientation(Vector3F::YAxis);
	mat.scaleBy(defBox->geomSize() );
    mat.glMatrix(m_transBuf);
	
	drawCircle(m_transBuf);
	
	drawGridBounding();
	// drawGrid();

	if ( style == M3dView::kFlatShaded || 
		    style == M3dView::kGouraudShaded ) {		
		drawPlants();
	}
	else 
		drawWiredPlants();
	
    if(hasView() ) drawViewFrustum();
    
	drawBrush(view);
	drawActivePlants();
	drawGround();
	glPopMatrix();
	view.endGL();
}

bool ProxyViz::isBounded() const
{ return true; }

MBoundingBox ProxyViz::boundingBox() const
{   
	BoundingBox bbox = plantExample(0)->geomBox();
	if(numPlants() > 0) bbox = gridBoundingBox();
	else if(!isGroundEmpty() ) bbox = ground()->getBBox();
	
	MPoint corner1(bbox.m_data[0], bbox.m_data[1], bbox.m_data[2]);
	MPoint corner2(bbox.m_data[3], bbox.m_data[4], bbox.m_data[5]);

	return MBoundingBox( corner1, corner2 );
}

void* ProxyViz::creator()
{
	return new ProxyViz();
}

MStatus ProxyViz::initialize()
{ 
	MFnNumericAttribute numFn;
	MStatus			 stat;
	
	alodgatehigh = numFn.create( "lodGateMax", "ldmx", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	numFn.setMin(0.001f);
	numFn.setMax(2.f);
	addAttribute(alodgatehigh);
	
	alodgatelow = numFn.create( "lodGateMin", "ldmin", MFnNumericData::kFloat, 0.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	numFn.setMin(0.f);
	numFn.setMax(0.999f);
	addAttribute(alodgatelow);

	abboxminx = numFn.create( "bBoxMinX", "bbmnx", MFnNumericData::kFloat, -1.f );
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxminx);
	
	abboxminy = numFn.create( "bBoxMinY", "bbmny", MFnNumericData::kFloat, -1.f );
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxminy);
	
	abboxminz = numFn.create( "bBoxMinZ", "bbmnz", MFnNumericData::kFloat, -1.f );
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxminz);
	
	abboxmaxx = numFn.create( "bBoxMaxX", "bbmxx", MFnNumericData::kFloat, 1.f );
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxmaxx);
	
	abboxmaxy = numFn.create( "bBoxMaxY", "bbmxy", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxmaxy);
	
	abboxmaxz = numFn.create( "bBoxMaxZ", "bbmxz", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	addAttribute(abboxmaxz);
	
	aradiusMult = numFn.create( "radiusMultiplier", "rml", MFnNumericData::kFloat);
	numFn.setStorable(true);
	numFn.setKeyable(true);
	numFn.setDefault(1.f);
	numFn.setMin(.05f);
	addAttribute(aradiusMult);
	
	axmultiplier = numFn.create( "visualMultiplierX", "vmx", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	numFn.setMin(0.001f);
	addAttribute(axmultiplier);	
	aymultiplier = numFn.create( "visualMultiplierY", "vmy", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	numFn.setMin(0.001f);
	addAttribute(aymultiplier);
	
	azmultiplier = numFn.create( "visualMultiplierZ", "vmz", MFnNumericData::kFloat, 1.f);
	numFn.setKeyable(true);
	numFn.setStorable(true);
	numFn.setMin(0.001f);
	addAttribute(azmultiplier);
	
	agroupcount = numFn.create( "numberInstances", "nis", MFnNumericData::kInt, 1);
	numFn.setKeyable(false);
	numFn.setStorable(true);
	numFn.setMin(1);
	addAttribute(agroupcount);
	
	ainstanceId = numFn.create( "instanceId", "iis", MFnNumericData::kInt, 0);
	numFn.setKeyable(false);
	numFn.setStorable(true);
	numFn.setMin(0);
	addAttribute(ainstanceId);

	MFnTypedAttribute typedAttrFn;
	MVectorArray defaultVectArray;
	MFnVectorArrayData vectArrayDataFn;
	vectArrayDataFn.create( defaultVectArray );
	outPositionPP = typedAttrFn.create( "outPosition",
											"opos",
											MFnData::kVectorArray,
											vectArrayDataFn.object(),
											&stat );
												
												if(!stat) MGlobal::displayWarning("failed create pospp");

	typedAttrFn.setStorable(false);
	if(addAttribute( outPositionPP ) != MS::kSuccess) MGlobal::displayWarning("failed add pospp");
	
	outScalePP = typedAttrFn.create( "outScale",
											"oscl",
											MFnData::kVectorArray,
											vectArrayDataFn.object(),
											&stat );
												
												if(!stat) MGlobal::displayWarning("failed create sclpp");

        typedAttrFn.setStorable(false);
        if(addAttribute(outScalePP) != MS::kSuccess) MGlobal::displayWarning("failed add sclpp");
		
	outRotationPP = typedAttrFn.create( "outRotation",
											"orot",
											MFnData::kVectorArray,
											vectArrayDataFn.object(),
											&stat );
												
												if(!stat) MGlobal::displayWarning("failed create rotpp");

        typedAttrFn.setStorable(false);
        if(addAttribute(outRotationPP) != MS::kSuccess) MGlobal::displayWarning("failed add rotpp");
		

        
    outValue = numFn.create( "outValue", "ov", MFnNumericData::kFloat );
	numFn.setStorable(false);
	numFn.setWritable(false);
	addAttribute(outValue);
	
	outValue1 = numFn.create( "outValue1", "ov1", MFnNumericData::kFloat );
	numFn.setStorable(false);
	numFn.setWritable(false);
	addAttribute(outValue1);
	
	MFnTypedAttribute   stringAttr;
	acachename = stringAttr.create( "cachePath", "cp", MFnData::kString );
 	stringAttr.setStorable(true);
	addAttribute( acachename );
	
	astandinNames = stringAttr.create( "standinNames", "sdn", MFnData::kString );
 	stringAttr.setStorable(true);
	stringAttr.setArray(true);
	addAttribute(astandinNames);
	
	MFnMatrixAttribute matAttr;
	acameraspace = matAttr.create( "cameraSpace", "cspc", MFnMatrixAttribute::kDouble );
 	matAttr.setStorable(false);
	matAttr.setWritable(true);
	matAttr.setConnectable(true);
	addAttribute(acameraspace);
	
	ahapeture = numFn.create( "horizontalFilmAperture", "hfa", MFnNumericData::kDouble, 1.0 );
	numFn.setStorable(false);
	numFn.setConnectable(true);
	addAttribute( ahapeture );
	
	avapeture = numFn.create( "verticalFilmAperture", "vfa", MFnNumericData::kDouble, 1.0 );
	numFn.setStorable(false);
	numFn.setConnectable(true);
	addAttribute( avapeture );
	
	afocallength = numFn.create( "focalLength", "fl", MFnNumericData::kDouble );
	numFn.setStorable(false);
	numFn.setConnectable(true);
	addAttribute( afocallength );
	
	ainmesh = typedAttrFn.create("displayMesh", "dspm", MFnMeshData::kMesh);
	typedAttrFn.setStorable(false);
	typedAttrFn.setWritable(true);
	typedAttrFn.setConnectable(true);
	addAttribute( ainmesh );
	
	aconvertPercentage = numFn.create( "convertPercentage", "cvp", MFnNumericData::kDouble );
	numFn.setStorable(false);
	numFn.setConnectable(true);
	numFn.setDefault(1.0);
	numFn.setMax(1.0);
	numFn.setMin(0.01);
	addAttribute(aconvertPercentage);
    
    agroundMesh = typedAttrFn.create("groundMesh", "grdm", MFnMeshData::kMesh);
	typedAttrFn.setStorable(false);
	typedAttrFn.setWritable(true);
	typedAttrFn.setConnectable(true);
    typedAttrFn.setArray(true);
    typedAttrFn.setDisconnectBehavior(MFnAttribute::kDelete);
	addAttribute( agroundMesh );
	attributeAffects(agroundMesh, outValue);
	
	agroundSpace = matAttr.create("groundSpace", "grdsp", MFnMatrixAttribute::kDouble);
	matAttr.setStorable(false);
	matAttr.setWritable(true);
	matAttr.setConnectable(true);
    matAttr.setArray(true);
    matAttr.setDisconnectBehavior(MFnAttribute::kDelete);
	addAttribute( agroundSpace );
	attributeAffects(agroundSpace, outValue);
	
	MPointArray defaultPntArray;
	MFnPointArrayData pntArrayDataFn;
	pntArrayDataFn.create( defaultPntArray );
	aplantTransformCache = typedAttrFn.create( "transformCachePlant",
											"tmcpl",
											MFnData::kPointArray,
											pntArrayDataFn.object(),
											&stat );
    typedAttrFn.setStorable(true);
	addAttribute(aplantTransformCache);
	
	MIntArray defaultIntArray;
	MFnIntArrayData intArrayDataFn;
	intArrayDataFn.create( defaultIntArray );
	aplantIdCache = typedAttrFn.create( "idCachePlant",
											"idcpl",
											MFnData::kIntArray,
											intArrayDataFn.object(),
											&stat );
    typedAttrFn.setStorable(true);
	addAttribute(aplantIdCache);
	
	aplantTriangleIdCache = typedAttrFn.create( "triCachePlant",
											"trcpl",
											MFnData::kIntArray,
											intArrayDataFn.object(),
											&stat );
    typedAttrFn.setStorable(true);
	addAttribute(aplantTriangleIdCache);
	
	aplantTriangleCoordCache = typedAttrFn.create( "coordCachePlant",
											"crcpl",
											MFnData::kVectorArray,
											vectArrayDataFn.object(),
											&stat );
    typedAttrFn.setStorable(true);
	addAttribute(aplantTriangleCoordCache);
	
	ainexamp = typedAttrFn.create("inExample", "ixmp", MFnData::kPlugin);
	typedAttrFn.setStorable(false);
	typedAttrFn.setConnectable(true);
	typedAttrFn.setArray(true);
	addAttribute(ainexamp);
    
    adisplayVox = numFn.create( "showVoxelThreshold", "svt", MFnNumericData::kFloat );
	numFn.setDefault(1.0);
    numFn.setMin(.7);
    numFn.setMax(1.0);
    numFn.setStorable(true);
	numFn.setKeyable(true);
    addAttribute(adisplayVox);
	
	acheckDepth = numFn.create( "checkDepth", "cdp", MFnNumericData::kBoolean );
	numFn.setDefault(0);
	numFn.setStorable(false);
	addAttribute(acheckDepth);
	
	ainoverscan = numFn.create( "cameraOverscan", "cos", MFnNumericData::kDouble );
	numFn.setDefault(1.33);
	numFn.setStorable(false);
	addAttribute(ainoverscan);
    
    aactivated = numFn.create( "activated", "act", MFnNumericData::kBoolean );
	numFn.setDefault(0);
	numFn.setStorable(false);
	addAttribute(aactivated);
    
	attributeAffects(ainexamp, outValue1);
	attributeAffects(aradiusMult, outValue1);
	attributeAffects(abboxminx, outValue);
	attributeAffects(abboxmaxx, outValue);
	attributeAffects(abboxminy, outValue);
	attributeAffects(abboxmaxy, outValue);
	attributeAffects(abboxminz, outValue);
	attributeAffects(abboxmaxz, outValue);
	attributeAffects(ainmesh, outValue);
	attributeAffects(outPositionPP, outValue);
	
	return MS::kSuccess;
}

void ProxyViz::attachSceneCallbacks()
{
	fBeforeSaveCB  = MSceneMessage::addCallback(MSceneMessage::kBeforeSave,  releaseCallback, this);
}

void ProxyViz::detachSceneCallbacks()
{
	if (fBeforeSaveCB)
		MMessage::removeCallback(fBeforeSaveCB);

	fBeforeSaveCB = 0;
}

void ProxyViz::releaseCallback(void* clientData)
{
	ProxyViz *pThis = (ProxyViz*) clientData;
	pThis->saveInternal();
}

void ProxyViz::saveInternal()
{
	AHelper::Info<MString>("prxnode save internal", MFnDependencyNode(thisMObject()).name() );
	updateNumPlants();
	const unsigned n = numPlants();
	AHelper::Info<unsigned>("num plants", n );
	if(n<1) return;
	
	MPointArray plantTms;
	MIntArray plantIds;
	MIntArray plantTris;
	MVectorArray plantCoords;
	
	savePlants(plantTms, plantIds, plantTris, plantCoords);
	
	MFnPointArrayData tmFn;
	MObject otm = tmFn.create(plantTms);
	MPlug tmPlug(thisMObject(), aplantTransformCache);
	tmPlug.setValue(otm);
	
	MFnIntArrayData idFn;
	MObject oid = idFn.create(plantIds);
	MPlug idPlug(thisMObject(), aplantIdCache);
	idPlug.setValue(oid);
	
	MFnIntArrayData triFn;
	MObject otri = idFn.create(plantTris);
	MPlug triPlug(thisMObject(), aplantTriangleIdCache);
	triPlug.setValue(otri);
	
	MFnVectorArrayData crdFn;
	MObject ocrd = crdFn.create(plantCoords);
	MPlug crdPlug(thisMObject(), aplantTriangleCoordCache);
	crdPlug.setValue(ocrd);
}

bool ProxyViz::loadInternal(MDataBlock& block)
{
	MDataHandle tmH = block.inputValue(aplantTransformCache);
	MFnPointArrayData tmFn(tmH.data());
	MPointArray plantTms = tmFn.array();
	if(plantTms.length() < 1) return false;
	
	MDataHandle idH = block.inputValue(aplantIdCache);
	MFnIntArrayData idFn(idH.data());
	MIntArray plantIds = idFn.array();
	if(plantIds.length() < 1) return false;
	
	MDataHandle triH = block.inputValue(aplantTriangleIdCache);
	MFnIntArrayData triFn(triH.data());
	MIntArray plantTris = triFn.array();
	if(plantTris.length() < 1) return false;
	
	MDataHandle crdH = block.inputValue(aplantTriangleCoordCache);
	MFnVectorArrayData crdFn(crdH.data());
	MVectorArray plantCoords = crdFn.array();
	if(plantCoords.length() < 1) return false;
		
	return loadPlants(plantTms, plantIds, plantTris, plantCoords);
}

void ProxyViz::adjustPosition(short start_x, short start_y, short last_x, short last_y, float clipNear, float clipFar)
{
    Vector3F v0, v1;
	Ray r = getRayDisplace(v0, v1, start_x, start_y, last_x, last_y);
	
	movePlant(r, v0, v1, clipNear, clipFar);
}

void ProxyViz::rotateToDirection(short start_x, short start_y, short last_x, short last_y, float clipNear, float clipFar)
{
	Vector3F v0, v1;
	Ray r = getRayDisplace(v0, v1, start_x, start_y, last_x, last_y);
	
	rotatePlant(r, v0, v1, clipNear, clipFar);
}

Ray ProxyViz::getRayDisplace(Vector3F & v0, Vector3F & v1,
			short start_x, short start_y, short last_x, short last_y)
{
	useActiveView();
	MPoint toNear, toFar;
	_viewport.viewToWorld ( last_x, last_y, toNear, toFar );
	
	MPoint fromNear, fromFar;
	_viewport.viewToWorld ( start_x, start_y, fromNear, fromFar );
	
	MVector dispNear = toNear - fromNear;
	MVector dispFar = toFar - fromFar;
	
	Vector3F a(toNear.x, toNear.y, toNear.z);
	Vector3F b(toFar.x, toFar.y, toFar.z);
	Ray r(a, b);
	v0.set(dispNear.x, dispNear.y, dispNear.z);
	v1.set(dispFar.x, dispFar.y, dispFar.z);
	return r;
}

void ProxyViz::pressToSave()
{
	MObject thisNode = thisMObject();
	MPlug plugc( thisNode, acachename );
	const MString filename = plugc.asString();
	if(filename != "")
	    saveExternal(replaceEnvVar(filename).c_str());
	else 
		AHelper::Info<int>("ProxyViz error empty external cache filename", 0);
}

void ProxyViz::pressToLoad()
{
	MObject thisNode = thisMObject();
	MPlug plugc( thisNode, acachename );
	const MString filename = plugc.asString();
	if(filename != "")
		loadExternal(replaceEnvVar(filename).c_str());
	else 
		AHelper::Info<int>("ProxyViz error empty external cache filename", 0);
}

void ProxyViz::updateWorldSpace()
{
	MObject thisNode = thisMObject();
	MDagPath thisPath;
	MDagPath::getAPathTo(thisNode, thisPath);
	_worldSpace = thisPath.inclusiveMatrix();
	_worldInverseSpace = thisPath.inclusiveMatrixInverse();
}

MMatrix ProxyViz::localizeSpace(const MMatrix & s) const
{
	MMatrix m = s;
	m *= _worldInverseSpace;
	return m;
}

MMatrix ProxyViz::worldizeSpace(const MMatrix & s) const
{
	MMatrix m = s;
	m *= _worldSpace;
	return m;
}

void ProxyViz::useActiveView()
{ _viewport = M3dView::active3dView(); }

char ProxyViz::hasDisplayMesh() const
{
	MPlug pm(thisMObject(), ainmesh);
	if(!pm.isConnected())
		return 0;
		
	if(fDisplayMesh == MObject::kNullObj)
		return 0;

	return 1;
}

const MMatrix & ProxyViz::worldSpace() const
{ return _worldSpace; }

std::string ProxyViz::replaceEnvVar(const MString & filename) const
{
    EnvVar var;
	std::string sfilename(filename.asChar());
	if(var.replace(sfilename))
	    MGlobal::displayInfo(MString("substitute file path "+filename+" to ")+sfilename.c_str());
	return sfilename;
}

MStatus ProxyViz::connectionMade ( const MPlug & plug, const MPlug & otherPlug, bool asSrc )
{
	if(plug == acameraspace) enableView();
	else if(plug == outPositionPP) m_hasParticle = true;
	//AHelper::Info<MString>("connect", plug.name());
	return MPxLocatorNode::connectionMade (plug, otherPlug, asSrc );
}

MStatus ProxyViz::connectionBroken ( const MPlug & plug, const MPlug & otherPlug, bool asSrc )
{
	if(plug == acameraspace) disableView();
	else if(plug == outPositionPP) m_hasParticle = false;
	//AHelper::Info<MString>("disconnect", plug.name());
	return MPxLocatorNode::connectionMade (plug, otherPlug, asSrc );
}

void ProxyViz::updateViewFrustum(MObject & thisNode)
{
	MPlug matplg(thisNode, acameraspace);
	MObject matobj;
	matplg.getValue(matobj);
	MFnMatrixData matdata(matobj);
    MMatrix cameramat = matdata.matrix(); 
	AHelper::ConvertToMatrix44F(*cameraSpaceR(), cameramat);
	AHelper::ConvertToMatrix44F(*cameraInvSpaceR(), cameramat.inverse() );
	float peye[3];
	peye[0] = cameramat.matrix[3][0];
	peye[1] = cameramat.matrix[3][1];
	peye[2] = cameramat.matrix[3][2];
	setEyePosition(peye);
	
	MPlug hfaplg(thisNode, ahapeture);
	float hfa = hfaplg.asFloat();
	MPlug vfaplg(thisNode, avapeture);
	float vfa = vfaplg.asFloat();
	MPlug flplg(thisNode, afocallength);
	float fl = flplg.asFloat();
	
    float farClip = -20.f;
    if(numPlants() > 0) getFarClipDepth(farClip, gridBoundingBox() );
    
    setFrustum(hfa, vfa, fl, -10.f, farClip );
	
	MPlug overscanPlug(thisNode, ainoverscan);
	setOverscan(overscanPlug.asDouble() );
}

void ProxyViz::updateViewFrustum(const MDagPath & cameraPath)
{
	MMatrix cameraMat = cameraPath.inclusiveMatrix();
	AHelper::ConvertToMatrix44F(*cameraSpaceR(), cameraMat);
	MMatrix cameraInvMat = cameraPath.inclusiveMatrixInverse();
	AHelper::ConvertToMatrix44F(*cameraInvSpaceR(), cameraInvMat);
	float peye[3];
	peye[0] = cameraMat.matrix[3][0];
	peye[1] = cameraMat.matrix[3][1];
	peye[2] = cameraMat.matrix[3][2];
	setEyePosition(peye);
	
    float farClip = -20.f;
    if(numPlants() > 0) getFarClipDepth(farClip, gridBoundingBox() );
    
	MFnCamera fcam(cameraPath.node() );
	if(fcam.isOrtho() ) {
		float orthoW = fcam.orthoWidth();
		float orthoH = orthoW * fcam.aspectRatio();
		setOrthoFrustum(orthoW, orthoH, -10.f, farClip );
		
	} else {
		float hfa = fcam.horizontalFilmAperture();
		float vfa = fcam.verticalFilmAperture();
		float fl = fcam.focalLength();
	
		setFrustum(hfa, vfa, fl, -10.f, farClip );
	}
	setOverscan(fcam.overscan() );
}

void ProxyViz::beginPickInView()
{
	MGlobal::displayInfo("MForest begin pick in view");
	initRandGroup();
	selection()->deselect();
	m_toCheckVisibility = true;
}

void ProxyViz::processPickInView(const int & plantTyp)
{
	useActiveView();
	_viewport.refresh();
	
	MObject node = thisMObject();
	
	MPlug gateHighPlg(node, alodgatehigh);
	float gateHigh = gateHighPlg.asFloat();
	
	MPlug gateLowPlg(node, alodgatelow);
	float gateLow = gateLowPlg.asFloat();
	
	MPlug gcPlg(node, agroupcount);
	int groupCount = gcPlg.asInt();
	
	MPlug giPlg(node, ainstanceId);
	int groupId = giPlg.asInt();
	
	MPlug perPlg(node, aconvertPercentage);
	double percentage = perPlg.asDouble();
	pickVisiblePlants(gateLow, gateHigh, groupCount, groupId, percentage, plantTyp);
}

void ProxyViz::endPickInView()
{ m_toCheckVisibility = false; }

void ProxyViz::setEnableCompute(bool x)
{ m_enableCompute = x; }
	
void ProxyViz::updateGeomBox(ExampVox * dst, MObject & node)
{
	dst->setGeomSizeMult(MPlug(node, aradiusMult).asFloat() );
	dst->setGeomBox(MPlug(node, abboxminx).asFloat(),
			MPlug(node, abboxminy).asFloat(), 
			MPlug(node, abboxminz).asFloat(), 
			MPlug(node, abboxmaxx).asFloat(), 
			MPlug(node, abboxmaxy).asFloat(), 
			MPlug(node, abboxmaxz).asFloat());
}

void ProxyViz::drawBrush(M3dView & view)
{
    const float & radius = selectionRadius();
    MString radstr("radius: ");
    radstr += radius;
    const Vector3F & position = selectionCenter();
    view.drawText(radstr, MPoint(position.x, position.y, position.z) );
	
    DrawForest::drawBrush();
}

void ProxyViz::deselectFaces()
{
    activeGround()->deselect();
    MGlobal::displayInfo(" discard selected faces");
    if(hasView() ) _viewport.refresh();
}

void ProxyViz::deselectPlants()
{
    selection()->deselect();
    MGlobal::displayInfo(" discard selected plants");
    if(hasView() ) _viewport.refresh();
}

void ProxyViz::injectPlants(const std::vector<Matrix44F> & ms, GrowOption & option)
{
    std::vector<Matrix44F>::const_iterator it = ms.begin();
    for(;it!=ms.end();++it) {
		if(!growAt(*it, option) )
			AHelper::Info<Vector3F>("no grow at ", (*it).getTranslation());		
	}
		
	finishGrow();
}

}
//:~