/* 
       _________   ________     ________       
      / U OF M  \ | LSA    \   / Physics\
      \__ATLAS__/ |   ___   | |   ______/
         |   |    |  |   \  | |  |
         |   |    |  |___/  | |  \______     
         |   |    |         | |         \
         \___/    |________/   \________/
*/  

// -----------------------------------------------------------------------------
// Author : Yuxiang Guo  gyuxiang@umich.edu
// File   : send_back_data.v
// Create : 2021-07-08 11:30:04
// Revise : 2021-07-08 11:30:11
// Editor : sublime text4, tab size (4)
// -----------------------------------------------------------------------------
// Description: 
//


`timescale 1ns / 1ps

module send_back_data(
    input clk,
    input reset,

    input data_back_fifo_empty,
    output reg data_back_fifo_rd,
    input [159:0] data_back,
    
    input send_ready,
    output reg start_send,
    output reg [7:0] data_send
    );

reg [2:0] state=3'b00;
reg [159:0] data_back_r;
reg [4:0] data_count= 5'b0;
reg long_mode = 1'b0;

localparam IDLE = 3'b000;
localparam CONTENT_RESOLVE = 3'b001;
localparam SEND_PACKET = 3'b010;
localparam WAIT_ACK = 3'b011;

always @(posedge clk) begin
	if (reset) begin
		state <= IDLE;
		start_send <= 1'b0;
		data_send <= 8'b0;
		data_back_fifo_rd <= 1'b0;
		data_count <= 5'b0;
	end	else begin
		start_send <= 1'b0;
		data_send <= 8'b0;
		data_back_fifo_rd <= 1'b0;
		state <= IDLE;
		case(state)
			IDLE: if(~data_back_fifo_empty)begin
				data_back_r <= data_back;
				data_back_fifo_rd <= 1'b1;
				state <= CONTENT_RESOLVE;
			end
			CONTENT_RESOLVE: begin				
				if(data_back_r[159:152]==8'b1111_1111)begin					
					data_count <= 5'b10100;
					long_mode <= 1'b1;
					state <= SEND_PACKET;
				end else if(data_back_r[159:24]=={128'b0,8'b1010_1010}) begin
					data_count <= 5'b00100;
					long_mode <= 1'b0;
					state <= SEND_PACKET;
				end 
			end
			SEND_PACKET:begin
				if(|data_count)begin
					start_send <= 1'b1;
					data_send <= long_mode ? data_back_r[159:152]:data_back_r[31:24];
					data_back_r <= {data_back_r[151:0],8'b0};
					data_count <= data_count-1;
					state <= WAIT_ACK;
				end else begin
					start_send <= 1'b0;
					data_count <= 5'b0;
					state <= IDLE;
				end
			end
			WAIT_ACK:
				if(send_ready)begin
					state <= SEND_PACKET; 
				end else begin
					state <= WAIT_ACK; 
				end
		     default: begin end
		endcase
	end
end
endmodule
