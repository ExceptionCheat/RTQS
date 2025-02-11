import numpy as np
import matplotlib.pyplot as plt
import seaborn as sns

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

labels = [
    'All BBR', 'Half BBR Half Cubic', 'All Vegas', 'Half Vegas Half Cubic'
]
queue_labels = ['RTQS', 'Cebinae', 'FIFO']

basedirs=['./4bbr','./2bbr_2cubic','./4vegas','./2vegas_2cubic'];
subBasedirs=['/ns3::RtqsQueueDisc','/ns3::CebinaeQueueDisc','/ns3::FifoQueueDisc'];
resultDir='./Exp4';
colors=[(38/255,70/255,83/255),(42/255,157/255,142/255),(122/255,27/255,109/255),(230/255,111/255,81/255)];

JFIS=[];
delays=[];

for subBasedir in subBasedirs:
    cnt=0;
    for basedir in basedirs:
        throught_data = np.loadtxt(basedir+subBasedir + '/bottleneck_tpt_100.dat');
        flows=[];
        for i in range(0,4):
            flow_tpt = throught_data[:, i];
            window_size = 200
            flow_tpt = calDraw(flow_tpt, window_size);
            flow_tpt = [i / 1000000 for i in flow_tpt];
            flows.append(flow_tpt);
        JFI=[];
        for i in range(len(flows[0])):
            sum1=0;
            sum2=0;
            for j in range(0,4):
                sum1+=flows[j][i];
                sum2+=flows[j][i]*flows[j][i];
            JFI.append(sum1*sum1/(sum2*4));
        JFIS.append(JFI);

        rtt_data = np.loadtxt(basedir +subBasedir + '/rtt_100.dat');
        flows=[];
        for i in range(0,4):
            flow_rtt = rtt_data[:, i];
            window_size = 200
            flow_rtt=calDraw(flow_rtt,window_size);
            flow_rtt=[i / 1000 for i in flow_rtt];
            flows.append(flow_rtt);
        if (cnt==0):
            rtt1=[];
            rtt2=[];
            for i in range(len(flows[0])):
                sum1=0;
                sum2=0;
                for j in [0,3]:
                    sum1+=flows[j][i];
                for j in [1,2]:
                    sum2+=flows[j][i];
                print(sum1,sum2);
                rtt1.append(sum1/2);
                rtt2.append(sum2/2);
            delays.append([rtt1,rtt2]);
        else:
            rtt=[];
            for i in range(len(flows[0])):
                sum1=0;
                for j in range(0,8):
                    sum1+=flows[j][i];
                rtt.append(sum1/4);
            delays.append(rtt);
    cnt+=1;

fig = plt.figure(figsize=(8, 6))
ax1 = fig.add_axes([0.14, 0.1, 0.7, 0.82]);

bar_width = 1
base_width = 0

JFI_means=[];
delay_means=[];

for i in range(3):  # 3 queue models
    for j in range(4):  # 4 experiment setups per queue model
        jfi_mean = np.mean(JFIS[i * 4 + j][2:])
        JFI_means.append(jfi_mean);

        if (i==0):
            print(delays[i*4+j][0]);
            delay_mean1 = np.mean(delays[i * 4 + j][0])
            delay_25_1 = np.percentile(delays[i * 4 + j][0], 25)
            delay_75_1 = np.percentile(delays[i * 4 + j][0], 75)
            delay_mean2 = np.mean(delays[i * 4 + j][1])
            delay_25_2 = np.percentile(delays[i * 4 + j][1], 25)
            delay_75_2 = np.percentile(delays[i * 4 + j][1], 75)
            delay_means.append(((delay_mean1,delay_25_1,delay_75_1),(delay_mean2,delay_25_2,delay_75_2)));
        else:
            delay_mean = np.mean(delays[i * 4 + j])
            delay_25 = np.percentile(delays[i * 4 + j], 25)
            delay_75 = np.percentile(delays[i * 4 + j], 75)
            delay_means.append((delay_mean,delay_25,delay_75));

JFI_means=[0.9992978967152993, 0.4425204308502295, 0.9954623473803051, 0.7988901241911136, 0.22034170502622952, 0.9010968976148275, 0.9997099744868205, 0.36015842086248956, 0.9997099744868205, 0.9006001450522746, 0.5663399187840996, 0.9006357517594688];


print(JFI_means);
print(delay_means);

for i in range(3):
    for j in range(4):
        ax1.bar(base_width + j * bar_width, JFI_means[i*4+j], bar_width, label=f'{queue_labels[i]} - {labels[j]}', color=colors[j]);
        # ax.errorbar(index + i * bar_width, [jfi_means], yerr=[[jfi_means - delay_25], [delay_75 - jfi_means]],
        #             fmt='*', color='black', capsize=5)

# ax.set_xlabel('Experiment Setup')
# ax.set_ylabel('JFI Fairness Index')
# ax.set_title('JFI Fairness Index and Delay Box Plot')
# ax.set_xticks(index + bar_width)
# ax.set_xticklabels(queue_labels)
# ax.legend()

plt.show()

