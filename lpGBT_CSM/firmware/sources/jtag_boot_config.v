`timescale 1ns / 1ps
`include "definitions.sv"

module jtag_boot_config (
  input   wire        rst,        // Asynchronous reset.
  input   wire        tck,        // JTAG Clock.
  input   wire        tdo,        // JTAG TDO.
  input   wire  [3:0] tap_state,  // JTAG TAP State.
  output  reg         tms,        // JTAG TMS.
  output  reg         tdi,        // JTAG TDI.
  output  reg         done,       // Has the process finished?
  output  reg         p_f         // Pass or Fail. It there a device here or not?
);

  reg   [4:0]   dr_count;                                 // Counter for data register shifting.
  reg   [31:0]  dr_read;                                  // Data that is read from the mezzanine.
  reg           output_done;
  wire  [31:0]  dr_data;                                  // Data that is shifted into the mezzanine.
  
  assign dr_data = 32'b11000000000000000000000000000011;  // Value of the data that is shifted into the mezzanine.
  localparam DR_COUNT_MAX = 5'b11111;                     // Maximum number of shifts to the data register (0 to 31).

  // Control the TAP State Machine shifting.
  always @ (negedge tck, posedge rst) begin
    if (rst) begin
      tms <= 1;                                     // TMS to 1 by default.
      output_done <= 0;                             // The process is not finished by default.
    end
    else begin
      case (tap_state)
        `STATE_TEST_LOGIC_RESET: begin
          if (~output_done) tms <= 0;               // If the process is not done, proceed.
          else tms <= 1;                            // Else don't.
        end // end STATE_REST_LOGIC_RESET
        `STATE_RUN_TEST_IDLE: tms <= 1;
        `STATE_SELECT_DR_SCAN: begin
          if (~output_done) tms <= 0;               // Move to Capture Data Register.
          else tms <= 1;
        end
        `STATE_CAPTURE_DR: tms <= 0;                // Move to Shift Data Register.
        `STATE_SHIFT_DR: begin
          if (dr_count == DR_COUNT_MAX) tms <= 1;   // If counter is not maxed, then keep shifting.
          else tms <= 0;                            // Else stop shifting.
        end // end STATE_SHIFT_DR
        `STATE_EXIT1_DR: tms <= 1;                  // Exit the process.
        `STATE_UPDATE_DR: begin
          tms <= 0;                                 // Exit the process.
          output_done <= 1;                         // Declare that the process is finished.
        end // end STATE_UPDATE_IR
        default : tms <= 1;                         // Default to Test Logic Reset in the event of a failure.
      endcase
    end
  end
  
  // Control the TDI output.
  always @ (negedge tck, posedge rst) begin
    if (rst) tdi <= 0;                                                // Default no output.
    else if (tap_state == `STATE_SHIFT_DR) tdi <= dr_data[dr_count];  // Shift data into the Data Register to force IDCODE out.
  end
  
  // Control the counter.
  always @ (posedge tck, posedge rst) begin
    if (rst) dr_count <= 0;                                           // Default count of zero.
    else if (tap_state == `STATE_SHIFT_DR) dr_count <= dr_count + 1;  // Increment counter during shift phase.
  end
  
  // Capture the mezzanine output.
  always @ (posedge tck, posedge rst) begin
    if (rst) dr_read <= 0;                                            // Default nothing read.
    else if (tap_state == `STATE_SHIFT_DR) dr_read[dr_count] <= tdo;  // Capture the read data during the shift phase.
  end
  
  // Declare if JTAG Boot Config is done or not.
  always @ (posedge tck, posedge rst) begin
    if (rst) done <= 0;                                                       // Default not done.
    else if (tap_state == `STATE_TEST_LOGIC_RESET && output_done) done <= 1;  // If we are at JTAG base state and we have output our logic.
    else done <= 0;                                                           // Else we are not done.
  end
  
  // Control the pass or fail output.
  always @ (posedge done, posedge rst) begin
    if (rst) p_f <= 0;                                    // By default the card has not passed.
    else if (dr_read == `ID_CODE) p_f <= 1;               // If the IDCODE is read the it passes.
    else p_f <= 0;                                        // Else it fails.
  end
  
endmodule
