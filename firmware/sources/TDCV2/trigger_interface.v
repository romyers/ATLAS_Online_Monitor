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
// File   : trigger_interface.v
// Create : 2021-03-10 01:47:27
// Revise : 2021-08-31 19:41:16
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
module trigger_interface
(
    input clk_160,
    input rst_160,
    input bc_reset,
    input event_reset,
    input trigger_in,    
    input trigger_read,

    output trigger_ready_delayed,
    
    output [28:0] trigger_fifo_data,
    output trigger_fifo_empty,
    output [11:0] trigger_coarse,
    //configuration
    input trigger_redge,
    input [11:0] rollover,
    input [11:0] coarse_count_offset,
    input [7:0] trigger_latency

);


wire trigger_write;
wire [28:0] trigger_data_in;
wire [16:0] trigger_time;
wire [16:0] trigger_time_delayed;
wire trigger_fifo_full_tmp;
wire trigger_fifo_full;
wire trigger_fifo_empty_tmp;
reg [11:0] event_count;
reg [13:0] trigger_coarse_reg;
wire trigger_ready;
reg [1:0] tick_reg;
reg [1023:0] latency_reg;
// wire tick_40M;
wire delayed_write;
wire [28:0] delayed_trigger_data;

reg srst_160;
assign trigger_data_in = {event_count, trigger_time_delayed};


always @(posedge clk_160) begin
    srst_160 <= rst_160;
end

always @(posedge clk_160) begin
    if(rst_160|event_reset) begin
        event_count <= 'b0;
    end else if(trigger_ready_delayed&(~trigger_fifo_full)) begin
        event_count <= event_count + 1'b1;
    end
end

t0_timestamp t0_timestamp_inst(
    .clk_160(clk_160),
    .reset(rst_160|bc_reset),
    .trigger_ext(trigger_in),

    .t0_timestamp(trigger_time),
    .t0_ready(trigger_ready),
    .coarse_counter(trigger_coarse),

    .is_rising_edge(trigger_redge),    
    .coarse_count_offset(coarse_count_offset),
    .rollover(rollover)  
);



shift_ram_trigger shift_ram_trigger_inst (
  .A({trigger_latency,2'b00}),        // input wire [9 : 0] A
  .D({trigger_time,trigger_ready}),        // input wire [17 : 0] D
  .CLK(clk_160),    // input wire CLK
  .SCLR(srst_160),  // input wire SCLR, only reset output register
  .Q({trigger_time_delayed,trigger_ready_delayed})        // output wire [17 : 0] Q
);


trigger_FIFO trigger_FIFO_delayed_inst (
  .clk(clk_160),                  // input wire clk
  .srst(srst_160),                // input wire srst
  .din(trigger_data_in),                  // input wire [28 : 0] din
  .wr_en(trigger_ready_delayed),              // input wire wr_en
  .rd_en(trigger_read),              // input wire rd_en
  .dout(trigger_fifo_data),                // output wire [28 : 0] dout
  .full(trigger_fifo_full),                // output wire full
  .empty(trigger_fifo_empty),              // output wire empty
  .wr_rst_busy(),  // output wire wr_rst_busy
  .rd_rst_busy()  // output wire rd_rst_busy
);


ila_trigger_interface ila_trigger_interface_inst (
  .clk(clk_160), // input wire clk


  .probe0(srst_160), // input wire [0:0]  probe0  
  .probe1(trigger_ready), // input wire [0:0]  probe1 
  .probe2(trigger_ready_delayed), // input wire [0:0]  probe2 
  .probe3(trigger_fifo_full), // input wire [0:0]  probe3 
  .probe4(trigger_read), // input wire [0:0]  probe4 
  .probe5(trigger_fifo_data), // input wire [28:0]  probe5 
  .probe6(trigger_fifo_empty), // input wire [0:0]  probe6 
  .probe7(trigger_coarse), // input wire [11:0]  probe7 
  .probe8(trigger_time_delayed), // input wire [16:0]  probe8 
  .probe9(event_count), // input wire [11:0]  probe9  
  .probe10(trigger_in) // input wire [0:0]  probe10
);

// always @(posedge clk) begin
//     if(rst) begin
//         latency_reg <= 'b0;
//     end else begin
//         latency_reg <= {latency_reg[1022:0],trigger_ready};
//     end 
// end

// //note: built in FIFO has 2 clock latency for written data to be visible
// assign delayed_write = latency_reg[{trigger_latency,2'b10}] & (~trigger_fifo_empty_tmp);

// trigger_FIFO trigger_FIFO_inst (
//   .clk(clk),                  // input wire clk
//   .srst(rst),                // input wire srst
//   .din(trigger_data_in),                  // input wire [28 : 0] din
//   .wr_en(trigger_write),              // input wire wr_en
//   .rd_en(delayed_write),              // input wire rd_en
//   .dout(trigger_fifo_data_tmp),                // output wire [28 : 0] dout
//   .full(trigger_fifo_full_tmp),                // output wire full
//   .empty(trigger_fifo_empty_tmp),              // output wire empty
//   .wr_rst_busy(),  // output wire wr_rst_busy
//   .rd_rst_busy()  // output wire rd_rst_busy
// );
endmodule
