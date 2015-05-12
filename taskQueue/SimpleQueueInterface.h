#ifndef SIMPLEQUEUEINTERFACE_H
#define SIMPLEQUEUEINTERFACE_H

#include "bvh_common.h"
#define SIZE_OF_SIMPLEQUEUEINTERFACE 32
struct SimpleQueueInterface {
    uint tbid;
    uint numNodes;
    uint maxNumWorks;
    uint lock;
    uint workDone;
    uint workBlock;
    uint padding[2];
};
#endif        //  #ifndef SIMPLEQUEUEINTERFACE_H

