 ######## Please put user defined parameters here #############################

set main_threshold_chamber 108
set main_threshold_trigger 60
set asd_deadtime 010
set connected_chamber_mezz_position 111111111111111011
set connected_trigger_mezz_position 000000000000000100

 ######### End of user defined parameters ############################


set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/start_action_asd_write -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/channel_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/chip_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/deadtime -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/hyst_dac_reversed -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/int_gate -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/main_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/not_used -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/rundown_curr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/start_action_asd_read -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property NAME.SELECT short [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/wilk_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]] 
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/start_action_asd_write -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/channel_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/chip_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/deadtime -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/hyst_dac_reversed -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/int_gate -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/main_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/not_used -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/rundown_curr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/start_action_asd_read -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE_RADIX UNSIGNED [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/wilk_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]



set_property NAME.SELECT short [get_hw_probes JTAG_readback -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes mezz_enable_vio -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes JTAG_data_length_mezz -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes chain_success -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes lock_OBUF -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes sa_done -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes ASD_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes clk_div -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes rst_global -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes JTAG_bits_mezz -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes JTAG_instr_mezz -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes active_chains -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes config_done_OBUF -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes num_active_ports -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes tdi_tdo_equal -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes TDC_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]] 
set_property NAME.SELECT short [get_hw_probes enable_vio_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
set_property INPUT_VALUE_RADIX BINARY [get_hw_probes active_chains -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
set_property INPUT_VALUE_RADIX BINARY [get_hw_probes chain_success -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
set_property OUTPUT_VALUE_RADIX BINARY [get_hw_probes mezz_enable_vio -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
set_property INPUT_VALUE_RADIX UNSIGNED [get_hw_probes num_active_ports -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]




# common mezz ASD parameters


set_property OUTPUT_VALUE 0 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/channel_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/channel_mode} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 0 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/deadtime -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/deadtime} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 14 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/hyst_dac_reversed -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/hyst_dac_reversed} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 4 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/int_gate -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/int_gate} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 2 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/rundown_curr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/rundown_curr} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 2 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/wilk_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/wilk_thr} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
set_property OUTPUT_VALUE 0 [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/chip_mode -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/chip_mode} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
# end of common mezz ASD parameters



set_property OUTPUT_VALUE 010 [get_hw_probes clk_div -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {clk_div} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]

# configure trigger mezz ASD 
set_property OUTPUT_VALUE $connected_trigger_mezz_position [get_hw_probes mezz_enable_vio -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {mezz_enable_vio} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]

set_property OUTPUT_VALUE $main_threshold_trigger [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/main_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/main_thr} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]

startgroup
set_property OUTPUT_VALUE 1 [get_hw_probes rst_global -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {rst_global} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 0 [get_hw_probes rst_global -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {rst_global} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 1 [get_hw_probes ASD_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {ASD_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 0 [get_hw_probes ASD_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {ASD_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
set_property OUTPUT_VALUE $main_threshold_chamber [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/main_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/main_thr} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
# end of configuring trigger mezz ASD


# configure chamber mezz ASD

set_property OUTPUT_VALUE $connected_chamber_mezz_position [get_hw_probes mezz_enable_vio -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {mezz_enable_vio} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]

set_property OUTPUT_VALUE $main_threshold_chamber [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/main_thr -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/main_thr} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_asd_setup_inst"}]]

startgroup
set_property OUTPUT_VALUE 1 [get_hw_probes rst_global -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {rst_global} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 0 [get_hw_probes rst_global -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {rst_global} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 1 [get_hw_probes ASD_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {ASD_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
startgroup
set_property OUTPUT_VALUE 0 [get_hw_probes ASD_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {ASD_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 1000
# end of configuring chamber mezz ASD

