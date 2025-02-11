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

def autolabel1(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax1.annotate('{}'.format(round(height,3)),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom',size=16);

def autolabel2(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax2.annotate('{}\nms'.format(int(height)),
                    xy=(rect.get_x() + rect.get_width() / 2, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom',size=16);

def autolabel3(rects):
    """Attach a text label above each bar in *rects*, displaying its height."""
    for rect in rects:
        height = rect.get_height()
        ax3.annotate('{}%'.format(round(height*100,1)),
                    xy=(rect.get_x() + rect.get_width() / 2+0.15, height),
                    xytext=(0, 3),  # 3 points vertical offset
                    textcoords="offset points",
                    ha='center', va='bottom',size=16);

basedirs=['./ns3::RtqsQueueDisc','./ns3::CebinaeQueueDisc','./ns3::FifoQueueDisc'];
resultDir='./Figure_1';
resultNames=['(a) RtqsQueueDisc','(b) CebinaeQueueDisc','(c) FifoQueueDisc'];
colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];

fig = plt.figure(figsize=(8, 6))
cnt=0;
base=0
ax1 = fig.add_axes([0.06, 0.1, 0.88, 0.82]);
ax2 = ax1.twinx();
ax3 = ax1.twinx();

ax1.set_ylim(0.4,1.4);
ax2.set_ylim(0,2700);
ax3.set_ylim(0,1.4);

ax1.set_xticks([]);
ax1.set_yticks([]);
ax2.set_xticks([]);
ax2.set_yticks([]);
ax3.set_xticks([]);
ax3.set_yticks([]);

for basedir,resultName in zip(basedirs,resultNames):
    rtt_data = np.loadtxt(basedir + '/rtt_100.dat');
    throught_data = np.loadtxt(basedir + '/bottleneck_tpt_100.dat');
    jfiValue = np.loadtxt(basedir + '/jfi_100.dat');

    flow1_rtt = rtt_data[:, 0]
    flow2_rtt = rtt_data[:, 1]
    flow3_rtt = rtt_data[:, 2]
    flow4_rtt = rtt_data[:, 3]

    flow1_throught = throught_data[:, 0]
    flow2_throught = throught_data[:, 1]
    flow3_throught = throught_data[:, 2]
    flow4_throught = throught_data[:, 3]

    window_size = 200
    pkt_window_size=10;

    smoothed_throught1 = calDraw(flow1_throught,window_size);
    smoothed_throught2 = calDraw(flow2_throught,window_size);
    smoothed_throught3 = calDraw(flow3_throught,window_size);
    smoothed_throught4 = calDraw(flow4_throught,window_size);

    smoothed_throught1=[i/1000000 for i in smoothed_throught1];
    smoothed_throught2 = [i / 1000000 for i in smoothed_throught2];
    smoothed_throught3 = [i / 1000000 for i in smoothed_throught3];
    smoothed_throught4 = [i / 1000000 for i in smoothed_throught4];

    smoothed_rtt1 = calDraw(flow1_rtt,window_size);
    smoothed_rtt2 = calDraw(flow2_rtt,window_size);
    smoothed_rtt3 = calDraw(flow3_rtt,window_size);
    smoothed_rtt4 = calDraw(flow4_rtt,window_size);

    smoothed_rtt1=[i/1000 for i in smoothed_rtt1];
    smoothed_rtt2=[i/1000 for i in smoothed_rtt2];
    smoothed_rtt3=[i/1000 for i in smoothed_rtt3];
    smoothed_rtt4=[i/1000 for i in smoothed_rtt4];

    curTime=[i for i in np.arange(0.1,200,0.1)];

    curJFI=[];
    for x,y,z,w in zip(smoothed_throught1,smoothed_throught2,smoothed_throught3,smoothed_throught4):
        sum1=x+y+z+w;
        sum2=x*x+y*y+z*z+w*w;
        curJFI.append(sum1*sum1/(sum2*4));

    if (cnt==0):
        realtime_rtt=[(i+j)/2 for i,j in zip(smoothed_rtt1,smoothed_rtt4)];
        nonrealtime_rtt=[(i+j)/2 for i,j in zip(smoothed_rtt2,smoothed_rtt3)];
    else:
        rtt=[(i+j+k+l)/4 for i,j,k,l in zip(smoothed_rtt1,smoothed_rtt2,smoothed_rtt3,smoothed_rtt4)];
    throught=[i+j+k+l for i,j,k,l in zip(smoothed_throught1,smoothed_throught2,smoothed_throught3,smoothed_throught4)];

    if (cnt==0):
        realtime_rtt=sum(realtime_rtt)/ len(realtime_rtt);
        nonrealtime_rtt=sum(nonrealtime_rtt)/len(nonrealtime_rtt);
    else:
        rtt=sum(rtt)/len(rtt);
    throught=sum(throught)/len(throught)/15;
    JFI=sum(curJFI[1:])/len(curJFI[1:]);

    if (cnt==0):
        width = 0.95
        print(JFI,realtime_rtt,nonrealtime_rtt,throught)
        ret1=ax1.bar(base , [JFI], width, label='JFI', color='#FF6666',hatch="*",linewidth=2,edgecolor='black');
        ret2=ax2.bar(base + width, [realtime_rtt], width, label='Realtime rtt (ms)', color='#FFC966',hatch="/",linewidth=2,edgecolor='black');
        ret3=ax2.bar(base + 2*width, [nonrealtime_rtt], width, label='Non-Realtime rtt (ms)', color='#FFC966',hatch="\\",linewidth=2,edgecolor='black');
        ret4=ax3.bar(base + 3*width, [throught], width, label='Bandwidth utilization (%)', color='#6666FF',hatch="o",linewidth=2,edgecolor='black');
        autolabel1(ret1);
        autolabel2(ret2);
        autolabel2(ret3);
        autolabel3(ret4);
        base+=4.2;
    else:
        width = 0.95
        x = cnt * 4
        print(JFI, rtt, throught)
        ret1=ax1.bar(base , [JFI], width, label='JFI', color='#FF6666',hatch="*",linewidth=2,edgecolor='black');
        ret5=ax2.bar(base + width, [rtt], width, label='rtt (ms)', color='#FFC966',hatch="x",linewidth=2,edgecolor='black');
        ret4=ax3.bar(base + 2 * width, [throught], width, label='Bandwidth utilization (%)', color='#6666FF',hatch="o",linewidth=2,edgecolor='black');
        autolabel1(ret1);
        autolabel2(ret5);
        autolabel3(ret4);
        base+=3.2;

    cnt+=1;

xindex=[1.4, 5, 8.4];
xText=["RtqsQueueDisc","CebinaeQueueDisc","FifoQueueDisc"];
ax1.set_xticks(xindex);
ax1.set_xticklabels(xText,position=(1.02,0),rotation=0,fontsize=15);
for label in ax1.get_xticklabels():
    label.set_fontweight('bold');

rets=[ret2,ret3,ret5];
labels = [line.get_label() for line in rets];
ax2.legend(rets,labels,prop={'size':15},loc=1);

rets=[ret1,ret4];
labels = [line.get_label() for line in rets];
ax1.legend(rets,labels,prop={'size':15},loc=2);

plt.savefig(resultDir+"/"+"Figure3.png");
plt.show();