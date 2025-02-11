/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2017 Universita' degli Studi di Napoli Federico II
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Authors:  Stefano Avallone <stavallo@unina.it>
 */

#include "rtqs-queue-disc.h"

namespace ns3 {

NS_LOG_COMPONENT_DEFINE ("RtqsQueueDisc");

NS_OBJECT_ENSURE_REGISTERED (RtqsQueueDisc);

TypeId RtqsQueueDisc::GetTypeId (void)
{
  static TypeId tid = TypeId ("ns3::RtqsQueueDisc")
    .SetParent<QueueDisc> ()
    .SetGroupName ("TrafficControl")
    .AddConstructor<RtqsQueueDisc> ()
    .AddAttribute ("default_detection_buffer_length",
                   "the default queue length of detection queue",
                   UintegerValue (200),
                   MakeUintegerAccessor (&RtqsQueueDisc::setDefaultDetectionBuffer,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t> ())      
    .AddAttribute ("default_realtime_buffer_length",
                   "the default queue length of realtime queue",
                   UintegerValue (200),
                   MakeUintegerAccessor (&RtqsQueueDisc::setDefaultRealtimeBuffer,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("default_non_realtime_buffer_length",
                   "the default queue length of non-realtime queue",
                   UintegerValue (1000),
                   MakeUintegerAccessor (&RtqsQueueDisc::setDefaultNonRealtimeBuffer,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("default_other_buffer_length",
                   "the default queue length of other queue",
                   UintegerValue (500),
                   MakeUintegerAccessor (&RtqsQueueDisc::setDefaultOtherBuffer,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t> ())
    .AddAttribute ("default_black_house_buffer_length",
                   "the default queue length of black house queue",
                   UintegerValue (500),
                   MakeUintegerAccessor (&RtqsQueueDisc::setDefaultBlackHouseBuffer,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t> ())

    .AddAttribute ("source_id_for_update_ADD_and_MPD",
                   "current source_id for update ADD and MPD",
                   UintegerValue (0),
                   MakeUintegerAccessor (&RtqsQueueDisc::source_id_for_update_ADD_and_MPD),
                   MakeUintegerChecker<uint32_t>())    
    .AddAttribute ("type_id_for_update_ADD_and_MPD",
                   "current type_id for update ADD and MPD",
                   UintegerValue (0),
                   MakeUintegerAccessor (&RtqsQueueDisc::type_id_for_update_ADD_and_MPD),
                   MakeUintegerChecker<uint32_t>())       
    .AddAttribute ("MPD_value",
                   "current MPD for update ADD and MPD",
                   TimeValue(Time(0)),
                   MakeTimeAccessor (&RtqsQueueDisc::MPD_value),
                   MakeTimeChecker())
    .AddAttribute ("ADD_value",
                   "current ADD for update ADD and MPD",
                   TimeValue(Time(0)),
                   MakeTimeAccessor (&RtqsQueueDisc::ADD_value),
                   MakeTimeChecker())
    .AddAttribute ("update_ADD_and_MPD",
                   "To update the value of ADD and MPD",
                   TimeValue(Time(0)),
                   MakeTimeAccessor (&RtqsQueueDisc::updateADDAndDownMPD,&RtqsQueueDisc::getPerQueueingTime),
                   MakeTimeChecker ())
                   
    .AddAttribute ("source_id_for_update_dequeue_throughput",
                   "current source_id for update dequeue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::source_id_for_update_dequeue_throughput),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("queue_id_for_update_dequeue_throughput",
                   "current queue_id for update dequeue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::queue_id_for_update_dequeue_throughput),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("update_num_for_update_dequeue_throughput",
                   "current update value for update dequeue_throughput",
                   IntegerValue(int32_t(0)),
                   MakeIntegerAccessor(&RtqsQueueDisc::update_num_for_update_dequeue_throughput),
                   MakeIntegerChecker<int32_t>())
    .AddAttribute ("update_dequeue_throughput",
                   "update the value of dequeue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::updateDequeueThroughput,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("source_id_for_update_enqueue_throughput",
                   "current source_id for update enqueue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::source_id_for_update_enqueue_throughput),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("queue_id_for_update_enqueue_throughput",
                   "current queue_id for update enqueue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::queue_id_for_update_enqueue_throughput),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("update_num_for_update_enqueue_throughput",
                   "current update value for update enqueue_throughput",
                   IntegerValue(int32_t(0)),
                   MakeIntegerAccessor(&RtqsQueueDisc::update_num_for_update_enqueue_throughput),
                   MakeIntegerChecker<int32_t>())
    .AddAttribute ("update_enqueue_throughput",
                   "update the value of enqueue_throughput",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::updateEnqueueThroughput,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("source_id_for_update_if_max_flow_reject",
                   "current source_id for update if_max_flow_reject",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::source_id_for_update_if_max_flow_reject),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("update_if_max_flow_reject",
                   "update the value of if_max_flow_reject",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::updateIfMaxFlowReject,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("source_id_for_update_max_flow_reject_times",
                   "current source_id for update max_flow_reject_times",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::source_id_for_update_max_flow_reject_times),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("update_max_flow_reject_times",
                   "update the value of max_flow_reject_times",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::updateMaxFlowRejectTimes,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("queue_id_for_queue_len_increase",
                   "current queue_id for queue len increase",
                   UintegerValue(0),
                   MakeUintegerAccessor (&RtqsQueueDisc::queue_id_for_queue_len_increase),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("queue_len_increase",
                   "To increase the queue len",
                   UintegerValue(0),
                   MakeUintegerAccessor (&RtqsQueueDisc::increaseQueueLen,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("source_id_for_update_flow_status",
                   "current source_id for update flow_status",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::source_id_for_update_flow_status),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("update_flow_status",
                   "update the value of flow_status",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::updateFlowStatus,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("writeQLogFiles",
                   "write qdisc information to qlog files",
                   UintegerValue(uint32_t(0)),
                   MakeUintegerAccessor(&RtqsQueueDisc::writeQLogFiles,&RtqsQueueDisc::defaultUintegerGetter),
                   MakeUintegerChecker<uint32_t>())

    .AddAttribute ("if_max_flow_reject_enable",
                   "set if enable max flow reject",
                   BooleanValue(true),
                   MakeBooleanAccessor(&RtqsQueueDisc::if_max_flow_reject_enable),
                   MakeBooleanChecker())
    .AddAttribute ("max_reject_times_threshold",
                   "set the max times for max flow reject with time interval",
                   UintegerValue(7),
                   MakeUintegerAccessor(&RtqsQueueDisc::max_reject_times_threshold),
                   MakeUintegerChecker<uint32_t>())
    .AddAttribute ("max_flow_reject_function",
                   "set the max flow reject function",
                   CallbackValue(),
                   MakeCallbackAccessor(&RtqsQueueDisc::max_flow_reject_function),
                   MakeCallbackChecker())
    .AddAttribute ("max_flow_reject_alpha",
                   "set the max flow reject threshold between all queues",
                   DoubleValue(double(1.2)),
                   MakeDoubleAccessor(&RtqsQueueDisc::max_flow_reject_alpha),
                   MakeDoubleChecker<double_t>())
    .AddAttribute ("max_flow_reject_beta",
                   "set the max flow reject threshold within one queue",
                   DoubleValue(double(1.2)),
                   MakeDoubleAccessor(&RtqsQueueDisc::max_flow_reject_beta),
                   MakeDoubleChecker<double_t>())
    .AddAttribute ("m_interval",
                   "Latency requirements for real-time streams",
                   TimeValue(Time("150ms")),
                   MakeTimeAccessor (&RtqsQueueDisc::m_interval),
                   MakeTimeChecker())
      
    .AddAttribute ("qlog_dir",
                   "the base path where rtqs-queue-disc log are stored",
                   StringValue("./"),
                   MakeStringAccessor(&RtqsQueueDisc::qlog_dir),
                   MakeStringChecker())
  ;
  return tid;
}

RtqsQueueDisc::RtqsQueueDisc ()
  : QueueDisc (QueueDiscSizePolicy::SINGLE_INTERNAL_QUEUE)
{
  NS_LOG_FUNCTION (this);
}

RtqsQueueDisc::~RtqsQueueDisc ()
{
  NS_LOG_FUNCTION (this);
}

void RtqsQueueDisc::setDefaultDetectionBuffer(uint32_t update_num){
  setBuffer(update_num,0);
}

void RtqsQueueDisc::setDefaultRealtimeBuffer(uint32_t update_num){
  for (uint32_t queue_id=1;queue_id<=3;queue_id++){
    setBuffer(update_num,queue_id);
  }
}

void RtqsQueueDisc::setDefaultNonRealtimeBuffer(uint32_t update_num){
  setBuffer(update_num,4);
}

void RtqsQueueDisc::setDefaultOtherBuffer(uint32_t update_num){
  setBuffer(update_num,5);
}

void RtqsQueueDisc::setDefaultBlackHouseBuffer(uint32_t update_num){
  setBuffer(update_num,6);
}


void RtqsQueueDisc::updateADDAndDownMPD(Time tagValue){
  // At the beginning, NS3 will run this method, so we should check to avoid unexpect assignment
  if (ADD_value==Time(0)) return;

  // Set the value of ADD
  ADD[source_id_for_update_ADD_and_MPD]=ADD_value;

  // Update the value of min_ADD
  if (min_ADD.find(source_id_for_update_ADD_and_MPD)==min_ADD.end()){
    min_ADD[source_id_for_update_ADD_and_MPD]=ADD_value;
  }
  else {
    if(ADD_value<min_ADD[source_id_for_update_ADD_and_MPD]){
      min_ADD[source_id_for_update_ADD_and_MPD]=ADD_value;
    }
  }

  // Set the value of downMPD
  downMPD[source_id_for_update_ADD_and_MPD]=MPD_value;

  // Update the value of cur_queue_id, this value represent current queuing time for source: source_id_for_update_ADD_and_MPD.
  if (min_ADD.find(source_id_for_update_ADD_and_MPD)!=min_ADD.end() && min_ADU.find(source_id_for_update_ADD_and_MPD)!=min_ADU.end()){
    uint32_t cur_queue_id=getQueueId(source_id_for_update_ADD_and_MPD,type_id_for_update_ADD_and_MPD);
    per_queueing_time=getCurQueueingTime(cur_queue_id);
  }
  else {
    per_queueing_time=Time(0);
  }

}

Time RtqsQueueDisc::getPerQueueingTime(void)const{
  return per_queueing_time;
}

void RtqsQueueDisc::updateDequeueThroughput(uint32_t newValue){
  setDequeueThrouthput(source_id_for_update_dequeue_throughput,queue_id_for_update_dequeue_throughput,update_num_for_update_dequeue_throughput);
}

void RtqsQueueDisc::updateEnqueueThroughput(uint32_t newValue){
  setEnqueueThroughput(source_id_for_update_enqueue_throughput,queue_id_for_update_enqueue_throughput,update_num_for_update_enqueue_throughput);
}

void RtqsQueueDisc::updateIfMaxFlowReject(uint32_t newValue){
  setIfMaxFlowReject(source_id_for_update_if_max_flow_reject,false);
}

void RtqsQueueDisc::updateMaxFlowRejectTimes(uint32_t curSid){
  setMaxFlowRejectTimes(source_id_for_update_max_flow_reject_times,-1);
}

void RtqsQueueDisc::increaseQueueLen(uint32_t){
  if (queue_id_for_queue_len_increase==0) return;
  QueueSize cur_queue_size=getBuffer(queue_id_for_queue_len_increase);
  setBuffer(cur_queue_size.GetValue()+1,queue_id_for_queue_len_increase);
  Time next_increase_time=std::max(getSendPacketDeltaT(queue_id_for_queue_len_increase)*10, defalut_increase_queue_len_interval);
  Simulator::Schedule(next_increase_time, &doQueueLengthIncrease, queue_id_for_queue_len_increase);
}

void RtqsQueueDisc::updateFlowStatus(uint32_t){
  flow_status[source_id_for_update_flow_status]=2;
}

void RtqsQueueDisc::writeQLogFiles(uint32_t){
  writeDequeueThroughput();
  writeEnqueueThroughput();
  writeQueuePktCount();
  writeQueueBufferSize();
  writeFlowCount();
  writeQueueingTime();
}

uint32_t RtqsQueueDisc::defaultUintegerGetter()const{
  return 0;
}

bool RtqsQueueDisc::DoEnqueue (Ptr<QueueDiscItem> item)
{
  NS_LOG_FUNCTION (this << item);

  MySourceIDType type_id_tag;
  item->GetPacket()->FindFirstMatchingByteTag(type_id_tag);

  MySourceIDTag source_id_tag;
  bool totalFlag=item->GetPacket()->FindFirstMatchingByteTag(source_id_tag);

  // If the current packet does not have a source_id_tag, this packet is probably a handshake packet.
  // The source_id of the handshake packet is set to 1e9+7.
  if (!totalFlag) {
    source_id_tag.Set(1e9+7);
    // Put handshake packet to detection queue to protect the flow connection.
    type_id_tag.Set(0);
  }

  // Mark that flow: source_id has started.
  if (source_id_tag.Get()!=1e9+7 && flow_status[source_id_tag.Get()]==0){
    setFlowStart(source_id_tag.Get());
    Simulator::Schedule(Seconds(25), &doUpdateFlowStatus, source_id_tag.Get());
  }

  // If the min_ADD or min_ADU is empty, it indicates that this flow has just started to send packets. 
  // If so, all packets are placed in the detection queue to obtain more accurate min_ADD and min_ADU.
  if(type_id_tag.Get()==1){
    if(min_ADD.find(source_id_tag.Get())==min_ADD.end()){
      type_id_tag.Set(0);
    }
    if(min_ADU.find(source_id_tag.Get())==min_ADU.end()){
      type_id_tag.Set(0);
    }
  }

  // Max Flow Reject!!!
  if (if_max_flow_reject_enable){
    // Judge if the max flow.
    bool if_max_flow=judgeIfMaxFlowReject(source_id_tag.Get(),type_id_tag.Get());
    uint32_t cur_flow_status=flow_status[source_id_tag.Get()];
    // If the current flow is the maximum flow and has entered the stable state.
    if (if_max_flow && cur_flow_status==2){
      // Here are two situations:
      // 1. If the black house Queue will dequeue the packets.
      // It means that there are some flows that are not using up their fair bandwidth share allocated to them.
      uint32_t black_house_flow_cnt=getDequeueFlowCount(6);
      if (black_house_flow_cnt!=0){
        // If so, we set all rebundant packets to the black house queue.
        type_id_tag.Set(4);
        writeMaxFlowReject(source_id_tag.Get(),Simulator::Now());
      }
      // 2. If the black house queue will not dequeue any packets. 
      // It means that there are no more bandwidth for the balck house queue.
      else {
        // Further determine the number of times that the current flow has been performed. 
        // If this flow has been performed maximum flow reject more than max_reject_times in a time window,
        // all the newly arrived packets will be put into the black house queue.
        int32_t has_rejected_times=max_flow_reject_times[source_id_tag.Get()];
        if (has_rejected_times<=(int32_t)max_reject_times_threshold){
          // To determine whether enough time has passed since the last maximum flow reject, 
          // this time is determined by the max_flow_reject_function.
          bool if_has_change=if_max_flow_reject[source_id_tag.Get()];
          if (if_has_change==false){
            // Put current packets to the black house queue.
            type_id_tag.Set(4);
            // Write qlog files.
            writeMaxFlowReject(source_id_tag.Get(),Simulator::Now());
            // Set that the current flow has performed the maximum flow reject, and the next flow reject 
            // performent should be after wait_time_interval, which is determined by the max_flow_reject_function.
            setIfMaxFlowReject(source_id_tag.Get(),true);
            double wait_time_interval=max_flow_reject_function(has_rejected_times);
            Simulator::Schedule(Seconds(wait_time_interval), &doUpdateIfMaxFlowReject, source_id_tag.Get());
            // Count the number of times the max flow reject was performed.
            // At this point, we assume that the time window is 5 second.
            setMaxFlowRejectTimes(source_id_tag.Get(),1);
            Simulator::Schedule(Seconds(5), &doUpdateMaxFlowRejectTimes, source_id_tag.Get());
          }
        }
        else {
          // The maximum flow reject is also performed (all newly arrived packets are put into the black room queue), 
          // but the interval is no longer counted.
          type_id_tag.Set(4);
          writeMaxFlowReject(source_id_tag.Get(),Simulator::Now());
          setMaxFlowRejectTimes(source_id_tag.Get(),1);
          Simulator::Schedule(Seconds(5), &doUpdateMaxFlowRejectTimes, source_id_tag.Get());
        }
      }
    }
  }

  // Obtain queue_id depend on source_id and type_id.
  uint32_t queue_id=getQueueId(source_id_tag.Get(), type_id_tag.Get());
  // The realtime flow (type_id=1) may also be assigned to other queues (queue_id=5) if the irrelevant delay is too large.
  // In this case, we directly change the type_id to 3, that is, it is considered to be other flow (abandoned flow).
  if (queue_id==5){
    type_id_tag.Set(3);
  }
  // Resize the queue length, only for realtime flows.
  if (type_id_tag.Get()==1){
    // Calculate m_point value for queue: queue_id.
    m_point[queue_id]=setMPoint(item->GetPacket(),queue_id);
    // Resize The Queue Length!!!
    if(m_point[queue_id]>0){
      // If the resize operation has been performed in a short period of time before, 
      // and is still in the draining queue phase, no further adjustment is made.
      if (getBuffer(queue_id).GetValue()>=GetInternalQueue(queue_id)->GetCurrentSize().GetValue()){
        Time cur_deltaT=getSendPacketDeltaT(queue_id);
        // Calculate the resized queue length, that is, how long the queue length needs to be reduced.
        int32_t cur_drain_packet_num = m_point[queue_id]/cur_deltaT.GetNanoSeconds();
        writeRbufResize(queue_id, queue_buffer_size[queue_id].GetValue(), queue_buffer_size[queue_id].GetValue()-cur_drain_packet_num);
        setBuffer(queue_buffer_size[queue_id].GetValue()-cur_drain_packet_num, queue_id);
      }else{
        // now is draining the queue, we can't further modify the queue size.
      }
    }
  }
  // Record The UpMPD!!!
  setUpMPD(item->GetPacket(),source_id_tag.Get(),queue_id);
  // Record The ADU!!!
  MySourceIDSendTime send_time_tag;
  item->GetPacket()->FindFirstMatchingByteTag(send_time_tag);
  setADU(source_id_tag.Get(),send_time_tag.MyGetTime());
  // Judge if can enter the queue: queue_id, or drop this packet because queue full.
  if((GetInternalQueue(queue_id)->GetCurrentSize()+item).GetValue()>getBuffer(queue_id).GetValue()){
    // For non-black-house queue, we will drop this pakcet. 
    if (queue_id!=6){
      DropBeforeEnqueue (item, LIMIT_EXCEEDED_DROP);
      writePktLoss(source_id_tag.Get(), Simulator::Now());
      return false;
    }
    // For balck house queue, we will drop the first packet in this queue, and enter current one.
    else if (queue_id==6){
      Ptr<QueueDiscItem> front_packet=GetInternalQueue(6)->Dequeue();
      DropAfterDequeue(front_packet, TIME_OUT);
      MySourceIDTag front_packet_source_id_tag;
      front_packet->GetPacket()->FindFirstMatchingByteTag(front_packet_source_id_tag);
      UpdatePktCount(front_packet_source_id_tag.Get(),queue_id,-1);
      writePktLoss(front_packet_source_id_tag.Get(), Simulator::Now());
    }
  }
  bool ret_val=GetInternalQueue(queue_id)->Enqueue(item);
  NS_ASSERT(ret_val==true);
  // Update the value of packet count for each queue.
  UpdatePktCount(source_id_tag.Get(),queue_id,1);
  // Update the enqueue throughtput for each queue and each source.
  setEnqueueThroughput(source_id_tag.Get(),queue_id,1);
  // We assume that the time window is 15s here.
  Simulator::Schedule(Seconds(15), &doUpdateEnqueueThroughput, source_id_tag.Get(), queue_id, -1);
  return ret_val;
}

Ptr<QueueDiscItem>
RtqsQueueDisc::dequeuePacket(uint32_t queue_id){
  Ptr<QueueDiscItem> packet = GetInternalQueue (queue_id)->Dequeue ();
  // Get source_id of the dequeue packet.
  MySourceIDTag source_id_tag;
  bool find_tag_flag= packet->GetPacket()->FindFirstMatchingByteTag(source_id_tag);
  // For handshake packets, the source_id is 1e9+7.
  if (!find_tag_flag) source_id_tag.Set(1e9+7);
  // update the packets count for each queue.
  UpdatePktCount(source_id_tag.Get(),queue_id,-1);
  // Update dequeue throughput value for each source_id and each queue_id.
  setDequeueThrouthput(source_id_tag.Get(),queue_id,1);
  // We assume that the time window is 5 seconds here.
  Simulator::Schedule(Seconds(5), &doUpdateDequeueThroughput, source_id_tag.Get(), queue_id, -1);
  // Update the value of packet send deltaT.
  setSendPacketDeltaT(queue_id);
  return packet;
}

Ptr<QueueDiscItem> RtqsQueueDisc::DoDequeue (void)
{
  NS_LOG_FUNCTION (this);

  // The detection queue always takes precedence over the other queues.
  if(GetInternalQueue(0)->GetCurrentSize ().GetValue()!=0){
    uint32_t queue_id=0;
    return dequeuePacket(queue_id);
  }
  // If the current realtime queue is in the draining phase, the priority is higher than other queues, 
  // but lower than the detection queue.
  for (uint32_t queue_id=1;queue_id<=3;queue_id++){
    if (GetInternalQueue(queue_id)->GetCurrentSize().GetValue()>getBuffer(queue_id).GetValue()){
      return dequeuePacket(queue_id);
    }
  }

  // Random-Based Packet Dequeue (TODO:FIXME)!
  double cur_total_rate{0};
  double cur_random_num{0};
  // Get the current fair bandwidth.
  setFairBandwidthShare();
  // Only flows that currently have packets in the queue will be assigned a packet sending opportunity.
  // This is to ensure that when a flow cannot occupied all of its fair bandwidth, the redundant bandwidth will be fairly 
  // distributed to other flows, rather than distributed all of it to the black house queue (which may have only one flow packets).
  // Note that there is no fixed opportunity to send packets in the black house queue, 
  // and packets in the black house queue are sent if and only if no other queue has any packets to send.
  for (uint32_t queue_id=1;queue_id<=5;queue_id++){
    if (GetInternalQueue(queue_id)->GetCurrentSize().GetValue()!=0){
      cur_total_rate+=fair_bandwidth_share[queue_id];
    }
  }
  // Some non-black-house queue has packets to be sent.
  if (cur_total_rate!=0){
    // Get a random value from 0 to 1.
    cur_random_num=rand() / double(RAND_MAX);
    for (uint32_t queue_id=0;queue_id<=5;queue_id++){
      if (GetInternalQueue(queue_id)->GetCurrentSize().GetValue()!=0){
        // The packet sending opportunity is allocated to the current queue.
        if (cur_random_num<=fair_bandwidth_share[queue_id]/cur_total_rate){
          return dequeuePacket(queue_id);
        }
        // The current queue is not assigned to send packets. The value of cur_random_num is updated for subsequent queues to judge.
        else {
          cur_random_num-=fair_bandwidth_share[queue_id]/cur_total_rate;
        } 
      }
    }
  }
  // all other queue has no packet, so now we can dequeue the black house queue!
  else {
    Ptr<QueueDiscItem> packet;
    while (GetInternalQueue(6)->GetCurrentSize().GetValue()!=0){
      packet=GetInternalQueue(6)->Dequeue();
      MySourceIDTag source_id_tag;
      bool find_tag_flag=packet->GetPacket()->FindFirstMatchingByteTag(source_id_tag);
      if (!find_tag_flag) source_id_tag.Set(1e9+7);
      MySourceIDType type_id_tag;
      packet->GetPacket()->FindFirstMatchingByteTag(type_id_tag);
      // For the black house queue, if the current packet is a realtime flow packet, we need to determine whether the packet has timed out.
      if (type_id_tag.Get()==1){
        MySourceIDSendTime send_time_tag;
        packet->GetPacket()->FindFirstMatchingByteTag(send_time_tag);
        Time cur_ADU=Simulator::Now()-send_time_tag.MyGetTime();
        Time cur_ADD=ADD[source_id_tag.Get()];
        if (cur_ADU+cur_ADD-m_interval<Time(0)){
          //this packet is bound to late for realtime flow
          //so we would like to send the next packet
          DropAfterDequeue(packet, TIME_OUT);
          UpdatePktCount(source_id_tag.Get(),6,-1);
          writePktLoss(source_id_tag.Get(), Simulator::Now());
          continue;
        }
        else {
          //We choose this pakcet as the next packet to be sent
          break;
        }
      }
      //for none realtime stream and other stream
      else if (type_id_tag.Get()==2||type_id_tag.Get()==3){
        //we choose this packet as the next pakcet to be sent
        break;
      }
    }
    if (packet){
      MySourceIDTag source_id_tag;
      bool find_tag_flag=packet->GetPacket()->FindFirstMatchingByteTag(source_id_tag);
      if (!find_tag_flag) source_id_tag.Set(1e9+7);
      setDequeueThrouthput(source_id_tag.Get(),6,1);
      Simulator::Schedule(Seconds(5), &doUpdateDequeueThroughput, source_id_tag.Get(), 6, -1);
      UpdatePktCount(source_id_tag.Get(),6,-1);
    }
    return packet;
  }
}

Ptr<const QueueDiscItem> RtqsQueueDisc::DoPeek (void)
{
  NS_LOG_FUNCTION (this);

  Ptr<const QueueDiscItem> item = GetInternalQueue (0)->Peek ();

  if (!item)
    {
      NS_LOG_LOGIC ("Queue empty");
      return 0;
    }

  return item;
}

bool RtqsQueueDisc::CheckConfig (void)
{
  NS_LOG_FUNCTION (this);
  if (GetNQueueDiscClasses () > 0){
    NS_LOG_ERROR ("RtqsQueueDisc cannot have classes");
    return false;
  }

  if (GetNPacketFilters () > 0){
    NS_LOG_ERROR ("RtqsQueueDisc needs no packet filter");
    return false;
  }

  // The MaxSize set here is the maximum length of the physical device, 
  // while the actual queue length is dynamic and obtained by getBuffer.
  if (GetNInternalQueues () == 0){
    // detection queue!
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));
    // three realtime queues!
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));
    // non-realtime queue!
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));
    // other queue!
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p")))); 
    // balck house queue!
    AddInternalQueue (CreateObjectWithAttributes<DropTailQueue<QueueDiscItem> >
                        ("MaxSize", QueueSizeValue (std::string("5000p"))));           
  }

  if (GetNInternalQueues () != 7){
    NS_LOG_ERROR ("RtqsQueueDisc needs 7 internal queue");
    return false;
  }

  // The first interval to increase the queue length is the default time (10ms).
  Simulator::Schedule(defalut_increase_queue_len_interval, &doQueueLengthIncrease, 1U);
  Simulator::Schedule(defalut_increase_queue_len_interval, &doQueueLengthIncrease, 2U);
  Simulator::Schedule(defalut_increase_queue_len_interval, &doQueueLengthIncrease, 3U);

  return true;
}

void RtqsQueueDisc::InitializeParams (void)
{
  NS_LOG_FUNCTION (this);
}


QueueSize RtqsQueueDisc::getBuffer(uint32_t queue_id){
  return queue_buffer_size[queue_id];
}

void RtqsQueueDisc::setBuffer(int32_t update_num, uint32_t queue_id){
  if (update_num>=MY_MIN_QUEUE && update_num<=MY_MAX_QUEUE){
    queue_buffer_size[queue_id]=QueueSize(PACKETS, update_num);
  }
  else if (update_num<=MY_MIN_QUEUE) {
    queue_buffer_size[queue_id]=QueueSize(PACKETS, MY_MIN_QUEUE);
  }
  else if (update_num>=MY_MAX_QUEUE){
    queue_buffer_size[queue_id]=QueueSize(PACKETS, MY_MAX_QUEUE);
  }
}

void RtqsQueueDisc::setEnqueueThroughput(uint32_t source_id,uint32_t queue_id,int32_t update_num){
  // Get current enqueue throughput recorder for Queue: queue_id.
  if (enqueue_throughput.find(queue_id)==enqueue_throughput.end()){
    enqueue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
  }
  std::unordered_map<uint32_t, int32_t>& cur_enqueue_throughput=enqueue_throughput[queue_id];

  // Update the value of enqueue_throughput for current flow (distinguished by source_id).
  if (cur_enqueue_throughput.find(source_id)!=cur_enqueue_throughput.end()){
    cur_enqueue_throughput[source_id]+=update_num;
  }
  else {
    cur_enqueue_throughput[source_id]=update_num;
  }
}

uint32_t RtqsQueueDisc::getEnqueueFlowCount(uint32_t queue_id){
  // Get current enqueue throughput recorder for Queue: queue_id.
  if (enqueue_throughput.find(queue_id)==enqueue_throughput.end()){
    enqueue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
  }
  std::unordered_map<uint32_t, int32_t>& cur_enqueue_throughput=enqueue_throughput[queue_id];

  // Count the number of flow in this queue.
  uint32_t flow_count=0;
  for (auto it=cur_enqueue_throughput.begin();it!=cur_enqueue_throughput.end();it++){
    if (it->first!=1e9+7 && it->second>0){
      flow_count++;
    }
  }
  return flow_count;
}

void RtqsQueueDisc::setDequeueThrouthput(uint32_t source_id,uint32_t queue_id,int32_t update_num){
  // Get current dequeue throughput recorder for Queue: queue_id.
  if (dequeue_throughput.find(queue_id)==dequeue_throughput.end()){
    dequeue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
  }
  std::unordered_map<uint32_t, int32_t>& cur_dequeue_throughput=dequeue_throughput[queue_id];

  // Update the value of dequeue_throughput for current flow (distinguished by source_id).
  if (cur_dequeue_throughput.find(source_id)!=cur_dequeue_throughput.end()){
    cur_dequeue_throughput[source_id]+=update_num;
  }
  else {
    cur_dequeue_throughput[source_id]=update_num;
  }
}

uint32_t RtqsQueueDisc::getDequeueFlowCount(uint32_t queue_id){
  // Get current dequeue throughput recorder for Queue: queue_id.
  if (dequeue_throughput.find(queue_id)==dequeue_throughput.end()){
    dequeue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
  }
  std::unordered_map<uint32_t, int32_t>& cur_dequeue_throughput=dequeue_throughput[queue_id];

  // Count the number of flow in this queue.
  uint32_t flow_count=0;
  for (auto it=cur_dequeue_throughput.begin();it!=cur_dequeue_throughput.end();it++){
    if (it->first!=1e9+7 && it->second>0){
      flow_count++;
    }
  }
  return flow_count;
}

void RtqsQueueDisc::setUpMPD(Ptr<Packet> packet,uint32_t source_id, uint32_t queue_id){
  // Find the last matching ByteTag.
  // This is because if there are multiple MPDs, they must be added in order, and the last one is the latest MPD.
  MySourceIDMPD MPD_tag;
  bool flag=false;
  ByteTagIterator byte_tag_it=packet->GetByteTagIterator();
  TypeId MPD_tag_tid=MPD_tag.GetInstanceTypeId();
  while (byte_tag_it.HasNext()){
    ByteTagIterator::Item cur_byte_tag_item = byte_tag_it.Next ();
    if (MPD_tag_tid == cur_byte_tag_item.GetTypeId ()){
      cur_byte_tag_item.GetTag (MPD_tag);
      flag=true;
    }
  }

  // If the current packet contains MDP, we need to compare whether the current queueing time needs to be added to the packet as MPD.
  if(flag){
    upMPD[source_id]=MPD_tag.MyGetTime();
    Time cur_queueing_time=getCurQueueingTime(queue_id);
    if (cur_queueing_time>MPD_tag.MyGetTime()){
      MPD_tag.MySetTime(cur_queueing_time);
      packet->AddByteTag(MPD_tag);
    }
  }
  // If not, we must put the current queueing time into packet as MPD.
  else {
    Time cur_queueing_time=getCurQueueingTime(queue_id);
    MPD_tag.MySetTime(cur_queueing_time);
    packet->AddByteTag(MPD_tag);
  }
}

void RtqsQueueDisc::setADU(uint32_t source_id,Time send_time){
  //set the value of ADU
  Time curADU=Simulator::Now()-send_time;
  ADU[source_id]=curADU;
  //update the value of min_ADU
  if (min_ADU.find(source_id)==min_ADU.end()){
    min_ADU[source_id]=curADU;
  }
  else {
    if(curADU<min_ADU[source_id]){
      min_ADU[source_id]=curADU;
    }
  }
}

uint32_t RtqsQueueDisc::setMPoint(Ptr<Packet> packet,uint32_t queue_id)
{
  // cur_deltaT =0 means that this queue almost no queuing time at present 
  // (the application packet sending rate is less than the available bandwidth).
  // So at that point, we do not need to adjust queue length.
  Time cur_deltaT=getSendPacketDeltaT(queue_id);
  if (cur_deltaT==Time(0)) return 0;

  MySourceIDTag tag_source_id;
  packet->FindFirstMatchingByteTag(tag_source_id);

  MySourceIDSendTime tag_send_time;
  packet->FindFirstMatchingByteTag(tag_send_time);


  uint32_t source_id=tag_source_id.Get();
  Time cur_MPD=std::max(upMPD[source_id],downMPD[source_id]);

  Time cur_queueing_time=getCurQueueingTime(queue_id);
  if(cur_queueing_time<cur_MPD){
    // Not the one with the largest delay currently.
    return 0;
  }

  Time cur_ADU;
  if (ADU.find(source_id)!=ADU.end()){
    cur_ADU=ADU[source_id];
  }
  else {
    cur_ADU=Time(0);
  }
  Time cur_ADD;
  if (ADD.find(source_id)!=ADD.end()){
    cur_ADD=ADD[source_id];
  }
  else {
    cur_ADD=Time(0);
  }
  Time cur_max_queueing_time;
  cur_max_queueing_time=getMaxQueueingTime(queue_id);

  if((cur_ADU+cur_ADD+cur_max_queueing_time-m_interval)<Time(0)){
    return 0;
  }
  m_point[queue_id]=(cur_ADU+cur_ADD+cur_max_queueing_time-m_interval).GetNanoSeconds();
  // std::cout<<"=====update m_point("<<queue_num<<")===="<<std::endl;
  // std::cout<<"cur_ADU:"<<1.0*cur_ADU.GetNanoSeconds()/1e6<<"ms   cur_ADD:"<<1.0*cur_ADD.GetNanoSeconds()/1e6
  //   <<"ms   cur_max_queueing_time:"<<1.0*cur_max_queueing_time.GetNanoSeconds()/1e6<<"ms   m_interval:"
  //     <<1.0*m_interval.GetNanoSeconds()/1e6<<"ms   m_point:"<<m_point[queue_num]<<std::endl;
  return m_point[queue_id];
} 

void RtqsQueueDisc::setFairBandwidthShare(){
  // Obtain the flow count of each queue. Note that the detection queue and the black house queue will not 
  // participate in the calculation of fair bandwidth, so we no need to obtain this two flow count.
  uint32_t cur_flow_count[7];
  uint32_t cur_flow_count_sum=0;
  for (uint32_t queue_id=1;queue_id<=5;queue_id++){
    cur_flow_count[queue_id]=getEnqueueFlowCount(queue_id);
    cur_flow_count_sum+=cur_flow_count[queue_id];
  }
  if (cur_flow_count_sum!=0){
    for (uint32_t queue_id=1;queue_id<=5;queue_id++){
      fair_bandwidth_share[queue_id]=(double)cur_flow_count[queue_id]/cur_flow_count_sum*full_rate;
    }
  }
  else {
    for (uint32_t queue_id=1;queue_id<=5;queue_id++){
      fair_bandwidth_share[queue_id]=0;
    }
  }
}

void RtqsQueueDisc::setSendPacketDeltaT(uint32_t queue_id){
  // Get current send packet deltaT for queue: queue_id.
  Time new_interval_time=Time(0);
  if (send_packet_deltaT_last_record.find(queue_id)==send_packet_deltaT_last_record.end()){
    send_packet_deltaT_last_record[queue_id]=Simulator::Now();
  }
  // If the number of queueing packets for queue: queue_id is too small (<10), we believe that it is not the 
  // bandwidth that limits the packet send interval, but the packet sending rate of the current flow,
  // so we believe that the packet sending interval can be 0.
  else if (GetInternalQueue(queue_id)->GetCurrentSize().GetValue()<10){
    send_packet_deltaT_last_record[queue_id]=Simulator::Now();
  }
  else {
    new_interval_time=Simulator::Now()-send_packet_deltaT_last_record[queue_id];
    send_packet_deltaT_last_record[queue_id]=Simulator::Now();
  }

  // Get current send packet interval recorder for queue: queue_id.
  if (send_packet_deltaT_queue.find(queue_id)==send_packet_deltaT_queue.end()){
    send_packet_deltaT_queue[queue_id]=std::queue<Time>();
    send_packet_deltaT_sum[queue_id]=Time(0);
    // At the begining, we assume that the packet_send_deltaT is zero, it means the queueing time is zero
    send_packet_deltaT[queue_id]=Time(0);
  }
  std::queue<Time>& cur_queue_for_packet_send_deltaT=send_packet_deltaT_queue[queue_id];

  // Update the value of packet_send_deltaT for queue: queue_id
  if (cur_queue_for_packet_send_deltaT.size()<send_packet_deltaT_window){
    cur_queue_for_packet_send_deltaT.push(new_interval_time);
    send_packet_deltaT_sum[queue_id]=send_packet_deltaT_sum[queue_id]+new_interval_time;
  }
  else {
    Time front_deltaT=cur_queue_for_packet_send_deltaT.front();
    send_packet_deltaT_sum[queue_id]=send_packet_deltaT_sum[queue_id]-front_deltaT;
    cur_queue_for_packet_send_deltaT.pop();
    cur_queue_for_packet_send_deltaT.push(new_interval_time);
    send_packet_deltaT_sum[queue_id]=send_packet_deltaT_sum[queue_id]+new_interval_time;
  }
  send_packet_deltaT[queue_id]=send_packet_deltaT_sum[queue_id]/cur_queue_for_packet_send_deltaT.size();
}

Time RtqsQueueDisc::getSendPacketDeltaT(uint32_t queue_id){
  if (send_packet_deltaT.find(queue_id)!=send_packet_deltaT.end()){
    return send_packet_deltaT[queue_id];
  }
  else {
    // For default, we assume that the packet_send_deltaT is zero, it means the queueing time is zero
    return Time(0);
  }
}

bool RtqsQueueDisc::judgeIfMaxFlowReject(uint32_t source_id, uint32_t type_id){
  // We will never do a maximum flow reject on the detection queue.
  if (type_id==0) return false;

  // Statistics on the total uplink throughput of each flow (regardless of which queue it comes from).
  // Sometimes a flow may send packets through multiple queues. (e.g.: non-realtime queue and black house queue).
  // The value of total_dequeue_throughput is source_id -> packets count.
  std::unordered_map<uint32_t,int32_t> total_dequeue_throughput;
  // Count how much flow sent by each queue.
  uint32_t flow_count[7]={0,0,0,0,0,0,0};
  for (uint32_t queue_id=0; queue_id<=6; queue_id++){
    if (dequeue_throughput.find(queue_id)==dequeue_throughput.end()){
      dequeue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
    }
    std::unordered_map<uint32_t, int32_t>& cur_dequeue_throughput=dequeue_throughput[queue_id];
    for (auto it=cur_dequeue_throughput.begin();it!=cur_dequeue_throughput.end();it++){
      total_dequeue_throughput[it->first]+=it->second;
      if (it->first!=1e9+7&&it->second>0) flow_count[queue_id]++;
    }
  }

  // Count the total throughput (pakcets sent by this router) within one time window.
  // This value can represet the bandwidth of upload link for this router when throughput is full.
  int32_t total_pkt_count=0;
  // Count how much flow sent by this router.
  int32_t total_flow_count=0;
  for (auto it=total_dequeue_throughput.begin();it!=total_dequeue_throughput.end();it++){
    total_pkt_count+=it->second;
    if (it->first!=1e9+7&&it->second>0) total_flow_count++;
  }

  // Obtain current queue_id for flow source_id.
  uint32_t queue_id=getQueueId(source_id, type_id);

  // If the current queue contains only one flow, no maximum flow reject is required.
  // Maximum flow reject only maintains the fairness of different flows in one queue, 
  // and the fairness between queues is maintained by random packet sending mechanism(TODO:Check me!!!).
  if (flow_count[queue_id]<=1) return false;

  uint32_t queue_pkt_count=0;
  // The minimum throughput flow in the current queue (packets count).
  int32_t queue_min_pkt_count=1e9;
  // Get current dequeue throughput recorder for Queue: queue_id.
  std::unordered_map<uint32_t, int32_t>& cur_dequeue_throughput=dequeue_throughput[queue_id];
  for (auto it=cur_dequeue_throughput.begin();it!=cur_dequeue_throughput.end();it++){
    if (it->first!=1e9+7&&it->second>0){
      queue_pkt_count+=it->second;
      queue_min_pkt_count=std::min(queue_min_pkt_count, it->second);
    }
  }

  // First condition: The number of queueing packets reaches half of the current queue length (congestion may occurs).
  bool flag1=queue_pkt_count>=getBuffer(queue_id).GetValue()/2;
  // Second condition: The total throughput occupied by current flow is greater than max_flow_reject_alpha times the fair bandwidth. 
  bool flag2=total_dequeue_throughput[source_id]>total_pkt_count/total_flow_count*max_flow_reject_alpha;
  // Third condition: The throughput of the current flow through this queue is greater than max_flow_reject_beta times
  // the throughput of the minimum throughput in this queue.
  bool flag3=cur_dequeue_throughput[source_id]>queue_min_pkt_count*max_flow_reject_beta;

  if (flag1&&flag2&&flag3){
    return true;
  }
  else {
    return false;
  }
}

void RtqsQueueDisc::UpdatePktCount(uint32_t source_id,uint32_t queue_id,uint32_t update_num){
  // Get current packets count recorder for Queue: queue_id.
  if (queue_pkt_count.find(queue_id)==queue_pkt_count.end()){
    queue_pkt_count[queue_id]=std::unordered_map<uint32_t, int32_t>();
  }
  std::unordered_map<uint32_t, int32_t>& cur_queue_pkt_count=queue_pkt_count[queue_id];

  // Update the value of queue_pkt_count for current flow (distinguished by source_id).
  if (cur_queue_pkt_count.find(source_id)!=cur_queue_pkt_count.end()){
    cur_queue_pkt_count[source_id]+=update_num;
  }
  else {
    cur_queue_pkt_count[source_id]=update_num;
  }
}

void RtqsQueueDisc::setIfMaxFlowReject(uint32_t source_id,bool update_num){
  if_max_flow_reject[source_id]=update_num;
}

void RtqsQueueDisc::setMaxFlowRejectTimes(uint32_t source_id,int32_t update_num){
  max_flow_reject_times[source_id]=max_flow_reject_times[source_id]+update_num;
}

void RtqsQueueDisc::setFlowStart(uint32_t source_id){
  flow_status[source_id]=1;
}

void RtqsQueueDisc::writeDequeueThroughput(){
  // Statistics on the total uplink throughput of each flow (regardless of which queue it comes from).
  // Sometimes a flow may send packets through multiple queues. (e.g.: non-realtime queue and black house queue).
  // The value of total_dequeue_throughput is source_id -> packets count.
  std::unordered_map<uint32_t,int32_t> total_dequeue_throughput;
  for (uint32_t queue_id=0; queue_id<=6; queue_id++){
    if (dequeue_throughput.find(queue_id)==dequeue_throughput.end()){
      dequeue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
    }
    std::unordered_map<uint32_t, int32_t>& cur_dequeue_throughput=dequeue_throughput[queue_id];
    for (auto it=cur_dequeue_throughput.begin();it!=cur_dequeue_throughput.end();it++){
      total_dequeue_throughput[it->first]+=it->second;
    }
  }

  // Output the dequeue throughput of each flow to the file.
  std::ofstream flowCountFs(qlog_dir+"flowPktCount.dat",std::ios::out|std::ios::app);
  for (uint32_t ii=0;ii<=3;ii++){
    if (ii!=3) flowCountFs << total_dequeue_throughput[ii] << " ";
    else flowCountFs << total_dequeue_throughput[ii] << std::endl;
  }
}

void RtqsQueueDisc::writeEnqueueThroughput(){
  // Statistics on the total downlink throughput of each flow (regardless of which queue it comes from).
  // Sometimes a flow may send packets through multiple queues. (e.g.: non-realtime queue and black house queue).
  // The value of total_enqueue_throughput is source_id -> packets count.
  std::unordered_map<uint32_t,int32_t> total_enqueue_throughput;
  for (uint32_t queue_id=0; queue_id<=6; queue_id++){
    if (enqueue_throughput.find(queue_id)==enqueue_throughput.end()){
      enqueue_throughput[queue_id]=std::unordered_map<uint32_t, int32_t>();
    }
    std::unordered_map<uint32_t, int32_t>& cur_enqueue_throughput=enqueue_throughput[queue_id];
    for (auto it=cur_enqueue_throughput.begin();it!=cur_enqueue_throughput.end();it++){
      total_enqueue_throughput[it->first]+=it->second;
    }
  }

  // Output the enqueue throughput of each flow to the file.
  std::ofstream flowCountFs(qlog_dir+"sourcePktCount.dat",std::ios::out|std::ios::app);
  for (uint32_t ii=0;ii<=3;ii++){
    if (ii!=3) flowCountFs << total_enqueue_throughput[ii] << " ";
    else flowCountFs << total_enqueue_throughput[ii] << std::endl;
  }
}

void RtqsQueueDisc::writeMaxFlowReject(uint32_t source_id,Time now_time){
  std::ofstream file(qlog_dir+"MaxFlowReject.dat",std::ios::out|std::ios::app);
  file << now_time<<" "<<source_id << std::endl;
}

void RtqsQueueDisc::writePktLoss(uint32_t source_id,Time now_time){
  std::ofstream file(qlog_dir+"PktLoss.dat",std::ios::out|std::ios::app);
  file << now_time<<" "<<source_id << std::endl;
}

void RtqsQueueDisc::writeQueuePktCount(){
  // Statistics on the total packets count of each flow (regardless of which queue it comes from).
  // Sometimes a flow may send packets through multiple queues. (e.g.: non-realtime queue and black house queue).
  // The value of total_queue_pkt_count is source_id -> packets count.
  std::unordered_map<uint32_t,int32_t> total_queue_pkt_count;
  // Count queueing packets for each queue.
  uint32_t pkt_count[7]={0,0,0,0,0,0,0};
  for (uint32_t queue_id=0; queue_id<=6; queue_id++){
    if (queue_pkt_count.find(queue_id)==queue_pkt_count.end()){
      queue_pkt_count[queue_id]=std::unordered_map<uint32_t, int32_t>();
    }
    std::unordered_map<uint32_t, int32_t>& cur_queue_pkt_count=queue_pkt_count[queue_id];
    for (auto it=cur_queue_pkt_count.begin();it!=cur_queue_pkt_count.end();it++){
      total_queue_pkt_count[it->first]+=it->second;
      pkt_count[queue_id]+=it->second;
    }
  }

  // Output the packets count of each flow to the file.
  std::ofstream file(qlog_dir+"PktCount.dat",std::ios::out|std::ios::app);
  for (uint32_t queue_id=0;queue_id<=6;queue_id++){
    file<<pkt_count[queue_id]<<" ";
  }
  for (uint32_t source_id=0;source_id<=3;source_id++){
    if (source_id!=3) file << total_queue_pkt_count[source_id] << " ";
    else file << total_queue_pkt_count[source_id] << std::endl;
  }
}

void RtqsQueueDisc::writeQueueBufferSize(){
  std::ofstream file(qlog_dir+"CurBufSize.dat",std::ios::out|std::ios::app);
  for (uint32_t queue_id=0;queue_id<=6;queue_id++){
    if (queue_id!=6) file<<queue_buffer_size[queue_id]<<" ";
    else file<<queue_buffer_size[queue_id]<<std::endl;
  }
}

void RtqsQueueDisc::writeFlowCount(){
  // Obtain the flow count of each queue.
  uint32_t cur_flow_count[7];
  for (uint32_t queue_id=0;queue_id<=6;queue_id++){
    cur_flow_count[queue_id]=getEnqueueFlowCount(queue_id);
  }
  std::ofstream file(qlog_dir+"flowcount.dat",std::ios::out|std::ios::app);
  for (uint32_t queue_id=0;queue_id<=6;queue_id++){
    if (queue_id!=6) file<<cur_flow_count[queue_id]<<" ";
    else file<<cur_flow_count[queue_id]<<std::endl;
  }
}

void RtqsQueueDisc::writeQueueingTime(){
  std::ofstream file(qlog_dir+"queueing_time.dat",std::ios::out|std::ios::app);
  for (uint32_t queue_id=0;queue_id<=6;queue_id++){
    Time queueing_time=getCurQueueingTime(queue_id);
    if (queue_id!=6) file<<queueing_time<<" ";
    else file<<queueing_time<<std::endl;
  }
}

void RtqsQueueDisc::writeRbufResize(uint32_t queue_id,uint32_t queue_size_from,uint32_t queue_size_to){
  std::ofstream flowCountFs(qlog_dir+"RBufResize.dat",std::ios::out|std::ios::app);
  flowCountFs<<Simulator::Now().GetSeconds()<<" "<<queue_id<<" "<<queue_size_from<<" "<<queue_size_to<<std::endl;
}

uint32_t RtqsQueueDisc::getQueueId(uint32_t source_id,uint32_t type_id){
  if (type_id==0){
    return 0;
  }
  else if (type_id==1){
    Time cur_important_time=min_ADD[source_id]+min_ADU[source_id];
    if (cur_important_time<=m_important_delay_queue1){
      return 1;
    }
    else if (cur_important_time<=m_important_delay_queue2){
      return 2;
    }
    else if (cur_important_time<=m_important_delay_queue3){
      return 3;
    }
    else {
      return 5;
    }
  }
  else if (type_id==2){
    return 4;
  }
  else if (type_id==3){
    return 5;
  }
  else if (type_id==4){
    return 6;
  }
}

uint32_t RtqsQueueDisc::getCurQueueLen(uint32_t queue_id){
  return getBuffer(queue_id).GetValue();
}

// Get the queueing time for queue with id as queue_id
Time RtqsQueueDisc::getCurQueueingTime(uint32_t queue_id){
  // Maybe this queue is not built yet.
  if (GetNInternalQueues()<=queue_id){
    return Time(0);
  }
  else{
    uint32_t queueing_packet_size=GetInternalQueue(queue_id)->GetCurrentSize ().GetValue();
    Time cur_deltaT=getSendPacketDeltaT(queue_id);
    return queueing_packet_size*cur_deltaT;
  }

}

Time RtqsQueueDisc::getMaxQueueingTime(uint32_t queue_id){
  uint32_t max_queueing_size=getCurQueueLen(queue_id);
  Time cur_deltaT=getSendPacketDeltaT(queue_id);
  return max_queueing_size*cur_deltaT;
}

}// namespace ns3
