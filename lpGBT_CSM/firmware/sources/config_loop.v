module config_loop(
    input clk,
    input rst,
    input tdc_config_done,
    input asd_config_done,
    input [5:0] chain_success,
    output reg TDC_loop_start,
    output reg ASD_loop_start,
    output enable_loop_TRST,
    output reg loop_TRST
);

reg [31:0] tdc_config_counter;
reg [31:0] asd_config_counter;
reg [31:0] setup0_fail_counter;
reg [31:0] setup1_fail_counter;
reg [31:0] setup2_fail_counter;
reg [31:0] control0_fail_counter;
reg [31:0] control1_fail_counter;
reg [31:0] asd_fail_counter;
wire loop_start;
wire loop_TDC;
wire loop_ASD;


localparam IDLE = 0;
localparam LOOPTRST = 1;
localparam LOOPTDC = 2;
localparam LOOPASD = 3;
reg [1:0] state;
reg [1:0] nextstate;

reg [7:0] counter;
wire config_ready;
wire [7:0] TRST_interval;

assign config_ready = (counter==TRST_interval);

always @(posedge clk) begin
    if (rst) begin
        state <= IDLE;
    end else begin
        state <= nextstate;
    end
end

always @(posedge clk) begin
    if (rst) begin
        counter <= 'b0;
    end else if(state == LOOPTRST) begin
        counter <= counter + 1'b1;
    end else begin
        counter <= 'b0;
    end
end



always @(*) begin
    case(state)
        IDLE: 
            nextstate = (~loop_start)?IDLE:
                        enable_loop_TRST?LOOPTRST:
                        loop_TDC?LOOPTDC:
                        loop_ASD?LOOPASD:IDLE;
        LOOPTRST:
            nextstate = (~config_ready)?LOOPTRST:
                        loop_TDC?LOOPTDC:
                        loop_ASD?LOOPASD:IDLE;
        LOOPTDC:
            nextstate = (~tdc_config_done)?LOOPTDC:
                        loop_ASD?LOOPASD:IDLE;
        LOOPASD:
            nextstate = asd_config_done?IDLE:LOOPASD;

        default: nextstate = IDLE;
    endcase
end



always @(posedge clk) begin
    if (rst) begin
        TDC_loop_start <= 1'b0;
    end else if (state==LOOPTDC) begin
        TDC_loop_start <= 1'b1;
    end else begin
        TDC_loop_start <= 1'b0;
    end
end

always @(posedge clk) begin
    if (rst) begin
        ASD_loop_start <= 1'b0;
    end else if (state==LOOPASD) begin
        ASD_loop_start <= 1'b1;
    end else begin
        ASD_loop_start <= 1'b0;
    end
end

always @(posedge clk) begin
    if (rst) begin
        loop_TRST <= 1'b1;
    end else if ((state==IDLE)&&(nextstate==LOOPTRST)) begin
        loop_TRST <= 1'b0;
    end else begin
        loop_TRST <= 1'b1;
    end
end


always @(posedge clk) begin
    if (rst) begin 
        tdc_config_counter <= 'b0;
        asd_config_counter <= 'b0;
        setup0_fail_counter <= 'b0;
        setup1_fail_counter <= 'b0;
        setup2_fail_counter <= 'b0;
        control0_fail_counter <= 'b0;
        control1_fail_counter <= 'b0;
        asd_fail_counter <= 'b0;
    end else begin
        if (tdc_config_done) begin
            tdc_config_counter <= tdc_config_counter+1'b1;
            setup0_fail_counter <= (chain_success[5]==1'b0)? (setup0_fail_counter+1'b1):setup0_fail_counter;
            setup1_fail_counter <= (chain_success[4]==1'b0)? (setup1_fail_counter+1'b1):setup1_fail_counter;
            setup2_fail_counter <= (chain_success[3]==1'b0)? (setup2_fail_counter+1'b1):setup2_fail_counter;
            control0_fail_counter <= (chain_success[2]==1'b0)? (control0_fail_counter+1'b1):control0_fail_counter;
            control1_fail_counter <= (chain_success[1]==1'b0)? (control1_fail_counter+1'b1):control1_fail_counter;
        end
        if (asd_config_done) begin
            asd_config_counter <= asd_config_counter+1'b1;
            asd_fail_counter <= (chain_success[0]==1'b0)? (asd_fail_counter+1'b1):asd_fail_counter;
        end
    end
end

loop_config vio_loop_config_inst (
  .clk(clk),                // input wire clk
  .probe_in0(tdc_config_counter),    // input wire [31 : 0] probe_in0
  .probe_in1(asd_config_counter),    // input wire [31 : 0] probe_in1
  .probe_in2(setup0_fail_counter),    // input wire [31 : 0] probe_in2
  .probe_in3(setup1_fail_counter),    // input wire [31 : 0] probe_in3
  .probe_in4(setup2_fail_counter),    // input wire [31 : 0] probe_in4
  .probe_in5(control0_fail_counter),    // input wire [31 : 0] probe_in5
  .probe_in6(control1_fail_counter),    // input wire [31 : 0] probe_in6
  .probe_in7(asd_fail_counter),    // input wire [31 : 0] probe_in7
  .probe_out0(loop_start),  // output wire [0 : 0] probe_out0
  .probe_out1(loop_TDC),  // output wire [0 : 0] probe_out1
  .probe_out2(loop_ASD),  // output wire [0 : 0] probe_out2
  .probe_out3(enable_loop_TRST),  // output wire [0 : 0] probe_out3
  .probe_out4(TRST_interval)  // output wire [7 : 0] probe_out4
);

endmodule