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
// File   : single_ttc_generator_tb.v
// Create : 2021-08-29 18:34:05
// Revise : 2021-08-29 18:42:50
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

single_ttc_generator inst_single_ttc_generator
(
    .clk_40       (clk),
    .rst_40       (rst),
    .encode_ttc   (encode_ttc),
    .trigger      (trigger),
    .bc_reset     (bc_reset),
    .event_reset  (event_reset),
    .master_reset (master_reset)
);


// display output 
always @(posedge clk) begin
    if (chnl_data_valid) begin
        $display("output: %b", encode_ttc[0]);
    end
end

task input_base ();
    begin
        while (counter != 3) begin
            @(posedge clk);
        end
        `SD
        o_Kout = K;
        data_64b_decode = data; 
        @(posedge clk);
    end
endtask



initial begin
    clk = 0;
    rst = 0;

    @(posedge clk);
    `SD
    rst = 1;
    @(posedge clk);
    `SD
    rst = 0;

    //Run test cases here.
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk) 'SD bc_reset = 1;

    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $finish;
end


endmodule
