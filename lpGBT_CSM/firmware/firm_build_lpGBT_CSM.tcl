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
# // File   : firm_build_lpGBT_CSM.tcl
# // Create : 2021-03-16 21:39:48
# // Revise : 2021-03-16 21:39:48
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
set firmware_dir "$origin_dir/../../../firm_lpgbtCsm"


create_project lpgbtcsm_newmezz $firmware_dir -part xc7a35tfgg484-2 -force
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
set_property -name "top" -value "lpgbtcms_fpga_top" -objects $obj


create_ip -name clk_wiz -vendor xilinx.com -library ip -module_name clk_gen
set_property -dict [list CONFIG.Component_Name {clk_gen} CONFIG.PRIM_SOURCE {Differential_clock_capable_pin} CONFIG.PRIM_IN_FREQ {160} CONFIG.CLKOUT1_REQUESTED_OUT_FREQ {40} CONFIG.USE_RESET {false} CONFIG.CLKIN1_JITTER_PS {62.5} CONFIG.MMCM_CLKFBOUT_MULT_F {6.250} CONFIG.MMCM_CLKIN1_PERIOD {6.250} CONFIG.MMCM_CLKOUT0_DIVIDE_F {25.000} CONFIG.CLKOUT1_JITTER {140.177} CONFIG.CLKOUT1_PHASE_ERROR {92.596}] [get_ips clk_gen]

# vio ip JTAG
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_JTAG 
set_property -dict [list CONFIG.C_PROBE_OUT3_INIT_VAL {0x040} CONFIG.C_PROBE_OUT2_INIT_VAL {0x0E11} CONFIG.C_PROBE_OUT3_WIDTH {9} CONFIG.C_PROBE_OUT2_WIDTH {13} CONFIG.C_PROBE_OUT1_WIDTH {12} CONFIG.C_PROBE_OUT0_WIDTH {256} CONFIG.C_PROBE_IN4_WIDTH {256} CONFIG.C_PROBE_IN3_WIDTH {9} CONFIG.C_PROBE_IN2_WIDTH {256} CONFIG.C_PROBE_IN1_WIDTH {256} CONFIG.C_NUM_PROBE_OUT {6} CONFIG.C_NUM_PROBE_IN {5}] [get_ips vio_JTAG]

# vio ip setup0
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_setup0  
set_property -dict [list CONFIG.C_PROBE_OUT21_INIT_VAL {0x1} CONFIG.C_PROBE_OUT20_INIT_VAL {0x1} CONFIG.C_PROBE_OUT17_INIT_VAL {0x1} CONFIG.C_PROBE_OUT15_INIT_VAL {0x7aaaa} CONFIG.C_PROBE_OUT14_INIT_VAL {0xffffff} CONFIG.C_PROBE_OUT13_INIT_VAL {0xffffff} CONFIG.C_PROBE_OUT12_INIT_VAL {0x000000} CONFIG.C_PROBE_OUT10_INIT_VAL {0x1} CONFIG.C_PROBE_OUT1_INIT_VAL {0x1} CONFIG.C_PROBE_OUT20_WIDTH {3} CONFIG.C_PROBE_OUT15_WIDTH {19} CONFIG.C_PROBE_OUT14_WIDTH {24} CONFIG.C_PROBE_OUT13_WIDTH {24} CONFIG.C_PROBE_OUT12_WIDTH {24} CONFIG.C_NUM_PROBE_OUT {27} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}] [get_ips vio_setup0]

# vio ip setup1
create_ip -name vio -vendor xilinx.com -library ip  -module_name vio_setup1 
set_property -dict [list CONFIG.C_PROBE_OUT7_INIT_VAL {0x1f} CONFIG.C_PROBE_OUT5_INIT_VAL {0xf9c} CONFIG.C_PROBE_OUT3_INIT_VAL {0xfff} CONFIG.C_PROBE_OUT2_INIT_VAL {0xfff} CONFIG.C_PROBE_OUT1_INIT_VAL {0x100} CONFIG.C_PROBE_OUT0_INIT_VAL {0x050} CONFIG.C_PROBE_OUT7_WIDTH {12} CONFIG.C_PROBE_OUT6_WIDTH {12} CONFIG.C_PROBE_OUT5_WIDTH {12} CONFIG.C_PROBE_OUT4_WIDTH {12} CONFIG.C_PROBE_OUT3_WIDTH {12} CONFIG.C_PROBE_OUT2_WIDTH {12} CONFIG.C_PROBE_OUT1_WIDTH {12} CONFIG.C_PROBE_OUT0_WIDTH {10} CONFIG.C_NUM_PROBE_OUT {9} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}] [get_ips vio_setup1]

# vio ip setup2
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_setup2 
set_property -dict [list CONFIG.C_PROBE_OUT1_INIT_VAL {0x19CA01CC} CONFIG.C_PROBE_OUT0_INIT_VAL {0x3} CONFIG.C_PROBE_OUT1_WIDTH {32} CONFIG.C_PROBE_OUT0_WIDTH {4} CONFIG.C_NUM_PROBE_OUT {3} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}] [get_ips vio_setup2]


# vio ip control0
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_control0
set_property -dict [list CONFIG.C_NUM_PROBE_OUT {6} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}  CONFIG.C_PROBE_OUT4_WIDTH {4}] [get_ips vio_control0]

# vio ip control1
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_control1
set_property -dict [list CONFIG.C_PROBE_OUT6_INIT_VAL {0x2} CONFIG.C_PROBE_OUT5_INIT_VAL {0x4} CONFIG.C_PROBE_OUT4_INIT_VAL {0x2} CONFIG.C_PROBE_OUT3_INIT_VAL {0x2} CONFIG.C_PROBE_OUT1_INIT_VAL {0x4} CONFIG.C_PROBE_OUT0_INIT_VAL {0x08} CONFIG.C_PROBE_OUT6_WIDTH {2} CONFIG.C_PROBE_OUT5_WIDTH {4} CONFIG.C_PROBE_OUT4_WIDTH {4} CONFIG.C_PROBE_OUT3_WIDTH {4} CONFIG.C_PROBE_OUT2_WIDTH {4} CONFIG.C_PROBE_OUT1_WIDTH {4} CONFIG.C_PROBE_OUT0_WIDTH {5} CONFIG.C_NUM_PROBE_OUT {8} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}] [get_ips vio_control1]


# vio ip asd_setup
create_ip -name vio -vendor xilinx.com -library ip -module_name vio_asd_setup
set_property -dict [list CONFIG.C_PROBE_OUT7_INIT_VAL {0x6C} CONFIG.C_PROBE_OUT6_INIT_VAL {0x2} CONFIG.C_PROBE_OUT5_INIT_VAL {0xE} CONFIG.C_PROBE_OUT4_INIT_VAL {0x2} CONFIG.C_PROBE_OUT3_INIT_VAL {0x04} CONFIG.C_PROBE_OUT2_INIT_VAL {0x7} CONFIG.C_PROBE_OUT7_WIDTH {8} CONFIG.C_PROBE_OUT6_WIDTH {3} CONFIG.C_PROBE_OUT5_WIDTH {4} CONFIG.C_PROBE_OUT4_WIDTH {3} CONFIG.C_PROBE_OUT3_WIDTH {4} CONFIG.C_PROBE_OUT2_WIDTH {3} CONFIG.C_PROBE_OUT1_WIDTH {1} CONFIG.C_PROBE_OUT8_WIDTH {13} CONFIG.C_PROBE_OUT0_WIDTH {16} CONFIG.C_NUM_PROBE_OUT {11} CONFIG.C_NUM_PROBE_IN {0} CONFIG.C_EN_PROBE_IN_ACTIVITY {0}] [get_ips vio_asd_setup]


create_ip -name vio -vendor xilinx.com -library ip -module_name loop_config
set_property -dict [list CONFIG.C_PROBE_IN7_WIDTH {32} CONFIG.C_PROBE_IN6_WIDTH {32} CONFIG.C_PROBE_IN5_WIDTH {32} CONFIG.C_PROBE_IN4_WIDTH {32} CONFIG.C_PROBE_IN3_WIDTH {32} CONFIG.C_PROBE_IN2_WIDTH {32} CONFIG.C_PROBE_IN1_WIDTH {32} CONFIG.C_PROBE_IN0_WIDTH {32} CONFIG.C_NUM_PROBE_OUT {3} CONFIG.C_NUM_PROBE_IN {8} CONFIG.Component_Name {loop_config}] [get_ips loop_config]

