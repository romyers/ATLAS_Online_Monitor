Wi = [2**(10-i) for i in range(11)]
print(Wi)
para = [3.99, 4.06, 3.99, 4.01, 3.76, 4.18, 4.07, 4.12, 4.16, 3.82, 4.18]
Wi_para = [Wi[i]*(para[i]/100.0+1) for i in range(11)]
print(Wi_para)
Wi_para_sum = sum(Wi_para)
Wi_corrected = [data/Wi_para_sum*2048 for data in Wi_para]
print(Wi_corrected)