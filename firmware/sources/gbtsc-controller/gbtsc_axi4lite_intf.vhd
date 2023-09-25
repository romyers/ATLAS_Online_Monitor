library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;

entity gbtsc_axi4lite_intf is
	generic(
		-- Width of S_AXI data bus.
		C_S_AXI_DATA_WIDTH      : integer                       := 32;
		-- Width of S_AXI address bus.
		C_S_AXI_ADDR_WIDTH      : integer                       := 5
	);
	port(
		-- AXI4LITE Interface
		S_AXI_ACLK    		: in  std_logic;
		S_AXI_ARESETN 		: in  std_logic;
		S_AXI_AWADDR  		: in  std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
		S_AXI_AWVALID 		: in  std_logic;
		S_AXI_AWREADY 		: out std_logic;
		S_AXI_WDATA   		: in  std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
		S_AXI_WSTRB   		: in  std_logic_vector((C_S_AXI_DATA_WIDTH / 8) - 1 downto 0);
		S_AXI_WVALID  		: in  std_logic;
		S_AXI_WREADY  		: out std_logic;
		S_AXI_BRESP   		: out std_logic_vector(1 downto 0);
		S_AXI_BVALID  		: out std_logic;
		S_AXI_BREADY  		: in  std_logic;
		S_AXI_ARADDR  		: in  std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
		S_AXI_ARVALID 		: in  std_logic;
		S_AXI_ARREADY 		: out std_logic;
		S_AXI_RDATA   		: out std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
		S_AXI_RRESP   		: out std_logic_vector(1 downto 0);
		S_AXI_RVALID  		: out std_logic;
		S_AXI_RREADY  		: in  std_logic;
		
		-- Data to GBT-SC State machine
		reset_gbtsc		: out std_logic;
		start_reset		: out std_logic;
		start_connect		: out std_logic;
		start_command		: out std_logic;
			
		tx_ready_i          : in std_logic;
        tx_address          : out std_logic_vector(7 downto 0);
        tx_transID          : out std_logic_vector(7 downto 0);
        tx_channel          : out std_logic_vector(7 downto 0);
        tx_len              : out std_logic_vector(7 downto 0);
        tx_command          : out std_logic_vector(7 downto 0);
        tx_data             : out std_logic_vector(31 downto 0);
        
        rx_reply_received_i : in std_logic;
        rx_address          : in  std_logic_vector(7 downto 0);
        rx_transID          : in  std_logic_vector(7 downto 0);
        rx_channel          : in  std_logic_vector(7 downto 0);
        rx_len              : in  std_logic_vector(7 downto 0);
        rx_error            : in  std_logic_vector(7 downto 0);
        rx_data             : in  std_logic_vector(31 downto 0);
		
		-- Data to GBT-IC State machine		
        ic_ready            : in  std_logic;
        ic_empty            : in  std_logic;
        
        -- Configuration
        GBTx_address        : out std_logic_vector(7 downto 0);
        Register_addr        : out std_logic_vector(15 downto 0);
        nb_to_be_read        : out std_logic_vector(15 downto 0);
        
        -- Control        
        start_write            : out std_logic;
        start_read            : out std_logic;
        
        -- WRITE register(s)
        data_o                : out std_logic_vector(7 downto 0);
        wr                    : out std_logic;
        
        -- READ register(s)        
        data_i                 : in  std_logic_vector(7 downto 0);
        rd                    : out std_logic;
                
        rd_gbtx_addr        : in  std_logic_vector(7 downto 0);
        rd_mem_ptr            : in  std_logic_vector(15 downto 0);
        rd_nb_of_words        : in  std_logic_vector(15 downto 0)
	);
end gbtsc_axi4lite_intf;

architecture arch of gbtsc_axi4lite_intf is

	-- AXI4LITE signals
	signal axi_awaddr  : std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
	signal axi_awready : std_logic;
	signal axi_wready  : std_logic;
	signal axi_bresp   : std_logic_vector(1 downto 0);
	signal axi_bvalid  : std_logic;
	signal axi_araddr  : std_logic_vector(C_S_AXI_ADDR_WIDTH - 1 downto 0);
	signal axi_arready : std_logic;
	signal axi_rdata   : std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);
	signal axi_rresp   : std_logic_vector(1 downto 0);
	signal axi_rvalid  : std_logic;

	-- ADDR_LSB is used for addressing 32 bit registers/memories
	constant ADDR_LSB          : integer := (C_S_AXI_DATA_WIDTH / 32) + 1;
	constant OPT_MEM_ADDR_BITS : integer := 2;

	-- Slave register read/write signals
	signal slv_reg_rden : std_logic;
	signal slv_reg_wren : std_logic;

	-- Signal that stores the current register's value
	signal reg_data_out : std_logic_vector(C_S_AXI_DATA_WIDTH - 1 downto 0);

	signal new_control_val	: std_logic := '0';
	
	signal rx_reply_received_s : std_logic;
	
begin

	-- I/O Connections assignments
	S_AXI_AWREADY 	<= axi_awready;
	S_AXI_WREADY  	<= axi_wready;
	S_AXI_BRESP   	<= axi_bresp;
	S_AXI_BVALID  	<= axi_bvalid;
	S_AXI_ARREADY 	<= axi_arready;
	S_AXI_RDATA  	<= axi_rdata;
	S_AXI_RRESP   	<= axi_rresp;
	S_AXI_RVALID  	<= axi_rvalid;
	
	-- Implement axi_awready generation
	-- axi_awready is asserted for one S_AXI_ACLK clock cycle when both
	-- S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_awready is
	-- de-asserted when reset is low.
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_awready <= '0';
			else
				if (axi_awready = '0' and S_AXI_AWVALID = '1' and S_AXI_WVALID = '1') then
					-- slave is ready to accept write address when
					-- there is a valid write address and write data
					-- on the write address and data bus. This design 
					-- expects no outstanding transactions. 
					axi_awready <= '1';
				else
					axi_awready <= '0';
				end if;
			end if;
		end if;
	end process;

	-- Implement axi_awaddr latching
	-- This process is used to latch the address when both 
	-- S_AXI_AWVALID and S_AXI_WVALID are valid. 
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_awaddr <= (others => '0');
			else
				if (axi_awready = '0' and S_AXI_AWVALID = '1' and S_AXI_WVALID = '1') then
					-- Write Address latching
					axi_awaddr <= S_AXI_AWADDR;
				end if;
			end if;
		end if;
	end process;

	-- Implement axi_wready generation
	-- axi_wready is asserted for one S_AXI_ACLK clock cycle when both
	-- S_AXI_AWVALID and S_AXI_WVALID are asserted. axi_wready is 
	-- de-asserted when reset is low. 
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_wready <= '0';
			else
				if (axi_wready = '0' and S_AXI_WVALID = '1' and S_AXI_AWVALID = '1') then
					-- slave is ready to accept write data when 
					-- there is a valid write address and write data
					-- on the write address and data bus. This design 
					-- expects no outstanding transactions.           
					axi_wready <= '1';
				else
					axi_wready <= '0';
				end if;
			end if;
		end if;
	end process;

	-- Implement memory mapped register select and write logic generation
	-- The write data is accepted and written to memory mapped registers when
	-- axi_awready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted. Write strobes are used to
	-- select byte enables of slave registers while writing.
	-- These registers are cleared when reset (active low) is applied.
	-- Slave register write enable is asserted when valid address and data are available
	-- and the slave is ready to accept the write address and write data.
	slv_reg_wren <= axi_wready and S_AXI_WVALID and axi_awready and S_AXI_AWVALID;

	process(S_AXI_ACLK)
		variable loc_addr : std_logic_vector(OPT_MEM_ADDR_BITS downto 0);
	begin
		if rising_edge(S_AXI_ACLK) then
						
			start_read    <= '0';
            start_write   <= '0';
            wr            <= '0';
            
            reset_gbtsc 	<= '0';
            start_connect   <= '0';
            start_reset     <= '0';
            start_command   <= '0';
                                                				
			if S_AXI_ARESETN = '0' then
			
			else
				loc_addr := axi_awaddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB);
				if (slv_reg_wren = '1') then
					case loc_addr is
						when b"000" =>
						    tx_address          <= x"00";
						    
						    if (S_AXI_WSTRB(0) = '1') then
                                tx_transID          <= S_AXI_WDATA(7 downto 0);
                            end if;
                            
                            if (S_AXI_WSTRB(1) = '1') then
                                tx_channel          <= S_AXI_WDATA(15 downto 8);
                            end if;
                                
                            if (S_AXI_WSTRB(2) = '1') then
                                tx_len              <= S_AXI_WDATA(23 downto 16);
                            end if;
                            
                            if (S_AXI_WSTRB(3) = '1') then
                                tx_command          <= S_AXI_WDATA(31 downto 24);
							end if;
							
						when b"001" =>
						    if (S_AXI_WSTRB(0) = '1') then
                                tx_data(7 downto 0)             <= S_AXI_WDATA(7 downto 0);
                            end if;
                            
                            if (S_AXI_WSTRB(1) = '1') then
                                tx_data(15 downto 8)             <= S_AXI_WDATA(15 downto 8);
                            end if;
                            
                            if (S_AXI_WSTRB(0) = '1') then
                                tx_data(23 downto 16)             <= S_AXI_WDATA(23 downto 16);
                            end if;
                            
                            if (S_AXI_WSTRB(0) = '1') then
                                tx_data(31 downto 24)             <= S_AXI_WDATA(31 downto 24);
                            end if;                  
                            
                            start_command       <= '1';
							
						when b"010" =>
                            if (S_AXI_WSTRB(0) = '1') then

                                case S_AXI_WDATA(7 downto 0) is
                                
                                    when x"00"    =>    reset_gbtsc         <= '1';
                                    when x"01"    =>    start_connect       <= '1';
                                    when x"02"    =>    start_reset         <= '1';
                                    when others => null;
                                end case;
                                  
                            end if;
						
						when b"100" =>
                            if (S_AXI_WSTRB(0) = '1') then    
                                GBTx_address<= S_AXI_WDATA(7 downto 0);
                            end if;
                            
                            if (S_AXI_WSTRB(1) = '1') then
                            
                                case S_AXI_WDATA(15 downto 8) is
                                
                                    when x"01"    =>    start_read    <= '1';
                                    when x"02"    =>    start_write    <= '1';
                                    when others => null;
                                end case;
                                
                            end if;
                            
                        when b"101" =>    
                            if (S_AXI_WSTRB(0) = '1') then
                                Register_addr(7 downto 0) <= S_AXI_WDATA(7 downto 0);
                            end if;
                            
                            if (S_AXI_WSTRB(1) = '1') then
                                Register_addr(15 downto 8) <= S_AXI_WDATA(15 downto 8);
                            end if;
                            
                            if (S_AXI_WSTRB(2) = '1') then
                                nb_to_be_read(7 downto 0) <= S_AXI_WDATA(23 downto 16);
                            end if;
                            
                            if (S_AXI_WSTRB(3) = '1') then
                                nb_to_be_read(15 downto 8) <= S_AXI_WDATA(31 downto 24);
                            end if;
                            
                        when b"110" =>
                            if (S_AXI_WSTRB(0) = '1') then
                                data_o        <= S_AXI_WDATA(7 downto 0);
                                wr            <= '1';
                            end if;
                                                        	
						when others => NULL;
						
					end case;
				end if;
			end if;
		end if;
	end process;
	
	-- Implement write response logic generation
	-- The write response and response valid signals are asserted by the slave 
	-- when axi_wready, S_AXI_WVALID, axi_wready and S_AXI_WVALID are asserted.  
	-- This marks the acceptance of address and indicates the status of 
	-- write transaction.
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_bvalid <= '0';
				axi_bresp  <= "00";     --need to work more on the responses
			else
				if (axi_awready = '1' and S_AXI_AWVALID = '1' and axi_wready = '1' and S_AXI_WVALID = '1' and axi_bvalid = '0') then
					axi_bvalid <= '1';
					axi_bresp  <= "00";
				elsif (S_AXI_BREADY = '1' and axi_bvalid = '1') then --check if bready is asserted while bvalid is high)
					axi_bvalid <= '0';  -- (there is a possibility that bready is always asserted high)
				end if;
			end if;
		end if;
	end process;

	-- Implement axi_arready generation
	-- axi_arready is asserted for one S_AXI_ACLK clock cycle when
	-- S_AXI_ARVALID is asserted. axi_awready is 
	-- de-asserted when reset (active low) is asserted. 
	-- The read address is also latched when S_AXI_ARVALID is 
	-- asserted. axi_araddr is reset to zero on reset assertion.
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_arready <= '0';
				axi_araddr  <= (others => '1');
			else
				if (axi_arready = '0' and S_AXI_ARVALID = '1') then
					-- indicates that the slave has acceped the valid read address
					axi_arready <= '1';
					-- Read Address latching 
					axi_araddr  <= S_AXI_ARADDR;
				else
					axi_arready <= '0';
				end if;
			end if;
		end if;
	end process;

	-- Implement axi_arvalid generation
	-- axi_rvalid is asserted for one S_AXI_ACLK clock cycle when both 
	-- S_AXI_ARVALID and axi_arready are asserted. The slave registers 
	-- data are available on the axi_rdata bus at this instance. The 
	-- assertion of axi_rvalid marks the validity of read data on the 
	-- bus and axi_rresp indicates the status of read transaction.axi_rvalid 
	-- is deasserted on reset (active low). axi_rresp and axi_rdata are 
	-- cleared to zero on reset (active low).  
	process(S_AXI_ACLK)
	begin
		if rising_edge(S_AXI_ACLK) then
			if S_AXI_ARESETN = '0' then
				axi_rvalid <= '0';
				axi_rresp  <= "00";
			else
				if (axi_arready = '1' and S_AXI_ARVALID = '1' and axi_rvalid = '0') then
					-- Valid read data is available at the read data bus
					axi_rvalid <= '1';
					axi_rresp  <= "00"; -- 'OKAY' response
				elsif (axi_rvalid = '1' and S_AXI_RREADY = '1') then
					-- Read data is accepted by the master
					axi_rvalid <= '0';
				end if;
			end if;
		end if;
	end process;

	-- Implement memory mapped register select and read logic generation
	-- Slave register read enable is asserted when valid address is available
	-- and the slave is ready to accept the read address.
	slv_reg_rden <= axi_arready and S_AXI_ARVALID and (not axi_rvalid);

    reg_data_out <= rx_transID & rx_channel & rx_len & rx_error         when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"000" else
                    rx_data                                             when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"001" else
                    x"0000000" & "000" & rx_reply_received_s            when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"010" else
                    x"000000" & rd_gbtx_addr                            when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"100" else
                    rd_nb_of_words & rd_mem_ptr                         when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"101" else
                    x"0000000" & "00" & ic_ready & ic_empty             when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"110" else
                    x"000000" & data_i                                  when axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"111" else
                    x"00000000";
    
	-- Output register or memory read data
	process(S_AXI_ACLK) is
	begin
		if (rising_edge(S_AXI_ACLK)) then
		    rd           <= '0';
		    
		    if rx_reply_received_i = '1' then
               rx_reply_received_s     <= '1';
            end if;
                       
			if (S_AXI_ARESETN = '0') then
				axi_rdata <= (others => '0');
			else
				if (slv_reg_rden = '1') then
					-- When there is a valid read address (S_AXI_ARVALID) with 
					-- acceptance of read address by the slave (axi_arready), 
					-- output the read dada 
					-- Read address mux
					if (axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"111") then
					   rd           <= '1';
					end if;
                    if (axi_araddr(ADDR_LSB + OPT_MEM_ADDR_BITS downto ADDR_LSB) = b"010") then
                       rx_reply_received_s           <= '0';
                    end if;
					
					axi_rdata <= reg_data_out; -- register read data
				end if;
			end if;
		end if;
	end process;

end arch;