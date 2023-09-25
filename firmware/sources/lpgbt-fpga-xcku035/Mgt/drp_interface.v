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
// File   : drp_interface.v
// Create : 2023-02-19 22:38:55
// Revise : 2023-06-27 03:44:59
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
 

module drp_interface
(
    input drp_clk,
    input drprdy_i,
    input [15:0] drpdo_i,
    output drpen_o,
    output drpwe_o,
    output [8:0] drpaddr_o,
    output [15:0] drpdi_o,

    output rxprbscntreset_o,
    output [3:0] rxprbssel_o,
    input  rxprbserr_i,
    input  rxprbslocked_i
);

reg [1:0] drpen_syn;
reg [1:0] drpwe_syn;
reg [15:0] drpdo_reg;
wire drpen_vio;
wire drpwe_vio;


always @(posedge drp_clk) begin
    drpen_syn <= {drpen_syn[0],drpen_vio|drpwe_vio};
    drpwe_syn <= {drpwe_syn[0],drpwe_vio};
end

always @(posedge drp_clk) begin
    if (drprdy_i) begin
        drpdo_reg <= drpdo_i;
    end
end

assign drpen_o = (~drpen_syn[1]) & drpen_syn[0];
assign drpwe_o = (~drpwe_syn[1]) & drpwe_syn[0];


vio_drp vio_drp_inst (
  .clk(drp_clk),                // input wire clk
  .probe_in0(drprdy_i),    // input wire [0 : 0] probe_in0
  .probe_in1(drpdo_reg),    // input wire [15 : 0] probe_in1
  .probe_in2(rxprbserr_i),    // input wire [0 : 0] probe_in2
  .probe_in3(rxprbslocked_i),    // input wire [0 : 0] probe_in3
  .probe_out0(drpen_vio),  // output wire [0 : 0] probe_out0
  .probe_out1(drpwe_vio),  // output wire [0 : 0] probe_out1
  .probe_out2(drpaddr_o),  // output wire [8 : 0] probe_out2
  .probe_out3(drpdi_o),  // output wire [15 : 0] probe_out3
  .probe_out4(rxprbscntreset_o),  // output wire [0 : 0] probe_out4
  .probe_out5(rxprbssel_o)  // output wire [3 : 0] probe_out5
);

// ila_drp ila_drp_inst (
//     .clk(drp_clk), // input wire clk
//     .probe0(drprdy_i), // input wire [0:0]  probe0  
//     .probe1(drpdo_i), // input wire [15:0]  probe1 
//     .probe2(drpen_o), // input wire [0:0]  probe2 
//     .probe3(drpwe_o), // input wire [0:0]  probe3 
//     .probe4(drpaddr_o), // input wire [8:0]  probe4 
//     .probe5(drpdi_o), // input wire [15:0]  probe5
//     .probe6(rxprbserr_i), // input wire [0:0]  probe6 
//     .probe7(rxprbslocked_i) // input wire [0:0]  probe7
// );

endmodule


