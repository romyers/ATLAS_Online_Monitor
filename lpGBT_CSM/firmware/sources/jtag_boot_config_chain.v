`timescale 1ns / 1ps

module jtag_boot_config_chain (
  input   wire                rst,            // Asynchronous reset.
  input   wire  [`NUM_DEV:0]  tck,            // JTAG TCK fan-in.
  input   wire  [`NUM_DEV:0]  tdo,            // JTAG TDO fan-in.
  input   wire  [3:0]         tap_state,      // TAP State.
  output  wire  [`NUM_DEV:0]  tms,            // JTAG TMS fan-out.
  output  wire  [`NUM_DEV:0]  tdi,            // JTAG TDI fan-out.
  output  reg                 done,           // Has the process finished?
  output  wire  [`NUM_DEV:0]  active_chains   // What JTAG ports are active "Daisy Chain Vector"?
);
  
  genvar i;                         // For loop generator varaible.
  wire  [`NUM_DEV:0]  done_chain;   // What ports have finished their config process?
  
  // Run the boot config process on every port.
  for (i = 0; i <= `NUM_DEV; i = i + 1) begin
    jtag_boot_config bc (
      .rst(rst),              // Asynchronous reset
      .tck(tck[i]),           // JTAG Clock.
      .tdo(tdo[i]),           // JTAG TDO.
      .tap_state(tap_state),  // JTAG TAP State.
      .tms(tms[i]),           // JTAG TMS.
      .tdi(tdi[i]),           // JTAG TDI.
      .done(done_chain[i]),   // Has the process finished?
      .p_f(active_chains[i])  // Pass or Fail. It there a device here or not?
    );
  end
  
  // Determines if the process has completed or not.
  always @ * begin
    if (done_chain == {(`NUM_DEV + 1){1'b1}}) done <= 1;    // If every port has finished its process then the the chain process is done.
    else done <= 0;                                         // Else it is not.
  end
  
endmodule
