import numpy as np
import matplotlib.pyplot as plt
from scipy.signal import savgol_filter

def calDraw(arr,step):
    n=len(arr)
    ret=[]
    for i in range(0,n):
        cnt=0
        sum=0
        for j in range(max(0,i-step),min(n,i+1)):
            cnt+=1
            sum+=arr[j]
        ret.append(sum/cnt)
    return ret

ifRTQS=False;
ifPktLoss=False;
ifMaxReject=False;

basedir="./ns3::FifoQueueDisc"
# 假设.dat文件中存储的是以空格分隔的浮点数，表示二维数据点
rtt_data = np.loadtxt(basedir+'/rtt_100.dat')
throught_data = np.loadtxt(basedir+'/bottleneck_tpt_100.dat')
if(ifRTQS):data = np.loadtxt(basedir+'/PktCount.dat')
if (ifRTQS and ifMaxReject):maxFlowReject=np.loadtxt(basedir+'/MaxFlowReject.dat',dtype='str');
if(ifRTQS and ifPktLoss):PktLoss=np.loadtxt(basedir+'/PktLoss.dat',dtype='str');
if(ifRTQS):flowPktCount=np.loadtxt(basedir+'/flowPktCount.dat');
if(ifRTQS):sourcePktCount=np.loadtxt(basedir+'/sourcePktCount.dat');
jfiValue=np.loadtxt(basedir+'/jfi_100.dat');
# 假设数据是二维的，第一列为X轴数据，第二列为Y轴数据
if(ifRTQS):
    queue1 = data[:,7]
    queue2 = data[:, 8]
    queue3 = data[:, 9]
    queue4 = data[:, 10]

# 假设数据是二维的，第一列为X轴数据，第二列为Y轴数据
flow1_rtt = rtt_data[:, 0]
flow2_rtt = rtt_data[:, 1]
flow3_rtt = rtt_data[:, 2]
flow4_rtt = rtt_data[:, 3]

flow1_throught = throught_data[:, 0]
flow2_throught = throught_data[:, 1]
flow3_throught = throught_data[:, 2]
flow4_throught = throught_data[:, 3]

if(ifRTQS):
    flow1_PktCount=flowPktCount[:,0];
    flow2_PktCount=flowPktCount[:,1];
    flow3_PktCount=flowPktCount[:,2];
    flow4_PktCount=flowPktCount[:,3];

    source1_PktCount=sourcePktCount[:,0];
    source2_PktCount=sourcePktCount[:,1];
    source3_PktCount=sourcePktCount[:,2];
    source4_PktCount=sourcePktCount[:,3];

curJFI=jfiValue[:,0];

if (ifRTQS and ifMaxReject):
    RejectTime=maxFlowReject[:,0];
    RejectTime=[eval(i[:-2])/1e8 for i in RejectTime];
    RejectFlow=maxFlowReject[:,1];
    RejectFlow=[eval(i) for i in RejectFlow];

if (ifRTQS and ifPktLoss):
    LossTime=PktLoss[:,0];
    LossTime=[eval(i[:-2])/1e8 for i in LossTime];
    LossFlow=PktLoss[:,1];
    LossFlow=[eval(i) for i in LossFlow];

window_size = 200
pkt_window_size=10;

#做平滑处理1
# smoothed_rtt1 = np.convolve(flow1_rtt, np.ones(window_size) / window_size, mode='valid')
# smoothed_rtt2 = np.convolve(flow2_rtt, np.ones(window_size) / window_size, mode='valid')
# smoothed_rtt3 = np.convolve(flow3_rtt, np.ones(window_size) / window_size, mode='valid')
# smoothed_rtt4 = np.convolve(flow4_rtt, np.ones(window_size) / window_size, mode='valid')

# smoothed_throught1 = np.convolve(flow1_throught, np.ones(window_size) / window_size, mode='valid')
# smoothed_throught2 = np.convolve(flow2_throught, np.ones(window_size) / window_size, mode='valid')
# smoothed_throught3 = np.convolve(flow3_throught, np.ones(window_size) / window_size, mode='valid')
# smoothed_throught4 = np.convolve(flow4_throught, np.ones(window_size) / window_size, mode='valid')

smoothed_throught1 = calDraw(flow1_throught,window_size);
smoothed_throught2 = calDraw(flow2_throught,window_size);
smoothed_throught3 = calDraw(flow3_throught,window_size);
smoothed_throught4 = calDraw(flow4_throught,window_size);
if (ifRTQS):
    queue1=calDraw(queue1,pkt_window_size);
    queue2=calDraw(queue2,pkt_window_size);
    queue3=calDraw(queue3,pkt_window_size);
    queue4=calDraw(queue4,pkt_window_size);
    flow1_PktCount=calDraw(flow1_PktCount,pkt_window_size);
    flow2_PktCount=calDraw(flow2_PktCount,pkt_window_size);
    flow3_PktCount=calDraw(flow3_PktCount,pkt_window_size);
    flow4_PktCount=calDraw(flow4_PktCount,pkt_window_size);
smoothed_rtt1 = calDraw(flow1_rtt,window_size);
smoothed_rtt2 = calDraw(flow2_rtt,window_size);
smoothed_rtt3 = calDraw(flow3_rtt,window_size);
smoothed_rtt4 = calDraw(flow4_rtt,window_size);


# curJFI=calDraw(curJFI,window_size);
curJFI=[];
for x,y,z,w in zip(smoothed_throught1,smoothed_throught2,smoothed_throught3,smoothed_throught4):
    sum1=x+y+z+w;
    sum2=x*x+y*y+z*z+w*w;
    curJFI.append(sum1*sum1/(sum2*4));

'''
#做平滑处理2
window_size = 6
smoothed_rtt1 = savgol_filter(flow1_rtt, window_length=window_size, polyorder=2)
smoothed_rtt2 = savgol_filter(flow2_rtt, window_length=window_size, polyorder=2)
smoothed_rtt3 = savgol_filter(flow3_rtt, window_length=window_size, polyorder=2)
smoothed_rtt4 = savgol_filter(flow4_rtt, window_length=window_size, polyorder=2)

smoothed_throught1 = savgol_filter(flow1_throught, window_length=window_size, polyorder=2)
smoothed_throught2 = savgol_filter(flow2_throught, window_length=window_size, polyorder=2)
smoothed_throught3 = savgol_filter(flow3_throught, window_length=window_size, polyorder=2)
smoothed_throught4 = savgol_filter(flow4_throught, window_length=window_size, polyorder=2)

'''


'''
# 流1的吞吐量数据和延迟数据
throughput1 = [10, 15, 20, 18, 12]  # 替换为流1的吞吐量数据
latency1 = [5, 4, 6, 7, 3]  # 替换为流1的延迟数据

# 流2的吞吐量数据和延迟数据
throughput2 = [12, 17, 22, 19, 14]  # 替换为流2的吞吐量数据
latency2 = [4, 3, 5, 6, 2]  # 替换为流2的延迟数据

# 流3的吞吐量数据和延迟数据
throughput3 = [8, 13, 18, 16, 10]  # 替换为流3的吞吐量数据
latency3 = [6, 5, 7, 8, 4]  # 替换为流3的延迟数据
'''

# 创建图表
fig, ax1 = plt.subplots(figsize=(10, 6))  # 设置图表大小
ax1.set_xlabel('Time')  # 设置横轴标签
ax1.set_ylabel('Throughput', color='b')  # 设置左边纵轴标题为吞吐量4
ax1.tick_params(axis='y', labelcolor='b')  # 设置左边纵轴颜色为蓝色

if (ifRTQS and ifPktLoss):
    for time,flow in zip(LossTime,LossFlow):
        if (flow==0):
            ax1.axvline(x=time,color='b',linestyle='dashed',linewidth=0.1, alpha=0.3);
        if (flow==1):
            ax1.axvline(x=time,color='g',linestyle='dashed',linewidth=0.1, alpha=0.3);
        if (flow==2):
            ax1.axvline(x=time,color='r',linestyle='dashed',linewidth=0.1, alpha=0.3);
        if (flow==3):
            ax1.axvline(x=time,color='purple',linestyle='dashed',linewidth=0.1, alpha=0.3);

# 创建第二个纵轴用于绘制延迟数据
ax2 = ax1.twinx()
ax2.set_ylabel('Latency', color='b',labelpad=30)  # 设置右边纵轴标题为延迟
ax2.tick_params(axis='y', labelcolor='b')  # 设置右边纵轴颜色为蓝色

# 绘制流1的吞吐量曲线
ax1.plot(smoothed_throught1,  linestyle='-', color='b', label='Stream 1 Throughput')
ax2.plot(smoothed_rtt1, linestyle='-.', color='b', label='Stream 1 Latency')

# 绘制流2的吞吐量曲线
ax1.plot(smoothed_throught2,  linestyle='-', color='g', label='Stream 2 Throughput')
ax2.plot(smoothed_rtt2, linestyle='-.', color='g', label='Stream 2 Latency')

# 绘制流3的吞吐量曲线
ax1.plot(smoothed_throught3,  linestyle='-', color='r', label='Stream 3 Throughput')
ax2.plot(smoothed_rtt3,  linestyle='-.', color='r', label='Stream 3 Latency')

# 绘制流4的吞吐量曲线
ax1.plot(smoothed_throught4,  linestyle='-', color='purple', label='Stream 4 Throughput')
ax2.plot(smoothed_rtt4,  linestyle='-.', color='purple', label='Stream 4 Latency')

ax2.set_ylim(0,4e5);
# ax2.plot(queue1,  linestyle='dotted', color='b', label='Stream 1 QueueLen')
# # ax2.plot(queue2,  linestyle='dotted', color='g', label='Stream 2 QueueLen')
# # ax2.plot(queue3,  linestyle='dotted', color='r', label='Stream 3 QueueLen')
# ax2.plot(queue4,  linestyle='dotted', color='purple', label='Stream 4 QueueLen')


# ax3 = ax1.twinx()
# ax3.plot(flow1_PktCount,  linestyle='dashdot', color='b', label='Flow 1 PktCount')
# # ax3.plot(flow2_PktCount,  linestyle='dashdot', color='g', label='Flow 2 PktCount')
# # ax3.plot(flow3_PktCount,  linestyle='dashdot', color='r', label='Flow 3 PktCount')
# ax3.plot(flow4_PktCount,  linestyle='dashdot', color='purple', label='Flow 4 PktCount')

ax4=ax1.twinx();
ax4.plot(curJFI,linestyle='-', color='darkred', label='JFI',linewidth=3,alpha=0.3);
ax4.set_ylabel('JFI', color='black',labelpad=20)  # 设置右边纵轴标题为延迟
ax4.tick_params(axis='y', labelcolor='black')  # 设置右边纵轴颜色为蓝色

# ax2.plot(source1_PktCount,  linestyle='--', color='b', label='Stream 1 PktCount')
# ax2.plot(source2_PktCount,  linestyle='--', color='g', label='Stream 2 PktCount')
# ax2.plot(source3_PktCount,  linestyle='--', color='r', label='Stream 3 PktCount')
# ax2.plot(source4_PktCount,  linestyle='--', color='purple', label='Stream 4 PktCount')

if (ifRTQS and ifMaxReject):
    for time,flow in zip(RejectTime,RejectFlow):
        if (flow==0):
            ax2.axvline(x=time,color='b',linestyle='-',linewidth=0.1);
        if (flow==3):
            ax2.axvline(x=time,color='purple',linestyle='-',linewidth=0.1);



'''

# 绘制流1的吞吐量曲线
ax1.plot(flow1_throught, marker='o', linestyle='--', color='b', label='Stream 1 Throughput')
ax2.plot(flow1_rtt, marker='*', linestyle='-', color='b', label='Stream 1 Latency')

# 绘制流2的吞吐量曲线
ax1.plot(flow2_throught, marker='o', linestyle='--', color='g', label='Stream 2 Throughput')
ax2.plot(flow2_rtt, marker='*', linestyle='-', color='g', label='Stream 2 Latency')

# 绘制流3的吞吐量曲线
ax1.plot(flow3_throught, marker='o', linestyle='--', color='r', label='Stream 3 Throughput')
ax2.plot(flow3_rtt, marker='*', linestyle='-.', color='r', label='Stream 3 Latency')

# 绘制流4的吞吐量曲线
ax1.plot(flow4_throught, marker='o', linestyle='--', color='r', label='Stream 4 Throughput')
ax2.plot(flow4_rtt, marker='*', linestyle='-.', color='r', label='Stream 4 Latency')

'''

# 添加大标题和图例
plt.title('Throughput and Latency of Three Network Streams')
fig.legend(loc='upper right', bbox_to_anchor=(0.899, 0.87))  # 调整图例位置

# 显示图表
plt.show()