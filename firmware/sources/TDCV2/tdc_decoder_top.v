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
// File   : tdc_decoder_top.v
// Create : 2021-03-10 14:45:53
// Revise : 2023-06-23 17:31:40
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------

module tdc_decoder_top(
    input clk_40,
    input rst_in,
    input [229:0] userDataUpLink_m,
    input [229:0] userDataUpLink_s,
    // input [27:0] elink_config_i[1:0],
    input  trigger_in,
    output [27:0] error_detected_o,
    output [1:0] encode_ttc,
    output [23:0] hit,
    output mezz_TRST,

    output [7:0] SCA0_PRI,
    output [7:0] SCA0_AUX,
    output [7:0] SCA1_PRI,
    output [7:0] SCA1_AUX,
    output [7:0] SCA2_PRI,
    output [7:0] SCA2_AUX,

    
    input sys_clk_160,

    //  ethernet interface
    input  tx_axis_clk,
    output [7:0] tx_axis_fifo_tdata,
    output tx_axis_fifo_tvalid,
    input  tx_axis_fifo_tready,
    output tx_axis_fifo_tlast
);


//disable TDC features
assign error_detected_o    = 'b0;
assign encode_ttc          = 'b0;
assign hit                 = 'b0;
assign mezz_TRST           = 'b0;
assign SCA0_PRI            = 'b0;
assign SCA0_AUX            = 'b0;
assign SCA1_PRI            = 'b0;
assign SCA1_AUX            = 'b0;
assign SCA2_PRI            = 'b0;
assign SCA2_AUX            = 'b0;
assign tx_axis_fifo_tdata  = 'b0;
assign tx_axis_fifo_tvalid = 'b0;
assign tx_axis_fifo_tlast  = 'b0;






// localparam TDC_COUNT = 4;
// localparam TDC_COUNT_M = 4;
// localparam TDC_COUNT_S = 0;
// localparam TDC_DATA_WIDTH = 40;



// //clk_40 aligned
// wire enable_K28_1;
// wire enable_320M;
// wire [19:0] correct_counter_th;
// (* dont_touch = "true" *) wire [TDC_COUNT-1:0] locked_dline1;
// (* dont_touch = "true" *) wire [TDC_COUNT-1:0] locked_dline0;
// wire [TDC_COUNT-1:0] locked_dline;
// wire [TDC_COUNT-1:0] matching_busy;


// //sys_clk_160 aligned
// wire [TDC_COUNT-1:0] tdc_fifo_read;
// wire [TDC_COUNT-1:0] tdc_fifo_empty;
// wire [TDC_COUNT*TDC_DATA_WIDTH-1:0] tdc_fifo_data;

// wire tdc_fifo_empty_mux;
// wire matching_busy_mux;
// wire tdc_fifo_read_mux;
// wire [TDC_DATA_WIDTH-1:0] tdc_fifo_data_mux;

// wire [28:0] trigger_fifo_data;
// wire trigger_fifo_empty;
// wire building_busy;
// wire trigger_fifo_read;
// wire [11:0] trigger_coarse;
// wire [11:0] match_window;
// wire [11:0] search_margin;
// wire [11:0] bc_offset;
// wire [11:0] reject_offset;
// wire [11:0] rollover;


// wire enable_relative_trigger_BCID;
// wire enable_relative_trigger_data;

// wire [TDC_DATA_WIDTH-1:0] event_data;
// wire [TDC_DATA_WIDTH-1:0] readout_fifo_data;
// wire readout_fifo_empty;
// wire readout_fifo_full;
// wire readout_fifo_read;
// //vio for ethernet enable
// wire packet_rst;
// wire packet_enable;

// //vio for trigger matching
// wire enable_matching;
// wire matching_reset;
// wire bc_reset;
// wire event_reset;
// wire trigger_redge;
// wire [11:0] coarse_count_offset;
// wire [7:0] trigger_latency; 


// wire global_rst;
// wire fpga_bcr;
// wire rst_40;
// wire ttc_start_single;
// wire ttc_start_continuous;
// wire ttc_bc_reset_en;
// wire ttc_master_reset_en;
// wire ttc_bc_reset_vio;
// wire ttc_event_reset_vio;
// wire ttc_master_reset_vio;
// wire [11:0] ttc_interval;

// //vio for hit generator
// wire enable_hit;
// wire [11:0] width_hit;  //hit width = width_hit*6.25ns
// wire hit_single;
// wire hit_inv;
// wire [23:0] hit_mask;
// wire [23:0] delay_mask;
// wire [11:0] hit_delay;
// wire [11:0] hit_interval;
// wire [11:0] hit_trigger_ratio;
// wire trigger_internal;
// wire enable_internal_trigger;


// wire fpga_bcr_160;
// reg srst_160;
// wire rst_160_vio;
// wire trigger_detect;
// wire trigger_delayed;
// wire trigger_delayed_ttc;



// wire trigger_fifo_read_triggermode;
// wire trigger_fifo_read_triggerless;
// wire [TDC_COUNT-1:0] tdc_fifo_read_triggermode;
// wire [TDC_COUNT-1:0] tdc_fifo_read_triggerless;
// wire [TDC_DATA_WIDTH-1:0] event_data_triggermode;
// wire [TDC_DATA_WIDTH-1:0] event_data_triggerless;
// wire event_data_ready_triggermode;
// wire event_data_ready_triggerless;

// assign trigger_detect = enable_internal_trigger?trigger_internal:trigger_in;
// assign packet_enable = 1'b1;
// assign locked_dline = locked_dline1&locked_dline0;

// assign trigger_fifo_read = enable_matching?trigger_fifo_read_triggerless:trigger_fifo_read_triggermode;
// assign tdc_fifo_read = enable_matching?tdc_fifo_read_triggerless:tdc_fifo_read_triggermode;
// assign event_data = enable_matching?event_data_triggerless:event_data_triggermode;
// assign event_data_ready = enable_matching?event_data_ready_triggerless:event_data_ready_triggermode;


// (* dont_touch = "true" *) wire [44*TDC_COUNT-1:0] total_received_40bit_dline1;
// (* dont_touch = "true" *) wire [44*TDC_COUNT-1:0] total_received_40bit_dline0;
// (* dont_touch = "true" *) wire [36*TDC_COUNT-1:0] total_error_bit_dline1;
// (* dont_touch = "true" *) wire [36*TDC_COUNT-1:0] total_error_bit_dline0;

// (* dont_touch = "true" *) wire [43:0] tdc_received_40bit_dline1[9:0];
// (* dont_touch = "true" *) wire [43:0] tdc_received_40bit_dline0[9:0];
// (* dont_touch = "true" *) wire [35:0] tdc_error_bit_dline1[9:0];
// (* dont_touch = "true" *) wire [35:0] tdc_error_bit_dline0[9:0];


// genvar i;
// generate
//     for(i=0;i<10;i=i+1) begin
//         assign tdc_received_40bit_dline1[i] = i<TDC_COUNT_M ? total_received_40bit_dline1[44*(i+1)-1:44*i] : 'b0;
//         assign tdc_received_40bit_dline0[i] = i<TDC_COUNT_M ? total_received_40bit_dline0[44*(i+1)-1:44*i] : 'b0;
//         assign tdc_error_bit_dline1[i] = i<TDC_COUNT_M ? total_error_bit_dline1[36*(i+1)-1:36*i] : 'b0;
//         assign tdc_error_bit_dline0[i] = i<TDC_COUNT_M ? total_error_bit_dline0[36*(i+1)-1:36*i] : 'b0;
//     end
// endgenerate

// xlx_ku_mgt_ip_reset_synchronizer fpga_bcr_rst_sync(
//  .clk_in(sys_clk_160),
//  .rst_in(fpga_bcr),
//  .rst_out(fpga_bcr_160)
// );
// xlx_ku_mgt_ip_reset_synchronizer r160_rst_sync(
//  .clk_in(sys_clk_160),
//  .rst_in(rst_40|rst_160_vio),
//  .rst_out(rst_160)
// );
// xlx_ku_mgt_ip_reset_synchronizer r40_rst_sync(
//  .clk_in(clk_40),
//  .rst_in(rst_in|global_rst),
//  .rst_out(rst_40)
// );
// xlx_ku_mgt_ip_reset_synchronizer pck_rst_sync(
//  .clk_in(tx_axis_clk),
//  .rst_in(rst_40),
//  .rst_out(packet_rst)
// );

// xlx_ku_mgt_ip_reset_synchronizer trigger_sync(
//  .clk_in(clk_40),
//  .rst_in(trigger_delayed),
//  .rst_out(trigger_delayed_ttc)
// );

// always @(posedge sys_clk_160) begin
//     srst_160 <= rst_160;
// end


// trigger_interface inst_trigger_interface
// (
//     .clk_160             (sys_clk_160),
//     .rst_160             (rst_160),
//     .bc_reset            (fpga_bcr_160),
//     .event_reset         (event_reset),
//     .trigger_in          (trigger_detect),
//     .trigger_ready_delayed(trigger_delayed),
//     .trigger_read        (trigger_fifo_read),
//     .trigger_fifo_data   (trigger_fifo_data),
//     .trigger_fifo_empty  (trigger_fifo_empty),
//     .trigger_coarse      (trigger_coarse),
//     .trigger_redge       (trigger_redge),
//     .rollover            (rollover),
//     .coarse_count_offset (coarse_count_offset),
//     .trigger_latency     (trigger_latency)
// );


// tdcv2_decoder_wrapper #(
//     .TDC_COUNT(TDC_COUNT_M),
//     .IS_MASTER(1),
//     .CSM_ID(0),
//     .TDC_DATA_WIDTH(TDC_DATA_WIDTH)
// ) tdcv2_decoder_wrapper_master (
//     .clk_40                       (clk_40),
//     .rst_40                       (rst_40),
//     .userDataUpLink_i             (userDataUpLink_m),
//     .trigger_in                   (trigger_in),
//     .correct_counter_th           (correct_counter_th),
//     .enable_K28_1                 (enable_K28_1),
//     .enable_320M                  (enable_320M),
//     .locked_dline1                (locked_dline1[TDC_COUNT_M-1:0]),
//     .locked_dline0                (locked_dline0[TDC_COUNT_M-1:0]),

//     .sys_clk_160                  (sys_clk_160),
//     .rst_160                      (rst_160),
//     .tdc_fifo_read                (tdc_fifo_read[TDC_COUNT_M-1:0]),
//     .tdc_fifo_empty               (tdc_fifo_empty[TDC_COUNT_M-1:0]),
//     .tdc_fifo_data                (tdc_fifo_data[TDC_COUNT_M*TDC_DATA_WIDTH-1:0]),
//     .matching_busy                (matching_busy[TDC_COUNT_M-1:0]),
//     .enable_matching              (enable_matching),
//     .enable_matching_timeout      (1'b1),
//     .trigger_fifo_data            (trigger_fifo_data[16:0]),
//     .trigger_fifo_empty           (trigger_fifo_empty),
//     .building_busy                (building_busy),
//     .trigger_coarse               (trigger_coarse),
//     .match_window                 (match_window),
//     .search_margin                (search_margin),
//     .bc_offset                    (bc_offset),
//     .reject_offset                (reject_offset),
//     .rollover                     (rollover),
//     .enable_relative_trigger_data (enable_relative_trigger_data),

//     .total_received_40bit_dline1 (total_received_40bit_dline1),
//     .total_received_40bit_dline0 (total_received_40bit_dline0),
//     .total_error_bit_dline1      (total_error_bit_dline1),
//     .total_error_bit_dline0      (total_error_bit_dline0),

//     .SCA0_PRI                     (SCA0_PRI),
//     .SCA0_AUX                     (SCA0_AUX),
//     .SCA1_PRI                     (SCA1_PRI),
//     .SCA1_AUX                     (SCA1_AUX),
//     .SCA2_PRI                     (SCA2_PRI),
//     .SCA2_AUX                     (SCA2_AUX)
// );


// // tdcv2_decoder_wrapper #(
// //     .TDC_COUNT(TDC_COUNT_S),
// //     .IS_MASTER(0),
// //     .CSM_ID(0),
// //     .TDC_DATA_WIDTH(TDC_DATA_WIDTH)
// // ) tdcv2_decoder_wrapper_slave (
// //     .clk_40                       (clk_40),
// //     .rst_40                       (rst_40),
// //     .userDataUpLink_i             (userDataUpLink_m),
// //     .trigger_in                   (trigger_in),
// //     .correct_counter_th           (correct_counter_th),
// //     .enable_K28_1                 (enable_K28_1),
// //     .enable_320M                  (enable_320M),
// //     .locked_dline1                (locked_dline1[TDC_COUNT_S+TDC_COUNT_M-1:TDC_COUNT_M]),
// //     .locked_dline0                (locked_dline0[TDC_COUNT_S+TDC_COUNT_M-1:TDC_COUNT_M]),

// //     .sys_clk_160                  (sys_clk_160),
// //     .rst_160                      (rst_160),
// //     .tdc_fifo_read                (tdc_fifo_read[TDC_COUNT_S+TDC_COUNT_M-1:TDC_COUNT_M]),
// //     .tdc_fifo_empty               (tdc_fifo_empty[TDC_COUNT_S+TDC_COUNT_M-1:TDC_COUNT_M]),
// //     .tdc_fifo_data                (tdc_fifo_data[(TDC_COUNT_S+TDC_COUNT_M)*TDC_DATA_WIDTH-1:TDC_COUNT_M*TDC_DATA_WIDTH]),
// //     .matching_busy                (matching_busy[TDC_COUNT_S+TDC_COUNT_M-1:TDC_COUNT_M]),
// //     .enable_matching              (enable_matching),
// //     .enable_matching_timeout      (1'b1),
// //     .trigger_fifo_data            (trigger_fifo_data[16:0]),
// //     .trigger_fifo_empty           (trigger_fifo_empty),
// //     .building_busy                (building_busy),
// //     .trigger_coarse               (trigger_coarse),
// //     .match_window                 (match_window),
// //     .search_margin                (search_margin),
// //     .bc_offset                    (bc_offset),
// //     .reject_offset                (reject_offset),
// //     .rollover                     (rollover),
// //     .enable_relative_trigger_data (enable_relative_trigger_data)

// //     // .total_received_40bit_dline1 (total_received_40bit_dline1),
// //     // .total_received_40bit_dline0 (total_received_40bit_dline0),
// //     // .total_error_bit_dline1      (total_error_bit_dline1),
// //     // .total_error_bit_dline0      (total_error_bit_dline0)
// // );


// tdc_fifo_data_mux #(
//     .TDC_COUNT(TDC_COUNT),
//     .TDC_DATA_WIDTH(TDC_DATA_WIDTH)
// ) inst_tdc_fifo_data_mux (
//     .locked             (locked_dline),
//     .tdc_fifo_empty     (tdc_fifo_empty),
//     .tdc_fifo_data      (tdc_fifo_data),
//     .matching_busy      (matching_busy),
//     .tdc_fifo_read      (tdc_fifo_read_triggerless),

//     .tdc_fifo_empty_mux (tdc_fifo_empty_mux),
//     .tdc_fifo_data_mux  (tdc_fifo_data_mux),
//     .matching_busy_mux  (matching_busy_mux),
//     .tdc_fifo_read_mux  (tdc_fifo_read_mux)
// );

// // ila_tdc_fifo_mux ila_tdc_fifo_mux_inst (
// //     .clk(sys_clk_160), // input wire clk


// //     .probe0(tdc_fifo_empty), // input wire [9:0]  probe0  
// //     .probe1(matching_busy), // input wire [9:0]  probe1 
// //     .probe2(tdc_fifo_read), // input wire [9:0]  probe2 
// //     .probe3(tdc_fifo_empty_mux), // input wire [0:0]  probe3 
// //     .probe4(matching_busy_mux), // input wire [0:0]  probe4 
// //     .probe5(tdc_fifo_read_mux), // input wire [0:0]  probe5 
// //     .probe6(hit) // input wire [23:0]  probe5 
// // );

// event_builder inst_event_builder_triggerless
// (
//     .clk                (sys_clk_160),
//     .rst                (rst_160),
//     .enable             (enable_matching),
//     .trigger_fifo_empty (trigger_fifo_empty),
//     .trigger_fifo_data  (trigger_fifo_data),
//     .trigger_fifo_read  (trigger_fifo_read_triggerless),
//     .tdc_fifo_empty     (tdc_fifo_empty_mux),
//     .tdc_fifo_data      (tdc_fifo_data_mux),
//     .matching_busy      (matching_busy_mux),
//     .tdc_fifo_read      (tdc_fifo_read_mux),
//     .readout_fifo_full  (readout_fifo_full),
//     .event_data         (event_data_triggerless),
//     .event_data_ready   (event_data_ready_triggerless),
//     .building_busy      (building_busy)
// );



// event_builder_triggermode #(
//         .TDC_COUNT(TDC_COUNT),
//         .TDC_DATA_WIDTH(TDC_DATA_WIDTH)
// ) inst_event_builder_triggermode (
//     .clk                  (sys_clk_160),
//     .rst                  (rst_160),
//     .enable               (~enable_matching),

//     .trigger_fifo_empty   (trigger_fifo_empty),
//     .trigger_fifo_data    (trigger_fifo_data),
//     .trigger_fifo_read    (trigger_fifo_read_triggermode),

//     .locked_array         (locked_dline),
//     .tdc_fifo_empty_array (tdc_fifo_empty),
//     .tdc_fifo_data_array  (tdc_fifo_data),
//     .tdc_fifo_read_array  (tdc_fifo_read_triggermode),
    
//     .readout_fifo_full    (readout_fifo_full),
//     .event_data           (event_data_triggermode),
//     .event_data_ready     (event_data_ready_triggermode)
// );


// readout_fifo readout_fifo_inst (
//     .srst(srst_160),                //The synchronous reset (srst) 
//                                  //should be synchronous to wr_clk.
//     .wr_clk(sys_clk_160),            // input wire wr_clk
//     .rd_clk(tx_axis_clk), 
//     .din(event_data),                  // input wire [39 : 0] din
//     .wr_en(event_data_ready),              // input wire wr_en
//     .rd_en(readout_fifo_read),              // input wire rd_en
//     .dout(readout_fifo_data),                // output wire [39 : 0] dout
//     .full(readout_fifo_full),                // output wire full
//     .empty(readout_fifo_empty),              // output wire empty
//     .wr_rst_busy(),  // output wire wr_rst_busy
//     .rd_rst_busy()  // output wire rd_rst_busy
// );


// // ttc_generator ttc_generator_inst
// // (
// //     .clk_40           (clk_40),
// //     .rst_40           (rst_40),
// //     .encode_ttc       (encode_ttc),
// //     .fpga_bcr         (fpga_bcr),
// //     .start_single     (ttc_start_single),
// //     .start_continuous (ttc_start_continuous),
// //     .trigger_en       (1'b0),
// //     .bc_reset_en      (ttc_bc_reset_en),
// //     .event_reset_en   (1'b0),
// //     .master_reset_en  (ttc_master_reset_en),
// //     .interval         (ttc_interval)
// // );

// single_ttc_generator inst_single_ttc_generator
// (
//     .clk_40       (clk_40),
//     .rst_40       (rst_40),
//     .encode_ttc   (encode_ttc),
//     .fpga_bcr     (fpga_bcr),
//     .trigger      (trigger_delayed_ttc),
//     .bc_reset     (ttc_bc_reset_vio),
//     .event_reset  (ttc_event_reset_vio),
//     .master_reset (ttc_master_reset_vio)
// );


// data_packet data_packet_inst (
//     .clk(tx_axis_clk),
//     .rst(packet_rst),
//     .enable(packet_enable),
//     .TDC_fifo_data(readout_fifo_data),
//     .TDC_data_ready(~readout_fifo_empty),
//     .TDC_data_rd(readout_fifo_read),
    
//     .data_to_eth(tx_axis_fifo_tdata),
//     .packet_valid(tx_axis_fifo_tvalid),
//     .tx_axis_fifo_tready(tx_axis_fifo_tready),
//     .last(tx_axis_fifo_tlast)
// );




// hit_generator inst_hit_generator
// (
//     .clk               (sys_clk_160),
//     .rst               (rst_160),
//     .hit               (hit),
//     .trigger_out       (trigger_internal),
//     .hit_delay         (hit_delay),
//     .width             (width_hit),
//     .start             (enable_hit),
//     .inv               (hit_inv),
//     .hit_mask          (hit_mask),
//     .delay_mask        (delay_mask),
//     .interval          (hit_interval),
//     .hit_trigger_ratio (hit_trigger_ratio),
//     .start_single      (hit_single)
// );


// vio_trigger vio_trigger_inst (
//     .clk(sys_clk_160),                // input wire clk
//     .probe_out0(rst_160_vio),  // output wire [0 : 0] probe_out0
//     .probe_out1(event_reset),  // output wire [0 : 0] probe_out1
//     .probe_out2(match_window),  // output wire [11 : 0] probe_out2
//     .probe_out3(search_margin),  // output wire [11 : 0] probe_out3
//     .probe_out4(bc_offset),  // output wire [11 : 0] probe_out4
//     .probe_out5(reject_offset),  // output wire [11 : 0] probe_out5
//     .probe_out6(rollover),  // output wire [11 : 0] probe_out6
//     .probe_out7(coarse_count_offset), // output wire [11 : 0] probe_out7
//     .probe_out8(enable_relative_trigger_data),  // output wire [0 : 0] probe_out8
//     .probe_out9(enable_matching),  // output wire [0 : 0] probe_out9
//     .probe_out10(trigger_redge),  // output wire [0 : 0]
//     .probe_out11(trigger_latency) // output wire [7 : 0]
// );

// vio_hit vio_hit_inst (
//     .clk(sys_clk_160),                // input wire clk
//     .probe_out0(enable_hit),  // output wire [0 : 0] probe_out0
//     .probe_out1(enable_internal_trigger),  // output wire [0 : 0] probe_out1
//     .probe_out2(hit_inv),  // output wire [0 : 0] probe_out2
//     .probe_out3(width_hit),  // output wire [11 : 0] probe_out3
//     .probe_out4(hit_interval),  // output wire [11 : 0] probe_out4
//     .probe_out5(hit_mask),  // output wire [23 : 0] probe_out5
//     .probe_out6(hit_single),  // output wire [0 : 0] probe_out6
//     .probe_out7(hit_delay),  // output wire [11 : 0] probe_out7
//     .probe_out8(delay_mask), // output wire [23 : 0] probe_out8
//     .probe_out9(hit_trigger_ratio) // output wire [11 : 0] probe_out9
// );


// vio_40M vio_40M_inst (
//     .clk(clk_40),                // input wire clk
//     .probe_in0({{(10-TDC_COUNT){1'b0}},locked_dline1}),    // input wire [9 : 0] probe_in0
//     .probe_in1({{(10-TDC_COUNT){1'b0}},locked_dline0}),    // input wire [9 : 0] probe_in1

//     .probe_out0(global_rst),  // output wire [0 : 0] probe_out0
//     .probe_out1(correct_counter_th),  // output wire [19 : 0] probe_out1
//     .probe_out2(enable_K28_1),  // output wire [0 : 0] probe_out2
//     .probe_out3(ttc_bc_reset_vio),  // output wire [0 : 0] probe_out3
//     .probe_out4(ttc_event_reset_vio),  // output wire [0 : 0] probe_out4
//     .probe_out5(ttc_master_reset_vio),  // output wire [0 : 0] probe_out5
//     .probe_out6(enable_320M),  // output wire [0 : 0] probe_out6
//     .probe_out7(mezz_TRST) // output wire [0 : 0] probe_out7
// );



// vio_BER vio_BER_inst (
//   .clk(clk_40),                // input wire clk
//   .probe_in0 (tdc_received_40bit_dline1[0]),    // input wire [43 : 0] probe_in0
//   .probe_in1 (tdc_received_40bit_dline0[0]),    // input wire [43 : 0] probe_in1
//   .probe_in2 (tdc_error_bit_dline1[0]),      // input wire [35 : 0] probe_in2
//   .probe_in3 (tdc_error_bit_dline0[0]),      // input wire [35 : 0] probe_in3
//   .probe_in4 (tdc_received_40bit_dline1[1]),     // input wire [43 : 0] probe_in4
//   .probe_in5 (tdc_received_40bit_dline0[1]),     // input wire [43 : 0] probe_in5
//   .probe_in6 (tdc_error_bit_dline1[1]),          // input wire [35 : 0] probe_in6
//   .probe_in7 (tdc_error_bit_dline0[1]),          // input wire [35 : 0] probe_in7
//   .probe_in8 (tdc_received_40bit_dline1[2]),    // input wire [43 : 0] probe_in8
//   .probe_in9 (tdc_received_40bit_dline0[2]),    // input wire [43 : 0] probe_in9
//   .probe_in10(tdc_error_bit_dline1[2]),        // input wire [35 : 0] probe_in10
//   .probe_in11(tdc_error_bit_dline0[2]),        // input wire [35 : 0] probe_in11
//   .probe_in12(tdc_received_40bit_dline1[3]),   // input wire [43 : 0] probe_in12
//   .probe_in13(tdc_received_40bit_dline0[3]),   // input wire [43 : 0] probe_in13
//   .probe_in14(tdc_error_bit_dline1[3]),        // input wire [35 : 0] probe_in14
//   .probe_in15(tdc_error_bit_dline0[3]),        // input wire [35 : 0] probe_in15
//   .probe_in16(tdc_received_40bit_dline1[4]),   // input wire [43 : 0] probe_in16
//   .probe_in17(tdc_received_40bit_dline0[4]),   // input wire [43 : 0] probe_in17
//   .probe_in18(tdc_error_bit_dline1[4]),        // input wire [35 : 0] probe_in18
//   .probe_in19(tdc_error_bit_dline0[4]),        // input wire [35 : 0] probe_in19
//   .probe_in20(tdc_received_40bit_dline1[5]),   // input wire [43 : 0] probe_in20
//   .probe_in21(tdc_received_40bit_dline0[5]),   // input wire [43 : 0] probe_in21
//   .probe_in22(tdc_error_bit_dline1[5]),        // input wire [35 : 0] probe_in22
//   .probe_in23(tdc_error_bit_dline0[5]),        // input wire [35 : 0] probe_in23
//   .probe_in24(tdc_received_40bit_dline1[6]),   // input wire [43 : 0] probe_in24
//   .probe_in25(tdc_received_40bit_dline0[6]),   // input wire [43 : 0] probe_in25
//   .probe_in26(tdc_error_bit_dline1[6]),        // input wire [35 : 0] probe_in26
//   .probe_in27(tdc_error_bit_dline0[6]),        // input wire [35 : 0] probe_in27
//   .probe_in28(tdc_received_40bit_dline1[7]),   // input wire [43 : 0] probe_in28
//   .probe_in29(tdc_received_40bit_dline0[7]),   // input wire [43 : 0] probe_in29
//   .probe_in30(tdc_error_bit_dline1[7]),        // input wire [35 : 0] probe_in30
//   .probe_in31(tdc_error_bit_dline0[7]),        // input wire [35 : 0] probe_in31
//   .probe_in32(tdc_received_40bit_dline1[8]),   // input wire [43 : 0] probe_in32
//   .probe_in33(tdc_received_40bit_dline0[8]),   // input wire [43 : 0] probe_in33
//   .probe_in34(tdc_error_bit_dline1[8]),        // input wire [35 : 0] probe_in34
//   .probe_in35(tdc_error_bit_dline0[8]),        // input wire [35 : 0] probe_in35
//   .probe_in36(tdc_received_40bit_dline1[9]),   // input wire [43 : 0] probe_in36
//   .probe_in37(tdc_received_40bit_dline0[9]),   // input wire [43 : 0] probe_in37
//   .probe_in38(tdc_error_bit_dline1[9]),        // input wire [35 : 0] probe_in38
//   .probe_in39(tdc_error_bit_dline0[9])        // input wire [35 : 0] probe_in39
// );

endmodule
