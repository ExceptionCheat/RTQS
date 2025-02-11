
#ifndef MY_OTHER_SOURCE_H
#define MY_OTHER_SOURCE_H

#include "ns3/address.h"
#include "ns3/application.h"
#include "ns3/event-id.h"
#include "ns3/ptr.h"
#include "ns3/internet-module.h"
#include "ns3/data-rate.h"
#include "ns3/traced-callback.h"
#include "ns3/seq-ts-size-header.h"


#include "MySourceIDTag.h"
#include "MySourceIDSendTime.h"
#include "MySourceIDType.h"
#include "ns3/core-module.h"

using namespace ns3;

class MyOtherSource : public Application 
{
public:
  static TypeId GetTypeId (void);

  MyOtherSource ();
  virtual ~MyOtherSource();

  // Default OnOffApp creates socket until app start time, can't access and configure the tracing externally
  void Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate, uint32_t appid, bool poisson);

  uint32_t GetPacketsSent();

private:
  virtual void StartApplication (void);
  virtual void StopApplication (void);

  void ScheduleTx (void);
  void SendPacket (void);

  Ptr<Socket>     m_socket;
  Address         m_peer;
  uint32_t        m_packetSize;
  DataRate        m_dataRate;
  EventId         m_sendEvent;
  bool            m_running;
  uint32_t        m_packetsSent;

  uint32_t        m_sourceid;

  Ptr<ExponentialRandomVariable> m_var;
};


#endif