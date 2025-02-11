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

resultDir='./Figure_1';
basedir="./ns3::RtqsQueueDisc";
# colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];
colors=["darkred","darkgreen"];

fig = plt.figure(figsize=(8, 6))

cur_buffer = np.loadtxt(basedir + '/CurBufSize.dat', dtype="str");
queue_pkt = np.loadtxt(basedir + '/PktCount.dat');
rtt_data = np.loadtxt(basedir + '/rtt_100.dat');
queue_time = np.loadtxt(basedir + '/queueing_time.dat', dtype="str");

queue_buffer=cur_buffer[:,1];
queue_pkt=queue_pkt[:,1];
rtt = rtt_data[:, 0];
queue_time=queue_time[:,1];

queue_buffer=[int(i[:-1]) for i in queue_buffer];
queue_pkt=[int(i) for i in queue_pkt];
rtt=[i/1000 for i in rtt];
queue_time=[convert_time_to_ms(i) for i in queue_time];

window_size=20;
queue_buffer = calDraw(queue_buffer, window_size);
queue_pkt = calDraw(queue_pkt, window_size);
rtt = calDraw(rtt, window_size);
queue_time = calDraw(queue_time, window_size);

print(queue_buffer);
print(queue_pkt);
print(rtt);
print(queue_time);

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


line1,=ax1.plot(curTime, rtt,  linewidth=2, linestyle='-', color=colors[0], label='RTT',alpha=0.8)
line2,=ax1.plot(curTime, queue_time,  linewidth=2, linestyle='-.', color=colors[0], label='Queueing time',alpha=0.8)

line3,=ax2.plot(curTime, queue_pkt, linewidth=2,linestyle='-.', color=colors[1], label='Queueing packets')
line4,=ax2.plot(curTime, queue_buffer, linewidth=2,linestyle='-', color=colors[1], label='Queue buffer size')

max_1=max(rtt);
max_2=max(queue_time);
print(max_1,max_2);
ylim=max(max_1,max_2);
ax1.set_ylim(0,ylim*1.5);

max_1=max(queue_pkt);
max_2=max(queue_buffer);
print(max_1,max_2);

ylim=max(max_1,max_2);
ax2.set_ylim(0,ylim*1.2);

ax1.legend(loc='upper left',fontsize=14)
ax2.legend(loc='upper right',bbox_to_anchor=(1, 1),fontsize=14)

plt.savefig(resultDir+"/"+"Figure2.png");
plt.show();