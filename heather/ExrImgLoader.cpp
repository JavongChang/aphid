#include "ExrImgLoader.h"
#include <maya/MPlug.h>
#include <maya/MDataBlock.h>
#include <maya/MDataHandle.h>
#include <maya/MFnTypedAttribute.h>
#include <maya/MFnUnitAttribute.h>
#include <maya/MTime.h>
#include <maya/MVector.h>
#include <maya/MMatrix.h>
#include <maya/MFnMatrixData.h>
#include <maya/MFnPluginData.h>
#include <maya/MGlobal.h>
#include <maya/MFnIntArrayData.h>
#include <maya/MFnVectorArrayData.h>
#include <SHelper.h>
#include <CudaBase.h>
MTypeId     ExrImgLoader::id(0x22c78c48);
MObject     ExrImgLoader::adepthImageName;
MObject     ExrImgLoader::aframeNumber;
MObject     ExrImgLoader::aframePadding;
MObject     ExrImgLoader::aenableMultiFrames;
MObject     ExrImgLoader::aoutval;

ExrImgLoader::ExrImgLoader()
{
	_pDesc = new ExrImgData::DataDesc;
	_pDesc->_img = 0;
	_pDesc->_isValid = 0;
}

ExrImgLoader::~ExrImgLoader() 
{
	if(_pDesc) delete _pDesc;
}

MStatus ExrImgLoader::compute( const MPlug& plug, MDataBlock& block )
{
	if(plug == aoutval) {
	    MString filename = block.inputValue(adepthImageName).asString();
        int frame = block.inputValue(aframeNumber).asInt();
		int padding = block.inputValue(aframePadding).asInt();
		bool enableSequence = block.inputValue(aenableMultiFrames).asBool();

		MStatus status;
		MGlobal::displayInfo("heather loader compute");
		
		preLoadImage(_pDesc, filename.asChar(), frame, padding, enableSequence);
		
		MFnPluginData fnPluginData;
		MObject newDataObject = fnPluginData.create(ExrImgData::id, &status);
		
		ExrImgData * pData = (ExrImgData *) fnPluginData.data(&status);
		
		if(pData) pData->setDesc(_pDesc);
	
		MGlobal::displayInfo("update exr image data desc");
		
		MDataHandle outDescHandle = block.outputValue(aoutval);
		status = outDescHandle.set(pData);

		block.setClean(plug);
		
		return MS::kSuccess;
	}
	
	return MS::kUnknownParameter;
}

void* ExrImgLoader::creator()
{
	return new ExrImgLoader();
}

MStatus ExrImgLoader::initialize()		
{
    MFnTypedAttribute typedAttr;
	MFnNumericAttribute numAttr;
	MStatus			 stat;
	
	adepthImageName = typedAttr.create( "depthImage", "dmg", MFnData::kString );
 	typedAttr.setStorable(true);
	addAttribute(adepthImageName);
	
	aenableMultiFrames = numAttr.create( "useImageSequence", "uis", MFnNumericData::kBoolean );
	numAttr.setStorable(true);
	addAttribute(aenableMultiFrames);
	
	aframeNumber = numAttr.create( "frameNumber", "fnb", MFnNumericData::kInt );
	numAttr.setStorable(true);
	numAttr.setKeyable(true);
	addAttribute(aframeNumber);
	
	aframePadding = numAttr.create( "framePadding", "fpd", MFnNumericData::kInt );
	numAttr.setDefault(0);
	numAttr.setStorable(true);
	addAttribute(aframePadding);
	
	aoutval = typedAttr.create("outValue", "ov", MFnData::kPlugin);
	typedAttr.setStorable(false);
	typedAttr.setConnectable(true);
	addAttribute(aoutval);
    
	attributeAffects(adepthImageName, aoutval);
	attributeAffects(aenableMultiFrames, aoutval);
	attributeAffects(aframeNumber, aoutval);

	return MS::kSuccess;
}

void ExrImgLoader::preLoadImage(ExrImgData::DataDesc * dst, const char * name, int frame, int padding, bool useImageSequence)
{
    if(!CudaBase::HasDevice) return;
    std::string fileName(name);
    if(fileName.size() < 3) {
        dst->_img = 0;
        dst->_isValid = 0;
        return;
    }
	
	if(useImageSequence)
		SHelper::changeFrameNumber(fileName, frame, padding);
	
	if(m_files.find(fileName) != m_files.end()) {
	    MGlobal::displayInfo(MString("heather loader reuses image ")+fileName.c_str());
	    dst->_img = m_files[fileName];
	}
	else {	
        MGlobal::displayInfo(MString("heather loader loads image ")+fileName.c_str());
        ZEXRImage * exr = new ZEXRImage(fileName.c_str(), false);
        
        if(!exr->isOpened()) {
            //delete exr;
            MGlobal::displayInfo(MString("cannot open image ") + fileName.c_str());
            dst->_isValid = 0;
            return;
        }
    
        if(!exr->isRGBAZ()) {
            //delete exr;
            MGlobal::displayWarning(MString("image is not RGBAZ format.") + fileName.c_str());
            dst->_isValid = 0;
            return;
        }
        
        m_files[exr->fileName()] = exr;
        dst->_img = exr;
        std::cout<<" "<<dst->_img->getWidth()<<","<<dst->_img->getHeight()<<"\n";
    }
    dst->_isValid = 1;
}
//~: