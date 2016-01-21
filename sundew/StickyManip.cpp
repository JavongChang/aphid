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
/// use vertex matrix translate
	numData.setData(m_startPOffset.x, m_startPOffset.y, m_startPOffset.z);
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

    fDistanceManip = addDistanceManip("distanceManip",
									  "distance");
	MFnDistanceManip distanceManipFn(fDistanceManip);
	distanceManipFn.setDirection(MVector(0.0, 1.0, 0.0));
	
	fDirectionManip = addFreePointTriadManip("freePointTriadManip",
										"point");
	MFnFreePointTriadManip directionManipFn(fDirectionManip);
	
	fDropoffManip = addDistanceManip("dropoffManip",
									  "distance1");
	MFnDistanceManip dropoffManipFn(fDropoffManip);
	dropoffManipFn.setDirection(MVector(1.0, 0.0, 0.0));
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
	m_startPOffset = MVector(m[3][0], m[3][1], m[3][2]);
	// AHelper::Info<MVector>("start at", m_startPOffset);
	m_scalingF = MVector(m[0][0], m[0][1], m[0][2]).length();
	
	MFnDependencyNode nodeFn(node);
	
	MFnFreePointTriadManip directionManipFn(fDirectionManip);
	directionManipFn.set(tm);
	MPlug directionPlug = nodeFn.findPlug("displaceVec", &stat);
    if (MStatus::kFailure != stat) {
	    directionManipFn.connectToPointPlug(directionPlug);
	}
	
    MFnDistanceManip sizeManipFn(fDistanceManip);
	sizeManipFn.setScalingFactor(m_scalingF);
	
	MPlug sizePlug = nodeFn.findPlug("size", &stat);
    if (MStatus::kFailure != stat) {
	    sizeManipFn.connectToDistancePlug(sizePlug);
		unsigned startPointIndex = sizeManipFn.startPointIndex();
	    addPlugToManipConversionCallback(startPointIndex, 
										 (plugToManipConversionCallback) 
										 &StickyLocatorManip::startPointCallback);
		
	}
	
	double currentSize = 1.0;
	sizePlug.getValue(currentSize);

	MFnDistanceManip dropoffManipFn(fDropoffManip);
	dropoffManipFn.setScalingFactor(m_scalingF * currentSize);
	
	MPlug dropoffPlug = nodeFn.findPlug("dropoff", &stat);
    if (MStatus::kFailure != stat) {
	    dropoffManipFn.connectToDistancePlug(dropoffPlug);
		unsigned startPointIndex = dropoffManipFn.startPointIndex();
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
