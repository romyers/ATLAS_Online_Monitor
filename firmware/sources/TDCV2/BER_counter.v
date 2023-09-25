/* 
       _________   ________     ________       
      / U OF M  \ | LSA    \   / Physics\
      \__ATLAS__/ |   ___   | |   ______/
         |   |    |  |   \  | |  |
         |   |    |  |___/  | |  \______     
         |   |    |         | |         \
         \___/    |________/   \________/
*/  
//  File Name  : BER_counter.v
//  Author     : Yuxiang Guo
//  Revision   : 
//               First created on 2020-03-05 12:42:00
//  Note       : 
//   
module BER_counter(
   input clk,
   input rst,
   input data_ready,
   input enable,
   input [9:0] data_10b, 
   input [9:0] correct_value,
   output [35:0] error_count,
   output [47:0] correct_count
);

reg [11:0] error_LSB, error_MID, error_MSB;
reg [11:0] LSB_counter, Low_counter, Mid_counter, MSB_counter;

wire [9:0]cmp_result;
wire count_enable;
reg [9:0] cmp_result_reg;
reg count_enable_reg;

assign cmp_result = data_10b ^ correct_value;
assign count_enable = |cmp_result;
always @(posedge clk or posedge rst) begin
   if (rst) begin       
      cmp_result_reg <= 10'b0;
      count_enable_reg <= 1'b0;
   end else if (enable & data_ready) begin
      cmp_result_reg <= cmp_result;
      count_enable_reg <= count_enable;
   end
end

reg [9:0] count_bit;
always @(posedge clk or posedge rst) begin
   if (rst) begin
      error_LSB <= 12'b0;
      error_MID <= 12'b0;
      error_MSB <= 12'b0;
      count_bit <= 10'b1;      
   end else if (enable & count_enable_reg) begin
      error_LSB <= error_LSB + |(cmp_result_reg&count_bit);
      count_bit <= {count_bit[8:0],count_bit[9]};    
      if (&error_LSB) begin
         error_MID <= error_MID + 1'b1;
         if (&error_MID) 
            error_MSB <= error_MSB + 1'b1;  
      end 
   end
end
assign error_count = {error_MSB, error_MID, error_LSB};

wire correct_counter_enable;
assign correct_counter_enable = data_ready & (~count_enable);
always @(posedge clk or posedge rst) begin
   if (rst) begin
      LSB_counter <= 12'b0;
      Low_counter <= 12'b0;
      Mid_counter <= 12'b0;
      MSB_counter <= 12'b0;    
   end else if (enable & correct_counter_enable) begin
      LSB_counter <= LSB_counter + 1'b1;
      if (&LSB_counter) begin
         Low_counter <= Low_counter + 1'b1;
         if (&Low_counter) begin
            Mid_counter <= Mid_counter + 1'b1;  
            if (&Mid_counter)
               MSB_counter <= MSB_counter + 1'b1;
         end
      end
   end
end
assign correct_count = {MSB_counter, Mid_counter, Low_counter, LSB_counter};

endmodule
