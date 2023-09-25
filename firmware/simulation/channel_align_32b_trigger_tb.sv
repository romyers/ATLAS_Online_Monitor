`timescale 1ns/100ps
`define CLOCK_PERIOD 2 
`define SD #1

module channel_align_32b_trigger_tb();

logic clk;
logic rst;

always begin
    #(`CLOCK_PERIOD/2.0)
    clk = ~ clk; 
end


logic [63:0] data_64b_decode; 
logic [7:0] o_Kout;
logic data_ready;
logic chnl_data_valid;
logic [31:0] chnl_data_32b;
int counter;

always @(posedge clk) begin
    if (rst) begin
        counter <= `SD 0;
    end else begin 
        if (counter == 4) begin 
            counter <= `SD 0;
        end else begin
            counter <= `SD counter +1 ;
        end
    end
end

assign data_ready = counter == 4;


channel_align_32b_trigger channel_align_32b_trigger_inst (
    .clk             (clk),
    .rst             (rst),
    .data_64b_decode (data_64b_decode),
    .o_Kout          (o_Kout),
    .data_ready      (data_ready),
    .chnl_data_valid (chnl_data_valid),
    .chnl_data_32b   (chnl_data_32b)
);

// display output 
always @(posedge clk) begin
    if (chnl_data_valid) begin
        $display("output: %x", chnl_data_32b);
    end
end

task input_base (
    input [7:0] K,
    input [63:0] data
);
    begin
        while (counter != 3) begin
            @(posedge clk);
        end
        `SD
        o_Kout = K;
        data_64b_decode = data; 
        @(posedge clk);
    end
endtask

// Note: the output suppose to be:
// Header: f8a1a2a3
// traier: f0b1b2b3
task empty_test;
begin
    // 1
    $display("Empty case test, results are:");
    $display("Header: f8a1a2a3");
    $display("Header: f0b1b2b3");

    $display("Empty case 1");
    input_base(8'b0000_0011, 64'ha1a2a3b1_b2b3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 2
    $display("Empty case 2");
    input_base(8'b1000_0001, 64'hffa1a2a3_b1b2b3ff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 3
    $display("Empty case 3");
    input_base(8'b1100_0000, 64'hffffa1a2_a3b1b2b3);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 4
    $display("Empty case 4");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2);
    input_base(8'b0111_1111, 64'hb3ffffff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 5
    $display("Empty case 5");
    input_base(8'b1111_0000, 64'hffffffff_a1a2a3b1);
    input_base(8'b0011_1111, 64'hb2b3ffff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 6
    $display("Empty case 6");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3);
    input_base(8'b0001_1111, 64'hb1b2b3ff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 7
    $display("Empty case 7");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2);
    input_base(8'b0000_1111, 64'ha3b1b2b3_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    // 8
    $display("Empty case 8");
    input_base(8'b1111_1110, 64'hffffffff_ffffffa1);
    input_base(8'b0000_0111, 64'ha2a3b1b2_b3ffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

end
endtask

// Note: the output suppose to be:
// Header: f8a1a2a3
// Data: c1c2c3c4
// traier: f0b1b2b3
task one_data_packet_test;
begin
    // 1
    $display("==================================");
    $display("one data packet test, results are:");
    $display("Header: f8a1a2a3");
    $display("data: c1c2c3c4");
    $display("Header: f0b1b2b3");

    $display("case 1");
    input_base(8'b0000_0000, 64'ha1a2a3c1_c2c3c4b1 );
    input_base(8'b0011_1111, 64'hb2b3ffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 2");
    input_base(8'b1000_0000, 64'hffa1a2a3_c1c2c3c4 );
    input_base(8'b0001_1111, 64'hb1b2b3ff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 3");
    input_base(8'b1100_0000, 64'hffffa1a2_a3c1c2c3 );
    input_base(8'b0000_1111, 64'hc4b1b2b3_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 4");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3c1c2 );
    input_base(8'b0000_0111, 64'hc3c4b1b2_b3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 5");
    input_base(8'b1111_0000, 64'hffffffff_a1a2a3c1 );
    input_base(8'b0000_0011, 64'hc2c3c4b1_b2b3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    
    $display("case 6");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3 );
    input_base(8'b0000_0001, 64'hc1c2c3c4_b1b2b3ff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 7");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_0000, 64'ha3c1c2c3_c4b1b2b3 );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 8");
    input_base(8'b1111_1110, 64'hffffffff_ffffffa1 );
    input_base(8'b0000_0000, 64'ha2a3c1c2_c3c4b1b2 );
    input_base(8'b0111_1110, 64'hb3ffffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);


end
endtask

// Note: the output suppose to be:
// Header: f8a1a2a3
// Data: c1c2c3c4
// Data: c5c6c7c8 
// traier: f0b1b2b3
task two_data_packet_test;
begin
    // 1
    $display("==================================");
    $display("two data packet test, results are:");
    $display("Header: f8a1a2a3");
    $display("data: c1c2c3c4");
    $display("data: c5c6c7c8");
    $display("Header: f0b1b2b3");

    $display("case 1");
    input_base(8'b0000_0000, 64'ha1a2a3c1_c2c3c4c5 );
    input_base(8'b0000_0011, 64'hc6c7c8b1_b2b3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 2");
    input_base(8'b1000_0000, 64'hffa1a2a3c1_c2c3c4 );
    input_base(8'b0000_0001, 64'hc5c6c7c8b1_b2b3ff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 3");
    input_base(8'b1100_0000, 64'hffffa1a2a3c1_c2c3 );
    input_base(8'b0000_0000, 64'hc4c5c6c7c8b1_b2b3 );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 4");
    input_base(8'b1110_0000, 64'hffffffa1a2a3c1_c2 );
    input_base(8'b0000_0000, 64'hc3c4c5c6c7c8b1_b2 );
    input_base(8'b0111_1111, 64'hb3ffffff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 5");
    input_base(8'b1111_0000, 64'hffffffffa1a2a3c1_ );
    input_base(8'b0000_0000, 64'hc2c3c4c5c6c7c8b1_ );
    input_base(8'b0011_1111, 64'hb2b3ffff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 6");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3 );
    input_base(8'b0000_0000, 64'hc1c2c3c4_c5c6c7c8 );
    input_base(8'b0001_1111, 64'hb1b2b3ff_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 7");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_0000, 64'ha3c1c2c3_c4c5c6c7 );
    input_base(8'b0000_1111, 64'hc8b1b2b3_ffffffff);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 8");
    input_base(8'b1111_1110, 64'hffffffff_ffffffa1 );
    input_base(8'b0000_0000, 64'ha2a3c1c2_c3c4c5c6 );
    input_base(8'b0000_0111, 64'hc7c8b1b2_b3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

end
endtask


// Note: the output suppose to be:
// Header: f8a1a2a3
// Data: c1c2c3c4
// Data: c5c6c7c8 
// Data: c9cacbcc
// traier: f0b1b2b3

task three_data_packet_test;
begin
    // 1
    $display("==================================");
    $display("three data packet test, results are:");
    $display("Header: f8a1a2a3");
    $display("data: c1c2c3c4");
    $display("data: c5c6c7c8");
    $display("data: c9cacbcc");
    $display("Header: f0b1b2b3");

    $display("case 1");
    input_base(8'b0000_0000, 64'ha1a2a3c1_c2c3c4c5 );
    input_base(8'b0000_0000, 64'hc6c7c8c9_cacbccb1 );
    input_base(8'b0011_1111, 64'hb2b3c8b1_b2b3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 2");
    input_base(8'b1000_0000, 64'hffa1a2a3_c1c2c3c4 );
    input_base(8'b0000_0000, 64'hc5c6c7c8_c9cacbcc );
    input_base(8'b0001_1111, 64'hb1b2b3ff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 3");
    input_base(8'b1100_0000, 64'hffffa1a2_a3c1c2c3 );
    input_base(8'b0000_0000, 64'hc4c5c6c7_c8c9cacb );
    input_base(8'b0000_1111, 64'hccb1b2b3_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 4");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3c1c2 );
    input_base(8'b0000_0000, 64'hc3c4c5c6_c7c8c9ca );
    input_base(8'b0000_0111, 64'hcbccb1b2_b3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 5");
    input_base(8'b1111_0000, 64'hffffffff_a1a2a3c1 );
    input_base(8'b0000_0000, 64'hc2c3c4c5_c6c7c8c9 );
    input_base(8'b0000_0011, 64'hcacbccb1_b2b3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 6");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3 );
    input_base(8'b0000_0000, 64'hc1c2c3c4_c5c6c7c8 );
    input_base(8'b0000_0001, 64'hc9cacbcc_b1b2b3ff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 7");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_0000, 64'ha3c1c2c3_c4c5c6c7 );
    input_base(8'b0000_0000, 64'hc8c9cacb_ccb1b2b3 );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 8");
    input_base(8'b1111_1110, 64'hffffffff_ffffffa1 );
    input_base(8'b0000_0000, 64'ha2a3c1c2_c3c4c5c6 );
    input_base(8'b0000_0000, 64'hc7c8c9ca_cbccb1b2 );
    input_base(8'b0111_1111, 64'hb3ffffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

end
endtask

// Note: the output suppose to be:
// Header: f8a1a2a3
// traier: f0b1b2b3
// Header: f8d1d2d3
// traier: f0e1e2e3

task double_empty_packet_test;
begin
    // 1
    $display("==================================");
    $display("double empty packet check, results are:");
    $display("Header: f8a1a2a3");
    $display("Trailer: f0b1b2b3");
    $display("Header: f8d1d2d3");
    $display("Trailer: f0e1e2e3");

    $display("case 1");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0100_0000, 64'hb3ffd1d2_d3e1e2e3 );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 2");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0110_0000, 64'hb3ffffd1_d2d3e1e2 );
    input_base(8'b0111_1111, 64'he3ffffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 3");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0111_0000, 64'hb3ffffff_d1d2d3e1 );
    input_base(8'b0011_1111, 64'he2e3ffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 4");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0111_1000, 64'hb3ffffff_ffd1d2d3 );
    input_base(8'b0001_1111, 64'he1e2e3ff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 5");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0111_1100, 64'hb3ffffff_ffffd1d2 );
    input_base(8'b0000_1111, 64'hd3e1e2e3_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 6");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0111_1110, 64'hb3ffffff_ffffffd1 );
    input_base(8'b0000_0111, 64'hd2d3e1e2_e3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 7");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0111_1111, 64'hb3ffffff_ffffffff );
    input_base(8'b0000_0011, 64'hd1d2d3e1_e2e3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 8");
    input_base(8'b1111_0000, 64'hffffffff_a1a2a3b1 );
    input_base(8'b0010_0000, 64'hb2b3ffd1_d2d3e1e2 );
    input_base(8'b0111_1111, 64'he3ffffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 9");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3 );
    input_base(8'b0001_0000, 64'hb1b2b3ff_d1d2d3e1 );
    input_base(8'b0011_1111, 64'he2e3ffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 10");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_1000, 64'ha3b1b2b3_ffd1d2d3 );
    input_base(8'b0001_1111, 64'he1e2e3ff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 11");
    input_base(8'b1111_1110, 64'hffffffff_ffffffa1 );
    input_base(8'b0000_0100, 64'ha2a3b1b2_b3ffd1d2 );
    input_base(8'b0000_1111, 64'hd3e1e2e3_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 12");
    input_base(8'b0000_0010, 64'ha1a2a3b1_b2b3ffd1 );
    input_base(8'b0000_0111, 64'hd2d3e1e2_e3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);


end
endtask

// Note: the output suppose to be:
// Header: f8a1a2a3
// traier: f0b1b2b3
// Header: f8d1d2d3
// Data: c1c2c3c4
// traier: f0e1e2e3

task double_packet_test;
begin
    // 1
    $display("==================================");
    $display("double packet check, results are:");
    $display("Header: f8a1a2a3");
    $display("Trailer: f0b1b2b3");
    $display("Header: f8d1d2d3");
    $display("data: c1c2c3c4");
    $display("Trailer: f0e1e2e3");

    $display("case 1");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0100_0000, 64'hb3ffd1d2_d3c1c2c3 );
    input_base(8'b0000_1111, 64'hc4e1e2e3_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 2");
    input_base(8'b1110_0000, 64'hffffffa1_a2a3b1b2 );
    input_base(8'b0110_0000, 64'hb3ffffd1_d2d3c1c2 );
    input_base(8'b0000_0111, 64'hc3c4e1e2_e3ffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 3");
    input_base(8'b1111_1000, 64'hffffffff_ffa1a2a3 );
    input_base(8'b0001_0000, 64'hb1b2b3ff_d1d2d3c1 );
    input_base(8'b0000_0011, 64'hc2c3c4e1_e2e3ffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 4");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_1000, 64'ha3b1b2b3_ffd1d2d3 );
    input_base(8'b0000_0001, 64'hc1c2c3c4_e1e2e3ff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 5");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_1100, 64'ha3b1b2b3_ffffd1d2 );
    input_base(8'b0000_0000, 64'hd3c1c2c3_c4e1e2e3 );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

    $display("case 6");
    input_base(8'b1111_1100, 64'hffffffff_ffffa1a2 );
    input_base(8'b0000_1110, 64'ha3b1b2b3_ffffffd1 );
    input_base(8'b0000_0000, 64'hd2d3c1c2_c3c4e1e2 );
    input_base(8'b0111_1111, 64'he3ffffff_ffffffff );
    input_base(8'b1111_1111, 0);
    input_base(8'b1111_1111, 0);

end
endtask


initial begin
    clk = 0;
    rst = 0;
    data_64b_decode = 0; 
    o_Kout = 8'hff;

    @(posedge clk);
    `SD
    rst = 1;
    @(posedge clk);
    `SD
    rst = 0;

    //Run test cases here.
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    empty_test();
    one_data_packet_test();
    two_data_packet_test();
    three_data_packet_test();
    double_empty_packet_test();
    double_packet_test();
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);
    @(posedge clk);

    $finish;
end


endmodule
