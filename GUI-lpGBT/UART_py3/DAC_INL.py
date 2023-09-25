import matplotlib.pyplot as plt

# Wi=[2**(11-i) for i in range(12)]
# print(Wi)
# # Wi_delta = [0,0,0,0.1,0,0,0,0,0,0,0,0]
# Wi_delta = [0.01 for i in range(12)]
# Wi_out = [Wi[i]+Wi_delta[i] for i in range(12)]
# print(Wi_out)
# def Vout(index):
#     V = 0.0
#     for i in range (12):
#         # V = V + Wi_out[i]/(sum(Wi_out)+1)*(index//(2**(11-i))>0)
#         V = V + Wi_out[i] * int(format(index,'b').zfill(12)[i])
#         # V = V/sum(Wi_out)*4096
#     return V
# x = [i for i in range (4096)]
# INL = [Vout(i)-i for i in range (4096)]
# DNL = [0]+[INL[i+1]-INL[i] for i in range(4095)]
# plt.plot(x,DNL)
# plt.show()

# Ci_MSB = [32,16.01,8,4,2,1]
# Ci_LSB = [16,8,4,2,1]
Ci_MSB = [33.2755,16.66488,8.31911,4.16058,2.07519,1.04184]
# Ci_MSB = [data*1.5 for data in Ci_MSB]
Ci_LSB = [16.6511,8.33,4.16649,2.07632,1.04184]
# Ci_LSB = [data*1.3 for data in Ci_LSB]



CB = 1.5
CP_B = 0.107
# CP_LSB = (32-1)*(CB+CP_B-1)
CP_LSB = (2*Ci_LSB[0]/Ci_MSB[5]-1)*(CB+CP_B) - sum(Ci_LSB)
print("CP_LSB")
print(CP_LSB)
CP_LSB = 17.5
CP_MSB = 0
Csum_LSB = sum(Ci_LSB)+CP_LSB
Csum_MSB = sum(Ci_MSB)+CP_MSB
Cx = Csum_LSB*Csum_MSB+(CB+CP_B)*Csum_LSB+(CB+CP_B)*Csum_MSB
Wi_MSB = [Ci_MSB[i]*(CB+CP_B+Csum_LSB)/Cx for i in range (6)]
Wi_LSB = [Ci_LSB[i]*(CB+CP_B)/Cx for i in range (5)]
total_W = sum(Wi_MSB)+sum(Wi_LSB)
print("total_W="+str(total_W))
# Wi_MSB = [data*2048 for data in Wi_MSB]
# Wi_LSB = [data*2048 for data in Wi_LSB]
print(Wi_MSB)
print(Wi_LSB)
def Vout(index):
    V = 0.0
    for i in range(6):
        V = V + Wi_MSB[i] * int(format(index,'b').zfill(11)[i])*1
    for i in range(5):
        V = V + Wi_LSB[i] * int(format(index,'b').zfill(11)[i+6])*1
    return V
print("Vout(2047)="+str(Vout(2047)))

x = [i for i in range(2048)]
y = [Vout(i) for i in range(2048)]
print(str(y[1024]),str(y[1025]))
DNL = [0]+[y[i+1]-y[i]-Vout(2047)/2048 for i in range(2047)]
DNL = [data*1024 for data in DNL]
INL = [y[i] -i*Vout(2047)/2048 for i in range (2048)]
INL = [data*1024 for data in INL]
plt.plot(x,DNL)
plt.xlabel('DAC Code',size=15)
plt.ylabel('DNL (LSB)',size=15)
plt.ylim(-1,1)
plt.show()
