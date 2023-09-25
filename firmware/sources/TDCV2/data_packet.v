/* 
       _________   ________     ________       
      / U OF M  \ | LSA    \   / Physics\
      \__ATLAS__/ |   ___   | |   ______/
         |   |    |  |   \  | |  |
         |   |    |  |___/  | |  \______     
         |   |    |         | |         \
         \___/    |________/   \________/
*/  
//  File Name  : data_packet.v
//  Author     : Yuxiang Guo
//  Revision   : 
//               First created on 2018-12-14 22:15:00
//  Note       : 
//   


module data_packet (
  input clk,    // Clock 125MHz

  input rst,  // Asynchronous reset active high

  input enable,

  input [39:0] TDC_fifo_data, //from TDC fifo
  input TDC_data_ready,
  output reg TDC_data_rd,

  //to ethernet interface tx_axis_fifo     
  output reg [7:0] data_to_eth,
  output reg packet_valid,
  input tx_axis_fifo_tready,
  output reg last
  
);




reg [39:0] TDC_data_reg;
reg [15:0] counter;
reg [15:0] packet_counter;
reg [3:0] data_flag;


localparam IDLE = 4'b0001,
           STARTPRE = 4'b0010,
           DATA = 4'b0100,
           LAST = 4'b1000;


//ethernet frame
//localparam PRE = 56'h55555555555555;
//localparam SFD = 8'hD5;
localparam DSTADDR = 48'he8393563c6c1;
localparam SRCADDR = 48'hFFFFFFC70501;
localparam LOADBYTE= 16'h0064; //loadsize d100, TDC data=d20    //below 1500 load size   above ethertype
localparam FCS = 32'h00000000;

localparam PRELOAD_BYTE = 16'd14;

localparam POSTLOAD_BYTE = 16'd4;



    
    
reg [3:0] state, next;

//state machine


reg  [111:0] preload;
reg  [31:0]  postload;

always @(posedge clk) begin
  if(rst) state <= IDLE;
  else    state <= next;
end



always @(*) begin   
  next = IDLE;
  data_to_eth = 8'b0;
  last = 1'b0;
  packet_valid = 1'b1;
  case (state)
    IDLE: begin
      packet_valid = 1'b0;
      if(TDC_data_ready&enable) next = STARTPRE;
      else                      next = IDLE;
    end
    STARTPRE: begin
      data_to_eth = preload[111:104];
      if(counter < PRELOAD_BYTE)next = STARTPRE;
      else                      next = DATA;
    end
    DATA:begin
      data_to_eth = TDC_data_reg[39:32];
      if(counter < LOADBYTE)    next = DATA;
      else                      next = LAST;
    end
    LAST:begin
      data_to_eth = postload[31:24];
      if(counter<POSTLOAD_BYTE) next = LAST;
      else begin                next = IDLE;
        last = 1'b1;  
      end
    end
  endcase // state
end

reg [3:0] data_counter = 4'b1;
always @(posedge clk) begin
  if(rst) begin
    counter <= 16'b0;
    data_counter <= 4'b1;
    packet_counter <= 16'b0;
  end else begin

    TDC_data_rd <= 1'b0;
    case (state)
      IDLE: begin 
        counter <= 16'b0;
        preload <= {DSTADDR,SRCADDR,LOADBYTE};
        postload <= {FCS[31:17],packet_counter};
        if(TDC_data_ready&enable) begin
          counter <= 1'b1;
          TDC_data_reg <= TDC_fifo_data; //preload the first TDC data
          TDC_data_rd <= 1'b1;
          packet_counter <= packet_counter +1'b1;
        end

      end
      STARTPRE:
        if(tx_axis_fifo_tready) begin
          counter <= counter==PRELOAD_BYTE? 16'b1 : counter +1'b1;
          preload <= {preload[103:0],preload[111:104]};
        end
        
      DATA:begin
        if(tx_axis_fifo_tready) begin
          counter <= counter==LOADBYTE? 16'b1 : counter + 1'b1;
          if(data_counter==5) begin   //one 40bit TDC data read finish
            data_counter <= 8'b1;
            if(TDC_data_ready) begin
              TDC_data_reg <= TDC_fifo_data;
              if(counter==LOADBYTE) TDC_data_rd <=1'b0;//last load, no read from TDC fifo
              else                  TDC_data_rd <= 1'b1; 
            end else TDC_data_reg <= 40'hFFFFFFFFFF;
          end else begin
            data_counter <= data_counter + 1'b1;
            TDC_data_reg <= {TDC_data_reg[31:0],TDC_data_reg[39:32]};
          end
        end
      end
      LAST:
        if(tx_axis_fifo_tready) begin
          counter <= counter==POSTLOAD_BYTE? 16'b1 : counter +1'b1;
          postload <= {postload[23:0],postload[31:24]};
        end
    
    endcase
  end
end
// vio_ethernet_enable vio_ethernet_enable_inst
// (
//     .clk(clk),
//     .probe_out0(enable_VIO),
//     .probe_out1(packet_enable_VIO)
//   );


endmodule