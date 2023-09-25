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
// File   : sca_sendback_single.v
// Create : 2022-08-30 16:02:49
// Revise : 2023-05-04 00:53:13
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
// sca_busy is set to 1 when rx_reply_received until current data sent out via uart.
  
module sca_sendback_single(
    input clk,
    input rst,
    input send_enable,

    input rx_reply_received,
    input [7:0] rx_address,
    input [7:0] rx_transID,
    input [7:0] rx_channel,
    input [7:0] rx_len,
    input [7:0] rx_error,
    input [31:0] rx_data,

    input uart_tx_ready,
    output reg [7:0] uart_data,
    output reg uart_data_write,
    output reg sca_busy

);
    
localparam IDLE        =3'd1;
localparam GET_DATA    =3'd2;
localparam WAIT_ENABLE =3'd3;
localparam SEND_PACKET =3'd4;
localparam WAIT_ACK    =3'd5;

reg [3:0]  state;
reg [3:0]  nextstate;
reg [71:0] sca_packet;
reg [3:0] data_counter;

always @(posedge clk  ) begin
    if (rst) begin
        // reset
        state <= IDLE;
    end else begin
        state <= nextstate;
    end
end

always @(*) begin
    case(state)
        IDLE: 
            nextstate = (rx_reply_received) ? GET_DATA : IDLE;
        GET_DATA:
            nextstate = WAIT_ENABLE;
        WAIT_ENABLE:
            nextstate = send_enable ? SEND_PACKET: WAIT_ENABLE;
        SEND_PACKET:
            nextstate = (|data_counter) ? WAIT_ACK : IDLE;
        WAIT_ACK:
            nextstate =  uart_tx_ready ? SEND_PACKET : WAIT_ACK;
        default: nextstate = IDLE;
    endcase
end

// state machine for uart tx send
always @(posedge clk) begin
    uart_data       <= 'b0;
    uart_data_write <= 'b0;
    sca_busy        <= 'b1;
    case(state)
        IDLE: begin
            data_counter <= 'b0;
            sca_busy     <= 'b0;  //sca_busy reset to 0 only in IDLE
              end
        GET_DATA: begin
            data_counter <= 4'd9; 
            sca_packet   <= {rx_address,rx_transID,rx_channel,
                            rx_error,rx_len,rx_data};
                end
        WAIT_ENABLE: begin end
        SEND_PACKET: begin
            if (|data_counter) begin
                data_counter    <= data_counter-1'b1;
                uart_data       <= sca_packet[71:64];
                uart_data_write <= 1'b1;
                sca_packet      <= {sca_packet[63:0],8'b0};
            end
          end
        WAIT_ACK: begin end
        default: begin end
    endcase
end

// ila_sca_sendback inst_ila_sca_sendback (
//     .clk(clk), // input wire clk


//     .probe0(state), // input wire [3:0]  probe0  
//     .probe1(nextstate), // input wire [3:0]  probe1 
//     .probe2(sca_packet), // input wire [71:0]  probe2 
//     .probe3(data_counter), // input wire [3:0]  probe3 
//     .probe4(send_enable), // input wire [0:0]  probe4 
//     .probe5(rx_reply_received), // input wire [0:0]  probe5 
//     .probe6(uart_tx_ready), // input wire [0:0]  probe6 
//     .probe7(uart_data), // input wire [7:0]  probe7 
//     .probe8(uart_data_write), // input wire [0:0]  probe8 
//     .probe9(sca_busy) // input wire [0:0]  probe9
// );


endmodule