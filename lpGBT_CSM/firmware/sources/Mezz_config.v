`include "definitions.sv"
`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
// Company: 
// Engineer: 
// 
// Create Date: 12/01/2017 05:26:35 PM
// Design Name: 
// Module Name: JTAG_master
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


module Mezz_config
#(parameter DATA_BIT = 8)
(
    input clk,
    input rst,
    input JTAG_busy,
    input tdi_tdo_equal,
    input TDC_config,
    input ASD_config,
    input enable_vio_config,
    input start_action_status0,
    input start_action_status1,

    input config_twice,
    

    output start_action_tdc,
    output start_action_asd,
    output reg tdc_config_done,
    output reg asd_config_done,
    output reg [5:0] chain_success,
    output reg [4:0] JTAG_instr_mezz,
    output reg [7:0] JTAG_data_length_mezz,
    output reg [255:0] JTAG_bits_mezz   
);


localparam IDLE              = 4'd1;
localparam CONFIG_SETUP0_0   = 4'd2;
localparam CONFIG_SETUP0_1   = 4'd3;
localparam CONFIG_SETUP1_0   = 4'd4;
localparam CONFIG_SETUP1_1   = 4'd5;
localparam CONFIG_SETUP2_0   = 4'd6;
localparam CONFIG_SETUP2_1   = 4'd7;
localparam CONFIG_CONTROL0_0 = 4'd8;
localparam CONFIG_CONTROL0_1 = 4'd9;
localparam CONFIG_CONTROL1_0 = 4'd10;
localparam CONFIG_CONTROL1_1 = 4'd11;
localparam CONFIG_ASD_W      = 4'd12;
localparam CONFIG_ASD_R      = 4'd13;

reg [3:0] state = IDLE;
reg [3:0] next = IDLE;
reg wait_for_JTAG_busy;



wire [`SETUP0_LENGTH-1:0]   setup0,   setup0_vio,   setup0_default; 
wire [`SETUP1_LENGTH-1:0]   setup1,   setup1_vio,   setup1_default;   
wire [`SETUP2_LENGTH-1:0]   setup2,   setup2_vio,   setup2_default;   
wire [`CONTROL0_LENGTH-1:0] control0, control0_vio, control0_default;     
wire [`CONTROL1_LENGTH-1:0] control1, control1_vio, control1_default;     
wire [`ASDWRITE_LENGTH-1:0] ASD_setup,ASD_setup_vio,ASD_setup_default;      
assign setup0    = enable_vio_config ? setup0_vio    : setup0_default;  
assign setup1    = enable_vio_config ? setup1_vio    : setup1_default;   
assign setup2    = enable_vio_config ? setup2_vio    : setup2_default;   
assign control0  = enable_vio_config ? control0_vio  : control0_default; 
assign control1  = enable_vio_config ? control1_vio  : control1_default; 
assign ASD_setup = enable_vio_config ? ASD_setup_vio : ASD_setup_default;

wire start_action_asd_write, start_action_asd_read, start_action_setup0, 
start_action_setup1, start_action_setup2, start_action_control0, 
start_action_control1;
wire start_action_asd_write_syn, start_action_asd_read_syn, start_action_setup0_syn,
start_action_setup1_syn, start_action_setup2_syn, start_action_control0_syn, 
start_action_control1_syn, start_action_status0_syn, start_action_status1_syn;
reg start_action_asd_write_reg, start_action_asd_read_reg, 
start_action_setup0_reg, start_action_setup1_reg, 
start_action_setup2_reg, start_action_control0_reg, 
start_action_control1_reg, start_action_status0_reg,
start_action_status1_reg;
reg TDC_config_reg, ASD_config_reg;
wire TDC_config_syn, ASD_config_syn;
always @(posedge clk) begin
    start_action_asd_write_reg <= start_action_asd_write;
    start_action_asd_read_reg  <= start_action_asd_read;
    start_action_setup0_reg    <= start_action_setup0;
    start_action_setup1_reg    <= start_action_setup1;
    start_action_setup2_reg    <= start_action_setup2;
    start_action_control0_reg  <= start_action_control0;
    start_action_control1_reg  <= start_action_control1;
    start_action_status0_reg  <= start_action_status0;
    start_action_status1_reg  <= start_action_status1;
    TDC_config_reg             <= TDC_config; 
    ASD_config_reg             <= ASD_config; 
end

assign TDC_config_syn             = (~TDC_config_reg & TDC_config);
assign ASD_config_syn             = (~ASD_config_reg & ASD_config);

assign start_action_asd_write_syn = ((next==CONFIG_ASD_W   )&(next!=state))|(~start_action_asd_write_reg & start_action_asd_write);
assign start_action_asd_read_syn  = ((next==CONFIG_ASD_R   )&(next!=state))|(~start_action_asd_read_reg  & start_action_asd_read);
assign start_action_setup0_syn    = (((next==CONFIG_SETUP0_0  )|(next==CONFIG_SETUP0_1  ))&(next!=state))|(~start_action_setup0_reg    & start_action_setup0);
assign start_action_setup1_syn    = (((next==CONFIG_SETUP1_0  )|(next==CONFIG_SETUP1_1  ))&(next!=state))|(~start_action_setup1_reg    & start_action_setup1);
assign start_action_setup2_syn    = (((next==CONFIG_SETUP2_0  )|(next==CONFIG_SETUP2_1  ))&(next!=state))|(~start_action_setup2_reg    & start_action_setup2);
assign start_action_control0_syn  = (((next==CONFIG_CONTROL0_0)|(next==CONFIG_CONTROL0_1))&(next!=state))|(~start_action_control0_reg  & start_action_control0);
assign start_action_control1_syn  = (((next==CONFIG_CONTROL1_0)|(next==CONFIG_CONTROL1_1))&(next!=state))|(~start_action_control1_reg  & start_action_control1);
assign start_action_status0_syn  = (~start_action_status0_reg  & start_action_status0);
assign start_action_status1_syn  = (~start_action_status1_reg  & start_action_status1);



mezz_config_const inst_mezz_config_const(
    .setup0    (setup0_default   ),
    .setup1    (setup1_default   ),
    .setup2    (setup2_default   ),
    .control0  (control0_default ),
    .control1  (control1_default ),
    .ASD_setup (ASD_setup_default)
);

mezz_config_vio inst_mezz_config_vio(
    .clk                    (clk),
    .setup0                 (setup0_vio   ),
    .setup1                 (setup1_vio   ),
    .setup2                 (setup2_vio   ),
    .control0               (control0_vio ),
    .control1               (control1_vio ),
    .ASD_setup              (ASD_setup_vio),
    .start_action_setup0    (start_action_setup0),
    .start_action_setup1    (start_action_setup1),
    .start_action_setup2    (start_action_setup2),
    .start_action_control0  (start_action_control0),
    .start_action_control1  (start_action_control1),
    .start_action_asd_write (start_action_asd_write),
    .start_action_asd_read  (start_action_asd_read)
);

always @(posedge clk  ) begin
    if (rst) begin
        state <= IDLE;
    end else begin
        state <= next;
    end
end


//when "state" changes, wait for JTAG_busy asserted to further change "next"
always @(posedge clk  ) begin
    if (rst) begin
        wait_for_JTAG_busy <= 1'b0;
    end else if (state!=next) begin
        wait_for_JTAG_busy <= 1'b1;
    end else if (JTAG_busy) begin
        wait_for_JTAG_busy <= 1'b0;
    end
end

always @(posedge clk) begin
    if (rst) begin 
        next <= IDLE;
    end else if ((~JTAG_busy)&(state==next)) begin
        if (state == IDLE) begin 
            next <= ASD_config_syn ? CONFIG_ASD_W:
                   TDC_config_syn ? (config_twice?CONFIG_SETUP0_0:CONFIG_SETUP0_1):IDLE;
        end else if (~wait_for_JTAG_busy) begin
            if (state == CONFIG_ASD_R|state == CONFIG_CONTROL1_1)
                next <= IDLE;
            else if (state == CONFIG_ASD_W)
                next <= CONFIG_ASD_R;
            else
                next <= config_twice? (state+1'b1):(state+2'b10);
        end          
    end  
end


always @(posedge clk) begin
    if (rst) begin 
        tdc_config_done <= 1'b0;
        asd_config_done <= 1'b0;
    end else if ((state==CONFIG_CONTROL1_1)&(state!=next)) begin
        tdc_config_done <= 1'b1;
    end else if ((state==CONFIG_ASD_R)&(state!=next)) begin
        asd_config_done <= 1'b1;
    end else begin
        tdc_config_done <= 1'b0;
        asd_config_done <= 1'b0;
    end
end



always @(posedge clk) begin
    if(rst)
        chain_success <= 'b0;
    else begin
        chain_success[5] <= (state==CONFIG_SETUP0_1)  &(state!=next)?tdi_tdo_equal:chain_success[5];
        chain_success[4] <= (state==CONFIG_SETUP1_1)  &(state!=next)?tdi_tdo_equal:chain_success[4];
        chain_success[3] <= (state==CONFIG_SETUP2_1)  &(state!=next)?tdi_tdo_equal:chain_success[3];
        chain_success[2] <= (state==CONFIG_CONTROL0_1)&(state!=next)?tdi_tdo_equal:chain_success[2];
        chain_success[1] <= (state==CONFIG_CONTROL1_1)&(state!=next)?tdi_tdo_equal:chain_success[1];
        chain_success[0] <= (state==CONFIG_ASD_R)     &(state!=next)?tdi_tdo_equal:chain_success[0];
    end
end
  
always @(posedge clk) begin
  if(rst) begin
    JTAG_instr_mezz       <= `IDCODE;
    JTAG_bits_mezz        <= 'b0; 
    JTAG_data_length_mezz <= `IDCODE_LENGTH;
  end else if(start_action_asd_write_syn) begin
    JTAG_instr_mezz <= `ASDWRITE;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`ASDWRITE_LENGTH){1'b0}}, ASD_setup,1'b0}; 
    JTAG_data_length_mezz <= `ASDWRITE_LENGTH;  
    //one extra bit for the known extra ASD TDO reg 
  end else if(start_action_asd_read_syn) begin
    JTAG_instr_mezz <= `ASDREAD;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`ASDREAD_LENGTH){1'b0}}, ASD_setup,1'b0};
    JTAG_data_length_mezz <= `ASDREAD_LENGTH;  
  end else if(start_action_setup0_syn) begin 
    JTAG_instr_mezz <= `SETUP0;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`SETUP0_LENGTH){1'b0}}, setup0};
    JTAG_data_length_mezz <= `SETUP0_LENGTH;  
  end else if(start_action_setup1_syn) begin
    JTAG_instr_mezz <= `SETUP1;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`SETUP1_LENGTH){1'b0}}, setup1};
    JTAG_data_length_mezz <= `SETUP1_LENGTH;
  end else if(start_action_setup2_syn) begin
    JTAG_instr_mezz <= `SETUP2;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`SETUP2_LENGTH){1'b0}}, setup2};
    JTAG_data_length_mezz <= `SETUP2_LENGTH;
  end else if(start_action_control0_syn) begin
    JTAG_instr_mezz <= `CONTROL0;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`CONTROL0_LENGTH){1'b0}}, control0};
    JTAG_data_length_mezz <= `CONTROL0_LENGTH;
  end else if(start_action_control1_syn) begin
    JTAG_instr_mezz <= `CONTROL1;
    JTAG_bits_mezz <= {{(2**DATA_BIT-`CONTROL1_LENGTH){1'b0}}, control1};
    JTAG_data_length_mezz <= `CONTROL1_LENGTH;
  end else if(start_action_status0_syn) begin
    JTAG_instr_mezz <= `STATUS0;
    JTAG_bits_mezz <= 'b0;
    JTAG_data_length_mezz <= `STATUS0_LENGTH;
  end else if(start_action_status1_syn) begin
    JTAG_instr_mezz <= `STATUS1;
    JTAG_bits_mezz <= 'b0;
    JTAG_data_length_mezz <= `STATUS1_LENGTH;
  end
end

assign start_action_tdc = start_action_setup0_syn|start_action_setup1_syn|
                          start_action_setup2_syn|start_action_control0_syn|
                          start_action_control1_syn|start_action_status0_syn|
                          start_action_status1_syn;
assign start_action_asd = start_action_asd_write_syn|start_action_asd_read_syn;



ila_config_mezz ila_config_mezz_inst (
    .clk(clk), // input wire clk

    .probe0(JTAG_busy), // input wire [0:0]  probe0  
    .probe1(wait_for_JTAG_busy), // input wire [0:0]  probe1 
    .probe2(tdi_tdo_equal), // input wire [0:0]  probe2 
    .probe3(TDC_config), // input wire [0:0]  probe3 
    .probe4(ASD_config), // input wire [0:0]  probe4 
    .probe5(start_action_tdc), // input wire [0:0]  probe5 
    .probe6(start_action_asd), // input wire [0:0]  probe6 
    .probe7(chain_success), // input wire [5:0]  probe7 
    .probe8(JTAG_instr_mezz), // input wire [4:0]  probe8 
    .probe9(state), // input wire [3:0]  probe9 
    .probe10(next) // input wire [3:0]  probe10
);



endmodule

