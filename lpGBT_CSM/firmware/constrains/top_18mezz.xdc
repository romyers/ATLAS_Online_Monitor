# Changed on 10/07/20 by Xueye for LpGBT-CSM prototype v2

# from HDMI J5 (--SMA) 
#set_property PACKAGE_PIN L3 [get_ports sys_clk_p];
#set_property IOSTANDARD LVDS_25 [get_ports sys_clk_p];
#set_property PACKAGE_PIN K3 [get_ports sys_clk_n];
#set_property IOSTANDARD LVDS_25 [get_ports sys_clk_n];

# from LpGBT ELINK_FPGASYS (ECLK 4)
set_property PACKAGE_PIN H4 [get_ports sys_clk_p];
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_p];
set_property PACKAGE_PIN G4 [get_ports sys_clk_n];
set_property IOSTANDARD LVDS_25 [get_ports sys_clk_n];

# set_property PACKAGE_PIN V4 [get_ports calibre_source_clk_p];
# set_property IOSTANDARD LVDS_25 [get_ports calibre_source_clk_p];
# set_property PACKAGE_PIN W4 [get_ports calibre_source_clk_n];
# set_property IOSTANDARD LVDS_25 [get_ports calibre_source_clk_n];


# Note 1: R183 connection on board. Rst fanout done one csm mother board (MB)
# Note 2: Only keep two calib 16,17 on v2 -- old mezz.
# Note 3: place holder 4 for new mezz



###
set_property PACKAGE_PIN K6 [get_ports elink_TTC_in_p];
set_property IOSTANDARD LVDS_25 [get_ports elink_TTC_in_p];
set_property PACKAGE_PIN J6 [get_ports elink_TTC_in_n];
set_property IOSTANDARD LVDS_25 [get_ports elink_TTC_in_n];

###
set_property PACKAGE_PIN K2 [get_ports mezz_enc_out_p[0]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[0]];
set_property PACKAGE_PIN J2 [get_ports mezz_enc_out_n[0]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[0]];

set_property PACKAGE_PIN A18 [get_ports mezz_enc_out_p[1]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[1]];
set_property PACKAGE_PIN A19 [get_ports mezz_enc_out_n[1]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[1]];

set_property PACKAGE_PIN B1 [get_ports mezz_enc_out_p[2]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[2]];
set_property PACKAGE_PIN A1 [get_ports mezz_enc_out_n[2]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[2]];

set_property PACKAGE_PIN D20 [get_ports mezz_enc_out_p[3]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[3]];
set_property PACKAGE_PIN C20 [get_ports mezz_enc_out_n[3]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[3]];

set_property PACKAGE_PIN A13 [get_ports mezz_enc_out_p[4]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[4]];
set_property PACKAGE_PIN A14 [get_ports mezz_enc_out_n[4]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[4]];

set_property PACKAGE_PIN C13 [get_ports mezz_enc_out_p[5]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[5]];
set_property PACKAGE_PIN B13 [get_ports mezz_enc_out_n[5]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[5]];

set_property PACKAGE_PIN K1 [get_ports mezz_enc_out_p[6]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[6]];
set_property PACKAGE_PIN J1 [get_ports mezz_enc_out_n[6]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[6]];

set_property PACKAGE_PIN D17 [get_ports mezz_enc_out_p[7]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[7]];
set_property PACKAGE_PIN C17 [get_ports mezz_enc_out_n[7]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[7]];

set_property PACKAGE_PIN C2 [get_ports mezz_enc_out_p[8]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[8]];
set_property PACKAGE_PIN B2 [get_ports mezz_enc_out_n[8]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[8]];

set_property PACKAGE_PIN C18 [get_ports mezz_enc_out_p[9]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[9]];
set_property PACKAGE_PIN C19 [get_ports mezz_enc_out_n[9]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[9]];

set_property PACKAGE_PIN R1 [get_ports mezz_enc_out_p[10]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[10]];
set_property PACKAGE_PIN P1 [get_ports mezz_enc_out_n[10]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[10]];

set_property PACKAGE_PIN AB7 [get_ports mezz_enc_out_p[11]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[11]];
set_property PACKAGE_PIN AB6 [get_ports mezz_enc_out_n[11]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[11]];

set_property PACKAGE_PIN U3 [get_ports mezz_enc_out_p[12]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[12]];
set_property PACKAGE_PIN V3 [get_ports mezz_enc_out_n[12]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[12]];

set_property PACKAGE_PIN W9 [get_ports mezz_enc_out_p[13]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[13]];
set_property PACKAGE_PIN Y9 [get_ports mezz_enc_out_n[13]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[13]];

set_property PACKAGE_PIN U6 [get_ports mezz_enc_out_p[14]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[14]];
set_property PACKAGE_PIN V5 [get_ports mezz_enc_out_n[14]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[14]];

set_property PACKAGE_PIN AB3 [get_ports mezz_enc_out_p[15]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[15]];
set_property PACKAGE_PIN AB2 [get_ports mezz_enc_out_n[15]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[15]];

set_property PACKAGE_PIN U2 [get_ports mezz_enc_out_p[16]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_p[16]];
set_property PACKAGE_PIN V2 [get_ports mezz_enc_out_n[16]];
set_property IOSTANDARD LVDS_25 [get_ports mezz_enc_out_n[16]];

set_property PACKAGE_PIN V17 [get_ports mezz_enc_out_p[17]];
set_property IOSTANDARD DIFF_SSTL15 [get_ports mezz_enc_out_p[17]];
set_property PACKAGE_PIN W17 [get_ports mezz_enc_out_n[17]];
set_property IOSTANDARD DIFF_SSTL15 [get_ports mezz_enc_out_n[17]];




## JTAG fanout


set_property PACKAGE_PIN Y3 [get_ports rst_AMT_MB];  
set_property IOSTANDARD LVCMOS25 [get_ports rst_AMT_MB];
#LED0
set_property PACKAGE_PIN T5 [get_ports lock];  
set_property IOSTANDARD LVCMOS25 [get_ports lock];
#LED1
set_property PACKAGE_PIN N3 [get_ports config_done];  
set_property IOSTANDARD LVCMOS25 [get_ports config_done];



set_property PACKAGE_PIN D2 [get_ports tdo[0]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[0]];
set_property PACKAGE_PIN D1 [get_ports tdi[0]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[0]];
set_property PACKAGE_PIN E2 [get_ports tck[0]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[0]];
set_property PACKAGE_PIN E1 [get_ports tms[0]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[0]];

set_property PACKAGE_PIN B22 [get_ports tdo[1]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[1]];
set_property PACKAGE_PIN A21 [get_ports tdi[1]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[1]];
set_property PACKAGE_PIN B21 [get_ports tck[1]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[1]];
set_property PACKAGE_PIN A20 [get_ports tms[1]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[1]];

set_property PACKAGE_PIN L4 [get_ports tdo[2]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[2]];
set_property PACKAGE_PIN F4 [get_ports tdi[2]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[2]];
set_property PACKAGE_PIN E3 [get_ports tck[2]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[2]];
set_property PACKAGE_PIN F3 [get_ports tms[2]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[2]];

set_property PACKAGE_PIN G22 [get_ports tdo[3]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[3]];
set_property PACKAGE_PIN F21 [get_ports tdi[3]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[3]];
set_property PACKAGE_PIN E22 [get_ports tck[3]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[3]];
set_property PACKAGE_PIN D22 [get_ports tms[3]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[3]];

set_property PACKAGE_PIN B17 [get_ports tdo[4]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[4]];
set_property PACKAGE_PIN A16 [get_ports tdi[4]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[4]];
set_property PACKAGE_PIN B15 [get_ports tck[4]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[4]];
set_property PACKAGE_PIN A15 [get_ports tms[4]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[4]];


set_property PACKAGE_PIN B16 [get_ports tdo[5]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[5]];
set_property PACKAGE_PIN C15 [get_ports tdi[5]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[5]];
set_property PACKAGE_PIN D14 [get_ports tck[5]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[5]];
set_property PACKAGE_PIN C14 [get_ports tms[5]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[5]];


set_property PACKAGE_PIN F1 [get_ports tdo[6]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[6]];
set_property PACKAGE_PIN G1 [get_ports tdi[6]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[6]];
set_property PACKAGE_PIN G2 [get_ports tck[6]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[6]];
set_property PACKAGE_PIN H2 [get_ports tms[6]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[6]];


set_property PACKAGE_PIN B20 [get_ports tdo[7]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[7]];
set_property PACKAGE_PIN B18 [get_ports tdi[7]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[7]];
set_property PACKAGE_PIN D16 [get_ports tck[7]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[7]];
set_property PACKAGE_PIN D15 [get_ports tms[7]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[7]];


set_property PACKAGE_PIN H5 [get_ports tdo[8]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[8]];
set_property PACKAGE_PIN L5 [get_ports tdi[8]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[8]];
set_property PACKAGE_PIN H3 [get_ports tck[8]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[8]];
set_property PACKAGE_PIN G3 [get_ports tms[8]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[8]];

set_property PACKAGE_PIN C22 [get_ports tdo[9]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[9]];
set_property PACKAGE_PIN E21 [get_ports tdi[9]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[9]];
set_property PACKAGE_PIN D21 [get_ports tck[9]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[9]];
set_property PACKAGE_PIN D19 [get_ports tms[9]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[9]];

set_property PACKAGE_PIN T4 [get_ports tdo[10]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[10]];
set_property PACKAGE_PIN T3 [get_ports tdi[10]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[10]];
set_property PACKAGE_PIN R2 [get_ports tck[10]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[10]];
set_property PACKAGE_PIN R3 [get_ports tms[10]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[10]];

set_property PACKAGE_PIN V9 [get_ports tdo[11]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[11]];
set_property PACKAGE_PIN Y8 [get_ports tdi[11]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[11]];
set_property PACKAGE_PIN AA8 [get_ports tck[11]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[11]];
set_property PACKAGE_PIN AB8 [get_ports tms[11]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[11]];


set_property PACKAGE_PIN Y4 [get_ports tdo[12]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[12]];
set_property PACKAGE_PIN AA3 [get_ports tdi[12]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[12]];
set_property PACKAGE_PIN Y2 [get_ports tck[12]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[12]];
set_property PACKAGE_PIN W2 [get_ports tms[12]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[12]];


set_property PACKAGE_PIN E19 [get_ports tdo[13]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[13]];
set_property PACKAGE_PIN E18 [get_ports tdi[13]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[13]];
set_property PACKAGE_PIN E16 [get_ports tck[13]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[13]];
set_property PACKAGE_PIN F15 [get_ports tms[13]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[13]];


set_property PACKAGE_PIN W7 [get_ports tdo[14]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[14]];
set_property PACKAGE_PIN W6 [get_ports tdi[14]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[14]];
set_property PACKAGE_PIN W5 [get_ports tck[14]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[14]];
set_property PACKAGE_PIN U5 [get_ports tms[14]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[14]];


set_property PACKAGE_PIN AA6 [get_ports tdo[15]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[15]];
set_property PACKAGE_PIN AB5 [get_ports tdi[15]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[15]];
set_property PACKAGE_PIN AA5 [get_ports tck[15]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[15]];
set_property PACKAGE_PIN AA4 [get_ports tms[15]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[15]];


set_property PACKAGE_PIN AB1 [get_ports tdo[16]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[16]];
set_property PACKAGE_PIN AA1 [get_ports tdi[16]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[16]];
set_property PACKAGE_PIN Y1 [get_ports tck[16]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[16]];
set_property PACKAGE_PIN W1 [get_ports tms[16]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[16]];

set_property PACKAGE_PIN G21 [get_ports tdo[17]];
set_property IOSTANDARD LVCMOS25 [get_ports tdo[17]];
set_property PACKAGE_PIN F20 [get_ports tdi[17]];
set_property IOSTANDARD LVCMOS25 [get_ports tdi[17]];
set_property PACKAGE_PIN F19 [get_ports tck[17]];
set_property IOSTANDARD LVCMOS25 [get_ports tck[17]];
set_property PACKAGE_PIN F18 [get_ports tms[17]];
set_property IOSTANDARD LVCMOS25 [get_ports tms[17]];

