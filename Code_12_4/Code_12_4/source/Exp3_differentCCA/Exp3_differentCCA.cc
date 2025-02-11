#include <chrono>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <math.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include <unordered_map>
#include <random>

#include "Exp3_differentCCA.h"

using namespace ns3;

NS_LOG_COMPONENT_DEFINE ("ScratchSimulator");

QueueDiscContainer qdiscs;
std::vector<std::uint64_t> router_byte_count;
std::vector<std::uint64_t> packet_sink_byte_count;
std::vector<double> source_rtt;
std::string queuedisc_type;
std::unordered_map<std::string,uint32_t> app_type_to_type_id = {{"ns3::RealTimeApplicantion",1},{"ns3::NotRealTimeApplicantion",2},{"ns3::OtherApplicantion",3}};
Time last_write_log_time=Seconds(0);

double default_max_flow_reject_function(uint32_t now_t){
  switch (now_t)
  {
    case 0: return 2;
    case 1: return 1;
    case 2: return 0.5;
    default: return 0.25;
  }
}

static void packetSinkReceivePacket(Ptr<const Packet> packet, const Address& from, const Address& local){
  MySourceIDTag source_id_tag;
  if (packet->FindFirstMatchingByteTag(source_id_tag)){
    packet_sink_byte_count[source_id_tag.Get()]+=packet->GetSize();
  }
}

static void addByteTagForACK(uint32_t source_id,uint32_t type_id,Ptr<const Packet> packet){
  // tag1: source_id
  MySourceIDTag source_id_tag;
  source_id_tag.Set(source_id);
  packet->AddByteTag(source_id_tag);

  // tag2: the time of this packet sent
  MySourceIDSendTime send_time_tag;
  send_time_tag.MySetTime(Simulator::Now());
  packet->AddByteTag(send_time_tag);

  // tag3:type id for current stream
  MySourceIDType type_id_tag;
  type_id_tag.Set(type_id);
  packet->AddByteTag(type_id_tag);
}

static void addByteTagForSource(Ptr<const Packet> packet){
  // tag1: the time of this packet sent
  // If we add in Application, it is the time when this packet move from applicantion layer to transport layer,
  // which cannot express the time of transport delay.
  MySourceIDSendTime send_time_tag;
  send_time_tag.MySetTime(Simulator::Now());
  packet->AddByteTag(send_time_tag);
}

static void updateRtt(uint32_t source_id,Time old_rtt,Time new_rtt){
  source_rtt[source_id]=new_rtt.GetMicroSeconds();
}

static void routerSendPacket(Ptr<const Packet> packet){
  MySourceIDTag source_id_tag;
  if (packet->FindFirstMatchingByteTag(source_id_tag)){
    router_byte_count[source_id_tag.Get()] += packet->GetSize();
  }
}

static void routerReceiveACK (Ptr<const Packet> p){
  // When the router receives the ACK, it needs to update the qdisc with valid information: ADD, downMPD.
  MySourceIDTag source_id_tag;
  p->FindFirstMatchingByteTag(source_id_tag);

  MySourceIDSendTime send_time_tag;
  p->FindFirstMatchingByteTag(send_time_tag);
  Time curADD(Simulator::Now()-send_time_tag.MyGetTime());

  MySourceIDType type_id_tag;
  p->FindFirstMatchingByteTag(type_id_tag);

  // Now we should find the LAST matching Byte Tag, but not the first one.
  // Because if current MDP is smaller than the value in Packet, we will add a new Tag but do not delete the one in the Packet
  MySourceIDMPD MPD_tag;
  bool flag=false;
  ByteTagIterator byte_tag_it=p->GetByteTagIterator();
  TypeId MPD_tag_tid=MPD_tag.GetInstanceTypeId();
  while (byte_tag_it.HasNext()){
    ByteTagIterator::Item cur_byte_tag_item = byte_tag_it.Next ();
    if (MPD_tag_tid == cur_byte_tag_item.GetTypeId ()){
      cur_byte_tag_item.GetTag (MPD_tag);
      flag=true;
    }
  }
  
  // Update ADD and downMPD!
  qdiscs.Get(0)->SetAttribute("source_id_for_update_ADD_and_MPD",UintegerValue(source_id_tag.Get()));
  qdiscs.Get(0)->SetAttribute("type_id_for_update_ADD_and_MPD",UintegerValue(type_id_tag.Get()));
  qdiscs.Get(0)->SetAttribute("ADD_value",TimeValue(curADD));
  if (flag) qdiscs.Get(0)->SetAttribute("MPD_value",TimeValue(MPD_tag.MyGetTime()));
  else qdiscs.Get(0)->SetAttribute("MPD_value",TimeValue(Time(0)));
  qdiscs.Get(0)->SetAttribute("update_ADD_and_MPD",TimeValue(Time(10086)));

  // Get current queueing time.
  TimeValue cur_queueing_time_record;
  qdiscs.Get(0)->GetAttribute("update_ADD_and_MPD",cur_queueing_time_record);
  Time cur_queueing_time=cur_queueing_time_record.Get();

  if(flag){
    if (MPD_tag.MyGetTime()<cur_queueing_time){
      MPD_tag.MySetTime(cur_queueing_time);
      p->AddByteTag(MPD_tag);
    }
  }else{
    MPD_tag.MySetTime(cur_queueing_time);
    p->AddByteTag(MPD_tag);
  }
}

void doUpdateDequeueThroughput(uint32_t source_id,uint32_t queue_id,int32_t update_num){
  qdiscs.Get(0)->SetAttribute("source_id_for_update_dequeue_throughput",UintegerValue(source_id));
  qdiscs.Get(0)->SetAttribute("queue_id_for_update_dequeue_throughput",UintegerValue(queue_id));
  qdiscs.Get(0)->SetAttribute("update_num_for_update_dequeue_throughput",IntegerValue(update_num));
  qdiscs.Get(0)->SetAttribute("update_dequeue_throughput",UintegerValue(10086));
}

void doUpdateEnqueueThroughput(uint32_t source_id,uint32_t queue_id,int32_t update_num){
  qdiscs.Get(0)->SetAttribute("source_id_for_update_enqueue_throughput",UintegerValue(source_id));
  qdiscs.Get(0)->SetAttribute("queue_id_for_update_enqueue_throughput",UintegerValue(queue_id));
  qdiscs.Get(0)->SetAttribute("update_num_for_update_enqueue_throughput",IntegerValue(update_num));
  qdiscs.Get(0)->SetAttribute("update_enqueue_throughput",UintegerValue(10086));
}

void doUpdateIfMaxFlowReject(uint32_t source_id){
  qdiscs.Get(0)->SetAttribute("source_id_for_update_if_max_flow_reject",UintegerValue(source_id));
  qdiscs.Get(0)->SetAttribute("update_if_max_flow_reject",UintegerValue(10086));
}

void doUpdateMaxFlowRejectTimes(uint32_t source_id){
  qdiscs.Get(0)->SetAttribute("source_id_for_update_max_flow_reject_times",UintegerValue(source_id));
  qdiscs.Get(0)->SetAttribute("update_max_flow_reject_times",UintegerValue(10086));
}

void doUpdateFlowStatus(uint32_t source_id){
  qdiscs.Get(0)->SetAttribute("source_id_for_update_flow_status",UintegerValue(source_id));
  qdiscs.Get(0)->SetAttribute("update_flow_status",UintegerValue(10086));
}

void doQueueLengthIncrease(uint32_t queue_id){
  qdiscs.Get(0)->SetAttribute("queue_id_for_queue_len_increase",UintegerValue(queue_id));
  qdiscs.Get(0)->SetAttribute("queue_len_increase",UintegerValue(10086));
}

// Periodically write all the current information to the file under qlog_dir.
static void writeLog(std::string result_dir, std::uint32_t tracing_period_ms, std::string bottleneck_fn, std::string app_fn, std::string jfi_fn,std::string rtt_fn){
  // Write receiver throughput.
  Time cur_time = Now();
  std::ofstream bottleneck_ofs;
  bottleneck_ofs.open(result_dir+bottleneck_fn,std::ios::app|std::ios::out);
  double total_byte_count =0.0;
  for(uint32_t source_id=0;source_id<router_byte_count.size();source_id++){
    bottleneck_ofs<<std::fixed<<std::setprecision(3)<<8.0*router_byte_count[source_id]/(cur_time.GetSeconds()-last_write_log_time.GetSeconds())<<" ";
    total_byte_count+=8.0*router_byte_count[source_id]/(cur_time.GetSeconds()-last_write_log_time.GetSeconds());
  }
  bottleneck_ofs<<std::fixed<<std::setprecision(3)<<total_byte_count<<std::endl;

  // Write router throughput.
  std::ofstream app_ofs(result_dir+app_fn,std::ios::out|std::ios::app);
  total_byte_count=0.0;
  for(uint32_t source_id=0;source_id<packet_sink_byte_count.size();source_id++){
    app_ofs<<std::fixed<< std::setprecision(3)<<8.0*packet_sink_byte_count[source_id]/(cur_time.GetSeconds()-last_write_log_time.GetSeconds())<<" ";
    total_byte_count+=8.0*packet_sink_byte_count[source_id]/(cur_time.GetSeconds()-last_write_log_time.GetSeconds());
  }
  app_ofs<<std::fixed<<std::setprecision(3)<<total_byte_count<<std::endl;

  // Write JFI value.
  std::ofstream jfi_ofs(result_dir+jfi_fn,std::ios::out|std::ios::app);
    uint64_t router_byte_squares_sum=0;
  uint64_t router_byte_sum=0;
  uint64_t packet_sink_byte_squares_sum=0;
  uint64_t packet_sink_byte_sum=0;
  for(uint32_t source_id=0;source_id<router_byte_count.size();source_id++){
    router_byte_sum+=router_byte_count[source_id];
    router_byte_squares_sum+=(router_byte_count[source_id]*router_byte_count[source_id]);
    router_byte_count[source_id]=0;
  }
  for(uint32_t source_id=0;source_id<packet_sink_byte_count.size();source_id++){
    packet_sink_byte_sum+=packet_sink_byte_count[source_id];
    packet_sink_byte_squares_sum+=(packet_sink_byte_count[source_id]*packet_sink_byte_count[source_id]);
    packet_sink_byte_count[source_id]=0;
  }
  double jfi_router = static_cast<double> (router_byte_sum * router_byte_sum) / router_byte_squares_sum / router_byte_count.size();
  double jfi_packet_sink = static_cast<double> (packet_sink_byte_sum * packet_sink_byte_sum) / packet_sink_byte_squares_sum / packet_sink_byte_count.size();
  jfi_ofs << std::fixed << std::setprecision (3) << jfi_router << " " << std::fixed << std::setprecision (3) << jfi_packet_sink << std::endl;

  // Write RTT.
  std::ofstream rtt_ofs(result_dir+rtt_fn,std::ios::out|std::ios::app);
  for(uint32_t source_id=0;source_id<source_rtt.size();source_id++){
    rtt_ofs<<std::fixed<< std::setprecision(3)<<source_rtt[source_id]<<" ";
  }
  rtt_ofs<<std::fixed<<std::setprecision(3)<<std::endl;

  // Write rtqs-queue-disc record value.
  if (TypeId::LookupByName(queuedisc_type)==RtqsQueueDisc::GetTypeId()) qdiscs.Get(0)->SetAttribute("writeQLogFiles",UintegerValue(10086));

  // Periodic call.
  last_write_log_time = cur_time;
  Simulator::Schedule(MilliSeconds(tracing_period_ms), &writeLog, result_dir, tracing_period_ms, bottleneck_fn, app_fn, jfi_fn,rtt_fn);
}

// Output the current script running progress, if there is no output, you may feel that the script is dead...
void PrintProgress (Time interval)
{
  std::cout << "[PID:" << getpid() << "] Progress: " << std::fixed << std::setprecision (1) << Simulator::Now ().GetSeconds () << "[s]" << std::endl;
  Simulator::Schedule (interval, &PrintProgress, interval);
}

int main (int argc, char *argv[])
{
  NS_LOG_UNCOND ("MainRack");
  NS_LOG_COMPONENT_DEFINE("Exp3_differentCCA");
  CommandLine cmd (__FILE__);

  // Experimental correlation parameter (========================= Change me ===========================)
  double MaxFlowRejectAlpha=1.2;                                // RTQS max flow reject hyperparameter alpha.
  double MaxFlowRejectBeta=1.2;                                 // RTQS max flow reject hyperparameter beta.
  bool IFMaxFlowRejectOn=true;                                  // If enable max flow reject.
  std::string FIFO_buffer_size = "1000p";                       // Queue length, valid only for FIFO.
  queuedisc_type="ns3::RtqsQueueDisc";                          // Queuing policy of the router. Options: ns3::FifoQueueDisc, ns3::RtqsQueueDisc, ns3::CebinaeQueueDisc
  Time m_interval=Time("180ms");                                // Latency requirements for real-time streams.

  // Network topology parameter (================================= Change me ===========================)
  static const uint32_t num_leaf=4;                             // The number of leaf node in the experiment.
  std::string bw_bottleneck="15Mbps";                           //Bottleneck bandwidth
  std::string bw[num_leaf]={"5Mbps","5Mbps","5Mbps","5Mbps"};   //Link bandwidth of leaf nodes.
  uint32_t mtu_bottleneck=1500;                                 //Bottleneck link MTU.
  uint32_t mtu[num_leaf]={1500,1500,1500,1500};                 //Leaf link MTU.
  std::string delay_bottleneck="1ms";                           //Bottleneck link delay.
  std::string delay[num_leaf]={"10ms","10ms","10ms","10ms"};    //Link delay of leaf nodes.
  std::string switch_netdev_size = "100p";                      //瓶颈路由器网络设备队列长度 DropTailQueue::MaxSize (TODO)
  std::string server_netdev_size = "100p";                      //叶子结点路由器网络设备队列长度 DropTailQueue::MaxSize (TODO)

  // Application program parameter (============================== Change me ===========================)
  uint32_t app_packet_size = 1440;                              // Packet size.
  bool sack = true;                                             // If enable sack.
  std::string recovery = "ns3::TcpClassicRecovery";             // Packet loss recovery mechanism.
  uint32_t max_cumul_ACK_count = 1;                             // The maximum cumulative number of ACK packets, =1 means without cumulative ACK.
  std::string p2p_cca[num_leaf] = {"ns3::TcpCubic",             // The congestion control algorithm used by each leaf node.
                                    "ns3::TcpCubic",            // Options: ns3::TcpCubic, ns3::TcpNewReno, ns3::TcpLinuxReno, ns3::TcpBbr ...
                                    "ns3::TcpNewReno",
                                    "ns3::TcpNewReno"};
  std::string app_type[num_leaf]={"ns3::RealTimeApplicantion",  // The applicantion type for each node.
                                "ns3::NotRealTimeApplicantion", // Options: ns3::RealTimeApplicantion, ns3::NotRealTimeApplicantion, ns3::OtherApplicantion.
                                "ns3::OtherApplicantion",
                                "ns3::RealTimeApplicantion"};
  std::string app_bw[num_leaf]={"5Mbps",                        // Packet sending rate at the application layer.
                                "5Mbps",
                                "5Mbps",
                                "5Mbps"};

  // Tracing correlation parameter (============================== Change me ===========================)
  std::string qlog_dir="/home/xuzhenge/Desktop/Result_exp3/";   // Output file (qlog file) dictionary.
  uint32_t tracing_period_ms = 100;                             // QLog output interval.
  double simulator_run_seconds = 200;                           // Total simulation duration.
  double app_start_seconds = 0.1;                               // Application start time.
  double app_end_seconds = 200;                                 // Application stop time.

  // Random seed setting (======================================== Change me ===========================)
  uint32_t seed=10086;
  SeedManager::SetSeed(seed);

  // Configure command line parameters, only for "queuedisc_type", default is RtqsQueueDisc
  std::cout<<"argc:"<<argc<<std::endl;
  for (int i=1;i<argc;i++){
    if (std::string(argv[i])=="--queuedisc_type"&&(i+1)<argc){
      queuedisc_type=std::string(argv[i+1]);
    }
  }

  // Create qlog file folder.
  qlog_dir=qlog_dir+queuedisc_type+"/";
  if (mkdir(qlog_dir.c_str(),0777)==0){
    std::cout<<"Result save to: "<<qlog_dir<<std::endl;
  }
  else {
    std::cout<<"Folder "<<qlog_dir<<" already exists, create fileFolder fail!"<<std::endl;
    return -1;
  }

  NS_LOG_DEBUG("=================================== Initialize link parameters ===================================");

  PointToPointHelper p2p_bottleneck;    
  PointToPointHelper p2p_leaf[num_leaf];

  NodeContainer router;
  NodeContainer leftleaf;
  NodeContainer rightleaf;
  
  router.Create(2);
  leftleaf.Create(num_leaf);
  rightleaf.Create(num_leaf);

  // Set the parameters of a p2p link
  p2p_bottleneck.SetDeviceAttribute("DataRate",StringValue(bw_bottleneck));
  p2p_bottleneck.SetDeviceAttribute("Mtu",UintegerValue(mtu_bottleneck));
  p2p_bottleneck.SetChannelAttribute("Delay",StringValue(StringValue(delay_bottleneck)));
  for(uint32_t i=0;i<num_leaf;i++){
    p2p_leaf[i].SetDeviceAttribute("DataRate",StringValue(bw[i]));
    p2p_leaf[i].SetDeviceAttribute("Mtu",UintegerValue(mtu[i]));
    p2p_leaf[i].SetChannelAttribute("Delay",StringValue(StringValue(delay[i])));
  }
  // Set the default queue length of the link device (the queue length of the NIC).
  p2p_bottleneck.SetQueue ("ns3::DropTailQueue", "MaxSize", StringValue (switch_netdev_size));
  for(uint32_t i=0;i<num_leaf;i++){
    p2p_leaf[i].SetQueue("ns3::DropTailQueue", "MaxSize", StringValue (server_netdev_size));
  }

  NS_LOG_DEBUG("================================= Install network device =========================================");

  NetDeviceContainer leftleaf_devices;
  NetDeviceContainer rightleaf_devices;
  NetDeviceContainer router_devices;
  NetDeviceContainer leftrouter_devices;
  NetDeviceContainer rightrouter_devices;

  router_devices = p2p_bottleneck.Install(router);

  for(uint32_t i=0;i<num_leaf;i++){
    NetDeviceContainer cl;
    NetDeviceContainer cr;
    cl=p2p_leaf[i].Install(router.Get(0),leftleaf.Get(i));
    cr=p2p_leaf[i].Install(router.Get(1),rightleaf.Get(i));

    leftrouter_devices.Add (cl.Get (0));
    leftleaf_devices.Add (cl.Get (1));
    rightrouter_devices.Add (cr.Get (0));
    rightleaf_devices.Add (cr.Get (1));
  }

  NS_LOG_DEBUG("================================ Install network stack ============================================");
  // Set the network protocol stack.
  InternetStackHelper stack;
  stack.Install(leftleaf);
  for(uint32_t i=0;i<num_leaf;i++){
    stack.Install(rightleaf.Get(i));
  }
  stack.Install(router.Get(0));
  stack.Install(router.Get(1));

  NS_LOG_DEBUG("====================== Initialize  transport layer protocol parameters ============================");
  // 2 MB (large enough) TCP buffers to prevent the applications from bottlenecking the exp
  Config::SetDefault ("ns3::TcpSocket::RcvBufSize", UintegerValue (1 << 21));
  Config::SetDefault ("ns3::TcpSocket::SndBufSize", UintegerValue (1 << 21));
  // Reset the default MSS ~500
  // IP MTU = IP header (20B-60B) + TCP header (20B-60B) + TCP MSS
  // Ethernet frame = Ethernet header (14B) + IP MTU + FCS (4B)
  // The additional header overhead for this instance is 20+20+14=54B, hence app_packet_size <= 1500-54 = 1446
  Config::SetDefault ("ns3::TcpSocket::SegmentSize", UintegerValue (app_packet_size));
  Config::SetDefault ("ns3::TcpSocketBase::Sack", BooleanValue (sack));
  Config::SetDefault ("ns3::TcpSocket::DelAckCount", UintegerValue (max_cumul_ACK_count));
  Config::SetDefault ("ns3::TcpL4Protocol::RecoveryType",TypeIdValue (TypeId::LookupByName (recovery)));

  NS_LOG_DEBUG("============================= Install transport layer protocol ===================================");

  Ptr<TcpL4Protocol> protocolL;
  Ptr<TcpL4Protocol> protocolR;

  for(uint32_t i=0;i<num_leaf;i++){
    protocolL=leftleaf.Get(i)->GetObject<TcpL4Protocol>();
    protocolR=rightleaf.Get(i)->GetObject<TcpL4Protocol>();
    protocolL->SetAttribute("SocketType",TypeIdValue(TypeId::LookupByName(p2p_cca[i])));
    protocolR->SetAttribute("SocketType",TypeIdValue(TypeId::LookupByName(p2p_cca[i])));
  }

  NS_LOG_DEBUG("============================ Install queuing mechanism for router =================================");

  TrafficControlHelper tch_switch;

  if(TypeId::LookupByName(queuedisc_type)==FifoQueueDisc::GetTypeId()){
    tch_switch.SetRootQueueDisc(queuedisc_type,"MaxSize",StringValue (FIFO_buffer_size));
    qdiscs=tch_switch.Install(router_devices.Get(0));
  }
  else if (TypeId::LookupByName(queuedisc_type)==CebinaeQueueDisc::GetTypeId()){
    bool enable_debug = 0;  
    Time dt {NanoSeconds (1048576)};
    Time vdt {NanoSeconds (1024)};
    Time l {NanoSeconds (65536)};
    uint32_t p {1};
    double tau {0.05};
    double delta_port {0.05};
    double delta_flow {0.05};
    bool pool = 0;
    std::string bottleneck_bw = "5Mbps";
    std::string switch_total_bufsize = "1000p";

    Config::SetDefault ("ns3::CebinaeQueueDisc::debug", BooleanValue (enable_debug));
    Config::SetDefault ("ns3::CebinaeQueueDisc::dT", TimeValue (dt));
    Config::SetDefault ("ns3::CebinaeQueueDisc::vdT", TimeValue (vdt));
    Config::SetDefault ("ns3::CebinaeQueueDisc::L", TimeValue (l));
    Config::SetDefault ("ns3::CebinaeQueueDisc::P", UintegerValue (p));
    Config::SetDefault ("ns3::CebinaeQueueDisc::tau", DoubleValue (tau));
    Config::SetDefault ("ns3::CebinaeQueueDisc::delta_port", DoubleValue (delta_port));
    Config::SetDefault ("ns3::CebinaeQueueDisc::delta_flow", DoubleValue (delta_flow));
    Config::SetDefault ("ns3::CebinaeQueueDisc::pool", BooleanValue (pool));
    Config::SetDefault ("ns3::CebinaeQueueDisc::DataRate", StringValue (bottleneck_bw));

    tch_switch.SetRootQueueDisc ("ns3::CebinaeQueueDisc", "MaxSize", StringValue (switch_total_bufsize));

    qdiscs = tch_switch.Install(router_devices.Get(0));
  }
  else if(TypeId::LookupByName(queuedisc_type)==RtqsQueueDisc::GetTypeId()){
    tch_switch.SetRootQueueDisc(queuedisc_type);
    qdiscs=tch_switch.Install(router_devices.Get(0));
    Ptr<QueueDisc>cur_queue =qdiscs.Get(0);
    cur_queue->SetAttribute("max_flow_reject_alpha",DoubleValue(MaxFlowRejectAlpha));
    cur_queue->SetAttribute("max_flow_reject_beta",DoubleValue(MaxFlowRejectBeta));
    cur_queue->SetAttribute("if_max_flow_reject_enable",BooleanValue(IFMaxFlowRejectOn));
    cur_queue->SetAttribute("m_interval",TimeValue(m_interval));
    cur_queue->SetAttribute("qlog_dir", StringValue(qlog_dir));
    cur_queue->SetAttribute("max_reject_times_threshold",UintegerValue(7));
    cur_queue->SetAttribute("max_flow_reject_function", CallbackValue(MakeCallback(&default_max_flow_reject_function)));
  }

  NS_LOG_DEBUG("================================== Assign IP address =============================================");
  // Three subnets are configured
  Ipv4AddressHelper ipv4_left("1.1.1.0","255.255.255.0");
  Ipv4AddressHelper ipv4_right("100.1.1.0","255.255.255.0");
  Ipv4AddressHelper ipv4_router("200.1.1.0","255.255.255.0");

  // Storage IP address
  Ipv4InterfaceContainer leftleaf_ifc;
  Ipv4InterfaceContainer leftrouter_ifc;
  Ipv4InterfaceContainer rightleaf_ifc;
  Ipv4InterfaceContainer rightrouter_ifc;
  Ipv4InterfaceContainer router_ifc;

  // Assign IP addresses to network cards between routers.
  router_ifc=ipv4_router.Assign(router_devices);
  // Assign ip addresses to leaf nodes.
  for(uint32_t i=0;i<num_leaf;i++){
    NetDeviceContainer ndc;
    ndc.Add(leftleaf_devices.Get(i));
    ndc.Add(leftrouter_devices.Get(i));

    Ipv4InterfaceContainer ifc = ipv4_left.Assign (ndc);
    leftleaf_ifc.Add (ifc.Get (0));
    leftrouter_ifc.Add (ifc.Get (1));
    //Adding a subnet
    ipv4_left.NewNetwork ();        
  }
  for (uint32_t i = 0; i < num_leaf; i++) {
    NetDeviceContainer ndc;
    ndc.Add (rightleaf_devices.Get (i));
    ndc.Add (rightrouter_devices.Get (i));
    Ipv4InterfaceContainer ifc = ipv4_right.Assign (ndc);
    rightleaf_ifc.Add (ifc.Get (0));
    rightrouter_ifc.Add (ifc.Get (1));
    ipv4_right.NewNetwork ();
  }  

  NS_LOG_DEBUG("================================== Create the receiver =============================================");
  // Generate a receiver application. PacketSink is a default receiver application that only accepts packets and does not send them.
  for(uint32_t i=0;i<num_leaf;i++){
    // Port number of the receiving application.
    uint16_t sinkPort =8080;
    Ptr<PacketSink>sink=CreateObject<PacketSink>();
    // Set application parameters.
    sink->SetAttribute("Protocol",StringValue("ns3::TcpSocketFactory"));
    sink->SetAttribute("Local",AddressValue(InetSocketAddress(Ipv4Address::GetAny(),sinkPort)));
    rightleaf.Get(i)->AddApplication(sink);
    // Set the start and end times.
    sink->SetStartTime(Seconds(0.));
    sink->SetStopTime(Seconds(app_end_seconds));
    // Start a TraceSource after receiving the packet.
    sink->TraceConnectWithoutContext("RxWithAddresses",MakeCallback(&packetSinkReceivePacket));
  }

  NS_LOG_DEBUG("================================== Create the sender =============================================");
  // Generate the sender application, which is divided into realtime, non-realttime, and other streams according to the application type.
  for(uint32_t i=0;i<num_leaf;i++){
    Ptr<Socket>ns3TcpSocket =Socket::CreateSocket(leftleaf.Get(i),TcpSocketFactory::GetTypeId());
    // Update RTT in real time.
    ns3TcpSocket->TraceConnectWithoutContext("RTT",MakeBoundCallback(&updateRtt,i));
    // Port number of the receiving application.
    uint16_t sinkPort =8080;
    // IP address of the receiving end.
    Address sinkAddress (InetSocketAddress(rightleaf_ifc.GetAddress(i),sinkPort));

    // Create an instantiated object for the application type.
    ObjectFactory objectFactory;
    objectFactory.SetTypeId(app_type[i]);
    Ptr<Application> app=objectFactory.Create<Application>();
    
    // Set application parameters.
    app->SetAttribute("socket",PointerValue(ns3TcpSocket));
    app->SetAttribute("target_address",AddressValue(sinkAddress));
    app->SetAttribute("packet_size",UintegerValue(app_packet_size));
    app->SetAttribute("dataRate",DataRateValue(DataRate(app_bw[i])));
    app->SetAttribute("source_id",UintegerValue(i));
    leftleaf.Get(i)->AddApplication(app);
    app->SetStartTime(Seconds(app_start_seconds));
    app->SetStopTime(Seconds(app_end_seconds));
  }

  NS_LOG_DEBUG("================================== Configure TraceSources ============================================");
  // The router sends packets.
  router_devices.Get(0)->TraceConnectWithoutContext("PhyTxEnd",MakeCallback(&routerSendPacket));
  // The router received the ACK packets. 
  if(TypeId::LookupByName(queuedisc_type)==RtqsQueueDisc::GetTypeId()){
    router_devices.Get(0)->TraceConnectWithoutContext("PhyRxEnd",MakeCallback(&routerReceiveACK));
  }
  for (uint32_t i=0;i<num_leaf;i++){
    // Add BytesTag to ACK packets.
    rightleaf_devices.Get(i)->TraceConnectWithoutContext("PhyTxBegin",MakeBoundCallback(&addByteTagForACK,i,app_type_to_type_id[app_type[i]]));
    // Add BytesTag to source packets.
    leftleaf_devices.Get(i)->TraceConnectWithoutContext("PhyTxBegin",MakeCallback(&addByteTagForSource));
  }

  NS_LOG_DEBUG("==================================== Start write qlog ==============================================");
  router_byte_count.resize(num_leaf, 0);
  packet_sink_byte_count.resize(num_leaf, 0);
  source_rtt.resize(num_leaf,0);
  Simulator::Schedule(MilliSeconds(tracing_period_ms), &writeLog, 
                      qlog_dir, tracing_period_ms,
                      "bottleneck_tpt_"+std::to_string(tracing_period_ms)+".dat",
                      "app_tpt_"+std::to_string(tracing_period_ms)+".dat",
                      "jfi_"+std::to_string(tracing_period_ms)+".dat",
                      "rtt_"+std::to_string(tracing_period_ms)+".dat");

  Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

  Simulator::Schedule (MilliSeconds(1000), &PrintProgress, MilliSeconds(1000));

  Simulator::Stop (Seconds (simulator_run_seconds));
  auto start = std::chrono::high_resolution_clock::now();
  Simulator::Run ();

  auto stop = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed_seconds = stop - start;

  Simulator::Destroy();
  return 0;
}

