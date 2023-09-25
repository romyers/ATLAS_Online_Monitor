`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: University of Michigan
// Engineer: Ly
// 
// Create Date: 08/16/2017 11:55:18 AM
// Design Name: 
// Module Name: UART_interface
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


module UART_interface(
    input clk40,
    input reset,
    //UART interface
    input  rxd,
    output txd,
    //command_interface
    input command_fifo_rd_en,
    output [159:0] command,
    output command_fifo_empty,
    //data interface
    // input [159:0] data_back,
    // input data_back_fifo_wr_en,
    //lpgbt ic rd_fifo interface
    input tx_start,
    input [7:0] tx_data,
    output tx_ready

);


wire [7:0] rx_data;
wire rx_ready;
wire tx_start;
wire rx_parity_error;

wire frame_ready;
wire [159:0] frame_data;
wire command_fifo_full;

wire [159:0] data_back_out;
wire data_back_fifo_full,data_back_fifo_empty,data_back_fifo_rd_en;

localparam parity_mode = 2'b00;
localparam baud_rate   = 2'b11;




uart_top uart_inst(
    .clk(clk40),
    .reset_n(~reset),
    .parity_mode(parity_mode),
    .baud_rate(baud_rate),
    .rxd(rxd),
    .rx_data(rx_data),
    .rx_ready(rx_ready),
    .rx_parity_error(rx_parity_error),

    .tx_data(tx_data),
    .tx_start((tx_start)&(~tx_ready)),
    .txd(txd), 
    //tx_start only recognizes the rising edge, so need a force down
    .tx_ready(tx_ready)
);
 

frame_build  frame_build_inst(
   .clk(clk40),
   .reset_n(~reset),
   .rx_data(rx_data),
   .rx_ready(rx_ready),
   .rx_parity_error(rx_parity_error),
   .frame_ready(frame_ready),
   .frame_data(frame_data)
);
                
                    
fifo_160bit command_fifo_inst (
  .clk(clk40),                  // input wire clk
  .srst(reset),                // input wire srst
  .din(frame_data),                  // input wire [159 : 0] din
  .wr_en(frame_ready),              // input wire wr_en
  .rd_en(command_fifo_rd_en),              // input wire rd_en
  .dout(command),                // output wire [159 : 0] dout
  .full(command_fifo_full),                // output wire full
  .empty(command_fifo_empty),              // output wire empty
  .wr_rst_busy(),  // output wire wr_rst_busy
  .rd_rst_busy()  // output wire rd_rst_busy
);
              

// fifo_160bit data_back_fifo (
//   .clk(clk40),                  // input wire clk
//   .srst(reset),                // input wire srst
//   .din(data_back),                  // input wire [159 : 0] din
//   .wr_en(data_back_fifo_wr_en),              // input wire wr_en
//   .rd_en(data_back_fifo_rd_en),              // input wire rd_en
//   .dout(data_back_out),                // output wire [159 : 0] dout
//   .full(data_back_fifo_full),                // output wire full
//   .empty(data_back_fifo_empty),              // output wire empty
//   .wr_rst_busy(),  // output wire wr_rst_busy
//   .rd_rst_busy()  // output wire rd_rst_busy
// );

// send_back_data send_back_data_inst(
//     .clk(clk40),
//     .reset(reset),

//     .data_back_fifo_empty(data_back_fifo_empty),
//     .data_back_fifo_rd(data_back_fifo_rd_en),
//     .data_back(data_back_out),

//     .send_ready(tx_ready),
//     .start_send(tx_start),
//     .data_send(tx_data)
// );

endmodule
