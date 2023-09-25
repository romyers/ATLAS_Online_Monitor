`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: University of Science and Technology of China 
// Engineer: Junbin Zhang
// 
// Create Date:    13:33:19 06/10/2014 
// Design Name:    uart_top
// Module Name:    uart_top 
// Project Name:   Target7 evaluation board V1.0
// Target Devices:   XC3S400PQ208
// Tool versions:  ISE14.7
// Description:  top module of uart,by the way baud rate and parity mode can be adjusted.
//                          parity_mode                 baud_rate
//                          00       EVEN               00           9600bps
//                          01       ODD                01           19200bps
//                          1x       NONE               10           38400bps
//                                                      11           115200bps
// Revision: V1.0
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
module uart_top(
    input clk,
    input reset_n,
    input [1:0] parity_mode,
    input [1:0] baud_rate,
    
    input rxd,
    output [7:0] rx_data,
    output rx_ready,
    output rx_parity_error,

    input [7:0] tx_data,
    input tx_start,
    output txd,
    output tx_ready
    );
/*----------BaudRate_Gen Rx instantiation---------*/
wire baud_start_rx;
wire clk_bps_rx;
BaudRate_Gen BaudRate_Rx(
                    .clk(clk),
                    .reset_n(reset_n),
                    .baud_rate(baud_rate), //top
                    .baud_start(baud_start_rx),                 
                    .clk_bps(clk_bps_rx)
                    );
/*----------uart_rx instantiation----------------*/
uart_rx uart_rx(
                    .clk(clk),
                    .reset_n(reset_n),
                    .clk_bps(clk_bps_rx), //Baud rx
                    .rxd_input(rxd), //top
                    .parity_mode(parity_mode),//top
                    .rx_ready(rx_ready),
                    .rx_data(rx_data),
                    .baud_start(baud_start_rx),  //Baud rx
                    .error(rx_parity_error)
                    );
/*---------BaudRate_Gen Tx instantiation----------*/
wire baud_start_tx;
wire clk_bps_tx;
BaudRate_Gen BaudRate_Tx(
                    .clk(clk),
                    .reset_n(reset_n),
                    .baud_rate(baud_rate), //top
                    .baud_start(baud_start_tx),
                    .clk_bps(clk_bps_tx)
                    );
/*---------uart_tx instantiation-----------------*/
uart_tx uart_tx(
                     .clk(clk),
                     .reset_n(reset_n),
                     .clk_bps(clk_bps_tx),//Baud tx       
                     .tx_data(tx_data),
                     .tx_start(tx_start),
                     .parity_mode(parity_mode),   //top        
                     .baud_start(baud_start_tx),  //Baud tx
                     .txd(txd),//top      
                     .tx_ready(tx_ready)                        
                    );
                                  
endmodule
