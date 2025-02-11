import numpy as np;
import matplotlib.pyplot as plt;

def calDraw(arr,step):
    n=len(arr);
    ret=[];
    for i in range(0,n):
        cnt=0;
        sum=0;
        for j in range(max(0,i-step),min(n,i+1)):
            cnt+=1;
            sum+=arr[j];
        ret.append(sum/cnt);
    return ret;


def convert_time_to_ms(time_str):
    value_str = time_str[:-2]
    unit = time_str[-2:]
    value = float(value_str)

    if unit == 'ns':
        value /= 1e6
    elif unit == 'us':
        value /= 1e3
    elif unit == 'ms':
        pass
    else:
        raise ValueError(f"未知的时间单位: {unit}")

    return value

resultDir='./Exp2';
basedir="./ns3::RtqsQueueDisc";
# colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];
colors=["darkred","darkgreen","darkblue","purple"];

fig = plt.figure(figsize=(8, 6))

cur_buffer = np.loadtxt(basedir + '/CurBufSize.dat', dtype="str");
queue_pkt = np.loadtxt(basedir + '/PktCount.dat');
rtt_data = np.loadtxt(basedir + '/rtt_100.dat');
queue_time = np.loadtxt(basedir + '/queueing_time.dat', dtype="str");

queue_buffer1=cur_buffer[:,1];
queue_buffer2=cur_buffer[:,3];
queue_pkt1=queue_pkt[:,1];
queue_pkt2=queue_pkt[:,3];
rtt1 = rtt_data[:, 0];
rtt2 = rtt_data[:, 3];
queue_time1=queue_time[:,1];
queue_time2=queue_time[:,3];


queue_buffer1=[int(i[:-1]) for i in queue_buffer1];
queue_buffer2=[int(i[:-1]) for i in queue_buffer2];
queue_pkt1=[int(i) for i in queue_pkt1];
queue_pkt2=[int(i) for i in queue_pkt2];
rtt1=[i/1000 for i in rtt1];
rtt2=[i/1000 for i in rtt2];
print(queue_time1);
print(queue_time2);
queue_time1=[convert_time_to_ms(i) for i in queue_time1];
queue_time2=[convert_time_to_ms(i) for i in queue_time2];

window_size=20;
queue_buffer1 = calDraw(queue_buffer1, window_size);
queue_buffer2 = calDraw(queue_buffer2, window_size);
queue_pkt1 = calDraw(queue_pkt1, window_size);
queue_pkt2 = calDraw(queue_pkt2, window_size);
rtt1 = calDraw(rtt1, window_size);
rtt2 = calDraw(rtt2, window_size);
queue_time1 = calDraw(queue_time1, window_size);
queue_time2 = calDraw(queue_time2, window_size);

print(queue_buffer1);
print(queue_buffer2);
print(queue_pkt1);
print(queue_pkt2);
print(rtt1);
print(rtt2);
print(queue_time1);
print(queue_time2);

curTime=[i for i in np.arange(0.1,200,0.1)];

ax1 = fig.add_axes([0.14, 0.1, 0.7, 0.82]);
ax1.set_xlabel('Time (s)',fontsize=16,fontweight='bold');
ax1.tick_params(axis='x',labelcolor='black',labelsize=12);
ax1.set_xlim(-10,210);
for label in ax1.get_xticklabels():
    label.set_fontweight('bold')
ax1.set_ylabel('RTT & queueing time (ms)', color='black',labelpad=5, fontsize=16,fontweight='bold');
ax1.tick_params(axis='y', labelcolor='black',labelsize=12);
for label in ax1.get_yticklabels():
    label.set_fontweight('bold')

ax2 = ax1.twinx()
ax2.set_ylabel('Buffer size & queueing size (packets)', color='black',labelpad=10,fontsize=16,fontweight='bold');
ax2.tick_params(axis='y', labelcolor='black',labelsize=12);
for label in ax2.get_yticklabels():
    label.set_fontweight('bold')


line1,=ax1.plot(curTime, rtt1,  linewidth=2, linestyle='-', color=colors[0], label='Stream1 RTT (link delay 30ms)',alpha=0.8)
# line2,=ax1.plot(curTime, queue_time1,  linewidth=2, linestyle='-.', color=colors[0], label='Queueing time',alpha=0.8)

line3,=ax2.plot(curTime, queue_pkt1, linewidth=2,linestyle='-.', color=colors[0], label='Stream1 Queueing packets')
line4,=ax2.plot(curTime, queue_buffer1, linewidth=4,linestyle='-', color=colors[0],alpha=0.3, label='Stream1 Queue buffer size')

line5,=ax1.plot(curTime, rtt2,  linewidth=2, linestyle='-', color=colors[2], label='Stream4 RTT (link delay 130ms)',alpha=0.8)
# line6,=ax1.plot(curTime, queue_time2,  linewidth=2, linestyle='-.', color=colors[2], label='Queueing time',alpha=0.8)

line7,=ax2.plot(curTime, queue_pkt2, linewidth=2,linestyle='-.', color=colors[2], label='Stream4 Queueing packets')
line8,=ax2.plot(curTime, queue_buffer2, linewidth=4,linestyle='-', color=colors[2],alpha=0.3, label='Stream4 Queue buffer size')

max_1=max(rtt1);
max_2=max(queue_time1);
max_3=max(rtt2);
max_4=max(queue_time2);
print(max_1,max_2,max_3,max_4);
ylim=max(max_1,max_2,max_3,max_4);
ax1.set_ylim(0,ylim*1.5);

max_1=max(queue_pkt1);
max_2=max(queue_buffer1);
max_3=max(queue_pkt2);
max_4=max(queue_buffer2);
print(max_1,max_2,max_3,max_4);

ylim=max(max_1,max_2,max_3,max_4);
ax2.set_ylim(0,ylim*1.2);

ax1.legend(loc='upper left',fontsize=14)
ax2.legend(loc='upper right',bbox_to_anchor=(1, 0.85),fontsize=14)

plt.savefig(resultDir+"/"+"Figure5.png");
plt.show();