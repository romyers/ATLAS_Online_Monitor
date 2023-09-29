/* 
       _________   ________     ________       
      / U OF M  \ | LSA    \   / Physics\
      \__ATLAS__/ |   ___   | |   ______/
         |   |    |  |   \  | |  |
         |   |    |  |___/  | |  \______     
         |   |    |         | |         \
         \___/    |________/   \________/
*/  

// Author : Yuxiang Guo  gyuxiang@umich.edu
// File   : mezz_config_vio.v
// Create : 2021-03-18 23:48:07
// Revise : 2021-11-13 23:43:06
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
module mezz_config_vio
(
    input clk,
	output [`SETUP0_LENGTH-1:0] setup0,
	output [`SETUP1_LENGTH-1:0] setup1,
	output [`SETUP2_LENGTH-1:0] setup2,
	output [`CONTROL0_LENGTH-1:0] control0,
	output [`CONTROL1_LENGTH-1:0] control1,
	// output [`STATUS0_LENGTH-1:0] status0,
 //    output [`STATUS1_LENGTH-1:0] status1,
    output [`ASDWRITE_LENGTH-1:0] ASD_setup,
    output start_action_setup0,
    output start_action_setup1,
    output start_action_setup2,
    output start_action_control0,
    output start_action_control1,
    output start_action_asd_write,
    output start_action_asd_read
);

wire enable_new_ttc, enable_master_reset_code, enable_direct_bunch_reset, disable_event_reset, disable_trigger_from_TTC;
wire auto_roll_over, bypass_bcr_distribution;
wire enable_trigger, channel_data_debug, enable_leading, enable_pair, enbale_fake_hit;
wire [23:0] rising_is_leading, channel_enable_r, channel_enable_f;
wire [18:0] TDC_ID;
wire enable_trigger_timeout, enable_high_speed, enable_legacy, full_width_res;
wire [2:0] width_select;
wire enable_8b10b, enable_insert, enable_error_packet, enable_TDC_ID, enable_error_notify;

assign setup0 = {enable_new_ttc, enable_master_reset_code, enable_direct_bunch_reset, disable_event_reset, disable_trigger_from_TTC,
auto_roll_over, bypass_bcr_distribution,enable_trigger, channel_data_debug, enable_leading, enable_pair, enbale_fake_hit, rising_is_leading, 
channel_enable_r, channel_enable_f, TDC_ID, enable_trigger_timeout, enable_high_speed, enable_legacy, full_width_res, width_select,
enable_8b10b, enable_insert, enable_error_packet, enable_TDC_ID, enable_error_notify};

vio_setup0 vio_setup0_inst (
    .clk(clk),                  // input wire clk
    .probe_out0(enable_new_ttc),    // output wire [0 : 0] probe_out0
    .probe_out1(enable_master_reset_code),    // output wire [0 : 0] probe_out1
    .probe_out2(enable_direct_bunch_reset),    // output wire [0 : 0] probe_out2
    .probe_out3(disable_event_reset),    // output wire [0 : 0] probe_out3
    .probe_out4(disable_trigger_from_TTC),    // output wire [0 : 0] probe_out4
    .probe_out5(auto_roll_over),    // output wire [0 : 0] probe_out5
    .probe_out6(bypass_bcr_distribution),    // output wire [0 : 0] probe_out6
    .probe_out7(enable_trigger),    // output wire [0 : 0] probe_out7
    .probe_out8(channel_data_debug),    // output wire [0 : 0] probe_out8
    .probe_out9(enable_leading),    // output wire [0 : 0] probe_out9
    .probe_out10(enable_pair),  // output wire [0 : 0] probe_out10
    .probe_out11(enbale_fake_hit),  // output wire [0 : 0] probe_out11
    .probe_out12(rising_is_leading),  // output wire [23 : 0] probe_out12
    .probe_out13(channel_enable_r),  // output wire [23 : 0] probe_out13
    .probe_out14(channel_enable_f),  // output wire [23 : 0] probe_out14
    .probe_out15(TDC_ID),  // output wire [18 : 0] probe_out15
    .probe_out16(enable_trigger_timeout),  // output wire [0 : 0] probe_out16
    .probe_out17(enable_high_speed),  // output wire [0 : 0] probe_out17
    .probe_out18(enable_legacy),  // output wire [0 : 0] probe_out18
    .probe_out19(full_width_res),  // output wire [0 : 0] probe_out19
    .probe_out20(width_select),  // output wire [2 : 0] probe_out20
    .probe_out21(enable_8b10b),  // output wire [0 : 0] probe_out21
    .probe_out22(enable_insert),  // output wire [0 : 0] probe_out22
    .probe_out23(enable_error_packet),  // output wire [0 : 0] probe_out23
    .probe_out24(enable_TDC_ID),  // output wire [0 : 0] probe_out24
    .probe_out25(enable_error_notify),  // output wire [0 : 0] probe_out25
    .probe_out26(start_action_setup0)  // output wire [0 : 0] probe_out26
);

wire [9:0] combine_time_out_config;
wire [11:0] fake_hit_time_interval, syn_packet_number, roll_over, coarse_count_offset, bunch_offset, event_offset, match_window;
assign setup1 = {combine_time_out_config, fake_hit_time_interval, syn_packet_number,
roll_over, coarse_count_offset, bunch_offset, event_offset, match_window};

vio_setup1 vio_setup1_inst (
    .clk(clk),                // input wire clk
    .probe_out0(combine_time_out_config),  // output wire [9 : 0] probe_out0
    .probe_out1(fake_hit_time_interval),  // output wire [11 : 0] probe_out1
    .probe_out2(syn_packet_number),  // output wire [11 : 0] probe_out2
    .probe_out3(roll_over),  // output wire [11 : 0] probe_out3
    .probe_out4(coarse_count_offset),  // output wire [11 : 0] probe_out4
    .probe_out5(bunch_offset),  // output wire [11 : 0] probe_out5
    .probe_out6(event_offset),  // output wire [11 : 0] probe_out6
    .probe_out7(match_window),  // output wire [11 : 0] probe_out7
    .probe_out8(start_action_setup1) // output wire [0 : 0] probe_out8
);



wire [3:0] fine_sel;
wire [31:0] lut;
assign setup2 = {fine_sel, lut};

vio_setup2 vio_setup2_inst (
    .clk(clk),                // input wire clk
    .probe_out0(fine_sel),  // output wire [3 : 0] probe_out0
    .probe_out1(lut),  // output wire [31 : 0] probe_out1
    .probe_out2(start_action_setup2) // output wire [0 : 0] probe_out2
);


wire rst_ePLL, reset_jtag_in, event_teset_jtag_in, chnl_fifo_overflow_clear;
wire [3:0] debug_port_select;
assign control0 = {rst_ePLL, reset_jtag_in, event_teset_jtag_in, chnl_fifo_overflow_clear, debug_port_select};
vio_control0 vio_control0_inst (
    .clk(clk),                // input wire clk
    .probe_out0(rst_ePLL),  // output wire [0 : 0] probe_out0
    .probe_out1(reset_jtag_in),  // output wire [0 : 0] probe_out1
    .probe_out2(event_teset_jtag_in),  // output wire [0 : 0] probe_out2
    .probe_out3(chnl_fifo_overflow_clear),  // output wire [0 : 0] probe_out3
    .probe_out4(debug_port_select),  // output wire [3 : 0] probe_out4
    .probe_out5(start_action_control0) // output wire [0 : 0] probe_out5
);


wire [4:0] phase_clk160;
wire [3:0] phase_clk320_0, phase_clk320_1, phase_clk320_2, ePllRes, ePllIcp;
wire [1:0] ePllCap;
assign control1 = {phase_clk160, phase_clk320_0, phase_clk320_1, phase_clk320_2, ePllRes, ePllIcp, ePllCap,
ePllRes, ePllIcp, ePllCap, ePllRes, ePllIcp, ePllCap};
vio_control1 vio_control1_inst (
    .clk(clk),                // input wire clk
    .probe_out0(phase_clk160),  // output wire [4 : 0] probe_out0
    .probe_out1(phase_clk320_0),  // output wire [3 : 0] probe_out1
    .probe_out2(phase_clk320_1),  // output wire [3 : 0] probe_out2
    .probe_out3(phase_clk320_2),  // output wire [3 : 0] probe_out3
    .probe_out4(ePllRes),  // output wire [3 : 0] probe_out4
    .probe_out5(ePllIcp),  // output wire [3 : 0] probe_out5
    .probe_out6(ePllCap),  // output wire [1 : 0] probe_out6
    .probe_out7(start_action_control1) // output wire [0 : 0] probe_out7
);

// wire instruction_error;
// wire [32:0] crc;
// wire epll_lock;
// wire [23:0] chnl_fifo_full;
// wire start_action_status0;
// wire start_action_status1;
// vio_status vio_status_inst (
//   .clk(clk),                // input wire clk
//   .probe_in0(instruction_error),    // input wire [0 : 0] probe_in0
//   .probe_in1(crc),    // input wire [31 : 0] probe_in1
//   .probe_in2(epll_lock),    // input wire [0 : 0] probe_in2
//   .probe_in3(chnl_fifo_full),    // input wire [23 : 0] probe_in3
//   .probe_out0(start_action_status0),  // output wire [0 : 0] probe_out0
//   .probe_out1(start_action_status1)  // output wire [0 : 0] probe_out1
// );


wire [15:0] channel_mode;
wire chip_mode;
wire [2:0] deadtime;
wire [3:0] int_gate;
wire [2:0] rundown_curr;
wire [3:0] hyst_dac_reversed;
wire [2:0] wilk_thr;
wire [7:0] main_thr;
wire [54:0] asd_chip, asd_chip_default;
wire [12:0] not_used;
assign asd_chip = {channel_mode, chip_mode, deadtime, int_gate, rundown_curr, hyst_dac_reversed, wilk_thr, main_thr, not_used};
assign ASD_setup = {asd_chip, asd_chip, asd_chip};
vio_asd_setup vio_asd_setup_inst (
    .clk(clk),                // input wire clk
    .probe_out0(channel_mode),  // output wire [15 : 0] probe_out0
    .probe_out1(chip_mode),  // output wire [0 : 0] probe_out1
    .probe_out2(deadtime),  // output wire [2 : 0] probe_out2
    .probe_out3(int_gate),  // output wire [3 : 0] probe_out3
    .probe_out4(rundown_curr),  // output wire [2 : 0] probe_out4
    .probe_out5(hyst_dac_reversed),  // output wire [3 : 0] probe_out5
    .probe_out6(wilk_thr),  // output wire [2 : 0] probe_out6
    .probe_out7(main_thr),  // output wire [7 : 0] probe_out7
    .probe_out8(not_used), // output wire [12 : 0] probe_out9
    .probe_out9(start_action_asd_write), // output wire [0 : 0] probe_out9
    .probe_out10(start_action_asd_read) // output wire [0 : 0] probe_out10
);

// wire [7:0] oldasd_chnl_mask;
// wire [2:0] oldasd_cap_select;
// wire [7:0] oldasd_main_thr;
// wire [2:0] oldasd_disc2_thr;
// wire [3:0] oldasd_disc1_hys;
// wire [3:0] oldasd_int_gate;
// wire [2:0] oldasd_rundown_cur;
// wire [2:0] oldasd_deadtime;
// wire [15:0] oldasd_chnl_mode;
// wire oldasd_chipmode;
// wire [52:0] oldasd_chip;
// wire [158:0] oldasd_setup;
// assign oldasd_chip = {oldasd_chnl_mask, oldasd_cap_select, oldasd_main_thr, oldasd_disc2_thr, oldasd_disc1_hys, oldasd_int_gate,
//                       oldasd_rundown_cur, oldasd_deadtime, oldasd_chnl_mode, oldasd_chipmode};
// assign oldasd_setup = {oldasd_chip, oldasd_chip, oldasd_chip};


// vio_oldasd_setup vio_oldasd_setup_inst (
//   .clk(clk),                  // input wire clk
//   .probe_out0(oldasd_chnl_mask),    // output wire [7 : 0] probe_out0
//   .probe_out1(oldasd_cap_select),    // output wire [2 : 0] probe_out1
//   .probe_out2(oldasd_main_thr),    // output wire [7 : 0] probe_out2
//   .probe_out3(oldasd_disc2_thr),    // output wire [2 : 0] probe_out3
//   .probe_out4(oldasd_disc1_hys),    // output wire [3 : 0] probe_out4
//   .probe_out5(oldasd_int_gate),    // output wire [3 : 0] probe_out5
//   .probe_out6(oldasd_rundown_cur),    // output wire [2 : 0] probe_out6
//   .probe_out7(oldasd_deadtime),    // output wire [2 : 0] probe_out7
//   .probe_out8(oldasd_chnl_mode),    // output wire [15 : 0] probe_out8
//   .probe_out9(oldasd_chipmode),    // output wire [0 : 0] probe_out9
//   .probe_out10(start_action_oldasd),  // output wire [0 : 0] probe_out10
//   .probe_out11(start_action_hptdc_oldasd),
//   .probe_out12(start_action_hptdc_oldasdread)
// );
endmodule
