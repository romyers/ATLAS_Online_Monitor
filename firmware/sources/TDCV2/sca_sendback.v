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
// File   : sca_sendback.v
// Create : 2022-08-29 17:56:29
// Revise : 2022-08-31 15:17:57
// Editor : sublime text4, tab size (4)
// Description: 
//
// -----------------------------------------------------------------------------
  
module sca_sendback(
    input clk,
    input rst,

    input [2:0] rx_reply_received,
    input [23:0] rx_address,
    input [23:0] rx_transID,
    input [23:0] rx_channel,
    input [23:0] rx_len,
    input [23:0] rx_error,
    input [95:0] rx_data,

    input uart_tx_ready,
    output [7:0] uart_data,
    output uart_data_write

);
    

wire [2:0] sca_send_enable_array;
reg [2:0] sca_busy_r;
reg [2:0] sca_busy_sample;
wire any_busy_rising;
wire any_busy_falling;
wire [7:0] uart_data_array[2:0];
wire [2:0] uart_data_write_array;
wire [2:0] sca_busy;

assign uart_data =  sca_send_enable_array[2] ? uart_data_array[2] : 
                    sca_send_enable_array[1] ? uart_data_array[1] : 
                    sca_send_enable_array[0] ? uart_data_array[0] : 'b0;
assign uart_data_write =sca_send_enable_array[2] ? uart_data_write_array[2] : 
                        sca_send_enable_array[1] ? uart_data_write_array[1] : 
                        sca_send_enable_array[0] ? uart_data_write_array[0] : 'b0;
assign any_busy_rising = (sca_busy[2]&(~sca_busy_r[2]))|(sca_busy[1]&(~sca_busy_r[1]))|
                         (sca_busy[0]&(~sca_busy_r[0]));
assign any_busy_falling= ((~sca_busy[2])&sca_busy_r[2])|((~sca_busy[1])&sca_busy_r[1])|
                         ((~sca_busy[0])&sca_busy_r[0]);
assign sca_send_enable_array =  sca_busy_sample[2]? 3'b100 :
                                sca_busy_sample[1]? 3'b010 :
                                sca_busy_sample[0]? 3'b001 : 'b0;

always @(posedge clk  ) begin
    sca_busy_r <= sca_busy;
end

always @(posedge clk  ) begin
    if (rst) begin
        // reset
        sca_busy_sample <= 'b0;
    end else if ((sca_busy_sample==3'b0)&any_busy_rising) begin
        sca_busy_sample <= sca_busy;
    end else if (any_busy_falling) begin
        sca_busy_sample <= sca_busy;
    end
end                       

genvar i;
generate
    for (i=0;i<3;i=i+1) begin : sca_sendback_i
        sca_sendback_single inst_sca_sendback_single(
            .clk               (clk),
            .rst               (rst),
            .send_enable       (sca_send_enable_array[i]),
            .rx_reply_received (rx_reply_received[i]),
            .rx_address        (rx_address[i*8+7:i*8]),
            .rx_transID        (rx_transID[i*8+7:i*8]),
            .rx_channel        (rx_channel[i*8+7:i*8]),
            .rx_len            (rx_len[i*8+7:i*8]),
            .rx_error          (rx_error[i*8+7:i*8]),
            .rx_data           (rx_data[i*32+31:i*32]),
            .uart_tx_ready     (uart_tx_ready),
            .uart_data         (uart_data_array[i]),
            .uart_data_write   (uart_data_write_array[i]),
            .sca_busy          (sca_busy[i])
        );

    end
endgenerate

endmodule

