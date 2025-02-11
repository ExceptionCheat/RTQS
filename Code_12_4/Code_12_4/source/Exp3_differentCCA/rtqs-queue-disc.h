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

#ifndef RTQS_QUEUE_DISC_H
#define RTQS_QUEUE_DISC_H

#include <unordered_map>
#include <algorithm>
#include <iostream>
#include <cmath>
#include <queue>
#include <fstream>

#include "ns3/log.h"
#include "ns3/object-factory.h"
#include "ns3/drop-tail-queue.h"
#include "ns3/queue-disc.h"
#include "ns3/simulator.h"

#include "MySourceIDMPD.h"
#include "MySourceIDType.h"
#include "MySourceIDTag.h"
#include "MySourceIDSendTime.h"
#include "Exp3_differentCCA.h"

namespace ns3 {

#define MY_MIN_QUEUE 50
#define MY_MAX_QUEUE 4000

class RtqsQueueDisc : public QueueDisc {
public:
  static TypeId GetTypeId (void);
  RtqsQueueDisc ();
  virtual ~RtqsQueueDisc();

  // Reasons for dropping packets
  static constexpr const char* LIMIT_EXCEEDED_DROP = "Queue disc limit exceeded";  //!< Packet dropped due to queue disc limit exceeded.
  static constexpr const char* TIME_OUT = "Realtime Packet timeout";  //!< Packet dropped due to packet timeout for realtime application.

  // To set the default length of queues.
  // Called by setAttribute (named default_detection_buffer_length, default_realtime_buffer_length, default_non_realtime_buffer_length,
  // default_other_buffer_length, default_black_house_buffer_length)
  void setDefaultDetectionBuffer(uint32_t);             //detection queue!
  void setDefaultRealtimeBuffer(uint32_t);              //realtime queue (include 3 level queues)!
  void setDefaultNonRealtimeBuffer(uint32_t);           //non-realtime queue!
  void setDefaultOtherBuffer(uint32_t);                 //other queue!
  void setDefaultBlackHouseBuffer(uint32_t);            //black house queue!

  // Getter and Setter for variable queue size!
  QueueSize getBuffer(uint32_t);      // for all queues, include detection queue, 3 realtime queues ...
  void setBuffer(int32_t, uint32_t);  // for all queues, include detection queue, 3 realtime queues ...

  // Update the value of enqueue_throughput,
  // called by setAttribute (named update_enqueue_throughput).
  void updateEnqueueThroughput(uint32_t);
  uint32_t source_id_for_update_enqueue_throughput{0};
  uint32_t queue_id_for_update_enqueue_throughput{0};
  int32_t update_num_for_update_enqueue_throughput{0};
  // The underlying method to set/update the value of enqueue_throughput,
  // called by doEnqueue and updateEnqueueThroughput.
  void setEnqueueThroughput(uint32_t,uint32_t,int32_t);
  // Obtain current flow count for each queue based on the number of keys in enqueue_throughput,
  // called by setFairBandwidthShare, used to allocate fair bandwidth of different queues.
  uint32_t getEnqueueFlowCount(uint32_t);

  // Update the value of dequeue_throughput,
  // called by setAttribute (named update_dequeue_throughput).
  void updateDequeueThroughput(uint32_t);
  uint32_t source_id_for_update_dequeue_throughput{0};
  uint32_t queue_id_for_update_dequeue_throughput{0};
  int32_t update_num_for_update_dequeue_throughput{0};
  // The underlying method to set/update the value of dequeue_throughput,
  // called by doDequeue and updateDequeueThroughput
  void setDequeueThrouthput(uint32_t,uint32_t,int32_t);
  // Obtain current flow count for each queue based on the number of keys in dequeue_throughput, called by doEnqueue, 
  // to judge if each queue can be allocate available bandwidth to send packets, especially the black house queue.
  uint32_t getDequeueFlowCount(uint32_t);

  // The underlying method to set/update the value of queue_pkt_count,
  // called when doEnqueue(+1) and doDequeue(-1).
  void UpdatePktCount(uint32_t sid,uint32_t qid,uint32_t delta);

  // Update the value of if_max_flow_reject,
  // called by setAttribute (named update_if_max_flow_reject).
  void updateIfMaxFlowReject(uint32_t);
  uint32_t source_id_for_update_if_max_flow_reject{0};
  // The underlying method to set/update the value of if_max_flow_reject,
  // called by doEnqueue and updateIfMaxFlowReject.
  void setIfMaxFlowReject(uint32_t,bool);

  // Update the value of max_flow_reject_times,
  // called by setAttribute (named update_max_flow_reject_times).
  void updateMaxFlowRejectTimes(uint32_t);
  uint32_t source_id_for_update_max_flow_reject_times{0};
  // The underlying method to set/update the value of max_flow_reject_times,
  // called by doEnqueue and updateMaxFlowRejectTimes.
  void setMaxFlowRejectTimes(uint32_t,int32_t);

  // Update the value of flow_status, 
  // called by setAttribute (named update_flow_status).
  // Specifically, set the state of this flow to 2, 
  // which means that the this has entered a stable period.
  void updateFlowStatus(uint32_t);
  uint32_t source_id_for_update_flow_status{0};
  // Set the state of this flow to 1, which means this flow is just started,
  // called when the first run for doenqueue for each flow.
  void setFlowStart(uint32_t);

  // Write logs about rtqs-queue-disc to files,
  // called by setAttribute (named writeQLogFiles).
  void writeQLogFiles(uint32_t);
  // The underlying methods to write logs to files, include enqueue_throughput, dequeue_throughput,
  // queue_pkt_count, queue buffer size, flow count and queueing time for each queue. All methods called by writeQLogFiles.
  void writeDequeueThroughput();
  void writeEnqueueThroughput();
  void writeQueuePktCount();
  void writeQueueBufferSize();
  void writeFlowCount();
  void writeQueueingTime();
  // Write logs about the buffer resize event to files,
  // called when doEnqueue, more specifically, when the buffer resize event occurs.
  void writeRbufResize(uint32_t, uint32_t, uint32_t);
  // Write logs infomation when max flow reject performed. 
  void writeMaxFlowReject(uint32_t, Time);
  // Write logs when a packet lost in this router.
  void writePktLoss(uint32_t, Time);


  // The underlying method to set/update the value of send_pakcet_deltaT 
  // (as well as send_packet_deltaT_sum and send_packet_deltaT_queue), 
  // to make sure the queueing time, m_point calculating and queue length increasement is correct!
  void setSendPacketDeltaT(uint32_t);

  // Update the value of ADD and downMPD when the ack packets arrives,
  // called by setAttribute and getAttribute method (named update_ADD_and_MPD).
  void updateADDAndDownMPD(Time);   
  Time getPerQueueingTime(void) const;
  uint32_t source_id_for_update_ADD_and_MPD{0};
  uint32_t type_id_for_update_ADD_and_MPD{0};
  Time MPD_value{Time(0)};
  Time ADD_value{Time(0)};
  Time per_queueing_time{Time(0)};

  // The underlying method to set/update the value of ADU,
  // called when doEnqueue, for only realtime flow.
  void setADU(uint32_t, Time);

  // The underlying method to set/update the value of upMPD,
  // called when doEnqueue, for only realtime flow.
  void setUpMPD(Ptr<Packet>, uint32_t, uint32_t);

  // Periodically increases the queue size of the real-time queues, 
  // called by setAttribute (named queue_len_increase).
  void increaseQueueLen(uint32_t);
  uint32_t queue_id_for_queue_len_increase{0};

  // Obtain current send_packet_deltaT with queue_id, called by setMPoint, getCurrentQueueingTime,
  // getMaxQueueingTime, doEnqueue and increaseQueueLen. 
  // Reflects the current packet sending interval between two packets. 
  // Value of 0 means that there is almost no queuing time at present 
  // (the application packet sending rate is less than the available bandwidth).
  Time getSendPacketDeltaT(uint32_t);

  // The underlying method to calculate/update the value of m_point,
  // called when doEnqueue, for only realtime flow.
  uint32_t setMPoint(Ptr<Packet>,uint32_t);

  // The underlying method to calculate/update the fair bandwidth share for each queue, 
  // called when doDequeue.
  void setFairBandwidthShare(void);

  // Default Uinteger Getter, which provides a default method for getAttribute.
  uint32_t defaultUintegerGetter()const;

  // Determines whether the current flow should be performed maximum flow reject by source_id and type_id.
  // called when doEnqueue.
  bool judgeIfMaxFlowReject(uint32_t, uint32_t);

  // Obtain the queueing time of queue: queue_id (estimated by send_packet_deltaT).
  // called by setUpMPD, setMPoint and writeQueueingTime. 
  Time getCurQueueingTime(uint32_t);
  // Obtain the maximum possible queueing time of queue: queue_id (estimated by send_packet_deltaT and queue length).
  // called by setMPoint.
  Time getMaxQueueingTime(uint32_t);

  // To determine which queue this flow should enter by source_id and type_id,
  // called by doEnqueue and judgeIfMaxFlowReject.
  uint32_t getQueueId(uint32_t, uint32_t);

  // Obtain the queue length by queue_id,
  // called by doEnqueue and getMaxQueueingTime.
  uint32_t getCurQueueLen(uint32_t);

private:
  // Base function defined by class QueueDisc.
  virtual bool DoEnqueue (Ptr<QueueDiscItem> item);
  virtual Ptr<QueueDiscItem> DoDequeue (void);
  virtual Ptr<const QueueDiscItem> DoPeek (void);
  virtual bool CheckConfig (void);
  virtual void InitializeParams (void);
  // Selects the packet from the front of specified queue.
  Ptr<QueueDiscItem> dequeuePacket(uint32_t);

  // Record the throughput (in packets) for each queue, and further record the throughput (in packets) 
  // for each flow (distinguided by source_id), within one time window. 
  // This value is updated when packets enqueue, so it represents the downlink throughput for the router.
  // The value of enqueue_throughput include two level map, 
  // the first level is: queue_id -> unorder_map, the second level is source_id -> packets count.
  std::unordered_map<uint32_t, std::unordered_map<uint32_t, int32_t>> enqueue_throughput;

  // Record the throughput (in packets) for each queue, and further record the throughput (in packets) 
  // for each flow (distinguided by source_id), within one time window. 
  // This value is updated when packets dequeue, so it represents the uplink throughput for the router.
  // The value of dequeue_throughput include two level map, 
  // the first level is: queue_id -> unorder_map, the second level is source_id -> packets count.
  std::unordered_map<uint32_t, std::unordered_map<uint32_t, int32_t>> dequeue_throughput;

  // Record the current packets count for each queue, and further record the packets count
  // for each flow (distinguided by source_id). 
  // This value is updated when packets enqueue (+1) and dequeue (-1).
  // The value of queue_pkt_count include two level map, 
  // the first level is: queue_id -> unorder_map, the second level is source_id -> packets count.
  std::unordered_map<uint32_t, std::unordered_map<uint32_t, int32_t>> queue_pkt_count;

  // Record whether the maximum flow reject operation has been performed in a time interval.
  // This interval is determined by the reject_function.
  // This value is updated when do max flow reject.
  // The value of if_max_flow_reject is: source_id -> bool
  std::unordered_map<uint32_t, bool> if_max_flow_reject;

  // Record the number of times that maximum flow reject operation has been performed within one time window (not the time interval above).
  // This value is updated when do max flow reject.
  // The value of max_flow_reject_times is: source_id -> reject times.
  std::unordered_map<uint32_t, int32_t> max_flow_reject_times;

  // Record the status of each flow.
  // This value is updated when the first run for doenqueue for each flow (distingushed by source_id).
  // The value of flow_status is: source_id -> flow status.
  // Flow status inclue three position:
  // 0 - This flow is not start yet.
  // 1 - This flow is just started and may be going through the slow start phase of the congestion control algorithm (not to do max flow reject).
  // 2 - This flow has been started for more than a time window and we consider it to have entered a stable period (maximum flow reject can be performed).
  std::unordered_map<uint32_t, int32_t> flow_status;

  // Calculates the average interval between sending two packets in each queue,
  // which is used to calculate the m_point value and control the frequency of queue length increasement.
  // Due to the randomness of packet sending between queues, the average value of the latest send_packet_deltaT_window packet 
  // sending interval is selected, noted as send_packet_deltaT. And send_packet_deltaT_sum is the sum of these sending interval,
  // send_packet_deltaT_queue is the sending interval value recorder, send_packet_deltaT_last_record is the timestamp for last recording.
  // These values are updated when packet dequeue.
  // The value of send_packet_deltaT is: queue_id -> Time (average sending interval time).
  // The value of send_packet_deltaT_sum is: queue_id -> Time (sum of latest send_packet_deltaT_window sending interval time).
  // The value of send_packet_deltaT_queue is: queue_id -> Queue<Time> (each sending interval time recorded).
  // The value of send_packet_deltaT_last_record is: queue_id -> Queue<Time> (last record timestamp).
  // That is: send_packet_deltaT[queue_id]=send_packet_deltaT_sum[queue_id]/len(send_packet_deltaT_sum[queue_id]),
  // send_packet_deltaT_sum[queue_id]=sum{send_packet_deltaT_queue[queue_id]},
  // and new_interval_time= Simulator::Now() - send_packet_deltaT_last_record.
  std::unordered_map<uint32_t, Time> send_packet_deltaT;
  std::unordered_map<uint32_t, Time> send_packet_deltaT_sum;
  std::unordered_map<uint32_t, std::queue<Time>> send_packet_deltaT_queue;
  std::unordered_map<uint32_t, Time> send_packet_deltaT_last_record;
  uint32_t send_packet_deltaT_window=100;
  // By default, the queue length increases by 1 every 10ms.
  // Note that, if the value of send_packet_deltaT is not equal to 0, the queue length will increases by 1 
  // after each 10 packets sent or defalut_increase_queue_len_interval, that is, every max(send_packet_deltaT x 10 ms, 10ms);
  // This is because if the current packet sending rate of the application is less than the available bandwidth, 
  // send_packet_deltaT may go to 0, which will enter an endless loop to increase queue length.
  Time defalut_increase_queue_len_interval{"10ms"};

  //1、point：每个队列维护一个point  //队列参数
  std::unordered_map<uint32_t, uint64_t> m_point;


  // Indicates the current queue length. There are 7 queues in total.
  // 0 - Detection queue. The default queue length is 200p.
  // 1~3 - Realtime queue. The default queue length is 200p
  // 4 - Non-realtime queue. The default queue length is 1000p
  // 5 - Other queue, the default queue length is 500p
  // 6 - Black house queue, the default queue length is 500p
  // The default queue length can be set by setAttribute (named DbufSize, RbufSize, NbufSize, ObufSize)
  QueueSize queue_buffer_size[7]={QueueSize("200p"),QueueSize("200p"),QueueSize("200p"),QueueSize("200p"),
                                  QueueSize("1000p"),QueueSize("500p"),QueueSize("500p")};

  // Record the fair bandwidth share of each queue.
  // This value will recalculat before each packet is sent (dequeue).
  // The value of fair_bandwidth_share is: queue_id -> fair_bandwidth_share (double).
  // It means the fair share bandwidth of queue: queue_id is: fair_bandwidth_share[queue_id]*100%
  std::unordered_map<uint32_t, double> fair_bandwidth_share;
  // The full bandwidth is 100%.
  double full_rate{100};

  // To distinguish different realtime queues based on independent delay. (TODO:Check me!!!)
  Time m_important_delay_queue1{30000000};        //30ms
  Time m_important_delay_queue2{80000000};        //80ms
  Time m_important_delay_queue3{150000000};       //150ms

  // Record the value of MDP for each flow.
  // UpMPD is the maximum hop delay (MPD) experienced by a flow before it reaches this router,
  // and downMPD is the maximum hop delay experienced by a flow after it leaves this router to the destination host.
  // UpMPD is updated by data Packet sent to destination host, and downMDP is updated by ack Packet sent back.
  // For flow source_id, the true MPD is max(upMPD[source_id], downMPD[source_id]).
  // The value of both upMPD and downMPD is source_id -> MPD(Time).
  std::unordered_map<uint32_t, Time> upMPD;
  std::unordered_map<uint32_t, Time> downMPD;

  // Record the value of ADD for each flow.
  // ADD is current ADD (TODO:FIXME)
  std::unordered_map<uint32_t, Time> min_ADD;
  std::unordered_map<uint32_t, Time> ADD;

  // Record the value of ADU for each flow.
  // TODO:FIXME
  std::unordered_map<uint32_t, Time> min_ADU;
  std::unordered_map<uint32_t, Time> ADU;

  // Latency requirements for real-time streams.
  // We believe that the total queue time should be less than this value.
  // That is ADU+ADD+queueing_time<m_interval.
  // This value can be set by setAttribute (named m_interval).
  Time m_interval=Time("200ms");

  // Whether to enable maximum flow reject.
  // This value can be set by setAttribute (named if_max_flow_reject_enable).
  bool if_max_flow_reject_enable{true};

  // The maximum number of times that the same flow performs the maximum flow reject with time interval in a time window.
  // If the number of performed times is less than this value, the time interval is determined by the max_flow_reject_function.
  // If the number of performed times is greater than this value (within a time window), 
  // all newly arrived packets will be placed in the black house queue.
  // This value can be set by setAttribute (named max_reject_times_thread).
  uint32_t max_reject_times_threshold{7};

  // The maximum flow reject function, which is a function pointer.
  // Input: the number of times of this flow performed max flow reject in one time window.
  // Output: the time interval between this and the next max flow reject.
  // This function is recommended as a monotonically decreasing function, and the value must be greater than 0.
  // This value can be set by setAttribute (named max_flow_reject_function).
  Callback<double, uint32_t> max_flow_reject_function;

  // Hyperparameter alpha that controls the maximum flow reject trigger condition, 
  // which means that the throughput of current global maximum flow must be greater than the fair bandwidth 
  // max_flow_reject_alpha times to trigger the maximum flow reject.
  // This value can be set by setAttribute (named max_flow_reject_alpha).
  double_t max_flow_reject_alpha{1.2};
  // Hyperparameter beta that controls the maximum flow reject trigger condition, 
  // which means that the maximum flow reject is triggered only when the throughput of the current maximum flow 
  // is max_flow_reject_beta times that of the minimum flow in corresponding queue. 
  // (if the current flow monopolizes a queue, this condition is not considered)
  // This value can be set by setAttribute (named max_flow_reject_beta).
  double_t max_flow_reject_beta{1.2};

  // The directory where the rtqs-queue-disc log files are stored.
  // The log files include: TODO:FIXME
  // This value can be set by setAttribute (named qlog_dir).
  std::string qlog_dir;
};

} // namespace ns3

#endif