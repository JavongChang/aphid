#ifndef QUICKSORT_CUH
#define QUICKSORT_CUH

#include <cuda_runtime_api.h>
#include "SimpleQueue.cuh"

__device__ void quickSort_redistribute(uint * data,
                            int2 range,
                            int & headToSecond)
{
    int low = range.x;
    int high = range.y;
    headToSecond = (low + high)/2;
    
    if(low > high) return;
    
    uint intermediate;
    uint separator = data[(low + high)/2];
    for(;;) {
        while(data[low]<separator) low++;
        while(data[high]>separator) high--;
            
        if(low <= high) {
            intermediate = data[low];
            data[low++] = data[high];
            data[high--] = intermediate;
        }
        
        if(low > high) break;
    }
    
    headToSecond = low;
}

__global__ void quickSort_checkQ_kernel(uint * maxN,
                        simpleQueue::SimpleQueue * q,
                        SimpleQueueInterface * qi,
                        uint * idata,
                        int2 * nodes)
{
    __shared__ int sWorkPerBlock[128];
    int2 root;
    int headToSecond, spawn;
    int i=0;
    for(;i<19;i++) {
        if(blockIdx.x < 1 && threadIdx.x < 1) {
            if(i<1) {
                q->init(qi);
                q->enqueue();
                i++;
            }
        }
        
        if(threadIdx.x < 1) {
            sWorkPerBlock[0] = 1;
            if(q->maxNumWorks() < 1)
                sWorkPerBlock[0] = 0;
        }
        __syncthreads();
        
        if(sWorkPerBlock[0] < 1) continue;

        if(threadIdx.x < 1) {
            sWorkPerBlock[0] = q->dequeue();
            
            if(sWorkPerBlock[0] > -1) {
                root = nodes[sWorkPerBlock[0]];
                quickSort_redistribute(idata,
                            root,
                            headToSecond);
                    
                    if(root.x +1 < root.y) 
                    {
                      if(root.x < headToSecond - 1) {
                        spawn = q->enqueue();
                        nodes[spawn].x = root.x;
                        nodes[spawn].y = headToSecond - 1;
                      }
                        
                      if(headToSecond < root.y) {
                        spawn = q->enqueue();
                        nodes[spawn].x = headToSecond;
                        nodes[spawn].y = root.y;
                      }
                    }
                    
                    q->setWorkDone();
                    qi->workBlock = i;
            }
        }
        __syncthreads();
        
        if(sWorkPerBlock[0] < -1) break;
        
        // if(q->workDoneCount() > 2086) break;
    }
    
    if(blockIdx.x < 1 && threadIdx.x < 1) {
        qi->tail = i;
        maxN[0] = q->workDoneCount();
    }
}
/*
__global__ void quickSort_kernel(int * obin,
                    int * idata,
                    int h,
                    int n)
{
    unsigned ind = blockIdx.x*blockDim.x + threadIdx.x;
	if(ind >= n) return;
	
	int d = idata[ind];
	
    atomicAdd(&obin[d/h], 1);
}
*/

#endif        //  #ifndef QUICKSORT_CUH

