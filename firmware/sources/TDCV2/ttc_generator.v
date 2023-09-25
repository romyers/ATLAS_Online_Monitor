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
// File   : ttc_generator.v
// Create : 2021-03-16 10:14:49
// Revise : 2021-03-28 15:31:53
// Editor : sublime text3, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------

module ttc_generator
(
    input clk_40,
    input rst_40,
    output [1:0] encode_ttc,
    output fpga_bcr, //width synchoronized to one clk_40

    input start_single,
    input start_continuous,
    input trigger_en,
    input bc_reset_en,
    input event_reset_en,
    input master_reset_en,    
    input [11:0] interval
);



//wire start_single;
wire  start_syn;
reg [1:0] start_r;
reg rst_40_reg;

reg  [11:0] interval_counter = 12'b0;
reg [1:0] ttc_busy_counter;
wire ttc_busy;
reg [5:0] legacy_ttc;

assign ttc_busy = |ttc_busy_counter;
assign start_pulse = start_single | (start_continuous&(interval_counter == interval))|
                     ((!rst_40)&rst_40_reg);  //A TTC is generated at the falling edge of rst_40
// blocking ttc input for 3 cycles until the last ttc is encoded
assign start_syn = (start_r[0])&(~start_r[1])&(~ttc_busy);
assign fpga_bcr = start_syn & bc_reset_en;
assign encode_ttc = legacy_ttc[5:4];

always @(posedge clk_40) begin
    if (rst_40) begin
        // reset
        interval_counter <= 12'b0;
    end else begin
        interval_counter <= (interval_counter == interval) ? 12'b0 : interval_counter +12'b1;
    end
end

always @(posedge clk_40) begin
    start_r <= {start_r[0],start_pulse};
end

always @(posedge clk_40) begin
    rst_40_reg <= rst_40;
end

always @(posedge clk_40) begin
    if(rst_40) 
        ttc_busy_counter <= 2'b0;
    if(start_syn) begin
        ttc_busy_counter <= 2'b11;
    end else if(|ttc_busy_counter) begin 
        ttc_busy_counter <= ttc_busy_counter - 2'b1;
    end
end

always @(posedge clk_40) begin
    if (rst_40) begin
        // reset
        legacy_ttc <= 6'b000000;
    end else if (start_syn) begin
        if(bc_reset_en)begin
            legacy_ttc <= 6'b111100; 
        end else if(trigger_en) begin
            legacy_ttc <= 6'b110000; 
        end else if(event_reset_en)  begin
            legacy_ttc <= 6'b111111; 
        end else if(master_reset_en) begin
            legacy_ttc <= 6'b110011; 
        end else begin
            legacy_ttc <= 6'b000000; 
        end         
    end else begin
        legacy_ttc <= {legacy_ttc[3:0],2'b00};
    end
end

// wire encode_ttc_legacy;
// wire encode_ttc_new;
//wire new_ttc_mode_VIO;
// reg encocde_ttc_r;
// always @(posedge clk_40) begin
//     encocde_ttc_r <= new_ttc_mode ? encode_ttc_new : encode_ttc_legacy;
// end

// reg encocde_ttc_r1;
// always @(posedge clk_320) begin
//     encocde_ttc_r1 <= encocde_ttc_r;
// end
// assign encode_ttc = delay ? encocde_ttc_r1 : encocde_ttc_r;

endmodule