`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/01/2017 05:26:35 PM
// Design Name: 
// Module Name: TDO_align
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


module TDO_align
(
	input clk,
	input align,
	input [4095:0] JTAG_data_out,
	input [7:0] bit_length,
	input [4:0] device_count,
	output reg [4095:0] bits_align,
	output reg align_busy
	);

wire[12:0] total_bits;
reg [12:0] shift_counter;
reg [4:0]  device_counter;
reg [1:0] align_reg;
wire align_syn;
assign align_syn = ~align_reg[1] & align_reg[0];

always @(posedge clk) begin
	align_reg <= {align_reg[0], align};
end

always @(posedge clk) begin
	if(align_syn) begin
		shift_counter <= 4096-total_bits;
		bits_align    <= JTAG_data_out;
		align_busy    <= 1'b1;
	end else if(|shift_counter) begin
		shift_counter <= shift_counter - 1'b1;
		bits_align    <= {1'b0, bits_align[4095:1]};
		align_busy    <= 1'b1;
	end else begin 
		align_busy    <= 1'b0;
	end
end


multiplier multiplier_inst (
  .CLK(clk),  // input wire CLK
  .A(bit_length),      // input wire [7 : 0] A
  .B(device_count),      // input wire [4 : 0] B
  .P(total_bits)      // output wire [12 : 0] P
);
endmodule