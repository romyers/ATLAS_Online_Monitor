##===================================================================================================##
##========================================  I/O PINS  ===============================================##
##===================================================================================================##


set_property IOSTANDARD LVDS [get_ports USER_CLOCK_P]
set_property IOSTANDARD LVDS [get_ports USER_CLOCK_N]
set_property PACKAGE_PIN T22 [get_ports USER_CLOCK_P]
set_property PACKAGE_PIN U22 [get_ports USER_CLOCK_N]
set_property DIFF_TERM_ADV TERM_100 [get_ports USER_CLOCK_P]
set_property DIFF_TERM_ADV TERM_100 [get_ports USER_CLOCK_N]

# set_property IOSTANDARD LVDS [get_ports USER_CLOCK_P]
# set_property PACKAGE_PIN E13 [get_ports USER_CLOCK_P]
# set_property IOSTANDARD LVDS [get_ports USER_CLOCK_N]
# set_property PACKAGE_PIN E12 [get_ports USER_CLOCK_N]
# set_property DIFF_TERM FALSE [get_ports USER_CLOCK_P]
# set_property DIFF_TERM FALSE [get_ports USER_CLOCK_N]



###==============##
### SYSTEM CLOCK 200MHz ##
###==============##
#set_property IOSTANDARD LVDS [get_ports SYS_CLOCK_200_P]
#set_property PACKAGE_PIN T22 [get_ports SYS_CLOCK_200_P]
#set_property IOSTANDARD LVDS [get_ports SYS_CLOCK_200_N]
#set_property PACKAGE_PIN U22 [get_ports SYS_CLOCK_200_N]
# set_property DIFF_TERM TRUE [get_ports SYS_CLOCK_200_P]
# set_property DIFF_TERM TRUE [get_ports SYS_CLOCK_200_N]


##==============##
# ## USER SI570 156.25MMHz ##
# ##==============##
# set_property IOSTANDARD LVDS [get_ports USER_SI570_CLK_P]
# set_property PACKAGE_PIN U24 [get_ports USER_SI570_CLK_P]
# set_property IOSTANDARD LVDS [get_ports USER_SI570_CLK_N]
# set_property PACKAGE_PIN V24 [get_ports USER_SI570_CLK_N]
# set_property DIFF_TERM TRUE [get_ports USER_SI570_CLK_P]
# set_property DIFF_TERM TRUE [get_ports USER_SI570_CLK_N]



# ##==============##
# ## ETHERNET CLOCK 125MHz ##
# ##==============##
# set_property IOSTANDARD LVDS [get_ports CLK_125M_P]
# set_property PACKAGE_PIN E13 [get_ports CLK_125M_P]
# set_property IOSTANDARD LVDS [get_ports CLK_125M_N]
# set_property PACKAGE_PIN E12 [get_ports CLK_125M_N]
# set_property DIFF_TERM FALSE [get_ports CLK_125M_P]
# set_property DIFF_TERM FALSE [get_ports CLK_125M_N]



# ##==============##
# ## USER 320 CLOCK 320MHz ##
# ##==============##
# set_property IOSTANDARD LVDS_25 [get_ports USER_CLK_320_P]
# set_property PACKAGE_PIN AC12 [get_ports USER_CLK_320_P]
# set_property IOSTANDARD LVDS_25 [get_ports USER_CLK_320_N]
# set_property PACKAGE_PIN AC11 [get_ports USER_CLK_320_N]
# set_property DIFF_TERM TRUE [get_ports USER_CLK_320_P]
# set_property DIFF_TERM TRUE [get_ports USER_CLK_320_N]

# ##==============##
# ## USER SMA GPIO ##
# ##==============##
# set_property IOSTANDARD LVDS [get_ports USER_SMA_GPIO_P]
# set_property PACKAGE_PIN N26 [get_ports USER_SMA_GPIO_P]
# set_property IOSTANDARD LVDS [get_ports USER_SMA_GPIO_N]
# set_property PACKAGE_PIN M26 [get_ports USER_SMA_GPIO_N]




##==========##
## MGT(GTH) ##
##==========##

## MGT CLOCK:
##--------------

## Comment: * The MGT reference clock MUST be provided by an external clock generator.
##
##          * The MGT reference clock frequency must be 320MHz.

set_property PACKAGE_PIN T5 [get_ports SMA_MGT_REFCLK_N]
set_property PACKAGE_PIN T6 [get_ports SMA_MGT_REFCLK_P]

## LOCK_TOP

#set_property PACKAGE_PIN AP8 [get_ports LOCK_TOP]
#set_property IOSTANDARD LVCMOS18 [get_ports LOCK_TOP]

## SERIAL LANES:
##--------------

# Note: the IP core-level XDC constrains the transceiver channel data pin locations
# Use of SFP
#set_property PACKAGE_PIN R4 [get_ports SFP0_TX_P]
#set_property PACKAGE_PIN R3 [get_ports SFP0_TX_N]
#set_property PACKAGE_PIN P2 [get_ports SFP0_RX_P]
#set_property PACKAGE_PIN P1 [get_ports SFP0_RX_N]

#set_property PACKAGE_PIN U4 [get_ports SFP1_TX_P]
#set_property PACKAGE_PIN U3 [get_ports SFP1_TX_N]
#set_property PACKAGE_PIN T2 [get_ports SFP1_RX_P]
#set_property PACKAGE_PIN T1 [get_ports SFP1_RX_N]

#set_property PACKAGE_PIN W4 [get_ports SFP2_TX_P]
#set_property PACKAGE_PIN W3 [get_ports SFP2_TX_N]
#set_property PACKAGE_PIN V2 [get_ports SFP2_RX_P]
#set_property PACKAGE_PIN V1 [get_ports SFP2_RX_N]

#set_property PACKAGE_PIN AA4 [get_ports SFP3_TX_P]
#set_property PACKAGE_PIN AA3 [get_ports SFP3_TX_N]
#set_property PACKAGE_PIN Y2 [get_ports SFP3_RX_P]
#set_property PACKAGE_PIN Y1 [get_ports SFP3_RX_N]

#set_property PACKAGE_PIN AB6 [get_ports SFP4_TX_P]
#set_property PACKAGE_PIN AB5 [get_ports SFP4_TX_N]
#set_property PACKAGE_PIN AB2 [get_ports SFP4_RX_P]
#set_property PACKAGE_PIN AB1 [get_ports SFP4_RX_N]

#set_property PACKAGE_PIN AC4 [get_ports SFP5_TX_P]
#set_property PACKAGE_PIN AC3 [get_ports SFP5_TX_N]
#set_property PACKAGE_PIN AD2 [get_ports SFP5_RX_P]
#set_property PACKAGE_PIN AD1 [get_ports SFP5_RX_N]

#set_property PACKAGE_PIN AD6 [get_ports SFP6_TX_P]
#set_property PACKAGE_PIN AD5 [get_ports SFP6_TX_N]
#set_property PACKAGE_PIN AE4 [get_ports SFP6_RX_P]
#set_property PACKAGE_PIN AE3 [get_ports SFP6_RX_N]

#set_property PACKAGE_PIN AF6 [get_ports SFP7_TX_P]
#set_property PACKAGE_PIN AF5 [get_ports SFP7_TX_N]
#set_property PACKAGE_PIN AF2 [get_ports SFP7_RX_P]
#set_property PACKAGE_PIN AF1 [get_ports SFP7_RX_N]

## SFP CONTROL:
##-------------
set_property PACKAGE_PIN Y13 [get_ports SFP0_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP0_TX_DISABLE]

set_property PACKAGE_PIN W9 [get_ports SFP1_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP1_TX_DISABLE]

set_property PACKAGE_PIN Y8 [get_ports SFP2_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP2_TX_DISABLE]

set_property PACKAGE_PIN AC9 [get_ports SFP3_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP3_TX_DISABLE]

set_property PACKAGE_PIN W11 [get_ports SFP4_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP4_TX_DISABLE]

set_property PACKAGE_PIN Y10 [get_ports SFP5_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP5_TX_DISABLE]

set_property PACKAGE_PIN AD9 [get_ports SFP6_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP6_TX_DISABLE]

set_property PACKAGE_PIN AF10 [get_ports SFP7_TX_DISABLE]
set_property IOSTANDARD LVCMOS25 [get_ports SFP7_TX_DISABLE]


##====================##
## SIGNALS FORWARDING ##
##====================##

set_property IOSTANDARD LVDS [get_ports TRIGGER0_P]
set_property IOSTANDARD LVDS [get_ports TRIGGER0_N]
set_property PACKAGE_PIN K26 [get_ports TRIGGER0_P]
set_property PACKAGE_PIN J26 [get_ports TRIGGER0_N]
set_property DIFF_TERM_ADV TERM_100 [get_ports TRIGGER0_P]
set_property DIFF_TERM_ADV TERM_100 [get_ports TRIGGER0_N]


## SMA OUTPUT:
##------------
set_property PACKAGE_PIN N26 [get_ports USER_SMA_GPIO_P]
set_property IOSTANDARD LVCMOS18 [get_ports USER_SMA_GPIO_P]
set_property SLEW FAST [get_ports USER_SMA_GPIO_P]

set_property PACKAGE_PIN M26 [get_ports USER_SMA_GPIO_N]
set_property IOSTANDARD LVCMOS18 [get_ports USER_SMA_GPIO_N]
set_property SLEW FAST [get_ports USER_SMA_GPIO_N]

# UART INTERFACE
##-------------
set_property PACKAGE_PIN AC18 [get_ports uart_rxd]
set_property IOSTANDARD LVCMOS25 [get_ports uart_rxd]

set_property PACKAGE_PIN AB17 [get_ports uart_txd]
set_property IOSTANDARD LVCMOS25 [get_ports uart_txd]

#set_property PACKAGE_PIN AC19 [get_ports uart_cts]
#set_property IOSTANDARD LVCMOS25 [get_ports uart_cts]

#set_property PACKAGE_PIN AA18 [get_ports uart_rts]
#set_property IOSTANDARD LVCMOS25 [get_ports uart_rts]

## MINISAS OUTPUT:
##------------
set_property PACKAGE_PIN AC24    [get_ports hit[ 0]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 0]]
set_property PACKAGE_PIN AC23    [get_ports hit[ 1]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 1]]
set_property PACKAGE_PIN AB24    [get_ports hit[ 2]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 2]]
set_property PACKAGE_PIN AA24    [get_ports hit[ 3]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 3]]
set_property PACKAGE_PIN AA23    [get_ports hit[ 4]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 4]]
set_property PACKAGE_PIN Y23     [get_ports hit[ 5]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 5]]
set_property PACKAGE_PIN AB22    [get_ports hit[ 6]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 6]]
set_property PACKAGE_PIN AA22    [get_ports hit[ 7]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 7]]
set_property PACKAGE_PIN U26     [get_ports hit[ 8]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 8]]
set_property PACKAGE_PIN V26     [get_ports hit[ 9]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[ 9]]
set_property PACKAGE_PIN W25     [get_ports hit[10]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[10]]
set_property PACKAGE_PIN W26     [get_ports hit[11]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[11]]
set_property PACKAGE_PIN AA25    [get_ports hit[12]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[12]]
set_property PACKAGE_PIN AB25    [get_ports hit[13]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[13]]
set_property PACKAGE_PIN AB26    [get_ports hit[14]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[14]]
set_property PACKAGE_PIN AC26    [get_ports hit[15]]
set_property IOSTANDARD LVCMOS18 [get_ports hit[15]]
set_property PACKAGE_PIN AB19    [get_ports hit[16]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[16]]
set_property PACKAGE_PIN AB20    [get_ports hit[17]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[17]]
set_property PACKAGE_PIN AD24    [get_ports hit[18]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[18]]
set_property PACKAGE_PIN AD25    [get_ports hit[19]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[19]]
set_property PACKAGE_PIN AD26    [get_ports hit[20]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[20]]
set_property PACKAGE_PIN AE26    [get_ports hit[21]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[21]]
set_property PACKAGE_PIN AE25    [get_ports hit[22]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[22]]
set_property PACKAGE_PIN AF25    [get_ports hit[23]]
set_property IOSTANDARD LVCMOS25 [get_ports hit[23]]

set_property PACKAGE_PIN Y16    [get_ports mezz_TRST]
set_property IOSTANDARD LVCMOS25 [get_ports mezz_TRST]


  

