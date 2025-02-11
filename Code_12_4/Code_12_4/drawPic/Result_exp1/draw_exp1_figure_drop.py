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

def compute_cdf(data):
    data_sorted = np.sort(data)
    cdf = np.arange(1, len(data_sorted) + 1) / len(data_sorted)
    return data_sorted, cdf;

basedirs=['./ns3::RtqsQueueDisc','./ns3::RtqsQueueDisc','./ns3::CebinaeQueueDisc','./ns3::FifoQueueDisc'];
resultDir='./Figure_1';
resultNames=['RtqsQueueDisc (Real-time)','RtqsQueueDisc (Else)','(b) CebinaeQueueDisc','(c) FifoQueueDisc'];
colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];

fig = plt.figure(figsize=(8, 6))
cnt=0;

ax1 = fig.add_axes([0.14, 0.1, 0.8, 0.82]);
ax1.set_title("CDF", fontsize=16, fontweight='bold');

ax1.set_xlabel('Latency (ms)', fontsize=16, fontweight='bold');
ax1.tick_params(axis='x', labelcolor='black', labelsize=12);
ax1.set_xscale('log');
ax1.set_xticks([0, 20, 50, 200, 500, 2000], ['0', '20', '50', '200', '500', '2000'])

ax1.set_ylim(0, 1.2);
ax1.set_ylabel("Cumulative distribution (%)",fontsize=16, fontweight='bold');
ax1.tick_params(axis='y', labelcolor='black', labelsize=12);

for label in ax1.get_yticklabels():
    label.set_fontweight('bold')
# for label in ax1.get_xticklabels():
#     label.set_fontweight('bold')

lines=[];

for basedir,resultName in zip(basedirs,resultNames):
    rtt_data = np.loadtxt(basedir + '/rtt_100.dat');

    flow1_rtt = rtt_data[:, 0]
    flow2_rtt = rtt_data[:, 1]
    flow3_rtt = rtt_data[:, 2]
    flow4_rtt = rtt_data[:, 3]

    window_size = 200
    pkt_window_size=10;

    smoothed_rtt1 = calDraw(flow1_rtt,window_size);
    smoothed_rtt2 = calDraw(flow2_rtt,window_size);
    smoothed_rtt3 = calDraw(flow3_rtt,window_size);
    smoothed_rtt4 = calDraw(flow4_rtt,window_size);

    smoothed_rtt1=[i/1000 for i in smoothed_rtt1];
    smoothed_rtt2=[i/1000 for i in smoothed_rtt2];
    smoothed_rtt3=[i/1000 for i in smoothed_rtt3];
    smoothed_rtt4=[i/1000 for i in smoothed_rtt4];

    sum_rtt=[];
    if (cnt==0):
        for i in smoothed_rtt1: sum_rtt.append(i);
        for i in smoothed_rtt4: sum_rtt.append(i);
    elif (cnt==1):
        for i in smoothed_rtt2: sum_rtt.append(i);
        for i in smoothed_rtt3: sum_rtt.append(i);
    else:
        for i in smoothed_rtt1: sum_rtt.append(i);
        for i in smoothed_rtt2: sum_rtt.append(i);
        for i in smoothed_rtt3: sum_rtt.append(i);
        for i in smoothed_rtt4: sum_rtt.append(i);

    data, cdf = compute_cdf(sum_rtt);

    # 绘制流1的吞吐量曲线
    line1,=ax1.plot(data, cdf, linewidth=2,linestyle='-', color=colors[cnt], label=resultName)

    cnt+=1;

ax1.axvline(x=250, color='black', linestyle='--')
ax1.fill_betweenx([0, 1.2], 0, 250, color='lightgreen', alpha=0.2)

labels=[line.get_label() for line in lines];

ax1.legend(loc='lower right',fontsize=14)
plt.savefig(resultDir+"/"+"Figure2.png");
plt.show();