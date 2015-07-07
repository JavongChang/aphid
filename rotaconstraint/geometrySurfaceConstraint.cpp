#include "geometrySurfaceConstraint.h"
#include <maya/MFnMatrixData.h>

MTypeId     geometrySurfaceConstraint::id( 0x33f77896 );
MObject     geometrySurfaceConstraint::compoundTarget;  
MObject		geometrySurfaceConstraint::targetTransform;
MObject     geometrySurfaceConstraint::targetGeometry;       
MObject     geometrySurfaceConstraint::targetWeight;       
MObject     geometrySurfaceConstraint::constraintParentInverseMatrix;       
MObject     geometrySurfaceConstraint::constraintGeometry;
MObject		geometrySurfaceConstraint::constraintTranslateX;
MObject		geometrySurfaceConstraint::constraintTranslateY;
MObject		geometrySurfaceConstraint::constraintTranslateZ;
MObject		geometrySurfaceConstraint::constraintTargetX;
MObject		geometrySurfaceConstraint::constraintTargetY;
MObject		geometrySurfaceConstraint::constraintTargetZ;
MObject		geometrySurfaceConstraint::constraintObjectX;
MObject		geometrySurfaceConstraint::constraintObjectY;
MObject		geometrySurfaceConstraint::constraintObjectZ;

geometrySurfaceConstraint::geometrySurfaceConstraint() 
{
	weightType = rotaBase::kLargestWeight;
    m_lastPos = MPoint::origin;
	m_isInitd = false;
}

geometrySurfaceConstraint::~geometrySurfaceConstraint() 
{
}

void geometrySurfaceConstraint::postConstructor()
{
}

MStatus geometrySurfaceConstraint::compute( const MPlug& plug, MDataBlock& block )
{	
	MStatus returnStatus;
 
    if(plug == constraintTranslateX || plug == constraintTranslateY || plug == constraintTranslateZ) {
        if(!m_isInitd) {
			MPlug pgTx(thisMObject(), constraintTargetX);
			double lastTx = pgTx.asDouble();
			MPlug pgTy(thisMObject(), constraintTargetY);
			double lastTy = pgTy.asDouble();
			MPlug pgTz(thisMObject(), constraintTargetZ);
			double lastTz = pgTz.asDouble();
			
			MGlobal::displayInfo(MString("init target p ")+lastTx+" "+lastTy+" "+lastTz);
		
			MPlug pgOx(thisMObject(), constraintObjectX);
			double lastOx = pgOx.asDouble();
			MPlug pgOy(thisMObject(), constraintObjectY);
			double lastOy = pgOy.asDouble();
			MPlug pgOz(thisMObject(), constraintObjectZ);
			double lastOz = pgOz.asDouble();
			
			MGlobal::displayInfo(MString("init object p ")+lastOx+" "+lastOy+" "+lastOz);
		
			m_lastPos = MPoint(lastTx, lastTy, lastTz);
			m_totalOffset.m_v = MVector(lastOx, lastOy, lastOz);
			
			m_isInitd = true;
		}
		
		MArrayDataHandle targetArray = block.inputArrayValue( compoundTarget );
		const unsigned int targetArrayCount = targetArray.elementCount();
        MMatrix tm;
        tm.setToIdentity();
        unsigned int i;
		for ( i = 0; i < targetArrayCount; i++ ) {
            MDataHandle targetElement = targetArray.inputValue(&returnStatus);
            if(!returnStatus) {
                MGlobal::displayInfo("failed to get input value target element");
            }
            MDataHandle htm = targetElement.child(targetTransform);
            MFnMatrixData ftm(htm.data(), &returnStatus);
            if(!returnStatus) {
                MGlobal::displayInfo("failed to get matrix data");
            }
            tm = ftm.matrix();
            targetArray.next();
        }

        MPoint curPos(tm(3,0), tm(3,1), tm(3,2));
		
// target translates in world space
        MVector dv = curPos - m_lastPos;
// into target object space
		dv *= MTransformationMatrix(tm).asRotateMatrix().inverse();
// cancel out
        m_totalOffset.add(-dv);
// object position in world space
		MPoint wp = m_totalOffset.asPoint() * tm;
		
		m_lastPos = curPos;
		
        MDataHandle hout;
        if(plug == constraintTranslateX) {
            hout = block.outputValue(constraintTranslateX);
			hout.set(wp.x);
        }
        else if(plug == constraintTranslateY) {
            hout = block.outputValue(constraintTranslateY);
			hout.set(wp.y);
        }
        else if(plug == constraintTranslateZ) {
            hout = block.outputValue(constraintTranslateZ);
			hout.set(wp.z);
        }
		
		MPlug pgTx(thisMObject(), constraintTargetX);
		pgTx.setValue(m_lastPos.x);
		MPlug pgTy(thisMObject(), constraintTargetY);
		pgTy.setValue(m_lastPos.y);
		MPlug pgTz(thisMObject(), constraintTargetZ);
		pgTz.setValue(m_lastPos.z);
		
		MPlug pgOx(thisMObject(), constraintObjectX);
		pgOx.setValue(m_totalOffset.m_v.x);
		MPlug pgOy(thisMObject(), constraintObjectY);
		pgOy.setValue(m_totalOffset.m_v.y);
		MPlug pgOz(thisMObject(), constraintObjectZ);
		pgOz.setValue(m_totalOffset.m_v.z);
    }
	else {
		return MS::kUnknownParameter;
	}

	return MS::kSuccess;
}

const MObject geometrySurfaceConstraint::weightAttribute() const
{
	return geometrySurfaceConstraint::targetWeight;
}

const MObject geometrySurfaceConstraint::targetAttribute() const
{
	return geometrySurfaceConstraint::compoundTarget;
}

void geometrySurfaceConstraint::getOutputAttributes(MObjectArray& attributeArray)
{
	attributeArray.clear();
	attributeArray.append( geometrySurfaceConstraint::constraintGeometry );
}

void* geometrySurfaceConstraint::creator()
{
	return new geometrySurfaceConstraint();
}

MStatus geometrySurfaceConstraint::initialize()
{
	MFnNumericAttribute nAttr;
	MStatus				status;

	// constraint attributes

	{	// Geometry: mesh, readable, not writable, delete on disconnect
		MFnTypedAttribute typedAttrNotWritable;
		geometrySurfaceConstraint::constraintGeometry =
			typedAttrNotWritable.create( "constraintGeometry", "cg", MFnData::kMesh, &status ); 	
		if (!status) { status.perror("typedAttrNotWritable.create:cgeom"); return status;}
		status = typedAttrNotWritable.setReadable(true);
		if (!status) { status.perror("typedAttrNotWritable.setReadable:cgeom"); return status;}
		status = typedAttrNotWritable.setWritable(false);
		if (!status) { status.perror("typedAttrNotWritable.setWritable:cgeom"); return status;}
		status = typedAttrNotWritable.setDisconnectBehavior(MFnAttribute::kDelete);
		if (!status) { status.perror("typedAttrNotWritable.setDisconnectBehavior:cgeom"); return status;}
	}
// Parent inverse matrix: delete on disconnect
	MFnTypedAttribute typedAttr;
		geometrySurfaceConstraint::constraintParentInverseMatrix =
			typedAttr.create( "constraintPim", "ci", MFnData::kMatrix, &status ); 	
		if (!status) { status.perror("typedAttr.create:matrix"); return status;}
		status = typedAttr.setDisconnectBehavior(MFnAttribute::kDelete);
		if (!status) { status.perror("typedAttr.setDisconnectBehavior:cgeom"); return status;}

		// Target geometry: mesh, delete on disconnect
		geometrySurfaceConstraint::targetGeometry =
			typedAttr.create( "targetGeometry", "tg", MFnData::kMesh, &status ); 	
		if (!status) { status.perror("typedAttr.create:tgeom"); return status;}
		status = typedAttr.setDisconnectBehavior(MFnAttribute::kDelete);
		if (!status) { status.perror("typedAttr.setDisconnectBehavior:cgeom"); return status;}
    
    targetTransform = typedAttr.create( "targetTransform", "ttm", MFnData::kMatrix, &status ); 	
    if (!status) { status.perror("typedAttr.create:targetTransform"); return status;}
    status = typedAttr.setDisconnectBehavior(MFnAttribute::kDelete);
    if (!status) { status.perror("typedAttr.setDisconnectBehavior:targetTransform"); return status;}
    
// Target weight: double, min 0, default 1.0, keyable, delete on disconnect
	MFnNumericAttribute typedAttrKeyable;
		geometrySurfaceConstraint::targetWeight 
			= typedAttrKeyable.create( "weight", "wt", MFnNumericData::kDouble, 1.0, &status );
		if (!status) { status.perror("typedAttrKeyable.create:weight"); return status;}
		status = typedAttrKeyable.setMin( (double) 0 );
		if (!status) { status.perror("typedAttrKeyable.setMin"); return status;}
		status = typedAttrKeyable.setKeyable( true );
		if (!status) { status.perror("typedAttrKeyable.setKeyable"); return status;}
		status = typedAttrKeyable.setDisconnectBehavior(MFnAttribute::kDelete);
		if (!status) { status.perror("typedAttrKeyable.setDisconnectBehavior:cgeom"); return status;}

	{	// Compound target(geometry,weight): array, delete on disconnect
		MFnCompoundAttribute compoundAttr;
		geometrySurfaceConstraint::compoundTarget = 
			compoundAttr.create( "target", "tgt",&status );
		if (!status) { status.perror("compoundAttr.create"); return status;}
        status = compoundAttr.addChild( geometrySurfaceConstraint::targetTransform );
		if (!status) { status.perror("compoundAttr.addChild targetTransform"); return status;}
		status = compoundAttr.addChild( geometrySurfaceConstraint::targetGeometry );
		if (!status) { status.perror("compoundAttr.addChild"); return status;}
		status = compoundAttr.addChild( geometrySurfaceConstraint::targetWeight );
		if (!status) { status.perror("compoundAttr.addChild"); return status;}
		status = compoundAttr.setArray( true );
		if (!status) { status.perror("compoundAttr.setArray"); return status;}
		status = compoundAttr.setDisconnectBehavior(MFnAttribute::kDelete);
		if (!status) { status.perror("typedAttrKeyable.setDisconnectBehavior:cgeom"); return status;}
	}
    
    MFnNumericAttribute numAttr;
    constraintTranslateX = numAttr.create( "constraintTranslateX", "ctx", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTranslateX");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(false);
    addAttribute(constraintTranslateX);
    
    constraintTranslateY = numAttr.create( "constraintTranslateY", "cty", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTranslateY");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(false);
    addAttribute(constraintTranslateY);
    
    constraintTranslateZ = numAttr.create( "constraintTranslateZ", "ctz", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTranslateY");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(false);
    addAttribute(constraintTranslateZ);

	constraintTargetX = numAttr.create( "constraintTargetX", "ttx", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTargetX");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
    numAttr.setStorable(true);
    addAttribute(constraintTargetX);
	
	constraintTargetY = numAttr.create( "constraintTargetY", "tty", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTargetY");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
    numAttr.setStorable(true);
    addAttribute(constraintTargetY);
	
	constraintTargetZ = numAttr.create( "constraintTargetZ", "ttz", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintTargetZ");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
    numAttr.setStorable(true);
    addAttribute(constraintTargetZ);
	
	constraintObjectX = numAttr.create( "constraintObjectX", "otx", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintObjectX");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
    numAttr.setStorable(true);
    addAttribute(constraintObjectX);
	
	constraintObjectY = numAttr.create( "constraintObjectY", "oty", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintObjectY");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
    numAttr.setStorable(true);
    addAttribute(constraintObjectY);
	
	constraintObjectZ = numAttr.create( "constraintObjectZ", "otz", MFnNumericData::kDouble, 0.0, &status );
    if(!status) {
        MGlobal::displayInfo("failed to create attrib constraintObjectZ");
        return status;
    }
    numAttr.setReadable(true);
	numAttr.setWritable(true);
	numAttr.setStorable(true);
    addAttribute(constraintObjectZ);

	status = addAttribute( geometrySurfaceConstraint::constraintParentInverseMatrix );
	if (!status) { status.perror("addAttribute"); return status;}
	status = addAttribute( geometrySurfaceConstraint::constraintGeometry );
	if (!status) { status.perror("addAttribute"); return status;}
	status = addAttribute( geometrySurfaceConstraint::compoundTarget );
	if (!status) { status.perror("addAttribute"); return status;}

	status = attributeAffects( compoundTarget, constraintGeometry );
	if (!status) { status.perror("attributeAffects"); return status;}
    status = attributeAffects( targetTransform, constraintGeometry );
	if (!status) { status.perror("attributeAffects"); return status;}
	status = attributeAffects( targetGeometry, constraintGeometry );
	if (!status) { status.perror("attributeAffects"); return status;}
	status = attributeAffects( targetWeight, constraintGeometry );
	if (!status) { status.perror("attributeAffects"); return status;}
	status = attributeAffects( constraintParentInverseMatrix, constraintGeometry );
	if (!status) { status.perror("attributeAffects"); return status;}
    
    attributeAffects(targetTransform, constraintTranslateX);
    attributeAffects(targetTransform, constraintTranslateY);
    attributeAffects(targetTransform, constraintTranslateZ);

	return MS::kSuccess;
}
