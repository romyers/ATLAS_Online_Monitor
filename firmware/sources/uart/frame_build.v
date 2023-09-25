`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 2015/10/14 11:28:17
// Design Name: 
// Module Name: frame_build
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


module frame_build(
    input clk,
    input reset_n,
    input [7:0] rx_data,
    input  rx_ready,
    input rx_parity_error,
    output reg frame_ready,
    output reg [159:0]  frame_data
  );
localparam IDLE = 2'b00;
localparam GET_DATA = 2'b01;
localparam UPDATE_FRAME = 2'b10;

reg [1:0] state = 2'b00; 
reg [3:0] data_counter = 4'b0001;
always @(posedge clk) begin
  if (~reset_n) begin
    // reset
    state <= IDLE;
    data_counter <=  4'b0001;
    frame_data  <= 160'b0;
    frame_ready <= 1'b0;
  end
  else begin
    case(state)
      IDLE: begin
        frame_ready <= 1'b0;
        if(rx_ready&(rx_data==8'b0000_0000)&(~rx_parity_error))begin
            state <= GET_DATA;
            data_counter <=  4'b0001;
            frame_data  <= 160'b0;
        end
      end
      GET_DATA: begin
        frame_ready <= 1'b0;
        if (rx_ready&(~rx_parity_error)) begin
          if(rx_data==8'b0000_0000)begin
            state <= GET_DATA;
            data_counter <=  4'b0001;
            frame_data  <= 160'b0;
            frame_ready <= 1'b0;            
          end else if(rx_data==8'b0000_1111)begin
            state <= UPDATE_FRAME;
          end else if(rx_data[7:4]==data_counter)begin
            state <= GET_DATA;
            frame_data <= {frame_data[155:0],rx_data[3:0]};
            data_counter <= (data_counter==4'b1111) ? 4'b0001:data_counter+4'b0001;
          end else begin
            state <= IDLE;
            frame_data  <= 160'b0;
          end
        end
      end
      UPDATE_FRAME: begin
         frame_ready <= 1'b1;
         state <= IDLE;
      end
      default:begin end
      endcase
  end
end
endmodule
