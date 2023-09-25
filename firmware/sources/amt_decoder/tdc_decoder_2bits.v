`timescale 1ns / 1ps
//////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////


module tdc_decoder_2bits(

  input RX_FRAMECLK_I,
  input user_rst,
  input [1:0] tdc_d0,
  output parity_chk,
  output [33:0] tdc_pkt_raw
    );
 
    
parameter idle=4'b0,st1=4'd1,st2=4'd2,st3=4'd3;
reg [3:0] state,state_r;
reg [9:0] cnt;
reg [4:0] bit_cnt_0,bit_cnt_1;
    
reg [33:0] tdc_0_pkt;
reg [33:0] tdc_0_pkt_r;
reg [1:0] tdc_d0_r,tdc_d0_buf;
    
assign tdc_pkt_raw =    tdc_0_pkt_r;
    
  always @(posedge RX_FRAMECLK_I) begin
        tdc_d0_r <= tdc_d0;
    end   
 
    always @(posedge   RX_FRAMECLK_I) begin
        if (user_rst) begin
           state <= idle;
           end
        else begin
          case (state)
          idle: begin
                if(cnt[9]==1'b1)
                  state <= st1; // go to st1, to track the header of TDC frames
                end
                
          st1: begin
               if(tdc_d0_r==2'b01)
                 state <=st2; // header start at pos. 0
               else if (tdc_d0_r[1]==1'b1)
                 state <=st3; // header start @ pos 1
               end
               
          st2: begin // deal with pkt with pos.0
               if(bit_cnt_0==6'd16)
                  state <=st1;
               end
          
          st3: begin // deal with pkt with pos 1   
               if(bit_cnt_0==6'd16)
                  state <=st1;
               end   
                   
          default: state <=idle;
          endcase
        end
    end
    
    
    //buffer state
    always @(posedge RX_FRAMECLK_I) begin
       state_r <= state;
    end
    
    always @(posedge RX_FRAMECLK_I) begin
     if(state==idle && tdc_d0_r[1]==1'b1)
       tdc_d0_buf <= tdc_d0_r;
    end
    
    //track total number of intial zeros
    always @(posedge RX_FRAMECLK_I) begin
       if(user_rst) 
          cnt <='b0;
       else if(state == idle) begin
         if(tdc_d0_r ==2'b0 && tdc_d0==2'b0 && cnt[9]==1'b0)
           cnt <= cnt+1;
         end
    end
    
    always @(posedge RX_FRAMECLK_I) begin
      if(user_rst)begin
        tdc_0_pkt <= 'b0;end
      else if(state == st2 || state==st3)
        tdc_0_pkt <= {tdc_0_pkt[31:0],tdc_d0_r};
    end
    
    always @(posedge RX_FRAMECLK_I) begin
     if(user_rst) 
         bit_cnt_0 <= 'b0;
     else if(state==st2|| state==st3)
         bit_cnt_0 <= bit_cnt_0 +1'b1;
     else
         bit_cnt_0 <='b0;
    end
    
    //process tdc pkt with pos @0 //------------------------------------
    always @(posedge RX_FRAMECLK_I) begin
      if(state==st1 && state_r ==st2)
        tdc_0_pkt_r <= tdc_0_pkt;
      else if(state==st1 && state_r ==st3)
        tdc_0_pkt_r <= {tdc_d0_buf[0],tdc_0_pkt[33:1]};
    end
    
    //process tdc pkt with pos @1 //------------------------------------   
    
    
    assign  parity_chk =^tdc_0_pkt_r[33:2];
endmodule
