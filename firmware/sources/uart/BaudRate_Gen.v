`timescale 1ns / 1ns
//////////////////////////////////////////////////////////////////////////////////
// Company: University of Science and Technology of China
// Engineer: Junbin Zhang
// 
// Create Date:    15:05:03 06/09/2014 
// Design Name:    BaudRate_Gen
// Module Name:    BaudRate_Gen 
// Project Name:   Target7 evaluation board V1.0 
// Target Devices: XC3S400PQ208
// Tool versions:  ISE 14.7
// Description:   When the 'baud_start' signal recieved from uart_tx or uart_rx part starts the Baud counter and Baud Rate generated
// eg:   9600bps =9600 bit/s (contain all bits,include 8-bit data ,start bit and end bit)
//         Transmit one-bit needs 1000 000/9600 us, perhaps the system clk is 50MHz,clk period is 0.02us.5208 clk cycles needed
//         So the  counter ranges 0 from 5207.
//          9600bps   =  5207
//         19200bps  =   2603
//         38400bps  =   1301
//         57600bps  =   867
//         115200bps=    433
// Revision: 
// Revision 0.01 - File Created
// Additional Comments: 
//
//////////////////////////////////////////////////////////////////////////////////
`include "AX516.def"
module BaudRate_Gen(
    input clk,  //50MHz
    input reset_n,
    input baud_start,
    input [1:0] baud_rate,
    output clk_bps
    );
    reg [12:0] r_Baud = `Baud_9600bps;
    reg [12:0] hr_Baud = `Half_Baud_9600bps;
    always @ (*) begin
       case(baud_rate)
          `Baud_9600:  begin r_Baud = `Baud_9600bps;hr_Baud = `Half_Baud_9600bps;end
          `Baud_19200: begin r_Baud = `Baud_19200bps;hr_Baud = `Half_Baud_19200bps;end
          `Baud_38400: begin r_Baud = `Baud_38400bps;hr_Baud = `Half_Baud_38400bps;end
          `Baud_115200:begin r_Baud = `Baud_115200bps;hr_Baud = `Half_Baud_115200bps;end
           default:begin r_Baud = `Baud_9600bps;hr_Baud = `Half_Baud_9600bps;end
        endcase
    end
    /*--------------------------------------*/
    reg [12:0] cnt;
    always @ (posedge clk , negedge reset_n) begin
        if(!reset_n) 
            cnt <= 13'd0;
        else if ( cnt == r_Baud || !baud_start)
            cnt <= 13'd0;
        else
            cnt <= cnt + 1'b1;
     end
     // at the middle of time ,generate a flag to uart_tx or uart_rx to perpare for transmitting or recieveing
     reg clk_bps_r;
     always @ (posedge clk) begin
        if(!reset_n)
            clk_bps_r <= 1'b0;
        else if((cnt == hr_Baud)) 
            clk_bps_r <= 1'b1;
        else
            clk_bps_r <= 1'b0;
      end
      assign clk_bps = clk_bps_r;
endmodule
