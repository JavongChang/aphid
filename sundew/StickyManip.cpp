#include "StickyManip.h"
#include <maya/MString.h> 
#include <maya/MVector.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MColor.h>
#include <maya/M3dView.h>
#include <maya/MDistance.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MFnDistanceManip.h> 
#include <maya/MFnFreePointTriadManip.h>
#include <maya/MFnNumericData.h>
#include <maya/MFnNumericAttribute.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnMeshData.h>
#include <maya/MFnIntArrayData.h>
#include <AHelper.h>

MManipData StickyLocatorManip::startPointCallback(unsigned index) const
{
	MFnNumericData numData;
	MObject numDataObj = numData.create(MFnNumericData::k3Double);
	MVector vec = nodeTranslation();
	numData.setData(vec.x, vec.y, vec.z);
	return MManipData(numDataObj);
}

MVector StickyLocatorManip::nodeTranslation() const
{
	MFnDagNode dagFn(fNodePath);
	MDagPath path;
	dagFn.getPath(path);
	path.pop();  // pop from the shape to the transform
	MFnTransform transformFn(path);
	return transformFn.translation(MSpace::kWorld);
}

MTypeId StickyLocatorManip::id( 0x38575951 );


StickyLocatorManip::StickyLocatorManip() 
{ 
    // Do not call createChildren from here 
}


StickyLocatorManip::~StickyLocatorManip() 
{
}


void* StickyLocatorManip::creator()
{
     return new StickyLocatorManip();
}


MStatus StickyLocatorManip::initialize()
{ 
    MStatus stat;
    stat = MPxManipContainer::initialize();
    return stat;
}


MStatus StickyLocatorManip::createChildren()
{
    MStatus stat = MStatus::kSuccess;

    MString manipName("distanceManip");
    MString distanceName("distance");

    MPoint startPoint(0.0, 0.0, 0.0);
    MVector direction(0.0, 1.0, 0.0);
    fDistanceManip = addDistanceManip(manipName,
									  distanceName);
	MFnDistanceManip distanceManipFn(fDistanceManip);
	distanceManipFn.setStartPoint(startPoint);
	distanceManipFn.setDirection(direction);
	
	fDirectionManip = addFreePointTriadManip("freePointTriadManip",
										"point");
	MFnFreePointTriadManip directionManipFn(fDirectionManip);
	MPoint ori(0.0, 0.0, 1.0);
	directionManipFn.setPoint(ori);
	return stat;
}

MStatus StickyLocatorManip::connectToDependNode(const MObject &node)
{
    MStatus stat;

	MFnDagNode dagNodeFn(node);
	dagNodeFn.getPath(fNodePath);
	
	MPlug spacePlug = dagNodeFn.findPlug("vertexMatrix");
	MObject om;
	spacePlug.getValue(om);
	MFnMatrixData fm(om);
	MMatrix m = fm.matrix();
	// AHelper::Info<MMatrix>("manip m", m);
	MTransformationMatrix tm(m);
	
	MFnDependencyNode nodeFn(node);
	
	MFnFreePointTriadManip directionManipFn(fDirectionManip);
	//directionManipFn.setTranslation(mean, MSpace::kObject);
	directionManipFn.set(tm);
	MPlug directionPlug = nodeFn.findPlug("displaceVec", &stat);
    if (MStatus::kFailure != stat) {
	    directionManipFn.connectToPointPlug(directionPlug);
	}
	
    MFnDistanceManip distanceManipFn(fDistanceManip);
	//distanceManipFn.setTranslation(mean, MSpace::kObject);    
	distanceManipFn.set(tm);
	MPlug sizePlug = nodeFn.findPlug("size", &stat);
    if (MStatus::kFailure != stat) {
	    distanceManipFn.connectToDistancePlug(sizePlug);
		unsigned startPointIndex = distanceManipFn.startPointIndex();
	    addPlugToManipConversionCallback(startPointIndex, 
										 (plugToManipConversionCallback) 
										 &StickyLocatorManip::startPointCallback);
	}

	finishAddingManips();
	return MPxManipContainer::connectToDependNode(node);
}


void StickyLocatorManip::draw(M3dView & view, 
								 const MDagPath &path, 
								 M3dView::DisplayStyle style,
								 M3dView::DisplayStatus status)
{ 
    MPxManipContainer::draw(view, path, style, status);
	return;
    view.beginGL(); 

    MPoint textPos = nodeTranslation();
    char str[100];
    sprintf(str, "Stretch Me!"); 
    MString distanceText(str);
    view.drawText(distanceText, textPos, M3dView::kLeft);
	
    view.endGL();
}
