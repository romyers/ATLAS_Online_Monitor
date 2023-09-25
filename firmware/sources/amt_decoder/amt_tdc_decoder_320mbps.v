`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: Umich
// Engineer: Xueye
// 
// Create Date: 10/30/2020 04:26:03 PM
// Design Name: 
// Module Name: amt_gtx_decode_320mbps
// Project Name: 
// Target Devices: 
// Tool Versions: 
// Description: 
// 
// Dependencies: 
// 
// Revision:
// Revision 0.01 - File Created
// Additional Comments:
// 
//////////////////////////////////////////////////////////////////////////////////


module amt_tdc_decoder_320mbps(

    input user_rst,
    input RESET_I,
    input RX_FRAMECLK_I,
    
    input [7:0] tdc_d0_DATA_I,  
    input [7:0] tdc_d1_DATA_I,          

    input GBT_RX_READY_I
    );
    
(* dont_touch = "true" *) wire [33:0] tdc_pkt_raw_0_c1,tdc_pkt_raw_1_c1;
(* dont_touch = "true" *) wire [33:0] tdc_pkt_raw_0_c2,tdc_pkt_raw_1_c2;

(* dont_touch = "true" *) wire [1:0] tdc_d0_c1,tdc_d1_c1;
(* dont_touch = "true" *) wire [1:0] tdc_d0_c2,tdc_d1_c2;

(* dont_touch = "true" *) wire [3:0] tdc_pkt_type_0_c1, tdc_pkt_type_1_c1;
(* dont_touch = "true" *) wire [3:0] tdc_pkt_type_0_c2, tdc_pkt_type_1_c2;

(* dont_touch = "true" *) wire [3:0] tdc_id_0_c1, tdc_id_1_c1;
(* dont_touch = "true" *) wire [3:0] tdc_id_0_c2, tdc_id_1_c2;

(* dont_touch = "true" *) wire [4:0] tdc_chnum_0_c1, tdc_chnum_1_c1;
(* dont_touch = "true" *) wire [4:0] tdc_chnum_0_c2, tdc_chnum_1_c2;

(* dont_touch = "true" *) wire tdc_edgetype_0_c1, tdc_edgetype_1_c1;
(* dont_touch = "true" *) wire tdc_edgetype_0_c2, tdc_edgetype_1_c2;

(* dont_touch = "true" *) wire parity_chk_0_c1, parity_chk_1_c1;
(* dont_touch = "true" *) wire parity_chk_0_c2, parity_chk_1_c2;    
    
// re-organize RX_DATA_I[229:0]
assign tdc_d0_c1={tdc_d0_DATA_I[4], tdc_d0_DATA_I[0]},  //CSM-MO: H[16]
       tdc_d1_c1={tdc_d1_DATA_I[4], tdc_d1_DATA_I[0]}; //CSM-MO: H[17]


assign tdc_d0_c2={tdc_d0_DATA_I[5] , tdc_d0_DATA_I[1]},
       tdc_d1_c2={tdc_d1_DATA_I[5], tdc_d1_DATA_I[1]};


// 3200Mbps sample 80Mbps, copy 1   
tdc_decoder_2bits tdc_decode_inst_0_c1(
  .RX_FRAMECLK_I(RX_FRAMECLK_I),
  .user_rst(user_rst),
  .tdc_d0(tdc_d0_c1),
  .parity_chk(parity_chk_0_c1),
  .tdc_pkt_raw(tdc_pkt_raw_0_c1)
    );

tdc_decoder_2bits tdc_decode_inst_1_c1(
  .RX_FRAMECLK_I(RX_FRAMECLK_I),
  .user_rst(user_rst),
  .tdc_d0(tdc_d1_c1),
  .parity_chk(parity_chk_1_c1),
  .tdc_pkt_raw(tdc_pkt_raw_1_c1)
    );       

assign tdc_pkt_type_0_c1=tdc_pkt_raw_0_c1[33:30], 
       tdc_pkt_type_1_c1=tdc_pkt_raw_1_c1[33:30];
       
assign tdc_id_0_c1=tdc_pkt_raw_0_c1[29:26], 
       tdc_id_1_c1=tdc_pkt_raw_1_c1[29:26];       
       
assign tdc_chnum_0_c1=tdc_pkt_raw_0_c1[25:21], 
       tdc_chnum_1_c1=tdc_pkt_raw_1_c1[25:21];       

assign tdc_edgetype_0_c1=tdc_pkt_raw_0_c1[20], 
       tdc_edgetype_1_c1=tdc_pkt_raw_1_c1[20];
       
       
ila_tdc_raw ila_inst_c1(
      .clk( RX_FRAMECLK_I),
      .probe0( tdc_d0_DATA_I), 
      
      .probe1(tdc_pkt_raw_0_c1),
      .probe2(tdc_pkt_raw_1_c1),
//      .probe3(tdc_pkt_raw_2_c1),
//      .probe4(tdc_pkt_raw_3_c1),
//      .probe5(tdc_pkt_raw_4_c1),

      .probe3(tdc_pkt_type_0_c1),
      .probe4(tdc_pkt_type_1_c1),
//      .probe8(tdc_pkt_type_2_c1),
//      .probe9(tdc_pkt_type_3_c1),
//      .probe10(tdc_pkt_type_4_c1),

      .probe5(tdc_id_0_c1),
      .probe6(tdc_id_1_c1),
//      .probe13(tdc_id_2_c1),
//      .probe14(tdc_id_3_c1),
//      .probe15(tdc_id_4_c1),
      
      .probe7({tdc_edgetype_0_c1, parity_chk_0_c1}),
      .probe8({tdc_edgetype_1_c1, parity_chk_1_c1}),
//      .probe18({tdc_edgetype_2_c1, parity_chk_2_c1}),
//      .probe19({tdc_edgetype_3_c1, parity_chk_3_c1}),
//      .probe20({tdc_edgetype_4_c1, parity_chk_4_c1}),

      .probe9(tdc_chnum_0_c1),
      .probe10(tdc_chnum_1_c1)
//      .probe23(tdc_chnum_2_c1),
//      .probe24(tdc_chnum_3_c1),
//      .probe25(tdc_chnum_4_c1)      
      
         );       
       
// 320Mbps sample 80Mbps, copy 2  

tdc_decoder_2bits tdc_decode_inst_0_c2(
  .RX_FRAMECLK_I(RX_FRAMECLK_I),
  .user_rst(user_rst),
  .tdc_d0(tdc_d0_c2),
  .parity_chk(parity_chk_0_c2),
  .tdc_pkt_raw(tdc_pkt_raw_0_c2)
    );

tdc_decoder_2bits tdc_decode_inst_1_c2(
  .RX_FRAMECLK_I(RX_FRAMECLK_I),
  .user_rst(user_rst),
  .tdc_d0(tdc_d1_c2),
  .parity_chk(parity_chk_1_c2),
  .tdc_pkt_raw(tdc_pkt_raw_1_c2)
    );       

assign tdc_pkt_type_0_c2=tdc_pkt_raw_0_c2[33:30], 
       tdc_pkt_type_1_c2=tdc_pkt_raw_1_c2[33:30];

assign tdc_id_0_c2=tdc_pkt_raw_0_c2[29:26], 
       tdc_id_1_c2=tdc_pkt_raw_1_c2[29:26];

assign tdc_chnum_0_c2=tdc_pkt_raw_0_c2[25:21], 
       tdc_chnum_1_c2=tdc_pkt_raw_1_c2[25:21];        

assign tdc_edgetype_0_c2=tdc_pkt_raw_0_c2[20], 
       tdc_edgetype_1_c2=tdc_pkt_raw_1_c2[20];  
              
       
ila_tdc_raw ila_inst_c2(
      .clk( RX_FRAMECLK_I),
      .probe0( tdc_d1_DATA_I), 
      
      .probe1(tdc_pkt_raw_0_c2),
      .probe2(tdc_pkt_raw_1_c2),

      .probe3(tdc_pkt_type_0_c2),
      .probe4(tdc_pkt_type_1_c2),

      .probe5(tdc_id_0_c2),
      .probe6(tdc_id_1_c2),
      
      .probe7({tdc_edgetype_0_c2, parity_chk_0_c2}),
      .probe8({tdc_edgetype_1_c2, parity_chk_1_c2}),

      .probe9(tdc_chnum_0_c2),
      .probe10(tdc_chnum_1_c2)
        
         );       
              
           
endmodule
