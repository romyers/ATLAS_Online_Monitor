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
// File   : hit_generator.v
// Create : 2021-03-20 21:34:02
// Revise : 2021-08-30 22:45:08
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
//     
module hit_generator(
	input clk,
	input rst,
	output [23:0] hit,
	output trigger_out,

	input [11:0] width,
	input start,
	input inv,
	input [23:0] hit_mask,
	input [23:0] delay_mask,
	input [11:0] interval,
	input [11:0] hit_delay,
	input [11:0] hit_trigger_ratio,
	input start_single
);

wire hit_inner;
reg  [23:0] hit_reg;
wire hit_delayed;
wire start_pulse;
reg  [11:0] interval_counter = 12'b0;
reg  [4095:0] delay_chain;

reg [11:0] hit_count;

assign hit_delayed = delay_chain[hit_delay];

always @(posedge clk) begin
	if(rst) begin
		hit_count<= 12'b1;
	end else if (interval_counter == interval) begin
		if (hit_count==hit_trigger_ratio)
			hit_count<= 12'b1;
		else
			hit_count<= hit_count+1'b1;
	end
end

assign trigger_out = (hit_count==hit_trigger_ratio)?hit_inner:
					 inv?1'b1:1'b0;   //if inv, trigger_out = high means no trigger

pulse_hit_generator pulse_hit_generator_inst (
	.clk(clk), 
	.rst(rst), 
	.width(width), 
	.start(start_pulse), 
	.inv(inv), 
	.hit(hit_inner)
);

always @(posedge clk) begin
	delay_chain <= {delay_chain[4094:0],hit_inner} ;
end

genvar i;
generate
	for (i = 0; i < 24; i = i + 1) begin
	   assign hit[i] = hit_mask[i]? (delay_mask[i]?hit_delayed:hit_inner) : 1'b0;
	end
endgenerate

// always @(posedge clk) begin
// 	trigger_out <= hit_inner;
// end

always @(posedge clk) begin
	if (rst) begin
		interval_counter <= 12'b0;
	end else begin
		interval_counter <= (interval_counter == interval) ? 12'b0 : interval_counter +12'b1;
	end
end

assign start_pulse = (start & ( interval_counter == interval ))|start_single;

endmodule
