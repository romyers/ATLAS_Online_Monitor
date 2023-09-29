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
// File   : mezz_config_const.v
// Create : 2021-03-18 23:40:30
// Revise : 2021-03-19 03:17:01
// Editor : sublime text3, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
module mezz_config_const
(
	output [`SETUP0_LENGTH-1:0] setup0,
	output [`SETUP1_LENGTH-1:0] setup1,
	output [`SETUP2_LENGTH-1:0] setup2,
	output [`CONTROL0_LENGTH-1:0] control0,
	output [`CONTROL1_LENGTH-1:0] control1,
	output [`ASDWRITE_LENGTH-1:0] ASD_setup	
);
wire [54:0] asd_chip;

assign setup0 = {
     1'b0,     //enable_new_ttc
     1'b1,     //enable_master_reset_code
     1'b0,     //enable_direct_bunch_reset
     1'b0,     //disable_event_reset
     1'b0,     //disable_trigger_from_TTC
     1'b0,     //auto_roll_over
     1'b0,     //bypass_bcr_distribution
     1'b0,     //enable_trigger
     1'b0,     //channel_data_debug
     1'b0,     //enable_leading
     1'b1,     //enable_pair
     1'b0,     //enbale_fake_hit
    24'b0,     //rising_is_leading, 
    24'hFFFF,  //channel_enable_r
    24'hFFFF,  //channel_enable_f
    19'h7AAAA, //TDC_ID
     1'b0,     //enable_trigger_timeout
     1'b1,     //enable_high_speed
     1'b0,     //enable_legacy
     1'b0,     //full_width_res
     3'b001,   //width_select
     1'b1,     //enable_8b10b
     1'b0,     //enable_insert
     1'b0,     //enable_error_packet
     1'b0,     //enable_TDC_ID
     1'b0      //enable_error_notify  
};

assign setup1 = {
    10'h050, //combine_time_out_config
    12'h100, //fake_hit_time_interval
    12'hFFF, //syn_packet_number
    12'hFFF, //roll_over
    12'b0  , //coarse_count_offset
    12'hf9c, //bunch_offset
    12'b0  , //event_offset
    12'h01F //match_window
};

assign setup2 = {
    4'h3,         //fine_sel
    32'h19CA01CC  //lut
};

assign control0 = {
    1'b0, //rst_ePLL
    1'b0, //reset_jtag_in
    1'b0, //event_teset_jtag_in
    1'b0, //chnl_fifo_overflow_clear
    4'b0 //debug_port_select
};

assign control1 = {
    5'h08,   //phase_clk160
    4'h4,    //phase_clk320_0
    4'b0,    //phase_clk320_1
    4'h2,    //phase_clk320_2
    4'h2,    //ePllRes
    4'h4,    //ePllIcp
    4'h2,    //ePllCap
    4'h2,    //ePllRes
    4'h4,    //ePllIcp
    4'h2,    //ePllCap
    4'h2,    //ePllRes
    4'h4,    //ePllIcp
    4'h2     //ePllCap
};



assign asd_chip = {
    16'b0, //channel_mode
    1'b0,  //chip_mode
    3'h7,  //deadtime
    4'h4,  //int_gate
    3'h2,  //rundown_curr
    4'hE,  //hyst_dac_reversed
    3'h2,  //wilk_thr
    8'd108, //main_thr  39mV
    13'b0  //not_used
};

assign ASD_setup = {asd_chip, asd_chip, asd_chip};


endmodule
