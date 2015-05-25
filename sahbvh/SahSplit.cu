#include "SahInterface.h"

#include "SahSplit.cuh"

namespace sahsplit {
int doSplitWorks(void * q, int * qelements,
                    int2 * nodes,
                    Aabb * nodeAabbs,
                    int * nodeParents,
                    int * nodeLevels,
                    KeyValuePair * primitiveIndirections,
                    Aabb * primitiveAabbs,
                    KeyValuePair * intermediateIndirections,
                    uint numPrimitives)
{
    simpleQueue::SimpleQueue * queue = (simpleQueue::SimpleQueue *)q;
    simpleQueue::init_kernel<<< 1,32 >>>(queue, qelements);
    
    DataInterface data;
    data.nodes = nodes;
    data.nodeAabbs = nodeAabbs;
    data.nodeParents = nodeParents;
    data.nodeLevels = nodeLevels;
    data.primitiveIndirections = primitiveIndirections;
    data.primitiveAabbs = primitiveAabbs;
    data.intermediateIndirections = intermediateIndirections;
    
    SplitTask task;
    
    const int tpb = 256;
    dim3 block(tpb, 1, 1);
    const unsigned nblk = 1024;
    dim3 grid(nblk, 1, 1);
    
    int lpb = 1 + numPrimitives>>10;
    
    work_kernel<simpleQueue::SimpleQueue, SplitTask, DataInterface, 24, 8, 256><<<grid, block, 16320>>>(queue,
                                task,
                                data,
                                lpb,
                                numPrimitives-1);
                                
    simpleQueue::SimpleQueue result;
    cudaError_t err = cudaMemcpy(&result, queue, SIZE_OF_SIMPLEQUEUE, cudaMemcpyDeviceToHost); 
    if (err != cudaSuccess) {
        printf(" cu error %s when retrieving task queue result\n", cudaGetErrorString(err));
    }
    
    //printf("q out tail %i\n", result._qouttail);
    return result._qouttail;
}
}
