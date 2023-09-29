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
// File   : JTAG_master.v
// Create : 2021-03-16 17:39:52
// Revise : 2021-11-15 22:08:58
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------


module JTAG_master
#(parameter DATA_BIT = 8,
            INSTR_LENGTH = 5,
            DEVICE_BIT = 5)
(
    input clk,
    input rst,
    output tck,
    output tms,
    output tdi,
    input tdo,
    input start_action_tdc,
    input start_action_asd,
    input [11:0] clk_div,
    input [255:0] JTAG_bits,
    input [7:0] bit_length,
    input [INSTR_LENGTH-1:0] JTAG_inst,
    input [4:0] device_count,
    output reg[4095:0] JTAG_readback,
    output reg JTAG_busy,
    output reg tdi_tdo_equal
);

wire tick;
reg start_action_r=1'b0;
reg start_action_syn;
reg [11:0] counter= 12'b0;
reg TCK_reg=1'b0;
reg [4:0] TAP_state=5'b00000;
reg TMS_reg=1'b1;
reg TDI_reg=1'b1;
reg [4:0] ir_reg; 
reg [7:0] bit_counter='b0;
reg [4:0] device_counter='b0;
reg [2:0] instr_counter='b0; 
reg [255:0] JTAG_bits_reg='b0;
reg TDO_valid=1'b0;
reg TDO_valid_reg=1'b0;

wire instr_shift_finished;
wire data_shift_finished;
reg config_ASD;

assign instr_shift_finished = (device_counter==device_count)&
                          (instr_counter==INSTR_LENGTH-2);
assign data_shift_finished = (device_counter==device_count)&
                          (bit_counter==bit_length-2);

assign tick = (counter == clk_div);
assign tdi = TDI_reg;
assign tck = TCK_reg;
assign tms = TMS_reg;

always @(posedge clk) begin
    start_action_r <= start_action_tdc|start_action_asd;
end 
always @(posedge clk) begin
    if ((~start_action_r)&(start_action_tdc|start_action_asd)) begin
        start_action_syn <= 1'b1;
    end else if(start_action_syn&tick&tck) begin
        start_action_syn <= 1'b0;   //sync start_action to tck
    end
end  
always @(posedge clk) begin
    if(start_action_asd)
        config_ASD <= 1'b1;
    else if(start_action_tdc)
        config_ASD <= 1'b0;
end 

always @(posedge clk) begin
    if(rst) begin
        counter <= 'b0;
    end else if(counter != clk_div) begin
        counter <= counter + 12'b1;
    end else begin
        counter <= 12'b0;  
    end
end

always @(posedge clk) begin
    if(tick)
        TCK_reg <= ~TCK_reg;
end

always @(posedge clk) begin 
    if(rst) begin
        TAP_state     <=5'b00001;
        ir_reg        <=5'h11;  //TDC IDCODE
        JTAG_busy     <= 1'b0;
        JTAG_bits_reg <= 'b0;
    end else if(tick&tck)begin
//        TMS_reg <= 1'b1;
        if(start_action_syn)begin
            TAP_state<=5'b00001;
            ir_reg <=JTAG_inst;
            JTAG_busy <= 1'b1;
            JTAG_bits_reg <= JTAG_bits;
        end else begin 
            if((TAP_state==5'b00001))begin//Reset->Idle
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end
            if((TAP_state==5'b00010))begin//Idle->Select-DR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
            end
            if((TAP_state==5'b00011))begin//Select-DR->Select-IR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
            end        
            if((TAP_state==5'b00100))begin//Select-IR->Capture-IR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end   
            if((TAP_state==5'b00101))begin//Capture-IR->Shift IR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end     
            if(TAP_state==5'b00110)begin//Shift IR->Shift IR
                if(instr_shift_finished) TAP_state <= 5'b01010;
                TMS_reg <= 1'b0;
                TDI_reg <=  ir_reg[0];
                ir_reg <= {ir_reg[0],ir_reg[INSTR_LENGTH-1:1]};
            end
            if((TAP_state==5'b01010))begin//shift IR->Exit1 IR
                TDI_reg <= ir_reg[0];
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
                ir_reg <= {ir_reg[0],ir_reg[INSTR_LENGTH-1:1]};   
            end
            if((TAP_state==5'b01011))begin//Exit1 IR->Update IR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
            end  
            if((TAP_state==5'b01100))begin//Update IR->Idle
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end     
            if((TAP_state==5'b01101))begin//Idle->Idle
                TAP_state <= 5'b10001;
                TMS_reg <= 1'b0;
            end     
            if((TAP_state==5'b10001))begin//Idle->Idle
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end
            if((TAP_state==5'b10010))begin//Idle->Select-DR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
            end
            if((TAP_state==5'b10011))begin//Select-DR->Capture-DR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end        
            if((TAP_state==5'b10100))begin
                if (config_ASD)
                    TAP_state <= 5'b11101; //Capture-DR->config_ASD_step0                     
                else
                    TAP_state <= TAP_state + 5'b1; //Capture-DR->Shift DR                   
                    TMS_reg <= 1'b0;
            end   
            if((TAP_state==5'b10101))begin//Shift DR->Shift DR
                if(data_shift_finished) TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
                TDI_reg <=  JTAG_bits_reg[bit_counter];
            end     
            if(TAP_state == 5'b10110)begin//Shift DR->Exit1 DR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
                TDI_reg <=  JTAG_bits_reg[bit_counter];
            end
            if((TAP_state==5'b10111))begin//Exit1 DR>update DR
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b1;
            end
            if((TAP_state==5'b11000))begin//update DR->Idle
                TAP_state <= TAP_state + 5'b1;
                TMS_reg <= 1'b0;
            end  
            if((TAP_state==5'b11001))begin//Idle->Select-DR
                TAP_state <= 5'b11010;
                TMS_reg <= 1'b1;
            end   
            if((TAP_state==5'b11010))begin//Select-DR->Select-IR
                TAP_state <= 5'b11011;
                TMS_reg <= 1'b1;
            end  
            if((TAP_state==5'b11011))begin//Select-IR->reset
                TAP_state <= 5'b11100;
                TMS_reg <= 1'b1;
                JTAG_busy <= 1'b0;
            end
            //1 extra bits at beginning of the chain:  1 blocked SCLK 
            if((TAP_state==5'b11101))begin//config_ASD_step0->config_ASD_step1
                TAP_state <= 5'b10101;
                TMS_reg <= 1'b0;
                TDI_reg <= 1'b0;
            end  
        end  
    end   
end

always @(posedge clk) begin 
    if(rst) begin
        instr_counter  <= 'b0;
        device_counter <= 'b0;
        bit_counter    <= 'b0;
    end else if(tick&tck)begin
        if((TAP_state==5'b00101))begin//Capture-IR->Shift IR
            instr_counter <= 'b0;
            device_counter<= 'b1;
        end     
        if(TAP_state==5'b00110)begin//Shift IR->Shift IR
            instr_counter <= instr_counter==(INSTR_LENGTH-1)?'b0:instr_counter+1'b1;
            device_counter<= instr_counter==(INSTR_LENGTH-1)?device_counter+1'b1:device_counter;
        end
        if((TAP_state==5'b01010))begin//shift IR->Exit1 IR
            instr_counter <= 'b0;  
            device_counter<= 'b0;
        end
        if((TAP_state==5'b10100))begin//Capture-DR->Shift DR
            bit_counter<=8'b0;
            device_counter<= 'b1;
        end   
        if((TAP_state==5'b10101))begin//Shift DR->Shift DR
            bit_counter <= bit_counter==(bit_length-1'b1)?'b0:bit_counter+1'b1;
            device_counter<= bit_counter==(bit_length-1'b1)?device_counter+1'b1:device_counter;
        end     
        if(TAP_state == 5'b10110)begin//Shift DR->Exit1 DR
            bit_counter<=8'b0;
        end
    end
end    

always @(posedge clk) begin
    if(rst) begin
        TDO_valid <= 1'b0;
    end else if(tick&~tck)begin
        TDO_valid <= (TAP_state == 5'b10101)|(TAP_state == 5'b10110)|
                     (TAP_state == 5'b11101);
    end
end

// delay TDO_valid one cycle for JTAG readback comparison
always @(posedge clk) begin
    if(rst) begin
        TDO_valid_reg <= 1'b0;
    end else if(tick&~tck)begin
        TDO_valid_reg <= TDO_valid;
    end
end

always @(posedge clk) begin
    if(rst) begin
        JTAG_readback <= 'b0;
    end else if(tick&~tck)begin
        if (TAP_state == 5'b10100)
            JTAG_readback <= 'b0;
        if (TDO_valid) 
            JTAG_readback <= {tdo,JTAG_readback[4095:1]};        
    end
end  


always @(posedge clk) begin
    if(rst) begin
        tdi_tdo_equal <= 1'b0;       
    end else if(tick&tck)begin
        if (TAP_state == 5'b10100)
            tdi_tdo_equal <= 1'b1;
        if (TDO_valid_reg & (tdo!=tdi))
            tdi_tdo_equal <= 1'b0;       
    end
end    


ila_jtag_master ila_jtag_master_inst (
    .clk(clk), // input wire clk

    .probe0(tck), // input wire [0:0]  probe0  
    .probe1(tms), // input wire [0:0]  probe1 
    .probe2(tdi), // input wire [0:0]  probe2 
    .probe3(tdo), // input wire [0:0]  probe3 
    .probe4(start_action_syn), // input wire [0:0]  probe4 
    .probe5(JTAG_bits), // input wire [255:0]  probe5 
    .probe6(bit_length), // input wire [7:0]  probe6 
    .probe7(JTAG_inst), // input wire [4:0]  probe7 
    .probe8(device_count), // input wire [4:0]  probe8 
    .probe9(JTAG_readback[4095:3840]), // input wire [255:0]  probe9 
    .probe10(JTAG_busy), // input wire [0:0]  probe10 
    .probe11(tick), // input wire [0:0]  probe11 
    .probe12(TAP_state), // input wire [4:0]  probe12 
    .probe13(bit_counter), // input wire [7:0]  probe13 
    .probe14(device_counter), // input wire [4:0]  probe14 
    .probe15(instr_counter), // input wire [2:0]  probe15 
    .probe16(JTAG_bits_reg), // input wire [255:0]  probe16 
    .probe17(TDO_valid), // input wire [0:0]  probe17 
    .probe18(instr_shift_finished), // input wire [0:0]  probe18 
    .probe19(data_shift_finished), // input wire [0:0]  probe19
    .probe20(tdi_tdo_equal) // input wire [0:0]  probe20
    
);

   
endmodule
