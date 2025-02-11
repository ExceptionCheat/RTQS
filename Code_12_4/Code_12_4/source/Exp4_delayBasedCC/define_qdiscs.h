#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/internet-module.h"
#include "ns3/traffic-control-module.h"
#include "ns3/queue-disc-container.h"
#include "ns3/packet-sink.h"
#include "ns3/applications-module.h"
#include "ns3/callback.h"
using namespace ns3;
QueueDiscContainer qdiscs;//TODO ：继承关系