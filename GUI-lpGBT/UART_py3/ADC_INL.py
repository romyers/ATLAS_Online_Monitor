import matplotlib.pyplot as plt

Ci_MSB = [32,16,8,4,2,1]
Ci_LSB = [16,8,4,2,1]
CB = 1
CP_B = 0
CP_LSB = (32-1)*(CB+CP_B-1)+10
print(CP_LSB)
CP_MSB = 0
Csum_LSB = sum(Ci_LSB)+CP_LSB
Csum_MSB = sum(Ci_MSB)+CP_MSB
Cx = Csum_LSB*Csum_MSB+(CB+CP_B)*Csum_LSB+(CB+CP_B)*Csum_MSB
Wi_MSB = [Ci_MSB[i]*(CB+CP_B+Csum_LSB)/Cx for i in range (6)]
Wi_LSB = [Ci_LSB[i]*(CB+CP_B)/Cx for i in range (5)]
print(Wi_MSB)
print(Wi_LSB)

def AtoD(vin):
    code = [0 for i in range(12)]
    code[0] = 1 if vin > 0 else 0
    for i in range(6):
        vin = vin - Wi_MSB[i] * (code[i]-0.5)*2
        code[i+1] = 1 if vin > 0 else 0
    for i in range(5):
        vin = vin - Wi_LSB[i] * (code[i+6]-0.5)*2
        code[i + 7] = 1 if vin > 0 else 0
    dec_num = 0
    for b in code:
        dec_num = dec_num * 2 + b
    return dec_num
print(AtoD(0.50))

def DtoA_ideal(code):
    v = -1.0
    for i in range(12):
        v += 2**(11-i)*code[i]/2048
    return v
print(DtoA_ideal([1,1,1,1,1,1,1,1,1,1,1,1]))


n=1000000
x = [-1+i/n for i in range(2*n+1)]
y = [AtoD(v) for v in x]
# print(y[int(n/6):int(n/6)+100])

bin_num = [i for i in range(4096)]
bin_size = [0 for i in range(4096)]

left = x[0]
for i in range(2*n):
    if y[i+1]!=y[i]:
        bin_size[y[i]] = x[i+1] - left
        left = x[i+1]
# print(bin_size)
DNL = [data*2047 -1 for data in bin_size]
plt.plot(bin_num[1:4094],DNL[1:4094])
# plt.xlim(1000,1005)

plt.show()





# y = [DtoA_ideal(AtoD(v)) for v in x]
# plt.plot(x,y)
# plt.xlim(-0.01,0)
# plt.ylim(-0.01,0)
# plt.show()
# def Vout(index):
#     V = 0.0
#     for i in range(6):
#         V = V + Wi_MSB[i] * int(format(index,'b').zfill(11)[i])*1
#     for i in range(5):
#         V = V + Wi_LSB[i] * int(format(index,'b').zfill(11)[i+6])*1
#     return V
# print(Vout(2047))
#
# x = [i for i in range(2048)]
# y = [Vout(i) for i in range(2048)]
# DNL = [0]+[y[i+1]-y[i]-Vout(2047)/2047 for i in range(2047)]
# DNL = [data*2048 for data in DNL]
# INL = [y[i] -i*Vout(2047)/2047 for i in range (2048)]
# INL = [data*2048 for data in INL]
#
# plt.plot(x,DNL)
# plt.show()
