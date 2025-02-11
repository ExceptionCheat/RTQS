#include "my_source_no_real_time.h"
using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("NotRealTimeApplicantion");
NS_OBJECT_ENSURE_REGISTERED (MyNoRealTimeSource);
TypeId MyNoRealTimeSource::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::NotRealTimeApplicantion")
    .SetParent<Application> ()
    .SetGroupName ("Applicantions")
    .AddConstructor<MyNoRealTimeSource> ()
    .AddAttribute ("socket",
                  "current socket to send packet",
                  PointerValue(nullptr),
                  MakePointerAccessor (&MyNoRealTimeSource::m_socket),
                  MakePointerChecker<Socket>())
    .AddAttribute ("target_address",
                  "The Ip address this applicantion send packet to",
                  AddressValue(InetSocketAddress("0.0.0.0",0)),
                  MakeAddressAccessor (&MyNoRealTimeSource::m_peer),
                  MakeAddressChecker())
    .AddAttribute ("packet_size",
                  "The packet size for each packet send",
                  UintegerValue(1500),
                  MakeUintegerAccessor (&MyNoRealTimeSource::m_packetSize),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute ("dataRate",
                  "The send packet rate of this Applicantion",
                  DataRateValue(DataRate("5Mbps")),
                  MakeDataRateAccessor (&MyNoRealTimeSource::m_dataRate),
                  MakeDataRateChecker())
    .AddAttribute ("source_id",
                  "The source_id of this Applicantion",
                  UintegerValue(0),
                  MakeUintegerAccessor (&MyNoRealTimeSource::m_sourceid),
                  MakeUintegerChecker<uint32_t>())
  ;
  return tid;
}

MyNoRealTimeSource::MyNoRealTimeSource ()
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0)
{
}

MyNoRealTimeSource::~MyNoRealTimeSource()
{
  m_socket = 0;
}

void
MyNoRealTimeSource::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate, uint32_t sourceid, bool poisson)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
  m_sourceid = sourceid;

  m_var = CreateObject<ExponentialRandomVariable> ();
  // No need for poisson as the demand is infinite anyway and the packet gap pattern is determined by the underlying transport
}

void
MyNoRealTimeSource::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void 
MyNoRealTimeSource::StopApplication (void)
{
  m_running = false;

  if (m_sendEvent.IsRunning ())
    {
      Simulator::Cancel (m_sendEvent);
    }

  if (m_socket)
    {
      m_socket->Close ();
    }
}

void 
MyNoRealTimeSource::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);
  //发送方塞的tag，接收方的ack里面没有tag

  //tag1:sourceid
  MySourceIDTag tag;
  tag.Set (m_sourceid);
  packet->AddByteTag (tag);
  
  //tag2:tag_type
  MySourceIDType tag_type;
  tag_type.Set(2);
  packet->AddByteTag(tag_type);

  //包发出
  m_socket->Send (packet);

  ++m_packetsSent;
  ScheduleTx ();
}

void 
MyNoRealTimeSource::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyNoRealTimeSource::SendPacket, this);
    }
}

uint32_t MyNoRealTimeSource::GetPacketsSent (void) {
  return m_packetsSent;
}
