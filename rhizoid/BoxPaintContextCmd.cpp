/*
 *  BoxPaintContextCmd.cpp
 *  proxyPaint
 *
 *  Created by jian zhang on 2/1/16.
 *  Copyright 2016 __MyCompanyName__. All rights reserved.
 *
 */

#include "BoxPaintContextCmd.h"
#include "boxPaintTool.h"
#define kMinMarginFlag "-mng"
#define kMinMarginFlagLong "-minMargin"
#define kMaxMarginFlag "-mxg"
#define kMaxMarginFlagLong "-maxMargin"
#define kOptFlag "-opt" 
#define kOptFlagLong "-option"
#define kLsegFlag "-brd" 
#define kLsegFlagLong "-brushRadius"
#define kMinFlag "-smn" 
#define kMinFlagLong "-scaleMin"
#define kMaxFlag "-smx" 
#define kMaxFlagLong "-scaleMax"
#define kRotateNoiseFlag "-rno" 
#define kRotateNoiseFlagLong "-rotateNoise"
#define kWeightFlag "-bwt" 
#define kWeightFlagLong "-brushWeight"
#define kNormalFlag "-anl" 
#define kNormalFlagLong "-alongNormal"
#define kWriteCacheFlag "-wch" 
#define kWriteCacheFlagLong "-writeCache"
#define kReadCacheFlag "-rch" 
#define kReadCacheFlagLong "-readCache"
#define kMultiCreateFlag "-mct" 
#define kMultiCreateFlagLong "-multiCreate"
#define kInstanceGroupCountFlag "-igc" 
#define kInstanceGroupCountFlagLong "-instanceGroupCount"
#define kPlantTypeFlag "-pty"
#define kPlantTypeFlagLong "-plantType"

proxyPaintContextCmd::proxyPaintContextCmd() {}

MPxContext* proxyPaintContextCmd::makeObj()
{
	fContext = new proxyPaintContext();
	return fContext;
}

void* proxyPaintContextCmd::creator()
{
	return new proxyPaintContextCmd;
}

MStatus proxyPaintContextCmd::doEditFlags()
{
	MStatus status = MS::kSuccess;
	
	MArgParser argData = parser();
	
	if (argData.isFlagSet(kOptFlag)) 
	{
		unsigned mode;
		status = argData.getFlagArgument(kOptFlag, 0, mode);
		if (!status) {
			status.perror("mode flag parsing failed.");
			return status;
		}
		fContext->setOperation(mode);
	}
	
	if (argData.isFlagSet(kLsegFlag)) 
	{
		double lseg;
		status = argData.getFlagArgument(kLsegFlag, 0, lseg);
		if (!status) {
			status.perror("lseg flag parsing failed.");
			return status;
		}
		fContext->setBrushRadius(lseg);
	}
	
	if (argData.isFlagSet(kWeightFlag)) 
	{
		double wei;
		status = argData.getFlagArgument(kWeightFlag, 0, wei);
		if (!status) {
			status.perror("lseg flag parsing failed.");
			return status;
		}
		fContext->setBrushWeight(wei);
	}
	
	if (argData.isFlagSet(kMinFlag)) 
	{
		double noi;
		status = argData.getFlagArgument(kMinFlag, 0, noi);
		if (!status) {
			status.perror("scale min flag parsing failed.");
			return status;
		}
		fContext->setScaleMin(noi);
	}
	
	if (argData.isFlagSet(kMaxFlag)) 
	{
		double noi;
		status = argData.getFlagArgument(kMaxFlag, 0, noi);
		if (!status) {
			status.perror("scale max flag parsing failed.");
			return status;
		}
		fContext->setScaleMax(noi);
	}
	
	if (argData.isFlagSet(kRotateNoiseFlag)) 
	{
		double noi;
		status = argData.getFlagArgument(kRotateNoiseFlag, 0, noi);
		if (!status) {
			status.perror("rotate noise flag parsing failed.");
			return status;
		}
		fContext->setRotationNoise(noi);
	}
	
	if (argData.isFlagSet(kNormalFlag)) 
	{
		unsigned aln;
		status = argData.getFlagArgument(kNormalFlag, 0, aln);
		if (!status) {
			status.perror("normal flag parsing failed.");
			return status;
		}
		fContext->setGrowAlongNormal(aln);
	}
	
	if (argData.isFlagSet(kWriteCacheFlag)) 
	{
		MString ch;
		status = argData.getFlagArgument(kWriteCacheFlag, 0, ch);
		if (!status) {
			status.perror("cache out flag parsing failed.");
			return status;
		}
		fContext->setWriteCache(ch);
	}
	
	if (argData.isFlagSet(kReadCacheFlag)) 
	{
		MString ch;
		status = argData.getFlagArgument(kReadCacheFlag, 0, ch);
		if (!status) {
			status.perror("cache in flag parsing failed.");
			return status;
		}
		fContext->setReadCache(ch);
	}
	
	if (argData.isFlagSet(kMultiCreateFlag)) {
		unsigned mcr;
		status = argData.getFlagArgument(kMultiCreateFlag, 0, mcr);
		if (!status) {
			status.perror("multi create flag parsing failed.");
			return status;
		}
		fContext->setMultiCreate(mcr);
	}
	
	if (argData.isFlagSet(kInstanceGroupCountFlag)) {
		unsigned igc;
		status = argData.getFlagArgument(kInstanceGroupCountFlag, 0, igc);
		if (!status) {
			status.perror("instance group count flag parsing failed.");
			return status;
		}
		fContext->setInstanceGroupCount(igc);
	}
	
	if (argData.isFlagSet(kMinMarginFlag)) {
		double margin = 1.0;
		if (argData.getFlagArgument(kMinMarginFlag, 0, margin) )
			fContext->setMinCreateMargin(margin);
	}
    
    if (argData.isFlagSet(kMaxMarginFlag)) {
		double margin = 1.0;
		if (argData.getFlagArgument(kMaxMarginFlag, 0, margin) )
			fContext->setMaxCreateMargin(margin);
	}
	
	if (argData.isFlagSet(kPlantTypeFlag)) {
		int pty = 0;
		if (argData.getFlagArgument(kPlantTypeFlag, 0, pty) )
			fContext->setPlantType(pty);
	}

	return MS::kSuccess;
}

MStatus proxyPaintContextCmd::doQueryFlags()
{
	MArgParser argData = parser();
	
	if (argData.isFlagSet(kOptFlag)) {
		setResult((int)fContext->getOperation());
	}
	
	if (argData.isFlagSet(kLsegFlag)) {
		setResult((float)fContext->getBrushRadius());
	}
	
	if (argData.isFlagSet(kWeightFlag)) {
		setResult((float)fContext->getBrushWeight());
	}
	
	if (argData.isFlagSet(kMinFlag)) {
		setResult((float)fContext->getScaleMin());
	}
	
	if (argData.isFlagSet(kMaxFlag)) {
		setResult((float)fContext->getScaleMax());
	}
	
	if (argData.isFlagSet(kRotateNoiseFlag)) {
		setResult((float)fContext->getRotationNoise());
	}
	
	if (argData.isFlagSet(kNormalFlag)) {
		setResult((int)fContext->getGrowAlongNormal());
	}
	
	if (argData.isFlagSet(kMultiCreateFlag)) {
		setResult((int)fContext->getMultiCreate());
	}
	
	if (argData.isFlagSet(kInstanceGroupCountFlag)) {
		setResult((int)fContext->getInstanceGroupCount());
	}
	
	if (argData.isFlagSet(kMinMarginFlag))
		setResult((float)fContext->minCreateMargin() );
        
    if (argData.isFlagSet(kMaxMarginFlag))
		setResult((float)fContext->maxCreateMargin() );
	
	if (argData.isFlagSet(kPlantTypeFlag))
		setResult(fContext->plantType() );
	
	return MS::kSuccess;
}

MStatus proxyPaintContextCmd::appendSyntax()
{
	MSyntax mySyntax = syntax();
	
	MStatus stat;
	stat = mySyntax.addFlag(kOptFlag, kOptFlagLong, MSyntax::kUnsigned);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add option arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kLsegFlag, kLsegFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add radius arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kWeightFlag, kWeightFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add weight arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kMinFlag, kMinFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add min arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kMaxFlag, kMaxFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add max arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kNormalFlag, kNormalFlagLong, MSyntax::kUnsigned);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add normal arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kWriteCacheFlag, kWriteCacheFlagLong, MSyntax::kString);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add cache out arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kReadCacheFlag, kReadCacheFlagLong, MSyntax::kString);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add cache in arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kRotateNoiseFlag, kRotateNoiseFlagLong, MSyntax::kDouble);
	if(!stat)
	{
		MGlobal::displayInfo("failed to add rotate noise arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kMultiCreateFlag, kMultiCreateFlagLong, MSyntax::kUnsigned);
	if(!stat) {
		MGlobal::displayInfo("failed to add multi create arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kInstanceGroupCountFlag, kInstanceGroupCountFlagLong, MSyntax::kUnsigned);
	if(!stat) {
		MGlobal::displayInfo("failed to add instance group count arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kMinMarginFlag, kMinMarginFlagLong, MSyntax::kDouble);
	if(!stat) {
		MGlobal::displayInfo("failed to add min margin arg");
		return MS::kFailure;
	}
    
    stat = mySyntax.addFlag(kMaxMarginFlag, kMaxMarginFlagLong, MSyntax::kDouble);
	if(!stat) {
		MGlobal::displayInfo("failed to add max margin arg");
		return MS::kFailure;
	}
	
	stat = mySyntax.addFlag(kPlantTypeFlag, kPlantTypeFlagLong, MSyntax::kLong);
	if(!stat) {
		MGlobal::displayInfo("failed to add plantType arg");
		return MS::kFailure;
	}
	
	return stat;
}