`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: University of Science and Technology of China
// Engineer: Junbin Zhang
// 
// Create Date:    17:15:31 06/10/2014 
// Design Name:    uart_rx
// Module Name:    uart_rx 
// Project Name:   Target7 evaluation board V1.0
// Target Devices:  XC3S400PQ208
// Tool versions:  ISE 14.7
// Description:    recieve module of uart 
//
// Dependencies:  updated 20150708
//
// Revision: V1.0
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
`include "AX516.def"
module uart_rx(
	input clk,		               //globle clk domain
	input reset_n,	
	input clk_bps,	             //sampling flag from the BaudRate Gen
	input rxd_input,	                 //data in
	input [1:0] parity_mode,     //parity mode
	output rx_ready,	           //when data recieved correctly and completly,rx_ready =1
	output [7:0]rx_data,         //data output
  output baud_start,           //start BaudRate Generator
	output error	               //error output
);
//syn rxd
(* ASYNC_REG = "TRUE" *)(* keep = "TRUE" *)  reg [3:0] rxd_syn=4'b1111;

always @(posedge clk) begin
  if (~reset_n) begin
    // reset
    rxd_syn <= 4'b1111;
  end else begin
    rxd_syn <= {rxd_syn[2:0],rxd_input};
  end
end
wire rxd;

assign rxd = rxd_syn[3];

//check whether there is a falling edge on rxd pin
reg rxd_sync;
always @ (posedge clk ) begin
  if(~reset_n) 
    rxd_sync <= 1'b0;
  else 
    rxd_sync <= rxd;
end
wire rxd_fall;
assign rxd_fall = rxd_sync & (~rxd);
//start recieve progress control
reg baud_start_r;
reg [3:0] num;
reg rx_start;
reg rx_busy;
always @ (posedge clk) begin
  if(~reset_n) begin
    baud_start_r <= 1'b0;
    rx_start <= 1'b0;
  end
  else if(rxd_fall && !rx_busy) begin
    baud_start_r <= 1'b1;
    rx_start <= 1'b1;
  end
  else if(num == 4'd11)begin
    baud_start_r <= 1'b0;
    rx_start <= 1'b0;
  end
  else begin
    baud_start_r <= baud_start_r;
    rx_start <= rx_start;
  end
end
assign baud_start = baud_start_r;
//start recieve progress
reg [7:0] buffer; //data recieve buffer
reg r_error;      //if data parity error
reg rx_ready_r;   //data recieve ready
reg [7:0] data;
localparam [1:0] Idle = 2'b00,
                RX_START = 2'b01,
                RX_DONE  = 2'b10;
reg [1:0] State;
always @ (posedge clk) begin
  if(~reset_n) begin
    rx_busy <= 1'b0;
    num <= 4'b0;
    r_error <= 1'b0;
    rx_ready_r <= 1'b0;
    buffer <= 8'b0;
    data <= 8'b0;
    State <= Idle;
  end
  else begin
    case(State)
      Idle:begin
        if(rx_start) begin
          State <= RX_START;
          rx_busy <= 1'b1;
        end
        else begin
          State <= Idle;
          rx_busy <= 1'b0;
        end
      end
      RX_START:begin
        if(clk_bps) begin          
          case(num)
            4'd0:r_error <= 1'b0;//num = 0 this is the start bit 
            4'd1:buffer[0] <= rxd;
            4'd2:buffer[1] <= rxd;
            4'd3:buffer[2] <= rxd;
            4'd4:buffer[3] <= rxd;
            4'd5:buffer[4] <= rxd;
            4'd6:buffer[5] <= rxd;
            4'd7:buffer[6] <= rxd;
            4'd8:buffer[7] <= rxd;            
            4'd9:case(parity_mode)
              `PARITY_EVEN:r_error <= ^buffer ^ rxd;
             `PARITY_ODD: r_error <= ~(^buffer ^ rxd);
                default:r_error <= ~rxd;
                endcase               
            default:;
          endcase
          num <= num + 1'b1;
        end
        else if(num == 4'd11) begin
           num <= 4'd0;
           data <= buffer;
           rx_ready_r <= 1'b1;
           State <= RX_DONE;
        end
        else begin
            rx_ready_r <= 1'b0;
            State <= RX_START;
        end
      end
      RX_DONE:begin
        rx_busy <= 1'b0;
        rx_ready_r <= 1'b0;
        //buffer <= 8'b0;
        State <= Idle;
      end
    endcase
  end
end
assign error = r_error;
assign rx_ready = rx_ready_r;
assign rx_data = data;

endmodule


