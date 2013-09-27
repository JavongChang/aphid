/*
 *  HDataset.cpp
 *  helloHdf
 *
 *  Created by jian zhang on 6/13/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include "HDataset.h"
#include <iostream>
HDataset::HDataset(const std::string & path)
{
	fObjectPath = ValidPathName(path);
}

char HDataset::validate()
{
	if(!exists())
		return 0;
		
	if(!verifyDataSpace())
		return 0;
		
	return 1;
}

char HDataset::create(int dimx, int dimy)
{
	fDimension[0] = dimx;
	fDimension[1] = dimy;
	fDimension[2] = 0;
	
	return raw_create();
}

char HDataset::raw_create()
{
	if(validate())
		return 1;
	else if(exists())
		return 0;
		
	createDataSpace();
				
	fObjectId = H5Dcreate(FileIO.fFileId, fObjectPath.c_str(), dataType(), fDataSpace, 
                          H5P_DEFAULT, H5P_DEFAULT, H5P_DEFAULT);
						  
	if(fObjectId < 0)
		return 0;
	return 1;
}

char HDataset::open()
{
	fObjectId = H5Dopen(FileIO.fFileId, fObjectPath.c_str(), H5P_DEFAULT);
	
	if(fObjectId < 0)
		return 0;
		
	fDataSpace = H5Dget_space(fObjectId);

	if(fDataSpace<0)
		return 0;
		
	return 1;
}

void HDataset::close()
{
	H5Sclose(fDataSpace);
	H5Dclose(fObjectId);
}

int HDataset::objectType() const
{
	return H5G_DATASET;
}

hid_t HDataset::dataType()
{
	return H5T_NATIVE_FLOAT;
}

void HDataset::createDataSpace()
{
	hsize_t     dims[3];
	dims[0] = fDimension[0];
	dims[1] = fDimension[1];
	dims[2] = fDimension[2];
	
	int ndim = dataSpaceNumDimensions();
		
	fDataSpace = H5Screate_simple(ndim, dims, NULL);
}

char HDataset::verifyDataSpace()
{	
	open();
		
	char res = dimensionMatched();
		
	close();
	return res;
}

char HDataset::dimensionMatched()
{
	int rank = H5Sget_simple_extent_ndims(fDataSpace);
	
	if(rank != dataSpaceNumDimensions()){
		FileIO.fCurrentError = HDocument::eLackOfDataSpaceDimension;
		return 0;
	}
	
	hsize_t     dims_out[3];
	H5Sget_simple_extent_dims(fDataSpace, dims_out, NULL);
	
	for(int i=0; i<rank; i++) {
		if(dims_out[i] != fDimension[i]) {
			FileIO.fCurrentError = HDocument::eDataSpaceDimensionMissMatch;
			return 0;
		}
	}
	return 1;
}

char HDataset::write()
{
	float * data = new float[fDimension[0]*fDimension[1]];
    int         i, j;
   for(j = 0; j < fDimension[0]; j++)
	for(i = 0; i < fDimension[1]; i++)
	    data[j*fDimension[1] +i] = i + j;
		
	write(data);
	
	delete[] data;
	return 1;
}

char HDataset::read()
{
	float *         data = new float[fDimension[0]*fDimension[1]];
	read(data);
	
	int i, j;				
	for(j = 0; j < fDimension[0]; j++) {
	for(i = 0; i < fDimension[1]; i++) {
		std::cout<<" "<<data[j*fDimension[1]+i];
	}
		std::cout<<" \n";
	}
	
	delete[] data;
	return 1;
}

char HDataset::write(float *data)
{
	herr_t status = H5Dwrite(fObjectId, dataType(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
	if(status < 0)
		return 0;
	return 1;
}

char HDataset::read(float *data)
{
	herr_t status = H5Dread(fObjectId, dataType(), H5S_ALL, H5S_ALL, H5P_DEFAULT, data);
	if(status < 0)
		return 0;
	return 1;
}

int HDataset::dataSpaceNumDimensions() const
{
	int ndim = 1;
	if(fDimension[1] > 0)
		ndim = 2;
	if(fDimension[2] > 0)
		ndim = 3;
		
	return ndim;
}

void HDataset::dataSpaceDimensions(int dim[3]) const
{
    hsize_t dims_out[3];
	H5Sget_simple_extent_dims(fDataSpace, dims_out, NULL);
	dim[0] = dims_out[0];
	dim[1] = dims_out[1];
	dim[2] = dims_out[2];
}
