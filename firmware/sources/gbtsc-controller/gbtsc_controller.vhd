----------------------------------------------------------------------------------
-- Company: 
-- Engineer: 
-- 
-- Create Date: 27.03.2017 14:35:29
-- Design Name: 
-- Module Name: gbtsc_controller - Behavioral
-- Project Name: 
-- Target Devices: 
-- Tool Versions: 
-- Description: 
-- 
-- Dependencies: 
-- 
-- Revision:
-- Revision 0.01 - File Created
-- Additional Comments:
-- 
----------------------------------------------------------------------------------


library IEEE;
use IEEE.STD_LOGIC_1164.ALL;

-- Uncomment the following library declaration if using
-- arithmetic functions with Signed or Unsigned values
--use IEEE.NUMERIC_STD.ALL;

-- Uncomment the following library declaration if instantiating
-- any Xilinx leaf cells in this code.
--library UNISIM;
--use UNISIM.VComponents.all;

entity gbtsc_controller is
	generic(
		-- Width of S_AXI data bus.
		C_S_AXI_DATA_WIDTH      : integer                       := 32;
		-- Width of S_AXI address bus.
		C_S_AXI_ADDR_WIDTH      : integer                       := 5
	);
    Port ( 
		-- AXI4LITE Interface
		tx_clock    		: in  std_logic;
		meas_clock          : in  std_logic;
		
		-- AXI4LITE Interface
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
		
		-- To GBT-SC
		reset_gbtsc         : out std_logic;
		start_reset         : out std_logic;
		start_connect       : out std_logic;
		start_command       : out std_logic;
		
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
                
        -- To GBT-IC
        ic_ready            : in  std_logic;
        ic_empty                : in  std_logic;
        
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
        rd_nb_of_words        : in  std_logic_vector(15 downto 0);
                
        -- Status
        delay_cnter_o       : out std_logic_vector(31 downto 0);
        wdt_error_o         : out std_logic;
        ready_o             : out std_logic
    );
end gbtsc_controller;

architecture Behavioral of gbtsc_controller is    
begin

    axi_intf_inst: entity work.gbtsc_axi4lite_intf
        generic map(
            C_S_AXI_DATA_WIDTH      => C_S_AXI_DATA_WIDTH,
            C_S_AXI_ADDR_WIDTH      => C_S_AXI_ADDR_WIDTH
        )
        port map(
            -- AXI4LITE Interface
            S_AXI_ACLK    		=> tx_clock,
            S_AXI_ARESETN 		=> S_AXI_ARESETN,
            S_AXI_AWADDR  		=> S_AXI_AWADDR,
            S_AXI_AWVALID 		=> S_AXI_AWVALID,
            S_AXI_AWREADY 		=> S_AXI_AWREADY,
            S_AXI_WDATA   		=> S_AXI_WDATA,
            S_AXI_WSTRB   		=> S_AXI_WSTRB,
            S_AXI_WVALID  		=> S_AXI_WVALID,
            S_AXI_WREADY  		=> S_AXI_WREADY,
            S_AXI_BRESP   		=> S_AXI_BRESP,
            S_AXI_BVALID  		=> S_AXI_BVALID,
            S_AXI_BREADY  		=> S_AXI_BREADY,
            S_AXI_ARADDR  		=> S_AXI_ARADDR,
            S_AXI_ARVALID 		=> S_AXI_ARVALID,
            S_AXI_ARREADY 		=> S_AXI_ARREADY,
            S_AXI_RDATA   		=> S_AXI_RDATA,
            S_AXI_RRESP   		=> S_AXI_RRESP,
            S_AXI_RVALID  		=> S_AXI_RVALID,
            S_AXI_RREADY  		=> S_AXI_RREADY,
            
            -- Data to GBT-SC State machine
            reset_gbtsc		    => reset_gbtsc,
            start_reset		    => start_reset,
            start_connect	    => start_connect,
            start_command		=> start_command,
            
            -- Data to GBT-SC DpRAM
            tx_ready_i          => '1',
            tx_address          => tx_address,
            tx_transID          => tx_transID,
            tx_channel          => tx_channel,
            tx_len              => tx_len,
            tx_command          => tx_command,
            tx_data             => tx_data,
            
            rx_reply_received_i => rx_reply_received_i,        
            rx_address          => rx_address,
            rx_transID          => rx_transID,
            rx_channel          => rx_channel,
            rx_len              => rx_len,
            rx_error            => rx_error,
            rx_data             => rx_data,
                    
            -- Data to GBT-IC State machine		
            ic_ready            => ic_ready,
            ic_empty            => ic_empty,
            
            -- Configuration
            GBTx_address        => GBTx_address,
            Register_addr       => Register_addr,
            nb_to_be_read       => nb_to_be_read,
            
            -- Control        
            start_write         => start_write,
            start_read          => start_read,
            
            -- WRITE register(s)
            data_o              => data_o,
            wr                  => wr,
            
            -- READ register(s)        
            data_i              => data_i,
            rd                  => rd,
                    
            rd_gbtx_addr        => rd_gbtx_addr,
            rd_mem_ptr          => rd_mem_ptr,
            rd_nb_of_words      => rd_nb_of_words
        );
    
end Behavioral;
