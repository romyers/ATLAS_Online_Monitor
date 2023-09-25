#configure all mezz TDC
set_property OUTPUT_VALUE 111111111111111111 [get_hw_probes mezz_enable_vio -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {mezz_enable_vio} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
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



set_property OUTPUT_VALUE FFFFFF [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/channel_enable_f -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_setup0_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/channel_enable_f} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_setup0_inst"}]]
set_property OUTPUT_VALUE FFFFFF [get_hw_probes inst_Mezz_config/inst_mezz_config_vio/channel_enable_r -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_setup0_inst"}]]
commit_hw_vio [get_hw_probes {inst_Mezz_config/inst_mezz_config_vio/channel_enable_r} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"inst_Mezz_config/inst_mezz_config_vio/vio_setup0_inst"}]]
startgroup
set_property OUTPUT_VALUE 1 [get_hw_probes TDC_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {TDC_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup
after 2000
startgroup
set_property OUTPUT_VALUE 0 [get_hw_probes TDC_config -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
commit_hw_vio [get_hw_probes {TDC_config} -of_objects [get_hw_vios -of_objects [get_hw_devices xc7a35t_0] -filter {CELL_NAME=~"vio_top_inst"}]]
endgroup