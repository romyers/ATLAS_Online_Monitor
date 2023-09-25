`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: University of Science and Technology of China 
// Engineer: Junbin Zhang
// 
// Create Date:    16:40:26 06/09/2014 
// Design Name:    uart_tx
// Module Name:    uart_tx 
// Project Name:   
// Target Devices: XC3S400PQ208
// Tool versions:  ISE 14.7
// Description:       the transmition part of uart 
//
// Dependencies: 
//
// Revision: Update 20150708 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
`include "AX516.def"
module uart_tx(
    input clk,                       //global clk domain 50MHz
    input reset_n,                   //reset
    input clk_bps,               //BaudRate Gen sampling flag    
    input [7:0] tx_data,             //data for transmitting
    input tx_start,                  //start transmit
    input [1:0] parity_mode,         //EVEN,ODD or NONE parity
    output baud_start,               //enable BaudRate Gen
    output txd,                      //serial data out
    output tx_ready                    //when transmition is complete, tx_int = 1
    );
    reg tx_start_sync;
    always @ (posedge clk) begin
      if(~reset_n)
        tx_start_sync <= 1'b0;
      else
        tx_start_sync <= tx_start;
    end
    wire tx_rise;
    assign tx_rise = ~tx_start_sync & tx_start;
    //start tx progress control
    reg baud_start_r;
    reg [7:0] data;
    reg send_en;
    reg [3:0]num;
    reg tx_busy;
    always @ (posedge clk) begin
      if(~reset_n) begin
        baud_start_r <= 1'b0;
        data <= 8'b0;
        send_en <= 1'b0;
      end
      else if(tx_rise && !tx_busy) begin //valid only once
        baud_start_r <= 1'b1;
        data <= tx_data;
        send_en <= 1'b1;
      end
      else if(num == 4'd11) begin
        baud_start_r <= 1'b0;
        send_en <= 1'b0;
      end
      else begin
        baud_start_r <= baud_start_r;
        data <= data;
        send_en <= send_en;
      end
    end
    assign baud_start = baud_start_r;
    //start tx progress
    reg r_txd=1'b1;
    reg tx_ready_r;
    localparam [1:0] Idle = 2'b00,
                    TX_START = 2'b01,
                    TX_DONE  = 2'b10;
    reg [1:0] State;
    always @ (posedge clk) begin
      if(~reset_n) begin
        num <= 4'd0;
        r_txd <= 1'b1;
        tx_busy <= 1'b0;
        tx_ready_r <= 1'b0;
        State <= Idle;
      end
      else begin
        case(State)
          Idle:begin
            tx_ready_r <= 1'b0;
            tx_busy <= 1'b0;
            if(send_en) begin
              State <= TX_START;
              tx_busy <= 1'b1;
            end
            else 
              State <= Idle;              
          end
          TX_START:begin
            if(clk_bps)begin
              num <= num + 1'b1;
              case(num)
                4'd0 : r_txd <= 1'b0;   //start bit '0'
                4'd1 : r_txd <= data[0]; //LSB first
                4'd2 : r_txd <= data[1]; 
                4'd3 : r_txd <= data[2]; 
                4'd4 : r_txd <= data[3]; 
                4'd5 : r_txd <= data[4]; 
                4'd6 : r_txd <= data[5]; 
                4'd7 : r_txd <= data[6]; 
                4'd8 : r_txd <= data[7]; //MSB
                4'd9 : case (parity_mode) 
                        `PARITY_EVEN : r_txd <= ^ data;    //even parity
                        `PARITY_ODD  : r_txd <= ~^ data;     //odd parity
                        //`PARITY_NONE : r_txd <= 1'b1;       //none parity
                         default: r_txd <= 1'b1;
                       endcase
                4'd10: r_txd <= 1'b1;     //stop bit '1'                
                default : r_txd <= 1'b1;
              endcase
            end
            else if(num == 4'd11)begin
                num <= 4'd0;
                tx_ready_r <= 1'b1;
                tx_busy <= 1'b0;
                State <= Idle;
            end
            else begin
                num <= num;
                State <= TX_START;
            end
          end
        endcase
      end
    end
    assign txd = r_txd;
    assign tx_ready = tx_ready_r;
endmodule
