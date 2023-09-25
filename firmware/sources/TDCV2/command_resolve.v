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
// File   : command_resolve.v
// Create : 2021-06-24 00:57:05
// Revise : 2023-05-04 00:54:37
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
// command[159]=1: lpGBT multi-byte write
// command[79]=1: SCA write
// command[41]=1: lpGBT single-byte write
// command[40]=1: lpGBT read

// lpGBT multi-byte write
// [7:0] GBTx_address,[15:0] Register_addr,[7:0] value,[7:0] nb_to_be_read(value_length)

// SCA write command format
// command[74:72]: 
// reset_gbtsc, start_reset, start_connect
// command[71:0]:
// [7:0] tx_address,[7:0] tx_transID,[7:0] tx_channel,[7:0] tx_len,[7:0] tx_command,[31:0] tx_data
// 
//
  
module command_resolve(
    input clk,
    input rst,

    input [159:0] command,//format from left to right:[7:0] GBTx_address,[15:0] Register_addr,value,[7:0] nb_to_be_read(value_length)
    
    input command_fifo_empty,
    output reg command_fifo_read,
    output reg [159:0] data_back,
    output reg data_back_wr,

    output reg [7:0] GBTx_address,
    output reg [15:0] Register_addr,
    output reg [7:0] nb_to_be_read,

    //control
    output reg ic_wr_start,
    output reg ic_rd_start,

    //write to internal ic FIFO
    output reg [7:0] ic_wfifo_data,
    output reg ic_fifo_wr,

    //To GBT-SC
    output reg reset_gbtsc,
    output reg start_reset,
    output reg start_connect,
    output reg start_command,
    output reg [2:0] sca_enable_array,
    
    // input tx_ready_i,
    output reg [7:0] tx_address,
    output reg [7:0] tx_transID,
    output reg [7:0] tx_channel,
    output reg [7:0] tx_len,
    output reg [7:0] tx_command,
    output reg [31:0] tx_data,

    input [2:0] rx_reply_received_i,
    input [23:0] rx_address,
    input [23:0] rx_transID,
    input [23:0] rx_channel,
    input [23:0] rx_len,
    input [23:0] rx_error,
    input [95:0] rx_data

);
    


reg [5:0]   state;
reg [5:0]   nextstate;
reg [159:0] command_r;
reg [4:0]   data_back_cnt;

reg [7:0]   value_length;
reg [7:0] multi_counter;

wire [7:0] fifo_write_data;
wire [7:0] GBTx_address_data;
wire [15:0] Register_addr_data;



localparam IDLE               =6'b000001;
localparam RESOLVE_COMMAND    =6'b000010;
localparam LPGBT_MULTI_WRITE  =6'b000100;
localparam LPGBT_SINGLE_WRITE =6'b001000;
localparam LPGBT_SEND         =6'b010000;
localparam GBTSCA_COMMAND     =6'b100000;
// localparam LPGBT_READBACK     =6'b100000;


localparam IDLE_BIT               =0;
localparam RESOLVE_COMMAND_BIT    =1;
localparam LPGBT_MULTI_WRITE_BIT  =2;
localparam LPGBT_SINGLE_WRITE_BIT =3;
localparam LPGBT_SEND_BIT         =4;
localparam GBTSCA_COMMAND_BIT     =5;
// localparam LPGBT_READBACK_BIT     =5;


always @(posedge clk  ) begin
    if (rst) begin
        // reset
        state <= IDLE;
    end else begin
        state <= nextstate;
    end
end

always @(posedge clk  ) begin
    if (rst) begin
        // reset
        multi_counter <= 'b0;
    end else if(state == LPGBT_MULTI_WRITE) begin
    multi_counter <= (~|multi_counter) ? 8'b0000_0000:( multi_counter -8'b0000_0001);
        
    end else if(nextstate==LPGBT_MULTI_WRITE) begin
       multi_counter <= value_length-1'b1; 
    end else begin
    
        multi_counter <= 'b0;
    end
end

always @(*) begin
    case(state)
        IDLE: 
            nextstate = (~command_fifo_empty) ? RESOLVE_COMMAND : IDLE;
        RESOLVE_COMMAND:
           // nextstate = command_r[24+{value_length,3'b0}+8+2] ? LPGBT_MULTI_WRITE : //LPGBT_MULTI bytes write config
            nextstate = command_r[159] ? LPGBT_MULTI_WRITE :
                        command_r[79]  ? GBTSCA_COMMAND :
                        command_r[41]  ? LPGBT_MULTI_WRITE :
                        command_r[40]  ? LPGBT_SEND : //LPGBT_MULTI bytes read config
                        IDLE;
        LPGBT_MULTI_WRITE:
            nextstate = (~|multi_counter) ? LPGBT_SEND : LPGBT_MULTI_WRITE;
        LPGBT_SEND:
            nextstate = IDLE;
        GBTSCA_COMMAND:
            nextstate = IDLE;
        default: nextstate = IDLE;
    endcase
end

always @(posedge clk) begin
    if (rst) begin
        // reset
        command_fifo_read <= 1'b0;
        command_r         <= 'b0;
    end else if (nextstate[RESOLVE_COMMAND_BIT]) begin
        command_fifo_read <= 1'b1;
        command_r         <= command;
        value_length      <= command[7:0];//get length
    end else begin
        command_fifo_read <= 1'b0;
    end
end

genvar i;
for (i=0;i<8;i=i+1) begin
    assign fifo_write_data[i] = command_r[8+{multi_counter,3'b000}+i];   
end

genvar j,k;
for (j=0;j<8;j=j+1) begin
    assign GBTx_address_data[j] = command_r[8+16+{value_length,3'b000}+j];   
end
for (k=0;k<16;k=k+1) begin
    assign Register_addr_data[k] = command_r[8+{value_length,3'b000}+k];   
end


// state machine for lpgbt config (read, single-byte write and multi-byte write)
always @(posedge clk) begin
        GBTx_address  <= 'b0;
        Register_addr <= 'b0;
        ic_wfifo_data <= 'b0;
        nb_to_be_read <= 'b0;
        ic_fifo_wr    <= 'b0;        
        ic_rd_start   <= 'b0;
        ic_wr_start   <= 'b0;
        start_command <= 'b0;
        reset_gbtsc   <= 'b0;
        start_reset   <= 'b0;
        start_connect <= 'b0;
    if (state[LPGBT_MULTI_WRITE_BIT]) begin 
        ic_wfifo_data <= fifo_write_data;              
        ic_fifo_wr    <= 1'b1;             
    end else if (state[LPGBT_SEND_BIT]) begin
       GBTx_address   <= GBTx_address_data;
        Register_addr <= Register_addr_data;
        nb_to_be_read <= command_r[7:0];//value_length
        ic_wr_start   <= command_r[159]|((~command_r[159])&&command_r[41]);
        ic_rd_start   <= (~command_r[159])&&command_r[40];       
    end else if (state[GBTSCA_COMMAND_BIT]) begin
        if (command_r[74]) begin
            reset_gbtsc <= 'b1;
        end else if (command_r[73]) begin
            start_reset <= 'b1;
        end else if (command_r[72]) begin
            start_connect <= 'b1;
        end else begin
            sca_enable_array <= command_r[78:76];
            tx_address       <= command_r[71:64];
            tx_transID       <= command_r[63:56];
            tx_channel       <= command_r[55:48];
            tx_len           <= command_r[47:40];
            tx_command       <= command_r[39:32];
            tx_data          <= command_r[31:0];
            start_command    <= 'b1;
        end
    end
end

// // state machine for lpgbt readback to UART
// always @(posedge clk) begin
//     if (rst) begin
//         // reset
//         data_back_cnt <= 'b0;
//         ic_fifo_rd    <= 'b0;
//         data_back_wr  <= 'b0;
//         data_back     <= 'b0;

//     end else if (state[LPGBT_READBACK_BIT]) begin
//         data_back_cnt <= (data_back_cnt==5'd19) ? 5'b0 : data_back_cnt+1'b1;
//         ic_fifo_rd    <= 'b1;
//         data_back     <= {data_back[]}

//     end else begin

//     end
// end



// ila_gbtsc_controller ila_gbtsc_controller_inst (
//     .clk(clk), // input wire clk

//     .probe0(reset_gbtsc), // input wire [0:0]  probe0  
//     .probe1(start_reset), // input wire [0:0]  probe1 
//     .probe2(start_connect), // input wire [0:0]  probe2 
//     .probe3(start_command), // input wire [0:0]  probe3 
//     .probe4(state), // input wire [5:0]  probe4 
//     .probe5(nextstate), // input wire [5:0]  probe5 
//     .probe6(command_r), // input wire [159:0]  probe6 
//     .probe7(rx_reply_received_i), // input wire [0:0]  probe7 
//     .probe8(rx_address), // input wire [7:0]  probe8 
//     .probe9(rx_transID), // input wire [7:0]  probe9 
//     .probe10(rx_channel), // input wire [7:0]  probe10 
//     .probe11(rx_len), // input wire [7:0]  probe11 
//     .probe12(rx_error), // input wire [7:0]  probe12 
//     .probe13(rx_data) // input wire [31:0]  probe13
// );
endmodule
