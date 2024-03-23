import os
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt
import re

path = './module0_miniDAQ/'
folders = os.listdir(path)

def extract_thershold(folder):
    pattern = r'_(\d+)mv'
    match = re.search(pattern,folder)

    if match:
        thre = int(match.group(1))
        return thre
    else:
        return None
    
# note: this script is not compatible to module0. Please use the script in that folder.

HVon_folder_CSM1 = [name for name in folders if 'HVon' in name and "CSM1" in name]
HVoff_folder_CSM1 = [name for name in folders if 'HVoff' in name and "CSM1" in name]
HVon_folder_CSM2 = [name for name in folders if 'HVon' in name and "CSM2" in name]
HVoff_folder_CSM2 = [name for name in folders if 'HVoff' in name and "CSM2" in name]
print(HVon_folder_CSM1)
# HVon_csv_name = [HVon_folder[0]+'/'+name for name in os.listdir(path+HVon_folder[0]) if name[-4:]=='.csv'][0]
# print(HVon_csv_name)

threshold = [19,21,23,25,27,31,35,39]
threshold_count = len(threshold)


def get_noise_rate(folders):
    noise_data_result = []
    for index in range(threshold_count):
        noise_data = np.array([])
        for folder in folders:
            print(folder)
            print(threshold)
            print(threshold[index])
            if folder[thr_offset:thr_offset+2]==str(threshold[index]):
                csv_name = [path+folder+'/'+ file for file in os.listdir(path+folder) if file[-4:]=='.csv'][0]
                print(csv_name)
                df = pd.read_csv(csv_name, header=None)
                df_data = df.iloc[1:22, 1:25]
                print(df_data)
                notused_row = [0,1]
                notused_column = [20,21,22,23]

                for i in notused_row:
                    for j in notused_column:
                        df_data.iloc[i, j] = float("NaN")
                # print(df_data)
                data = df_data.values.flatten()
                data = data[~np.isnan(data)]
                # print(data)
                noise_data = np.concatenate((noise_data, data))
                noise_data_result.append(noise_data)
    return noise_data

def combine_noise_rate(noise_data):
    mean = []
    std = []
    for index in range(threshold_count):
        mean.append(np.mean(noise_data)*1000)
        std.append(np.std(noise_data)*1000)
    return mean,std


HVoff_mean, HVoff_std = cal_noise_rate(HVoff_folder)
HVon_mean, HVon_std = cal_noise_rate(HVon_folder)
print(HVoff_mean)
print(HVon_mean)
print(HVoff_std)
print(HVon_std)
plt.semilogy(threshold,HVoff_mean,'bs-',markersize=5,label='HV off')
plt.semilogy(threshold,HVon_mean,'ro-',markersize=5,label='HV on')
plt.legend()
plt.xlabel('Main threshold (mV)',size=14)
plt.ylabel('Noise rate (Hz)',size=14)
plt.xlim(21,39)
plt.ylim(1,10000)
plt.savefig(path + path[2:9]+"_noise_rate_thresh.pdf")
plt.savefig(path + path[2:9]+"_noise_rate_thresh.png")
# plt.show()




