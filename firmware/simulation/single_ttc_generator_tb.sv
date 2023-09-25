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
// File   : single_ttc_generator_tb.sv
// Create : 2021-08-29 18:34:05
// Revise : 2021-09-01 02:13:22
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------

`timescale 1ns/100ps
`define CLOCK_PERIOD 2 
`define SD #1

module single_ttc_generator_tb();

logic clk;
logic rst;

always begin
    #(`CLOCK_PERIOD/2.0)
    clk = ~ clk; 
end


logic [1:0] encode_ttc;
logic trigger;
logic event_reset;
logic master_reset;
logic bc_reset;
logic fpga_bcr;

single_ttc_generator inst_single_ttc_generator
(
    .clk_40       (clk),
    .rst_40       (rst),
    .encode_ttc   (encode_ttc),
    .fpga_bcr     (fpga_bcr),
    .trigger      (trigger),
    .bc_reset     (bc_reset),
    .event_reset  (event_reset),
    .master_reset (master_reset)
);





initial begin
    clk = 0;
    rst = 0;

    @(posedge clk);
    `SD
    rst = 1;
    @(posedge clk);
    `SD
    rst = 0;
    bc_reset = 1'b0;
    trigger = 1'b0;
    master_reset = 1'b0;
    event_reset = 1'b0;

    //Run test cases here.
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    `SD
    #100
    
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk); 
    `SD
    
    trigger = 1'b1;
    master_reset = 1'b1;
    event_reset = 1'b1;

    # 8
    bc_reset = 1'b1;

    @(posedge clk);
    `SD
    trigger = 1'b0;
    master_reset = 1'b0;
    event_reset = 1'b0;
    bc_reset = 1'b0;
    #100

    @(posedge clk);
    `SD
    trigger = 1'b1;
    master_reset = 1'b1;
    event_reset = 1'b1;

    # 6
    bc_reset = 1'b1;
    @(posedge clk);
    `SD
    trigger = 1'b0;
    master_reset = 1'b0;
    event_reset = 1'b0;
    bc_reset = 1'b0;


    #100
    @(posedge clk);
    `SD
    trigger = 1'b1;
    master_reset = 1'b1;
    event_reset = 1'b1;

    # 4
    bc_reset = 1'b1;


    #21 
    bc_reset = 1'b0;
    trigger = 1'b0;
    master_reset = 1'b0;
    event_reset = 1'b0;
    #100
    @(posedge clk);
    #21
    bc_reset = 1'b1;


    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    #200;

    $finish;
end


endmodule
