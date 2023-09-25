##===================================================================================================##
##=========================================  CLOCKS  ================================================##
##===================================================================================================##

##==============##
## FABRIC CLOCK ##
##==============##
#create_clock -period 12.5000 -name free_running_clock [get_pins sys_clk_inst/mgt_freedrpclk_s]

##===========##
## MGT CLOCK ##
##===========##
create_clock -period 3.125 -name SMA_MGT_REFCLK [get_ports SMA_MGT_REFCLK_P]

##===========##
## User CDC  ##
##===========##
set_max_delay -datapath_only -from [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/data_a_reg_reg*/C}] -to [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/data_b_reg_reg*/D}] 6.250
set_max_delay -datapath_only -from [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/reset_a_strobe_sync_reg/C}] -to [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/reset_b_meta_reg/D}] 6.250
set_false_path -to [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/advance_toggle_meta_reg/D}]
set_false_path -to [get_pins -hierarchical -filter {NAME =~ *cdc_tx_inst/retard_toggle_meta_reg/D}]

set_max_delay -datapath_only -from [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/data_a_reg_reg*/C}] -to [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/data_b_reg_reg*/D}] 6.250
set_max_delay -datapath_only -from [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/strobe_b_toggle_reg/C}] -to [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/strobe_b_toggle_meta_reg/D}] 6.250
set_false_path -to [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/phase_calib_a_reg*/D}]
set_false_path -to [get_pins -hierarchical -filter {NAME =~ *cdc_rx_inst/phase_force_a_reg/D}]

##==================================##
## Timing exceptions for MGT design ##
##==================================##
# The generated clock mgt_freedrpclk_sys_clk is asynchronous to other clocks in design

set_clock_groups -asynchronous -group mgt_freedrpclk_sys_clk

# Reset synchronizer
set_false_path -to [get_pins lpgbtFpga_top_inst/mgt_inst/resetDoneSynch_rx/rst_in_*_reg/PRE]
set_false_path -to [get_pins lpgbtFpga_top_inst/mgt_inst/txAlignedSynch_tx/rst_in_*_reg/PRE]
set_false_path -to [get_pins lpgbtFpga_top_inst/mgt_inst/txPhaseAlignerResetSynch_sys/rst_in_*_reg/PRE]
set_false_path -to [get_pins -filter {REF_PIN_NAME =~ PRE} -of [get_cells -hier -regexp {.*\/*rst_sync.*}]]

