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
// File   : tdcv2_decoder.v
// Create : 2021-02-01 23:51:00
// Revise : 2021-10-23 20:07:51

// Editor : sublime text4, tab size (3)
// Description: 
//
// -----------------------------------------------------------------------------

module tdcv2_decoder (
	input  clk_40,
	input  rst_40,
	input  [7:0] tdc_dline_1, 
	input  [7:0] tdc_dline_0,
	input  [4:0] tdc_id,
	input  [2:0] csm_id,
	//TDC readout FIFO
	input  tdc_fifo_read,
	output tdc_fifo_empty,
	output [39:0] tdc_fifo_data,

	//dline info
	input  enable_K28_1,
	input  enable_320M,
	// input  tdc_trigger_mode,
	input  [19:0] correct_counter_th,
	output locked_dline1,
	output locked_dline0,


	//trigger info
	input  sys_clk_160,
	input  rst_160,
	output matching_busy,
	input  enable_matching,
	input  enable_matching_timeout,
	input  [16:0] trigger_fifo_data,
	input  trigger_fifo_empty,
	input  building_busy,
	input  [11:0] trigger_coarse,
	input  [11:0] match_window,
	input  [11:0] search_margin,
	input  [11:0] bc_offset,
	input  [11:0] reject_offset,
	input  [11:0] rollover,
	input  enable_relative_trigger_data,

	//BER monitor
	output [43:0] total_received_40bit_dline1,
	output [43:0] total_received_40bit_dline0,
	output [35:0] total_error_bit_dline1,
	output [35:0] total_error_bit_dline0
);

// raw data align and 10b/8b decoding





wire chnl_data_valid;
wire [31:0] chnl_data_32b;



// TDC trigger matching
wire [31:0] matched_data;
wire match_valid;

//TDC readout FIFO
wire tdc_fifo_full;



phase_select inst_phase_select
(
	.clk                (clk_40),
	.rst                (rst_40),
	.tdc_dline_1        (tdc_dline_1),
	.tdc_dline_0        (tdc_dline_0),

	// .data_ready         (data_ready),
	// .data_64b_decode    (data_64b_decode),
	// .o_Kout             (o_Kout),
	.enable_matching            (enable_matching),
	.chnl_data_valid            (chnl_data_valid),
	.chnl_data_32b              (chnl_data_32b),

	.enable_K28_1       (enable_K28_1),
	.enable_320M        (enable_320M),
	.correct_counter_th (correct_counter_th),
	.locked_dline1      (locked_dline1),
	.locked_dline0      (locked_dline0),

	.total_received_40bit_dline1 (total_received_40bit_dline1),
	.total_received_40bit_dline0 (total_received_40bit_dline0),
	.total_error_bit_dline1      (total_error_bit_dline1),
	.total_error_bit_dline0      (total_error_bit_dline0)
);









trigger_matching trigger_matching_inst
(
	.clk_40                       (clk_40),
	.tdc_hit_data                 (chnl_data_32b),
	.tdc_hit_data_valid           (chnl_data_valid),

	.sys_clk_160                  (sys_clk_160),
	.rst_160                      (rst_160),
	.ram_read                     (~tdc_fifo_full),
	.enable_matching              (enable_matching),
	.enable_matching_timeout      (enable_matching_timeout),
	.building_busy                (building_busy),
	.trigger_fifo_empty           (trigger_fifo_empty),
	.trigger_fifo_data            (trigger_fifo_data),

	.matched_data                 (matched_data),
	.match_valid                  (match_valid),
	.matching_busy                (matching_busy),

	.enable_relative_trigger_data (enable_relative_trigger_data),
	.rollover                     (rollover),
	.trigger_coarse               (trigger_coarse),
	.match_window                 (match_window),
	.search_margin                (search_margin),
	.bc_offset                    (bc_offset),
	.reject_offset                (reject_offset)
);


TDC_data_fifo_40b TDC_data_fifo_40b_inst (
	.clk(sys_clk_160),                  // input wire clk
	.srst(rst_160),                // input wire srst
	.din({csm_id,tdc_id,matched_data}),                  // input wire [39 : 0] din
	.wr_en(match_valid),              // input wire wr_en
	.rd_en(tdc_fifo_read),              // input wire rd_en
	.dout(tdc_fifo_data),                // output wire [39 : 0] dout
	.full(tdc_fifo_full),                // output wire full
	.empty(tdc_fifo_empty),              // output wire empty
	.wr_rst_busy(),  // output wire wr_rst_busy
	.rd_rst_busy()  // output wire rd_rst_busy
);

endmodule