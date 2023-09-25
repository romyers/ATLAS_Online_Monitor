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
// File   : tdcv2_decoder_wrapper.v
// Create : 2021-03-10 14:45:53
// Revise : 2022-08-23 02:47:13
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------

module tdcv2_decoder_wrapper
#(parameter TDC_COUNT = 10,
            IS_MASTER = 1,
            CSM_ID    = 0,
       TDC_DATA_WIDTH = 40)
    (
    //clk_40 aligned
    input  clk_40,
    input  rst_40,
    input  [229:0] userDataUpLink_i,
    input  trigger_in,
    input  [19:0] correct_counter_th,
    input  enable_K28_1,
    input  enable_320M,
    output [TDC_COUNT-1:0] locked_dline1,
    output [TDC_COUNT-1:0] locked_dline0, 

    //sys_clk_160 aligned
    input  sys_clk_160,
    input  rst_160,
    input  [TDC_COUNT-1:0] tdc_fifo_read,    
    output [TDC_COUNT-1:0] tdc_fifo_empty,
    output [TDC_COUNT*TDC_DATA_WIDTH-1:0] tdc_fifo_data,
    output [TDC_COUNT-1:0] matching_busy,

    //trigger interface
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
    output [44*TDC_COUNT-1:0] total_received_40bit_dline1,
    output [44*TDC_COUNT-1:0] total_received_40bit_dline0,
    output [36*TDC_COUNT-1:0] total_error_bit_dline1,
    output [36*TDC_COUNT-1:0] total_error_bit_dline0,

    //SCA
    output [7:0] SCA0_PRI,
    output [7:0] SCA0_AUX,
    output [7:0] SCA1_PRI,
    output [7:0] SCA1_AUX,
    output [7:0] SCA2_PRI,
    output [7:0] SCA2_AUX

);


wire [7:0] elink_320m [27:0]; //Maximum of 28 links @ 320Mbps
wire [7:0] tdc_dline_1[9:0];
wire [7:0] tdc_dline_0[9:0];
wire [4:0] tdc_id[TDC_COUNT-1:0];  
wire [7:0] TTC_ENC;

generate
    genvar i;
    for (i = 0; i < 28; i = i + 1) begin
        assign elink_320m[i] = userDataUpLink_i[i*8+7:i*8]; 
    end   
endgenerate

generate
    genvar k;
    for (k = 0; k < TDC_COUNT; k = k + 1) begin
        assign tdc_id[k] = IS_MASTER? k : k+5'd10; 
    end   
endgenerate


// TDC**_CH*_IN corresponds to lpGBT_CSM board schematic net name;
// tdc_dline_1 mapped to TDC**_CH1_IN, tdc_dline_0 mapped to TDC**_CH2_IN.
assign tdc_dline_1[0] = IS_MASTER ? elink_320m[27] : elink_320m[27];
assign tdc_dline_0[0] = IS_MASTER ? elink_320m[26] : elink_320m[26]; 
assign tdc_dline_1[1] = IS_MASTER ? elink_320m[21] : elink_320m[16];
assign tdc_dline_0[1] = IS_MASTER ? elink_320m[20] : elink_320m[15];
assign tdc_dline_1[2] = IS_MASTER ? elink_320m[25] : elink_320m[25];
assign tdc_dline_0[2] = IS_MASTER ? elink_320m[24] : elink_320m[24];
assign tdc_dline_1[3] = IS_MASTER ? elink_320m[19] : elink_320m[15];
assign tdc_dline_0[3] = IS_MASTER ? elink_320m[18] : elink_320m[14];
assign tdc_dline_1[4] = IS_MASTER ? elink_320m[23] : elink_320m[19];
assign tdc_dline_0[4] = IS_MASTER ? elink_320m[22] : elink_320m[18];
assign tdc_dline_1[5] = IS_MASTER ? elink_320m[4 ] : elink_320m[4 ]; 
assign tdc_dline_0[5] = IS_MASTER ? elink_320m[5 ] : elink_320m[5 ]; 
assign tdc_dline_1[6] = IS_MASTER ? elink_320m[0 ] : elink_320m[0 ]; 
assign tdc_dline_0[6] = IS_MASTER ? elink_320m[1 ] : elink_320m[1 ]; 
assign tdc_dline_1[7] = IS_MASTER ? elink_320m[6 ] : elink_320m[6 ]; 
assign tdc_dline_0[7] = IS_MASTER ? elink_320m[7 ] : elink_320m[7 ]; 
assign tdc_dline_1[8] = IS_MASTER ? elink_320m[2 ] : 'b0;
assign tdc_dline_0[8] = IS_MASTER ? elink_320m[3 ] : 'b0;
assign tdc_dline_1[9] = IS_MASTER ? elink_320m[8 ] : 'b0;
assign tdc_dline_0[9] = IS_MASTER ? elink_320m[9 ] : 'b0;

// SCA data extracted from 320Mbps elink bits 
assign SCA0_PRI       = IS_MASTER ? elink_320m[15] : 'b0;
assign SCA0_AUX       = IS_MASTER ? elink_320m[17] : 'b0;
assign SCA1_PRI       = IS_MASTER ? elink_320m[12] : 'b0;
assign SCA1_AUX       = IS_MASTER ? elink_320m[11] : 'b0;
assign SCA2_PRI       = IS_MASTER ? elink_320m[14] : 'b0;
assign SCA2_AUX       = IS_MASTER ? elink_320m[13] : 'b0;
assign TTC_ENC        = IS_MASTER ? elink_320m[10] : 'b0;

generate
    genvar j;
    for (j = 0; j < TDC_COUNT; j = j + 1) begin: tdcv2_decoder
        tdcv2_decoder inst_tdcv2_decoder
        (
            .clk_40                       (clk_40),
            .rst_40                       (rst_40),
            .tdc_dline_1                  (tdc_dline_1[j]),
            .tdc_dline_0                  (tdc_dline_0[j]),
            .tdc_id                       (tdc_id[j]),
            .csm_id                       (CSM_ID[2:0]),

            .tdc_fifo_read                (tdc_fifo_read[j]),
            .tdc_fifo_empty               (tdc_fifo_empty[j]),
            .tdc_fifo_data                (tdc_fifo_data[(j+1)*TDC_DATA_WIDTH-1:j*TDC_DATA_WIDTH]),

            .enable_K28_1                 (enable_K28_1),
            .enable_320M                  (enable_320M),
            .correct_counter_th           (correct_counter_th),
            .locked_dline1                (locked_dline1[j]),
            .locked_dline0                (locked_dline0[j]),

            .sys_clk_160                  (sys_clk_160),
            .rst_160                      (rst_160),
            .matching_busy                (matching_busy[j]),
            .enable_matching              (enable_matching),
            .enable_matching_timeout      (enable_matching_timeout),
            .trigger_fifo_data            (trigger_fifo_data),
            .trigger_fifo_empty           (trigger_fifo_empty),
            .building_busy                (building_busy),
            .trigger_coarse               (trigger_coarse),
            .match_window                 (match_window),
            .search_margin                (search_margin),
            .bc_offset                    (bc_offset),
            .reject_offset                (reject_offset),
            .rollover                     (rollover),
            .enable_relative_trigger_data (enable_relative_trigger_data),

            .total_received_40bit_dline1 (total_received_40bit_dline1[44*(j+1)-1:44*j]),
            .total_received_40bit_dline0 (total_received_40bit_dline0[44*(j+1)-1:44*j]),
            .total_error_bit_dline1      (total_error_bit_dline1[36*(j+1)-1:36*j]),
            .total_error_bit_dline0      (total_error_bit_dline0[36*(j+1)-1:36*j])
        );
    end   
endgenerate

endmodule
