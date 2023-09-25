# /* 
#        _________   ________     ________       
#       / U OF M  \ | LSA    \   / Physics\
#       \__ATLAS__/ |   ___   | |   ______/
#          |   |    |  |   \  | |  |
#          |   |    |  |___/  | |  \______     
#          |   |    |         | |         \
#          \___/    |________/   \________/
# */  

# // Author : Yuxiang Guo  gyuxiang@umich.edu
# // File   : vivado_build.tcl
# // Create : 2021-03-07 21:39:48
# // Revise : 2021-03-07 21:39:48
# // Editor : sublime text3, tab size (4)
# // Description: 
# //
# // -----------------------------------------------------------------------------


#*****************************************************************************************
#################################  README FIRST ##########################################
#*****************************************************************************************

# In order to execute the tcl script and build the project, run Vivado and go to: 
# Tools -> Run Tcl Script...
#
# An alternative way would be to open a terminal, and run this command:
# vivado -mode batch -source <PATH>/build.tcl
#
# For more info on how to make further changes to the script, see: 
# http://xillybus.com/tutorials/vivado-version-control-packaging



set_param general.maxThreads 8

# Set the reference directory for source file relative paths (by default the value is script directory path)
set origin_dir [file dirname [info script]]
set source_dir "$origin_dir/sources"
set constrain_dir "$origin_dir/constrains"
set firmware_dir "$origin_dir/../../firm_MiniDAQ_xcku035_triggerless"


create_project MiniDAQ_xcku035_triggerless $firmware_dir -part xcku035-fbva676-1-c -force
set_property target_language Verilog [current_project]
set_property simulator_language Mixed [current_project]

#*****************************************************************************************
#################################  Adding Source #########################################
#*****************************************************************************************


add_files -fileset sources_1 $source_dir

#*****************************************************************************************
#################################  Adding Constrain ######################################
#****************************************campu*************************************************
add_files -fileset constrs_1 $constrain_dir


#*****************************************************************************************
#################################  Set Top Module ######################################
#*****************************************************************************************
set obj [get_filesets sources_1]
set_property -name "top" -value "lpgbtfpga_xcku035_10g24_top" -objects $obj



##################################################################
# CREATE IP debug_jtag_master
##################################################################

set jtag_axi debug_jtag_master
create_ip -name jtag_axi -vendor xilinx.com -library ip -module_name $jtag_axi
set_property -dict {CONFIG.PROTOCOL {2}} [get_ips $jtag_axi]

##################################################################

##################################################################
# CREATE IP TDC_data_fifo_40b
##################################################################

set fifo_generator TDC_data_fifo_40b
create_ip -name fifo_generator -vendor xilinx.com -library ip -module_name $fifo_generator

set_property -dict { 
  CONFIG.Fifo_Implementation {Common_Clock_Builtin_FIFO}
  CONFIG.Performance_Options {First_Word_Fall_Through}
  CONFIG.Input_Data_Width {40}
  CONFIG.Input_Depth {512}
  CONFIG.Output_Data_Width {40}
  CONFIG.Output_Depth {512}
  CONFIG.Use_Embedded_Registers {false}
  CONFIG.Valid_Flag {false}
  CONFIG.Underflow_Flag {false}
  CONFIG.Write_Acknowledge_Flag {false}
  CONFIG.Overflow_Flag {false}
  CONFIG.Data_Count_Width {9}
  CONFIG.Write_Data_Count_Width {9}
  CONFIG.Read_Data_Count_Width {9}
  CONFIG.Read_Clock_Frequency {1}
  CONFIG.Write_Clock_Frequency {1}
  CONFIG.Full_Threshold_Assert_Value {511}
  CONFIG.Full_Threshold_Negate_Value {510}
  CONFIG.Empty_Threshold_Assert_Value {4}
  CONFIG.Empty_Threshold_Negate_Value {5}
} [get_ips $fifo_generator]

##################################################################

##################################################################
# CREATE IP vio_ethernet_enable
##################################################################



##################################################################

set fifo_generator sync_fifo_32b
create_ip -name fifo_generator -vendor xilinx.com -library ip -module_name $fifo_generator

set_property -dict { 
  CONFIG.Fifo_Implementation {Independent_Clocks_Distributed_RAM}
  CONFIG.Performance_Options {First_Word_Fall_Through}
  CONFIG.Input_Data_Width {32}
  CONFIG.Input_Depth {16}
  CONFIG.Output_Data_Width {32}
  CONFIG.Output_Depth {16}
  CONFIG.Use_Embedded_Registers {false}
  CONFIG.Reset_Pin {false}
  CONFIG.Reset_Type {Asynchronous_Reset}
  CONFIG.Full_Flags_Reset_Value {0}
  CONFIG.Use_Dout_Reset {false}
  CONFIG.Data_Count_Width {4}
  CONFIG.Write_Data_Count_Width {4}
  CONFIG.Read_Data_Count_Width {4}
  CONFIG.Full_Threshold_Assert_Value {15}
  CONFIG.Full_Threshold_Negate_Value {14}
  CONFIG.Empty_Threshold_Assert_Value {4}
  CONFIG.Empty_Threshold_Negate_Value {5}
  CONFIG.Enable_Safety_Circuit {false}
} [get_ips $fifo_generator]

##################################################################

##################################################################
# CREATE IP tdc_hit_ram
##################################################################

set blk_mem_gen tdc_hit_ram
create_ip -name blk_mem_gen -vendor xilinx.com -library ip -module_name $blk_mem_gen

set_property -dict { 
  CONFIG.Memory_Type {Simple_Dual_Port_RAM}
  CONFIG.Assume_Synchronous_Clk {true}
  CONFIG.Write_Width_A {32}
  CONFIG.Write_Depth_A {1024}
  CONFIG.Read_Width_A {32}
  CONFIG.Operating_Mode_A {READ_FIRST}
  CONFIG.Enable_A {Always_Enabled}
  CONFIG.Write_Width_B {32}
  CONFIG.Read_Width_B {32}
  CONFIG.Operating_Mode_B {READ_FIRST}
  CONFIG.Enable_B {Always_Enabled}
  CONFIG.Register_PortA_Output_of_Memory_Primitives {false}
  CONFIG.Register_PortA_Output_of_Memory_Core {false}
  CONFIG.Register_PortB_Output_of_Memory_Primitives {false}
  CONFIG.Use_RSTA_Pin {false}
  CONFIG.Use_RSTB_Pin {true}
  CONFIG.Port_A_Write_Rate {50}
  CONFIG.Port_B_Clock {100}
  CONFIG.Port_B_Write_Rate {0}
  CONFIG.Port_B_Enable_Rate {100}
  CONFIG.EN_SAFETY_CKT {true}
} [get_ips $blk_mem_gen]

##################################################################

##################################################################
# CREATE IP tri_mode_ethernet_mac_0
##################################################################

set tri_mode_ethernet_mac tri_mode_ethernet_mac_0
create_ip -name tri_mode_ethernet_mac -vendor xilinx.com -library ip -module_name $tri_mode_ethernet_mac

set_property -dict { 
  CONFIG.Physical_Interface {RGMII}
  CONFIG.Management_Frequency {100}
  CONFIG.Frame_Filter {false}
  CONFIG.Number_of_Table_Entries {0}
  CONFIG.Statistics_Counters {false}
  CONFIG.SupportLevel {1}
} [get_ips $tri_mode_ethernet_mac]

##################################################################


##################################################################
# CREATE IP xlx_k7v7_tx_pll
##################################################################

set clk_wiz xlx_k7v7_tx_pll
create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name $clk_wiz

set_property -dict { 
  CONFIG.PRIMITIVE {MMCM}
  CONFIG.USE_PHASE_ALIGNMENT {true}
  CONFIG.PRIM_IN_FREQ {40.000}
  CONFIG.SECONDARY_SOURCE {Single_ended_clock_capable_pin}
  CONFIG.CLKIN1_JITTER_PS {250.0}
  CONFIG.CLKOUT2_USED {true}
  CONFIG.NUM_OUT_CLKS {2}
  CONFIG.CLK_OUT1_PORT {txFrameClk}
  CONFIG.CLK_OUT2_PORT {sys_clk_160}
  CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {40.000}
  CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {160}
  CONFIG.PRIM_SOURCE {Global_buffer}
  CONFIG.CLKOUT1_DRIVES {BUFG}
  CONFIG.CLKOUT2_DRIVES {Buffer}
  CONFIG.CLKOUT3_DRIVES {Buffer}
  CONFIG.CLKOUT4_DRIVES {Buffer}
  CONFIG.CLKOUT5_DRIVES {Buffer}
  CONFIG.CLKOUT6_DRIVES {Buffer}
  CONFIG.CLKOUT7_DRIVES {Buffer}
  CONFIG.FEEDBACK_SOURCE {FDBK_AUTO}
  CONFIG.USE_LOCKED {true}
  CONFIG.USE_RESET {true}
  CONFIG.MMCM_DIVCLK_DIVIDE {1}
  CONFIG.MMCM_BANDWIDTH {OPTIMIZED}
  CONFIG.MMCM_CLKFBOUT_MULT_F {24.000}
  CONFIG.MMCM_CLKIN1_PERIOD {25.000}
  CONFIG.MMCM_CLKIN2_PERIOD {10.0}
  CONFIG.MMCM_COMPENSATION {AUTO}
  CONFIG.MMCM_CLKOUT0_DIVIDE_F {24.000}
  CONFIG.MMCM_CLKOUT1_DIVIDE {6}
  CONFIG.AUTO_PRIMITIVE {MMCM}
  CONFIG.CLKOUT1_JITTER {247.096}
  CONFIG.CLKOUT1_PHASE_ERROR {196.976}
  CONFIG.CLKOUT2_JITTER {169.111}
  CONFIG.CLKOUT2_PHASE_ERROR {196.976}
} [get_ips $clk_wiz]

##################################################################


##################################################################
# CREATE IP sys_clk
##################################################################

set clk_wiz sys_clk
create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name $clk_wiz

set_property -dict { 
  CONFIG.PRIM_IN_FREQ {200}
  CONFIG.CLKIN1_JITTER_PS {50.0}
  CONFIG.CLKOUT2_USED {true}
  CONFIG.NUM_OUT_CLKS {2}
  CONFIG.CLK_OUT1_PORT {mgt_freedrpclk}
  CONFIG.CLK_OUT2_PORT {clk_200}
  CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {200}
  CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {200.000}
  CONFIG.PRIM_SOURCE {Differential_clock_capable_pin}
  CONFIG.USE_RESET {false}
  CONFIG.MMCM_CLKFBOUT_MULT_F {5.000}
  CONFIG.MMCM_CLKIN1_PERIOD {5.000}
  CONFIG.MMCM_CLKIN2_PERIOD {10.0}
  CONFIG.MMCM_CLKOUT0_DIVIDE_F {5.000}
  CONFIG.MMCM_CLKOUT1_DIVIDE {5}
  CONFIG.CLKOUT1_JITTER {98.146}
  CONFIG.CLKOUT1_PHASE_ERROR {89.971}
  CONFIG.CLKOUT2_JITTER {98.146}
  CONFIG.CLKOUT2_PHASE_ERROR {89.971}
} [get_ips $clk_wiz]

##################################################################

##################################################################
# CREATE IP xlx_ku_mgt_ip_10g24
##################################################################

set gtwizard_ultrascale xlx_ku_mgt_ip_10g24
create_ip -name gtwizard_ultrascale -vendor xilinx.com -library ip -module_name $gtwizard_ultrascale

set_property -dict { 
  CONFIG.CHANNEL_ENABLE {X0Y4}
  CONFIG.TX_MASTER_CHANNEL {X0Y4}
  CONFIG.RX_MASTER_CHANNEL {X0Y4}
  CONFIG.TX_LINE_RATE {10.24}
  CONFIG.TX_PLL_TYPE {QPLL0}
  CONFIG.TX_REFCLK_FREQUENCY {320}
  CONFIG.TX_BUFFER_MODE {1}
  CONFIG.TX_OUTCLK_SOURCE {TXPLLREFCLK_DIV1}
  CONFIG.RX_LINE_RATE {10.24}
  CONFIG.RX_PLL_TYPE {QPLL0}
  CONFIG.RX_REFCLK_FREQUENCY {320}
  CONFIG.RX_BUFFER_MODE {0}
  CONFIG.RX_JTOL_FC {6.1427714}
  CONFIG.SIM_CPLL_CAL_BYPASS {0}
  CONFIG.RX_COMMA_P_ENABLE {true}
  CONFIG.RX_COMMA_MASK {1111111111}
  CONFIG.RX_COMMA_ALIGN_WORD {4}
  CONFIG.RX_SLIDE_MODE {PCS}
  CONFIG.ENABLE_OPTIONAL_PORTS {drpaddr_in drpclk_in drpdi_in drpen_in drpwe_in rxpolarity_in txpippmen_in txpippmovrden_in txpippmpd_in txpippmsel_in txpippmstepsize_in txpolarity_in drpdo_out drprdy_out txbufstatus_out}
  CONFIG.RX_REFCLK_SOURCE {}
  CONFIG.TX_REFCLK_SOURCE {}
  CONFIG.TXPROGDIV_FREQ_SOURCE {QPLL0}
  CONFIG.TXPROGDIV_FREQ_VAL {320}
  CONFIG.FREERUN_FREQUENCY {200}
} [get_ips $gtwizard_ultrascale]

##################################################################


##################################################################
# CREATE IP shift_ram_trigger
##################################################################

set c_shift_ram shift_ram_trigger
create_ip -name c_shift_ram -vendor xilinx.com -library ip -module_name $c_shift_ram

set_property -dict { 
  CONFIG.ShiftRegType {Variable_Length_Lossless}
  CONFIG.RegLastBit {true}
  CONFIG.Width {18}
  CONFIG.Depth {1024}
  CONFIG.DefaultData {000000000000000000}
  CONFIG.AsyncInitRadix {16}
  CONFIG.AsyncInitVal {00000}
  CONFIG.SCLR {true}
  CONFIG.SyncInitVal {000000000000000000}
} [get_ips $c_shift_ram]

##################################################################

##################################################################
# CREATE IP vio_0
##################################################################

set vio vio_0
create_ip -name vio -vendor xilinx.com -library ip -module_name $vio

set_property -dict { 
  CONFIG.C_PROBE_OUT10_INIT_VAL {0x00}
  CONFIG.C_PROBE_OUT49_WIDTH {3}
  CONFIG.C_PROBE_OUT48_WIDTH {10}
  CONFIG.C_PROBE_OUT45_WIDTH {1}
  CONFIG.C_PROBE_OUT44_WIDTH {2}
  CONFIG.C_PROBE_OUT43_WIDTH {2}
  CONFIG.C_PROBE_OUT42_WIDTH {2}
  CONFIG.C_PROBE_OUT41_WIDTH {2}
  CONFIG.C_PROBE_OUT40_WIDTH {2}
  CONFIG.C_PROBE_OUT39_WIDTH {2}
  CONFIG.C_PROBE_OUT38_WIDTH {2}
  CONFIG.C_PROBE_OUT37_WIDTH {2}
  CONFIG.C_PROBE_OUT36_WIDTH {2}
  CONFIG.C_PROBE_OUT35_WIDTH {2}
  CONFIG.C_PROBE_OUT34_WIDTH {2}
  CONFIG.C_PROBE_OUT33_WIDTH {2}
  CONFIG.C_PROBE_OUT32_WIDTH {2}
  CONFIG.C_PROBE_OUT31_WIDTH {2}
  CONFIG.C_PROBE_OUT30_WIDTH {2}
  CONFIG.C_PROBE_OUT29_WIDTH {2}
  CONFIG.C_PROBE_OUT28_WIDTH {2}
  CONFIG.C_PROBE_OUT27_WIDTH {2}
  CONFIG.C_PROBE_OUT26_WIDTH {2}
  CONFIG.C_PROBE_OUT25_WIDTH {2}
  CONFIG.C_PROBE_OUT24_WIDTH {2}
  CONFIG.C_PROBE_OUT23_WIDTH {2}
  CONFIG.C_PROBE_OUT22_WIDTH {2}
  CONFIG.C_PROBE_OUT21_WIDTH {2}
  CONFIG.C_PROBE_OUT20_WIDTH {2}
  CONFIG.C_PROBE_OUT19_WIDTH {2}
  CONFIG.C_PROBE_OUT18_WIDTH {2}
  CONFIG.C_PROBE_OUT16_WIDTH {2}
  CONFIG.C_PROBE_OUT15_WIDTH {2}
  CONFIG.C_PROBE_OUT14_WIDTH {2}
  CONFIG.C_PROBE_OUT13_WIDTH {2}
  CONFIG.C_PROBE_OUT10_WIDTH {7}
  CONFIG.C_PROBE_OUT9_WIDTH {1}
  CONFIG.C_PROBE_IN8_WIDTH {3}
  CONFIG.C_PROBE_IN7_WIDTH {10}
  CONFIG.C_PROBE_IN5_WIDTH {7}
  CONFIG.C_PROBE_IN3_WIDTH {28}
  CONFIG.C_PROBE_IN2_WIDTH {16}
  CONFIG.C_NUM_PROBE_OUT {52}
  CONFIG.C_NUM_PROBE_IN {9}
} [get_ips $vio]

##################################################################

# ##################################################################
# # CREATE IP vio_ethernet_enable
# ##################################################################

# set vio vio_ethernet_enable
# create_ip -name vio -vendor xilinx.com -library ip -module_name $vio

# set_property -dict { 
#   CONFIG.C_NUM_PROBE_OUT {2}
#   CONFIG.C_EN_PROBE_IN_ACTIVITY {0}
#   CONFIG.C_NUM_PROBE_IN {0}
# } [get_ips $vio]

# ##################################################################


##################################################################
# CREATE IP vio_40M
##################################################################

set vio vio_40M
create_ip -name vio -vendor xilinx.com -library ip -module_name $vio

set_property -dict { 
  # CONFIG.C_PROBE_OUT7_INIT_VAL {0xDEB}
  CONFIG.C_PROBE_OUT5_INIT_VAL {0x1}
  CONFIG.C_PROBE_OUT1_INIT_VAL {0xF4240}
  # CONFIG.C_PROBE_OUT7_WIDTH {12}
  CONFIG.C_PROBE_OUT2_WIDTH {1}
  CONFIG.C_PROBE_OUT1_WIDTH {20}
  CONFIG.C_PROBE_IN1_WIDTH {10}
  CONFIG.C_PROBE_IN0_WIDTH {10}
  CONFIG.C_NUM_PROBE_OUT {7}
  CONFIG.C_NUM_PROBE_IN {2}
} [get_ips $vio]

##################################################################


##################################################################
# CREATE IP vio_trigger
##################################################################

set vio vio_trigger
create_ip -name vio -vendor xilinx.com -library ip -module_name $vio

set_property -dict { 
  CONFIG.C_PROBE_OUT11_INIT_VAL {0x64}
  CONFIG.C_PROBE_OUT9_INIT_VAL {0x1}
  CONFIG.C_PROBE_OUT7_INIT_VAL {0xFE6}
  CONFIG.C_PROBE_OUT6_INIT_VAL {0xFFF}
  CONFIG.C_PROBE_OUT5_INIT_VAL {0x0C8}
  CONFIG.C_PROBE_OUT4_INIT_VAL {0x000}
  CONFIG.C_PROBE_OUT3_INIT_VAL {0x014}
  CONFIG.C_PROBE_OUT2_INIT_VAL {0x03C}
  CONFIG.C_PROBE_OUT11_WIDTH {8}
  CONFIG.C_PROBE_OUT7_WIDTH {12}
  CONFIG.C_PROBE_OUT6_WIDTH {12}
  CONFIG.C_PROBE_OUT5_WIDTH {12}
  CONFIG.C_PROBE_OUT4_WIDTH {12}
  CONFIG.C_PROBE_OUT3_WIDTH {12}
  CONFIG.C_PROBE_OUT2_WIDTH {12}
  CONFIG.C_NUM_PROBE_OUT {12}
  CONFIG.C_EN_PROBE_IN_ACTIVITY {0}
  CONFIG.C_NUM_PROBE_IN {0}
} [get_ips $vio]

##################################################################

# CREATE IP vio_drp
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_drp
set_property -dict [list CONFIG.C_PROBE_OUT3_WIDTH {16} CONFIG.C_PROBE_OUT2_WIDTH {9} CONFIG.C_PROBE_IN1_WIDTH {16} CONFIG.C_NUM_PROBE_OUT {4} CONFIG.C_NUM_PROBE_IN {2} CONFIG.Component_Name {vio_drp}] [get_ips vio_drp]
set_property -dict [list CONFIG.C_PROBE_OUT5_WIDTH {4} CONFIG.C_NUM_PROBE_OUT {6}] [get_ips vio_drp]

# ##################################################################
# # CREATE IP ila_tdcv2_decoder
# ##################################################################

set ila ila_tdcv2_decoder
create_ip -name ila -vendor xilinx.com -library ip -module_name $ila

set_property -dict { 
  CONFIG.C_PROBE18_WIDTH {17}
  CONFIG.C_PROBE17_WIDTH {8}
  CONFIG.C_PROBE16_WIDTH {2}
  CONFIG.C_PROBE15_WIDTH {5}
  CONFIG.C_PROBE14_WIDTH {32}
  CONFIG.C_PROBE13_WIDTH {1}
  CONFIG.C_PROBE12_WIDTH {17}
  CONFIG.C_PROBE11_WIDTH {8}
  CONFIG.C_PROBE10_WIDTH {2}
  CONFIG.C_PROBE9_WIDTH {5}
  CONFIG.C_PROBE8_WIDTH {32}
  CONFIG.C_PROBE7_WIDTH {1}
  CONFIG.C_PROBE6_WIDTH {2}
  CONFIG.C_PROBE5_WIDTH {2}
  CONFIG.C_PROBE4_WIDTH {8}
  CONFIG.C_PROBE3_WIDTH {64}
  CONFIG.C_PROBE2_WIDTH {8}
  CONFIG.C_PROBE1_WIDTH {64}
  CONFIG.C_PROBE0_WIDTH {1}
  CONFIG.C_NUM_OF_PROBES {19}
} [get_ips $ila]

# ##################################################################




# ##################################################################
# # CREATE IP ila_tdc_raw
# ##################################################################

# set ila ila_tdc_raw
# create_ip -name ila -vendor xilinx.com -library ip -module_name $ila

# set_property -dict { 
#   CONFIG.C_PROBE25_WIDTH {1}
#   CONFIG.C_PROBE24_WIDTH {1}
#   CONFIG.C_PROBE23_WIDTH {1}
#   CONFIG.C_PROBE22_WIDTH {1}
#   CONFIG.C_PROBE21_WIDTH {1}
#   CONFIG.C_PROBE20_WIDTH {1}
#   CONFIG.C_PROBE19_WIDTH {1}
#   CONFIG.C_PROBE18_WIDTH {1}
#   CONFIG.C_PROBE17_WIDTH {1}
#   CONFIG.C_PROBE16_WIDTH {1}
#   CONFIG.C_PROBE15_WIDTH {1}
#   CONFIG.C_PROBE14_WIDTH {1}
#   CONFIG.C_PROBE13_WIDTH {1}
#   CONFIG.C_PROBE12_WIDTH {1}
#   CONFIG.C_PROBE11_WIDTH {1}
#   CONFIG.C_PROBE10_WIDTH {5}
#   CONFIG.C_PROBE9_WIDTH {5}
#   CONFIG.C_PROBE8_WIDTH {2}
#   CONFIG.C_PROBE7_WIDTH {2}
#   CONFIG.C_PROBE6_WIDTH {4}
#   CONFIG.C_PROBE5_WIDTH {4}
#   CONFIG.C_PROBE4_WIDTH {4}
#   CONFIG.C_PROBE3_WIDTH {4}
#   CONFIG.C_PROBE2_WIDTH {34}
#   CONFIG.C_PROBE1_WIDTH {34}
#   CONFIG.C_PROBE0_WIDTH {8}
#   CONFIG.C_NUM_OF_PROBES {11}
#   CONFIG.C_ADV_TRIGGER {true}
# } [get_ips $ila]

# ##################################################################

##################################################################
# CREATE IP vio_hit
##################################################################

set vio vio_hit
create_ip -name vio -vendor xilinx.com -library ip -module_name $vio

set_property -dict { 
  CONFIG.C_PROBE_OUT9_INIT_VAL {0x001}
  CONFIG.C_PROBE_OUT5_INIT_VAL {0x1FFFFFF}
  CONFIG.C_PROBE_OUT4_INIT_VAL {0x190}
  CONFIG.C_PROBE_OUT3_INIT_VAL {0x00A}
  CONFIG.C_PROBE_OUT2_INIT_VAL {0x1}
  CONFIG.C_PROBE_OUT9_WIDTH {12}
  CONFIG.C_PROBE_OUT8_WIDTH {24}
  CONFIG.C_PROBE_OUT7_WIDTH {12}
  CONFIG.C_PROBE_OUT5_WIDTH {24}
  CONFIG.C_PROBE_OUT4_WIDTH {12}
  CONFIG.C_PROBE_OUT3_WIDTH {12}
  CONFIG.C_NUM_PROBE_OUT {10}
  CONFIG.C_EN_PROBE_IN_ACTIVITY {0}
  CONFIG.C_NUM_PROBE_IN {0}
} [get_ips $vio]

##################################################################

##################################################################
# CREATE IP ila_event_builder
##################################################################

set ila ila_event_builder
create_ip -name ila -vendor xilinx.com -library ip -version 6.2 -module_name $ila

set_property -dict { 
  CONFIG.C_PROBE14_WIDTH {1}
  CONFIG.C_PROBE13_WIDTH {6}
  CONFIG.C_PROBE12_WIDTH {6}
  CONFIG.C_PROBE11_WIDTH {10}
  CONFIG.C_PROBE8_WIDTH {40}
  CONFIG.C_PROBE4_WIDTH {40}
  CONFIG.C_PROBE1_WIDTH {29}
  CONFIG.C_NUM_OF_PROBES {14}
} [get_ips $ila]

##################################################################



create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name fpga_tdc_clock
set_property -dict [list CONFIG.Component_Name {fpga_tdc_clock} CONFIG.USE_FREQ_SYNTH {false} CONFIG.USE_PHASE_ALIGNMENT {true} CONFIG.PRIM_SOURCE {Global_buffer} CONFIG.PRIM_IN_FREQ {160} CONFIG.CLKOUT2_USED {true} CONFIG.CLKOUT3_USED {true} CONFIG.CLKOUT4_USED {true} CONFIG.CLKOUT5_USED {true} CONFIG.CLKOUT2_REQUESTED_PHASE {45} CONFIG.CLKOUT3_REQUESTED_PHASE {90} CONFIG.CLKOUT4_REQUESTED_PHASE {135} CONFIG.CLKOUT5_REQUESTED_PHASE {0} CONFIG.USE_LOCKED {false} CONFIG.USE_RESET {false} CONFIG.SECONDARY_SOURCE {Single_ended_clock_capable_pin} CONFIG.CLKIN1_JITTER_PS {62.5} CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT2_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT3_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT4_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT5_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT6_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT7_REQUESTED_OUT_FREQ {160} CONFIG.CLKOUT1_DRIVES {Buffer} CONFIG.CLKOUT2_DRIVES {Buffer} CONFIG.CLKOUT3_DRIVES {Buffer} CONFIG.CLKOUT4_DRIVES {Buffer} CONFIG.CLKOUT5_DRIVES {Buffer} CONFIG.CLKOUT6_DRIVES {Buffer} CONFIG.CLKOUT7_DRIVES {Buffer} CONFIG.MMCM_CLKFBOUT_MULT_F {6.000} CONFIG.MMCM_CLKIN1_PERIOD {6.250} CONFIG.MMCM_CLKIN2_PERIOD {10.0} CONFIG.MMCM_CLKOUT0_DIVIDE_F {6.000} CONFIG.MMCM_CLKOUT1_DIVIDE {6} CONFIG.MMCM_CLKOUT1_PHASE {45.000} CONFIG.MMCM_CLKOUT2_DIVIDE {6} CONFIG.MMCM_CLKOUT2_PHASE {90.000} CONFIG.MMCM_CLKOUT3_DIVIDE {6} CONFIG.MMCM_CLKOUT3_PHASE {135.000} CONFIG.MMCM_CLKOUT4_DIVIDE {6} CONFIG.NUM_OUT_CLKS {5} CONFIG.CLKOUT1_JITTER {108.430} CONFIG.CLKOUT1_PHASE_ERROR {95.076} CONFIG.CLKOUT2_JITTER {108.430} CONFIG.CLKOUT2_PHASE_ERROR {95.076} CONFIG.CLKOUT3_JITTER {108.430} CONFIG.CLKOUT3_PHASE_ERROR {95.076} CONFIG.CLKOUT4_JITTER {108.430} CONFIG.CLKOUT4_PHASE_ERROR {95.076} CONFIG.CLKOUT5_JITTER {108.430} CONFIG.CLKOUT5_PHASE_ERROR {95.076}] [get_ips fpga_tdc_clock]



create_ip -name ila -vendor xilinx.com -library ip -module_name ila_ram
set_property -dict [list CONFIG.C_PROBE17_WIDTH {12} CONFIG.C_PROBE17_WIDTH {17} CONFIG.C_PROBE12_WIDTH {5} CONFIG.C_PROBE11_WIDTH {5} CONFIG.C_PROBE10_WIDTH {1} CONFIG.C_PROBE7_WIDTH {10} CONFIG.C_PROBE6_WIDTH {10} CONFIG.C_PROBE5_WIDTH {10} CONFIG.C_PROBE4_WIDTH {32} CONFIG.C_PROBE2_WIDTH {32} CONFIG.C_PROBE0_WIDTH {32} CONFIG.C_NUM_OF_PROBES {19} CONFIG.Component_Name {ila_ram}] [get_ips ila_ram]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_trigger_interface
set_property -dict [list CONFIG.C_PROBE9_WIDTH {12} CONFIG.C_PROBE8_WIDTH {17} CONFIG.C_PROBE7_WIDTH {12} CONFIG.C_PROBE5_WIDTH {29} CONFIG.C_NUM_OF_PROBES {11} CONFIG.Component_Name {ila_trigger_interface}] [get_ips ila_trigger_interface]

create_ip -name fifo_generator -vendor xilinx.com -library ip -module_name readout_fifo
set_property -dict [list CONFIG.Component_Name {readout_fifo} CONFIG.Fifo_Implementation {Independent_Clocks_Builtin_FIFO} CONFIG.Performance_Options {First_Word_Fall_Through} CONFIG.Input_Data_Width {40} CONFIG.Output_Data_Width {40} CONFIG.Use_Embedded_Registers {false} CONFIG.Read_Clock_Frequency {125} CONFIG.Write_Clock_Frequency {160} CONFIG.Full_Threshold_Assert_Value {1022} CONFIG.Full_Threshold_Negate_Value {1021} CONFIG.Empty_Threshold_Assert_Value {6} CONFIG.Empty_Threshold_Negate_Value {7}] [get_ips readout_fifo]


create_ip -name fifo_generator -vendor xilinx.com -library ip -module_name trigger_FIFO
set_property -dict [list CONFIG.Component_Name {trigger_FIFO} CONFIG.Performance_Options {First_Word_Fall_Through} CONFIG.Input_Data_Width {29} CONFIG.Input_Depth {512} CONFIG.Output_Data_Width {29} CONFIG.Output_Depth {512} CONFIG.Use_Embedded_Registers {false} CONFIG.Data_Count_Width {9} CONFIG.Write_Data_Count_Width {9} CONFIG.Read_Data_Count_Width {9} CONFIG.Full_Threshold_Assert_Value {511} CONFIG.Full_Threshold_Negate_Value {510} CONFIG.Empty_Threshold_Assert_Value {4} CONFIG.Empty_Threshold_Negate_Value {5}] [get_ips trigger_FIFO]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_rawdata_8b
set_property -dict [list CONFIG.C_PROBE1_WIDTH {8} CONFIG.C_PROBE0_WIDTH {8} CONFIG.C_NUM_OF_PROBES {2} CONFIG.Component_Name {ila_rawdata_8b}] [get_ips ila_rawdata_8b]

create_ip -name vio -vendor xilinx.com -library ip -module_name vio_BER
set_property -dict [list CONFIG.C_PROBE_IN39_WIDTH {36} CONFIG.C_PROBE_IN38_WIDTH {36} CONFIG.C_PROBE_IN37_WIDTH {44} CONFIG.C_PROBE_IN36_WIDTH {44} CONFIG.C_PROBE_IN35_WIDTH {36} CONFIG.C_PROBE_IN34_WIDTH {36} CONFIG.C_PROBE_IN33_WIDTH {44} CONFIG.C_PROBE_IN32_WIDTH {44} CONFIG.C_PROBE_IN31_WIDTH {36} CONFIG.C_PROBE_IN30_WIDTH {36} CONFIG.C_PROBE_IN29_WIDTH {44} CONFIG.C_PROBE_IN28_WIDTH {44} CONFIG.C_PROBE_IN27_WIDTH {36} CONFIG.C_PROBE_IN26_WIDTH {36} CONFIG.C_PROBE_IN25_WIDTH {44} CONFIG.C_PROBE_IN24_WIDTH {44} CONFIG.C_PROBE_IN23_WIDTH {36} CONFIG.C_PROBE_IN22_WIDTH {36} CONFIG.C_PROBE_IN21_WIDTH {44} CONFIG.C_PROBE_IN20_WIDTH {44} CONFIG.C_PROBE_IN19_WIDTH {36} CONFIG.C_PROBE_IN18_WIDTH {36} CONFIG.C_PROBE_IN17_WIDTH {44} CONFIG.C_PROBE_IN16_WIDTH {44} CONFIG.C_PROBE_IN15_WIDTH {36} CONFIG.C_PROBE_IN14_WIDTH {36} CONFIG.C_PROBE_IN13_WIDTH {44} CONFIG.C_PROBE_IN12_WIDTH {44} CONFIG.C_PROBE_IN11_WIDTH {36} CONFIG.C_PROBE_IN10_WIDTH {36} CONFIG.C_PROBE_IN9_WIDTH {44} CONFIG.C_PROBE_IN8_WIDTH {44} CONFIG.C_PROBE_IN7_WIDTH {36} CONFIG.C_PROBE_IN6_WIDTH {36} CONFIG.C_PROBE_IN5_WIDTH {44} CONFIG.C_PROBE_IN4_WIDTH {44} CONFIG.C_PROBE_IN3_WIDTH {36} CONFIG.C_PROBE_IN2_WIDTH {36} CONFIG.C_PROBE_IN1_WIDTH {44} CONFIG.C_PROBE_IN0_WIDTH {44} CONFIG.C_NUM_PROBE_IN {40} CONFIG.Component_Name {vio_BER}] [get_ips vio_BER]


create_ip -name blk_mem_gen -vendor xilinx.com -library ip -module_name lpGBT_ram
set_property -dict [list CONFIG.Component_Name {lpGBT_ram} CONFIG.Memory_Type {Simple_Dual_Port_RAM} CONFIG.Use_Byte_Write_Enable {false} CONFIG.Byte_Size {9} CONFIG.Assume_Synchronous_Clk {true} CONFIG.Write_Width_A {8} CONFIG.Write_Depth_A {496} CONFIG.Read_Width_A {8} CONFIG.Operating_Mode_A {NO_CHANGE} CONFIG.Enable_A {Always_Enabled} CONFIG.Write_Width_B {8} CONFIG.Read_Width_B {8} CONFIG.Operating_Mode_B {READ_FIRST} CONFIG.Enable_B {Use_ENB_Pin} CONFIG.Register_PortA_Output_of_Memory_Primitives {false} CONFIG.Register_PortB_Output_of_Memory_Primitives {false} CONFIG.Load_Init_File {false} CONFIG.Coe_File {no_coe_file_loaded} CONFIG.Port_B_Clock {100} CONFIG.Port_B_Enable_Rate {100}] [get_ips lpGBT_ram]


create_ip -name fifo_generator -vendor xilinx.com -library ip -module_name fifo_160bit
set_property -dict [list CONFIG.Component_Name {fifo_160bit} CONFIG.Performance_Options {First_Word_Fall_Through} CONFIG.Input_Data_Width {160} CONFIG.Input_Depth {512} CONFIG.Output_Data_Width {160} CONFIG.Output_Depth {512} CONFIG.Use_Embedded_Registers {false} CONFIG.Data_Count_Width {9} CONFIG.Write_Data_Count_Width {9} CONFIG.Read_Data_Count_Width {9} CONFIG.Full_Threshold_Assert_Value {511} CONFIG.Full_Threshold_Negate_Value {510} CONFIG.Empty_Threshold_Assert_Value {4} CONFIG.Empty_Threshold_Negate_Value {5}] [get_ips fifo_160bit]

create_ip -name vio -vendor xilinx.com -library ip -module_name vio_gbtic_controller
set_property -dict [list CONFIG.C_PROBE_OUT5_WIDTH {8} CONFIG.C_PROBE_OUT2_WIDTH {16} CONFIG.C_PROBE_OUT1_WIDTH {16} CONFIG.C_PROBE_OUT0_WIDTH {8} CONFIG.C_PROBE_IN5_WIDTH {16} CONFIG.C_PROBE_IN4_WIDTH {16} CONFIG.C_PROBE_IN3_WIDTH {8} CONFIG.C_PROBE_IN2_WIDTH {8} CONFIG.C_NUM_PROBE_OUT {8} CONFIG.C_NUM_PROBE_IN {6} CONFIG.Component_Name {vio_gbtic_controller}] [get_ips vio_gbtic_controller]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_ic_rx
set_property -dict [list CONFIG.C_PROBE2_WIDTH {8} CONFIG.C_PROBE1_WIDTH {8} CONFIG.C_PROBE0_WIDTH {2} CONFIG.C_NUM_OF_PROBES {6} CONFIG.Component_Name {ila_ic_rx}] [get_ips ila_ic_rx]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_ic_tx
set_property -dict [list CONFIG.C_PROBE21_WIDTH {8} CONFIG.C_PROBE20_WIDTH {8} CONFIG.C_PROBE19_WIDTH {8} CONFIG.C_PROBE18_WIDTH {8} CONFIG.C_PROBE17_WIDTH {8} CONFIG.C_PROBE16_WIDTH {8} CONFIG.C_PROBE15_WIDTH {8} CONFIG.C_PROBE14_WIDTH {8} CONFIG.C_PROBE13_WIDTH {8} CONFIG.C_PROBE12_WIDTH {8} CONFIG.C_PROBE11_WIDTH {16} CONFIG.C_PROBE10_WIDTH {2} CONFIG.C_PROBE7_WIDTH {8} CONFIG.C_PROBE4_WIDTH {8} CONFIG.C_PROBE3_WIDTH {16} CONFIG.C_PROBE2_WIDTH {16} CONFIG.C_PROBE1_WIDTH {8} CONFIG.C_NUM_OF_PROBES {22} CONFIG.Component_Name {ila_ic_tx}] [get_ips ila_ic_tx]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_gbtic_controller
set_property -dict [list CONFIG.C_PROBE12_WIDTH {8} CONFIG.C_PROBE10_WIDTH {8} CONFIG.C_PROBE7_WIDTH {8} CONFIG.C_PROBE6_WIDTH {16} CONFIG.C_PROBE5_WIDTH {8} CONFIG.C_PROBE3_WIDTH {160} CONFIG.C_NUM_OF_PROBES {14} CONFIG.Component_Name {ila_gbtic_controller}] [get_ips ila_gbtic_controller]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_event_builder_triggermode
set_property -dict [list CONFIG.C_PROBE20_WIDTH {6} CONFIG.C_PROBE19_WIDTH {6} CONFIG.C_PROBE18_WIDTH {10} CONFIG.C_PROBE15_WIDTH {40} CONFIG.C_PROBE13_WIDTH {40} CONFIG.C_PROBE8_WIDTH {10} CONFIG.C_PROBE7_WIDTH {10} CONFIG.C_PROBE6_WIDTH {10} CONFIG.C_PROBE5_WIDTH {10} CONFIG.C_PROBE4_WIDTH {10} CONFIG.C_PROBE3_WIDTH {10} CONFIG.C_PROBE1_WIDTH {29} CONFIG.C_NUM_OF_PROBES {21} CONFIG.Component_Name {ila_event_builder_triggermode}] [get_ips ila_event_builder_triggermode]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_drp
set_property -dict [list CONFIG.C_PROBE5_WIDTH {16} CONFIG.C_PROBE4_WIDTH {9} CONFIG.C_PROBE1_WIDTH {16} CONFIG.C_NUM_OF_PROBES {8} CONFIG.Component_Name {ila_drp}] [get_ips ila_drp]

create_ip -name ila -vendor xilinx.com -library ip -module_name ila_edge_lock
set_property -dict [list CONFIG.C_PROBE7_WIDTH {36} CONFIG.C_PROBE6_WIDTH {6} CONFIG.C_PROBE5_WIDTH {40} CONFIG.C_PROBE4_WIDTH {40} CONFIG.C_PROBE3_WIDTH {4} CONFIG.C_PROBE2_WIDTH {20} CONFIG.C_PROBE1_WIDTH {8} CONFIG.C_NUM_OF_PROBES {9} CONFIG.Component_Name {ila_edge_lock}] [get_ips ila_edge_lock]





