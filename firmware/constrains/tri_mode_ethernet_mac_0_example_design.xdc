# PART is kintexu xcku035fbva676

#
####
#######
##########
#############
#################
## System level constraints




#
####
#######
##########
#############
#################
#EXAMPLE DESIGN CONSTRAINTS
# please keep phy_resetn in output which is used for reset PHY
set_property PACKAGE_PIN AB14 [get_ports phy_resetn]
set_property IOSTANDARD LVCMOS25 [get_ports phy_resetn]

# lock to unused header - ensure this is unused
#set_property PACKAGE_PIN AJ24     [get_ports serial_response]
#set_property PACKAGE_PIN AK25     [get_ports tx_statistics_s]
#set_property PACKAGE_PIN AE25     [get_ports rx_statistics_s]
#set_property IOSTANDARD  LVCMOS25 [get_ports serial_response]
#set_property IOSTANDARD  LVCMOS25 [get_ports tx_statistics_s]
#set_property IOSTANDARD  LVCMOS25 [get_ports rx_statistics_s]

set_property PACKAGE_PIN AC14 [get_ports mdc]
set_property PACKAGE_PIN AA14 [get_ports mdio]
set_property IOSTANDARD LVCMOS25 [get_ports mdc]
set_property IOSTANDARD LVCMOS25 [get_ports mdio]

########## RGMII SPECIFIC IO CONSTRAINTS FOR the MINIDAQ BOARD ##########
### These PIN constraints use the on-board PHY which requires 2.5V ###
### the Clock duty cycle is out of spec at this voltage ###

set_property PACKAGE_PIN AF19 [get_ports {rgmii_rxd[3]}]
set_property PACKAGE_PIN AE18 [get_ports {rgmii_rxd[2]}]
set_property PACKAGE_PIN AD18 [get_ports {rgmii_rxd[1]}]
set_property PACKAGE_PIN AF17 [get_ports {rgmii_rxd[0]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_rxd[3]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_rxd[2]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_rxd[1]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_rxd[0]}]

set_property PACKAGE_PIN AE13 [get_ports {rgmii_txd[3]}]
set_property PACKAGE_PIN AD15 [get_ports {rgmii_txd[2]}]
set_property PACKAGE_PIN AE16 [get_ports {rgmii_txd[1]}]
set_property PACKAGE_PIN AC13 [get_ports {rgmii_txd[0]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_txd[3]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_txd[2]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_txd[1]}]
set_property IOSTANDARD LVCMOS25 [get_ports {rgmii_txd[0]}]

set_property PACKAGE_PIN AD14 [get_ports rgmii_tx_ctl]
set_property PACKAGE_PIN AE15 [get_ports rgmii_txc]
set_property IOSTANDARD LVCMOS25 [get_ports rgmii_tx_ctl]
set_property IOSTANDARD LVCMOS25 [get_ports rgmii_txc]

set_property PACKAGE_PIN AC16 [get_ports rgmii_rx_ctl]
set_property IOSTANDARD LVCMOS25 [get_ports rgmii_rx_ctl]

set_property PACKAGE_PIN Y17 [get_ports rgmii_rxc]
set_property IOSTANDARD LVCMOS25 [get_ports rgmii_rxc]



############################################################
# Clock Period Constraints                                 #
############################################################

#############################################################
## TX Clock period Constraints                              #
#############################################################
## Transmitter clock period constraints: please do not relax
# create_clock -name clk_in_p -period 5.000 [get_ports USER_CLOCK_P]
# set_input_jitter clk_in_p 0.050



############################################################
# Get auto-generated clock names                           #
############################################################




## Ignore pause deserialiser as only present to prevent logic stripping
#set_false_path -from [get_ports pause_req*]
#set_false_path -from [get_cells pause_req* -filter {IS_SEQUENTIAL}]
#set_false_path -from [get_cells pause_val* -filter {IS_SEQUENTIAL}]


############################################################
# Output Delay constraints
############################################################

#set_false_path -to [get_ports frame_error]
#set_false_path -to [get_ports frame_errorn]
#set_false_path -to [get_ports serial_response]
#set_false_path -to [get_ports tx_statistics_s]
#set_false_path -to [get_ports rx_statistics_s]
set_output_delay -clock [get_clocks -of [get_pins ethernet_mac/example_clocks/clock_generator/mmcm_adv_inst/CLKOUT1]] 1.000 [get_ports mdc]

# no timing associated with output
set_false_path -from [get_cells -hier -filter {name =~ *phy_resetn_int_reg}] -to [get_ports phy_resetn]

############################################################
# Example design Clock Crossing Constraints                          #
############################################################
set_false_path -from [get_cells -hier -filter {name =~ *phy_resetn_int_reg}] -to [get_cells -hier -filter {name =~ *axi_lite_reset_gen/reset_sync*}]


# control signal is synched over clock boundary separately
#set_false_path -from [get_cells -hier -filter {name =~ *tx_stats_reg[*]}] -to [get_cells -hier -filter {name =~ ethernet_mac/tx_stats_shift_reg[*]}]
#set_false_path -from [get_cells -hier -filter {name =~ *rx_stats_reg[*]}] -to [get_cells -hier -filter {name =~ ethernet_mac/rx_stats_shift_reg[*]}]



############################################################
# Ignore paths to resync flops
############################################################
set_false_path -to [get_pins -filter {REF_PIN_NAME =~ PRE} -of [get_cells -hier -regexp {.*\/reset_sync.*}]]
set_false_path -to [get_pins -filter {REF_PIN_NAME =~ D} -of [get_cells -regexp {.*\/.*_sync.*}]]
set_max_delay -from [get_cells ethernet_mac/tx_stats_toggle_reg] -to [get_cells ethernet_mac/tx_stats_sync/data_sync_reg0] 6 -datapath_only
set_max_delay -from [get_cells ethernet_mac/rx_stats_toggle_reg] -to [get_cells ethernet_mac/rx_stats_sync/data_sync_reg0] 6 -datapath_only



#
####
#######
##########
#############
#################
#FIFO BLOCK CONSTRAINTS

############################################################
# FIFO Clock Crossing Constraints                          #
############################################################

# control signal is synched separately so this is a false path
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *rx_fifo_i/rd_addr_reg[*]}] -to [get_cells -hier -filter {name =~ *fifo*wr_rd_addr_reg[*]}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *rx_fifo_i/wr_store_frame_tog_reg}] -to [get_cells -hier -filter {name =~ *fifo_i/resync_wr_store_frame_tog/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *rx_fifo_i/update_addr_tog_reg}] -to [get_cells -hier -filter {name =~ *rx_fifo_i/sync_rd_addr_tog/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/rd_addr_txfer_reg[*]}] -to [get_cells -hier -filter {name =~ *fifo*wr_rd_addr_reg[*]}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/wr_frame_in_fifo_reg}] -to [get_cells -hier -filter {name =~ *tx_fifo_i/resync_wr_frame_in_fifo/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/wr_frames_in_fifo_reg}] -to [get_cells -hier -filter {name =~ *tx_fifo_i/resync_wr_frames_in_fifo/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/frame_in_fifo_valid_tog_reg}] -to [get_cells -hier -filter {name =~ *tx_fifo_i/resync_fif_valid_tog/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/rd_txfer_tog_reg}] -to [get_cells -hier -filter {name =~ *tx_fifo_i/resync_rd_txfer_tog/data_sync_reg0}] 3.200
set_max_delay -datapath_only -from [get_cells -hier -filter {name =~ *tx_fifo_i/rd_tran_frame_tog_reg}] -to [get_cells -hier -filter {name =~ *tx_fifo_i/resync_rd_tran_frame_tog/data_sync_reg0}] 3.200

set_power_opt -exclude_cells [get_cells -hierarchical -filter { PRIMITIVE_TYPE =~ *.bram.* }]

