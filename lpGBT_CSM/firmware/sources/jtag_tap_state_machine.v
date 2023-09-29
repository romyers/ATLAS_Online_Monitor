`timescale 1ns / 1ps
`include "definitions.sv"

module jtag_tap_state_machine (
  input   wire          rst,    // Asychronous reset (reset FMC)
  input   wire          tck,    // Test clock
  input   wire          tms,    // Test mode select
  output  reg   [3:0]   state   // Tap state
);

  // State movement machines
  always @ (posedge tck, posedge rst) begin
    if (rst) state <= `STATE_TEST_LOGIC_RESET;
    else begin
      case (state)
        `STATE_TEST_LOGIC_RESET: begin
          if (~tms) state <= `STATE_RUN_TEST_IDLE;
          else state <= `STATE_TEST_LOGIC_RESET;
        end // end STATE_TEST_LOGIC_RESET
        `STATE_RUN_TEST_IDLE: begin
          if (tms) state <= `STATE_SELECT_DR_SCAN;
          else state <= `STATE_RUN_TEST_IDLE;
        end // end STATE_RUN_TEST_IDLE
        `STATE_SELECT_DR_SCAN: begin
          if (tms) state <= `STATE_SELECT_IR_SCAN;
          else state <= `STATE_CAPTURE_DR;
        end // end STATE_SELECT_DR_SCAN
        `STATE_SELECT_IR_SCAN: begin
          if (tms) state <= `STATE_TEST_LOGIC_RESET;
          else state <= `STATE_CAPTURE_IR;
        end // end STATE_SELECT_IR_SCAN
        `STATE_CAPTURE_DR: begin
          if (tms) state <= `STATE_EXIT1_DR;
          else state <= `STATE_SHIFT_DR;
        end // end STATE_CAPTURE_DR
        `STATE_CAPTURE_IR: begin
          if (tms) state <= `STATE_EXIT1_IR;
          else state <= `STATE_SHIFT_IR;
        end // end STATE_CAPTURE_IR
        `STATE_SHIFT_DR: begin
          if (tms) state <= `STATE_EXIT1_DR;
          else state <= `STATE_SHIFT_DR;
        end // end STATE_SHIFT_DR
        `STATE_SHIFT_IR: begin
          if (tms) state <= `STATE_EXIT1_IR;
          else state <= `STATE_SHIFT_IR;
        end // end STATE_SHIFT_IR
        `STATE_EXIT1_DR: begin
          if (tms) state <= `STATE_UPDATE_DR;
          else state <= `STATE_PAUSE_DR;
        end // end STATE_EXIT1_DR
        `STATE_EXIT1_IR: begin
          if (tms) state <= `STATE_UPDATE_IR;
          else state <= `STATE_PAUSE_IR;
        end // end STATE_EXIT1_IR
        `STATE_PAUSE_DR: begin
          if (tms) state <= `STATE_EXIT2_DR;
          else state <= `STATE_PAUSE_DR;
        end // end STATE_PAUSE_DR
        `STATE_PAUSE_IR: begin
          if (tms) state <= `STATE_EXIT2_IR;
          else state <= `STATE_PAUSE_IR;
        end // end STATE_PAUSE_IR
        `STATE_EXIT2_DR: begin
          if (tms) state <= `STATE_UPDATE_DR;
          else state <= `STATE_SHIFT_DR;
        end // end STATE_EXIT2_DR
        `STATE_EXIT2_IR: begin
          if (tms) state <= `STATE_UPDATE_IR;
          else state <= `STATE_SHIFT_IR;
        end // end STATE_EXIT2_IR
        `STATE_UPDATE_DR: begin
          if (tms) state <= `STATE_SELECT_DR_SCAN;
          else state <= `STATE_RUN_TEST_IDLE;
        end // end STATE_UPDATE_DR
        `STATE_UPDATE_IR: begin
          if (tms) state <= `STATE_SELECT_DR_SCAN;
          else state <= `STATE_RUN_TEST_IDLE;
        end // end STATE_UPDATE_IR
        default: state <= `STATE_TEST_LOGIC_RESET;
      endcase
    end
  end

endmodule
