#ifndef CUDABUFFER_H
#define CUDABUFFER_H

/*
 *  CUDABuffer.h
 *  brdf
 *
 *  Created by jian zhang on 9/30/12.
 *  Copyright 2012 __MyCompanyName__. All rights reserved.
 *
 */

#include <BaseBuffer.h>
class CUDABuffer : public BaseBuffer {
public:
	CUDABuffer();
	virtual ~CUDABuffer();
	
	virtual void create(float * data, unsigned size);
	virtual void destroy();
	void create(unsigned size);
	
	struct cudaGraphicsResource ** resource();
	void * bufferOnDevice();
	void * bufferOnDeviceAt(unsigned loc);
	void hostToDevice(void * src, unsigned size);
	void deviceToHost(void * dst, unsigned size);
	void hostToDevice(void * src);
	void deviceToHost(void * dst);
	void hostToDevice(void * src, unsigned loc, unsigned size);
	void deviceToHost(void * dst, unsigned loc, unsigned size);
	void map(void ** p);
	void unmap();
	
private:
    const unsigned minimalMemSize(unsigned size) const;
	
private:
	struct cudaGraphicsResource *_cuda_vbo_resource;
	void *_device_vbo_buffer;
	unsigned m_realSize;
};

#endif        //  #ifndef CUDABUFFER_H

