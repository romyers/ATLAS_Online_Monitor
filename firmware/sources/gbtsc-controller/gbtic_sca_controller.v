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
// File   : gbtic_sca_controller.v
// Create : 2021-06-27 22:32:48
// Revise : 2022-08-30 18:30:32
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------



module gbtic_controller
(
    input clk,
    input rst,

    input ic_ready,
    input ic_empty,
    
    //configuration
    output [7:0] GBTx_address,
    output [15:0] Register_addr,
    output [15:0] nb_to_be_read,

    //control
    output ic_wr_start,
    output ic_rd_start,

    //write to internal ic FIFO
    output [7:0] ic_wfifo_data,
    output ic_fifo_wr,

    //read from internal ic FIFO
    input [7:0] ic_rfifo_data,
    output ic_fifo_rd,

    //uart interface
    input  rxd,
    output txd,
    
    //To GBT-SC
    output reset_gbtsc,
    output start_reset,
    output start_connect,
    output start_command,
    output [2:0] sca_enable_array,
    
    // input tx_ready_i,
    output [7:0] tx_address,
    output [7:0] tx_transID,
    output [7:0] tx_channel,
    output [7:0] tx_len,
    output [7:0] tx_command,
    output [31:0] tx_data,

    input [2:0] rx_reply_received_i,
    input [23:0] rx_address,
    input [23:0] rx_transID,
    input [23:0] rx_channel,
    input [23:0] rx_len,
    input [23:0] rx_error,
    input [95:0] rx_data
);




wire command_fifo_read,command_fifo_empty,data_back_wr;
wire [159:0] command,data_back;
wire uart_tx_ready;

wire [7:0]uart_data;
wire [7:0]uart_data_sca;

wire uart_data_write;
wire uart_data_write_sca;

assign uart_data_write = ic_empty ? uart_data_write_sca : (~ic_empty);
assign uart_data = ic_empty ? uart_data_sca : ic_rfifo_data;


assign nb_to_be_read [15:8] = 'b0;
assign ic_fifo_rd = uart_tx_ready;

UART_interface UART_interface_inst
(
    .clk40                (clk),
    .reset                (rst),
    .rxd                  (rxd),
    .txd                  (txd),
    .command_fifo_rd_en   (command_fifo_read),
    .command              (command),
    .command_fifo_empty   (command_fifo_empty),

    .tx_start       (uart_data_write),
    .tx_data        (uart_data),
    .tx_ready       (uart_tx_ready)

);


sca_sendback inst_sca_sendback
(
    .clk               (clk),
    .rst               (rst),
    .rx_reply_received (rx_reply_received_i),
    .rx_address        (rx_address),
    .rx_transID        (rx_transID),
    .rx_channel        (rx_channel),
    .rx_len            (rx_len),
    .rx_error          (rx_error),
    .rx_data           (rx_data),
    .uart_tx_ready     (uart_tx_ready),
    .uart_data         (uart_data_sca),
    .uart_data_write   (uart_data_write_sca)
);



command_resolve inst_command_resolve
    (
        .clk                 (clk),
        .rst                 (rst),
        .command             (command),
        .command_fifo_empty  (command_fifo_empty),
        .command_fifo_read   (command_fifo_read),
        .data_back           (data_back),
        .data_back_wr        (data_back_wr),
        .GBTx_address        (GBTx_address),
        .Register_addr       (Register_addr),
        .nb_to_be_read       (nb_to_be_read[7:0]),
        .ic_wr_start         (ic_wr_start),
        .ic_rd_start         (ic_rd_start),
        .ic_wfifo_data       (ic_wfifo_data),
        .ic_fifo_wr          (ic_fifo_wr),

        .reset_gbtsc         (reset_gbtsc),
        .start_reset         (start_reset),
        .start_connect       (start_connect),
        .start_command       (start_command),
        .sca_enable_array    (sca_enable_array),
        
        // .tx_ready_i          (tx_ready_i),
        .tx_address          (tx_address),
        .tx_transID          (tx_transID),
        .tx_channel          (tx_channel),
        .tx_len              (tx_len),
        .tx_command          (tx_command),
        .tx_data             (tx_data),
        .rx_reply_received_i (rx_reply_received_i),
        .rx_address          (rx_address),
        .rx_transID          (rx_transID),
        .rx_channel          (rx_channel),
        .rx_len              (rx_len),
        .rx_error            (rx_error),
        .rx_data             (rx_data)
    );


// vio_gbtic_controller vio_gbtic_controller_inst (
//     .clk(clk),                // input wire clk

//     .probe_in0(ic_ready),    // input wire [0 : 0] probe_in0
//     .probe_in1(ic_empty),    // input wire [0 : 0] probe_in1
//     .probe_in2(data_i),    // input wire [7 : 0] probe_in2
//     .probe_in3(rd_gbtx_addr),    // input wire [7 : 0] probe_in3
//     .probe_in4(rd_mem_ptr),    // input wire [15 : 0] probe_in4
//     .probe_in5(rd_nb_of_words),    // input wire [15 : 0] probe_in5

//     .probe_out0(GBTx_address),  // output wire [7 : 0] probe_out0
//     .probe_out1(Register_addr),  // output wire [15 : 0] probe_out1
//     .probe_out2(nb_to_be_read),  // output wire [15 : 0] probe_out2
//     .probe_out3(write_ic_vio),  // output wire [0 : 0] probe_out3
//     .probe_out4(read_ic_vio),  // output wire [0 : 0] probe_out4
//     .probe_out5(data_o),  // output wire [7 : 0] probe_out5
//     .probe_out6(write_fifo_vio),  // output wire [0 : 0] probe_out6
//     .probe_out7(read_fifo_vio)  // output wire [0 : 0] probe_out7
// );


// ila_gbtic_controller ila_gbtic_controller_inst (
//     .clk(clk), // input wire clk

//     .probe0(rxd), // input wire [0:0]  probe0  
//     .probe1(txd), // input wire [0:0]  probe1 
//     .probe2(command_fifo_read), // input wire [0:0]  probe2 
//     .probe3(command), // input wire [159:0]  probe3 
//     .probe4(command_fifo_empty), // input wire [0:0]  probe4 
//     .probe5(GBTx_address), // input wire [7:0]  probe5 
//     .probe6(Register_addr), // input wire [15:0]  probe6 
//     .probe7(nb_to_be_read[7:0]), // input wire [7:0]  probe7 
//     .probe8(ic_wr_start), // input wire [0:0]  probe8 
//     .probe9(ic_rd_start), // input wire [0:0]  probe9 
//     .probe10(ic_wfifo_data), // input wire [7:0]  probe10 
//     .probe11(ic_fifo_wr), // input wire [0:0]  probe11 
//     .probe12(ic_rfifo_data), // input wire [7:0]  probe12 
//     .probe13(ic_fifo_rd) // input wire [0:0]  probe13
// );


endmodule
