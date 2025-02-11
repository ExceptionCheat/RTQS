
#ifndef DUMBBELL_RACK
#define DUMBBELL_RACK

#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/queue-disc-container.h"
#include "ns3/packet-sink.h"
#include "ns3/applications-module.h"
#include "ns3/callback.h"

#include "my_source_no_real_time.h"
#include "my_source_real_time.h"
#include "my_source_other.h"
#include "rtqs-queue-disc.h"
#include "cebinae-queue-disc.h"
using namespace ns3;

// Global static methods, called in the rtqs-queue-disc class, are implemented in Exp1_symmetryNetwork.
void doUpdateDequeueThroughput(uint32_t,uint32_t,int32_t);
void doUpdateEnqueueThroughput(uint32_t,uint32_t,int32_t);
void doUpdateIfMaxFlowReject(uint32_t);
void doUpdateMaxFlowRejectTimes(uint32_t);
void doUpdateFlowStatus(uint32_t);
void doQueueLengthIncrease(uint32_t);
#endif