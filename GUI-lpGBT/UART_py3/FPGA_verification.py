import serial
from TDC_config_low_level_function import *
from serial_config_tdc import *
from TDCreg import *
# if used in linux, uncomment the initialization below and choose the right device
# ser = serial.Serial( port='/dev/ttyUSB0', baudrate = 115200, bytesize = serial.EIGHTBITS,parity =serial.PARITY_EVEN, stopbits = serial.STOPBITS_ONE, timeout=0.1)

# if used in windows, uncomment the initialization below and choose the right device
ser = serial.Serial(port='/dev/ttyUSB1', baudrate = 115200, bytesize = serial.EIGHTBITS,parity =serial.PARITY_EVEN, stopbits = serial.STOPBITS_ONE, timeout=0.1)



# write=(lpgbt_config_single_write(ser,117,108,44))


# SCA_connect(ser,sca_enable=0b100)
# SCA_tx_rx_reset(ser,sca_enable=0b100)
# SCA_start_reset(ser,sca_enable=0b100)
# # enable ADC channel
# SCA_config(ser,sca_enable=0b001,sca_address=0x77,transID=0x01,channel=0x0,datalen=4,command=0x06,data=0x10000000)
# MUX select
# SCA_config(ser,sca_enable=0b010,sca_address=0x77,transID=0x02,channel=0x14,datalen=4,command=0x50,data=21)
#ADC go
SCA_config(ser,sca_enable=0b010,sca_address=0x77,transID=0x05,channel=0x14,datalen=4,command=0x02,data=0x01)


# SCA_config(ser,sca_enable,sca_address,transID,channel,datalen,command,data)

#enable GPIO channel
# SCA_config(ser,sca_enable=0b111,sca_address=0x77,transID=0x01,channel=0x0,datalen=4,command=0x02,data=0x04000000)

# set GPIO direction
# SCA_config(ser,sca_enable=0b111,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x20,data=0xFFFFFFFF)
# set GPIO output
# SCA_config(ser,sca_enable=0b010,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x10,data=0x00080004)

#enable GPIO channel SCA2
# SCA_config(ser,sca_enable=0b111,sca_address=0x77,transID=0x01,channel=0x0,datalen=4,command=0x02,data=0x04000000)

# set GPIO direction SCA2
# SCA_config(ser,sca_enable=0b100,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x20,data=0x00000090)
# set GPIO output SCA2
# SCA_config(ser,sca_enable=0b100,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x10,data=0x00000000)

#enable GPIO channel SCA1
# SCA_config(ser,sca_enable=0b001,sca_address=0x77,transID=0x01,channel=0x0,datalen=4,command=0x02,data=0x04000000)

# set GPIO direction SCA2
# SCA_config(ser,sca_enable=0b001,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x20,data=0x60000000)
# set GPIO output SCA2
# SCA_config(ser,sca_enable=0b001,sca_address=0x77,transID=0x02,channel=0x02,datalen=4,command=0x10,data=0x00000000)

# readback=(lpgbt_config_read(ser,117,1,1))
# readback_list = []
# for i in range (len(readback)):
#     readback_list.append(format(write[i],'x').zfill(2))
# print(readback_list)
# config_reg_content = ser.read(20)
# print(config_reg_content)
# len(config_reg_content)
# TDC0 = TDCreg(ser)
# TDC0.DAQ_init()
# TDC0.update_periodic_hit()
# # #
# verificate_pass_TD_CODE = verificate_ID_CODE(ser)