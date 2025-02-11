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

basedirs=['./ns3::RtqsQueueDisc','./ns3::CebinaeQueueDisc','./ns3::FifoQueueDisc'];
resultDir='./Exp2';
resultNames=['(a) RtqsQueueDisc','(b) CebinaeQueueDisc','(c) FifoQueueDisc'];
colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];

fig = plt.figure(figsize=(20, 6))
cnt=0;

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


    ax1=fig.add_axes([0.04+cnt*0.33,0.1,0.24,0.82])
    ax1.set_xlabel('Time (s)',fontsize=16,fontweight='bold');
    ax1.tick_params(axis='x',labelcolor='black',labelsize=12);
    ax1.set_xlim(0,240);
    ax1.set_title(resultName,fontsize=16,fontweight='bold');
    for label in ax1.get_xticklabels():
        label.set_fontweight('bold')
    ax1.set_ylabel('Throughput (Mbps)', color='black',labelpad=5, fontsize=16,fontweight='bold');
    ax1.tick_params(axis='y', labelcolor='black',labelsize=12);
    for label in ax1.get_yticklabels():
        label.set_fontweight('bold')

    ax2 = ax1.twinx()
    ax2.set_ylabel('Latency (ms)', color='black',labelpad=10,fontsize=16,fontweight='bold');
    ax2.tick_params(axis='y', labelcolor='black',labelsize=12);
    for label in ax2.get_yticklabels():
        label.set_fontweight('bold')

    # 绘制流1的吞吐量曲线
    ax1.plot(curTime, smoothed_throught1,  linewidth=2, linestyle='-', color=colors[0], label='Stream 1 Throughput (Realtime stream)',alpha=0.8)
    line1,=ax2.plot(curTime, smoothed_rtt1, linewidth=2,linestyle='-.', color=colors[0], label='Stream 1 Latency (Realtime stream)')

    # 绘制流2的吞吐量曲线
    ax1.plot(curTime, smoothed_throught2,  linewidth=2,linestyle='-', color=colors[1], label='Stream 2 Throughput (Non-realtime stream)')
    line2,=ax2.plot(curTime, smoothed_rtt2, linewidth=2,linestyle='-.', color=colors[1], label='Stream 2 Latency (Non-realtime stream)')

    # 绘制流3的吞吐量曲线
    ax1.plot(curTime, smoothed_throught3,  linewidth=2,linestyle='-', color=colors[2], label='Stream 3 Throughput (Other stream)')
    line3,=ax2.plot(curTime, smoothed_rtt3,  linewidth=2,linestyle='-.', color=colors[2], label='Stream 3 Latency (Other stream)')

    # 绘制流4的吞吐量曲线
    ax1.plot(curTime, smoothed_throught4,  linewidth=2,linestyle='-', color=colors[3], label='Stream 4 Throughput (Realtime stream)')
    line4,=ax2.plot(curTime, smoothed_rtt4,  linewidth=2,linestyle='-.', color=colors[3], label='Stream 4 Latency (Realtime stream)')

    max_1=max(smoothed_rtt1);
    max_2=max(smoothed_rtt2);
    max_3=max(smoothed_rtt3);
    max_4=max(smoothed_rtt4);
    ylim=max(max(max_1,max_2),max(max_3,max_4))*2.1;
    if (cnt==1):ax2.set_ylim(0,ylim*0.9);
    else:ax2.set_ylim(0,ylim);
    if (cnt==0):
        new_ticks=[];
        ticks=ax2.get_yticks();
        new_ticks.append(ticks[0]);
        new_ticks.append(200.);
        for i in ticks[1:]:
            new_ticks.append(i);
        print(new_ticks);
        ax2.set_yticks(new_ticks);

    max_1=max(smoothed_throught1);
    max_2=max(smoothed_throught2);
    max_3=max(smoothed_throught3);
    max_4=max(smoothed_throught4);
    ylim=max(max(max_1,max_2),max(max_3,max_4))*1.5;
    ax1.set_ylim(0,ylim);
    if (cnt==1):ax1.set_ylim(0,ylim*0.9);
    else:ax1.set_ylim(0,ylim)

    ax3=ax1.twinx();
    ax3.yaxis.set_ticks_position('left')
    ax3.yaxis.set_label_position('left')
    ax3.spines['left'].set_position(('outward', -345))

    line5,=ax3.plot(curTime, curJFI,linestyle='-', color="darkred", label='JFI',linewidth=7,alpha=0.2);
    ax3.set_ylabel('JFI', color='black',labelpad=5,fontsize=16,fontweight='bold')
    ax3.tick_params(axis='y', labelcolor='black',labelsize=12);
    minlim=min(curJFI[1:]);
    maxlim=max(curJFI[1:]);
    if (cnt==1):ax3.set_ylim(minlim-(maxlim-minlim)*0.15,maxlim+(maxlim-minlim)*0.2);
    else: ax3.set_ylim(minlim-(maxlim-minlim)*0.15,maxlim+(maxlim-minlim)*0.6);
    if (cnt==0):ax3.set_yticks([tick for tick in ax3.get_yticks() if tick <= 1 and tick!=min(ax3.get_yticks())])
    else:ax3.set_yticks([tick for tick in ax3.get_yticks() if tick <= 1 and tick!=min(ax3.get_yticks())])
    for label in ax3.get_yticklabels():
        label.set_fontweight('bold')

    lines=[line1,line2,line3,line4,line5];
    print(lines);
    labels=[line.get_label() for line in lines];

    if (cnt==0): ax1.legend(loc='upper left',fontsize=12)
    if (cnt==2): ax2.legend(lines,labels,loc='upper right',bbox_to_anchor=(1, 1),fontsize=12)
    cnt+=1;
plt.savefig(resultDir+"/"+"Figure4.png");
plt.show();