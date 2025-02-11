#include "my_source_real_time.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("RealTimeApplicantion");
NS_OBJECT_ENSURE_REGISTERED (MyRealTimeSource);
TypeId MyRealTimeSource::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RealTimeApplicantion")
    .SetParent<Application> ()
    .SetGroupName ("Applicantions")
    .AddConstructor<MyRealTimeSource> ()
    .AddAttribute ("socket",
                  "current socket to send packet",
                  PointerValue(nullptr),
                  MakePointerAccessor (&MyRealTimeSource::m_socket),
                  MakePointerChecker<Socket>())
    .AddAttribute ("target_address",
                  "The Ip address this applicantion send packet to",
                  AddressValue(InetSocketAddress("0.0.0.0",0)),
                  MakeAddressAccessor (&MyRealTimeSource::m_peer),
                  MakeAddressChecker())
    .AddAttribute ("packet_size",
                  "The packet size for each packet send",
                  UintegerValue(1500),
                  MakeUintegerAccessor (&MyRealTimeSource::m_packetSize),
                  MakeUintegerChecker<uint32_t>())
    .AddAttribute ("dataRate",
                  "The send packet rate of this Applicantion",
                  DataRateValue(DataRate("5Mbps")),
                  MakeDataRateAccessor (&MyRealTimeSource::m_dataRate),
                  MakeDataRateChecker())
    .AddAttribute ("source_id",
                  "The source_id of this Applicantion",
                  UintegerValue(0),
                  MakeUintegerAccessor (&MyRealTimeSource::m_sourceid),
                  MakeUintegerChecker<uint32_t>())
  ;
  return tid;
}

MyRealTimeSource::MyRealTimeSource ()
  : m_socket (0), 
    m_peer (), 
    m_packetSize (0), 
    m_dataRate (0), 
    m_sendEvent (), 
    m_running (false), 
    m_packetsSent (0)
{
}

MyRealTimeSource::~MyRealTimeSource()
{
  m_socket = 0;
}

void
MyRealTimeSource::Setup (Ptr<Socket> socket, Address address, uint32_t packetSize, DataRate dataRate, uint32_t sourceid, bool poisson)
{
  m_socket = socket;
  m_peer = address;
  m_packetSize = packetSize;
  m_dataRate = dataRate;
  m_sourceid = sourceid;
}

void
MyRealTimeSource::StartApplication (void)
{
  m_running = true;
  m_packetsSent = 0;
  m_socket->Bind ();
  m_socket->Connect (m_peer);
  SendPacket ();
}

void 
MyRealTimeSource::StopApplication (void)
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
MyRealTimeSource::SendPacket (void)
{
  Ptr<Packet> packet = Create<Packet> (m_packetSize);

  //发送方塞的tag，接收方的ack里面没有tag
  //tag1:sourceid
  MySourceIDTag tag;
  tag.Set (m_sourceid);
  packet->AddByteTag (tag);
  
  //tag2:sendTime
  // MySourceIDSendTime tag_send_time;
  // tag_send_time.MySetTime(Simulator::Now());
  // packet->AddByteTag(tag_send_time);

  //tag3:tag_per_hop_delay
  // MySourceIDPerHopDelay tag_per_hop_delay;
  // tag_per_hop_delay.MySetTime(Simulator::Now());//初始值
  // packet->AddByteTag(tag_per_hop_delay);

  //tag3：tag_type
  MySourceIDType tag_type;
  tag_type.Set(1);
  packet->AddByteTag(tag_type);

  //包发出
  m_socket->Send (packet);
  
  ++m_packetsSent;
  ScheduleTx ();
}

void 
MyRealTimeSource::ScheduleTx (void)
{
  if (m_running)
    {
      Time tNext (Seconds (m_packetSize * 8 / static_cast<double> (m_dataRate.GetBitRate ())));
      m_sendEvent = Simulator::Schedule (tNext, &MyRealTimeSource::SendPacket, this);
    }
}

uint32_t MyRealTimeSource::GetPacketsSent (void) {
  return m_packetsSent;
}