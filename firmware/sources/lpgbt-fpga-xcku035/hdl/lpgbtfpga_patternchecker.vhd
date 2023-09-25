-- IEEE VHDL standard library:
library ieee;
use ieee.std_logic_1164.all;

package bus_multiplexer_pkg is
    type conf2b_array is array(natural range <>) of std_logic_vector(1 downto 0);
end package;
    
--! Xilinx devices library:
library unisim;
use unisim.vcomponents.all;

library ieee;
use ieee.std_logic_1164.all;
use ieee.numeric_std.all;
use work.bus_multiplexer_pkg.all;

entity lpgbtfpga_patternchecker is
    port (
        reset_checker_i  : in  std_logic;
        ser320_clk_i     : in  std_logic;
        ser320_clkEn_i   : in  std_logic;

        data_rate_i      : in  std_logic;

        elink_config_i   : in  conf2b_array(27 downto 0);

        error_detected_o : out std_logic_vector(27 downto 0);

        userDataUpLink_i : in  std_logic_vector(229 downto 0)
    );
end lpgbtfpga_patternchecker;

architecture rtl of lpgbtfpga_patternchecker is

    component prbs7_32b_checker
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            prbs_word_i      : in  std_logic_vector(31 downto 0);
            err_o            : out std_logic_vector(31 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    component prbs7_16b_checker
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            prbs_word_i      : in  std_logic_vector(15 downto 0);
            err_o            : out std_logic_vector(15 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    component prbs7_8b_checker
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            prbs_word_i      : in  std_logic_vector(7 downto 0);
            err_o            : out std_logic_vector(7 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    component prbs7_4b_checker
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            prbs_word_i      : in  std_logic_vector(3 downto 0);
            err_o            : out std_logic_vector(3 downto 0);
            rdy_o            : out std_logic
        );
    end component;


  component amt_gtx_decode_320mbps is      
       Port (
               user_rst                                                   : in std_logic;
               RESET_I                                                    : in  std_logic;             
               RX_FRAMECLK_I                                              : in  std_logic;     
               RX_DATA_I                                                  : in  std_logic_vector(229 downto 0);                      
               RX_EXTRA_DATA_WIDEBUS_I                                    : in  std_logic_vector(31 downto 0);                 
               GBT_RX_READY_I                                             : in std_logic
          );
  end component;
  
  
    type elink_1g28_T        is array(integer range <>) of std_logic_vector(31 downto 0);
    type elink_640m_T        is array(integer range <>) of std_logic_vector(15 downto 0);
    type elink_320m_T        is array(integer range <>) of std_logic_vector(7 downto 0);
    type elink_160m_T        is array(integer range <>) of std_logic_vector(3 downto 0);
    type bert_cnter_T        is array(integer range <>) of std_logic_vector(63 downto 0);
    type err_cnter_T         is array(integer range <>) of std_logic_vector(5 downto 0);
    type cnt_arr_T           is array(integer range <>) of unsigned(31 downto 0);

    signal elink_1g28_s      : elink_1g28_T(6 downto 0);   -- Maximum of 7 links @ 1.28gbps
    signal elink_640m_s      : elink_640m_T(14 downto 0);  -- Maximum of 14 links @ 640Mbps
    signal elink_320m_s      : elink_320m_T(27 downto 0);  -- Maximum of 28 links @ 320Mbps
    signal elink_160m_s      : elink_160m_T(27 downto 0);  -- Maximum of 28 links @ 160Mbps

    signal elink_1g28_err_s  : elink_1g28_T(27 downto 0);
    signal elink_640m_err_s  : elink_640m_T(27 downto 0);
    signal elink_320m_err_s  : elink_320m_T(27 downto 0);
    signal elink_160m_err_s  : elink_160m_T(27 downto 0);

    signal elink_err_s       : elink_1g28_T(27 downto 0);
    signal cntr_rst_s        : std_logic_vector(27 downto 0);
    signal elink_prbs_rdy    : std_logic_vector(27 downto 0);

    signal elink_1g28_prbsRdy_s  : std_logic_vector(27 downto 0);
    signal elink_640m_prbsRdy_s  : std_logic_vector(27 downto 0);
    signal elink_320m_prbsRdy_s  : std_logic_vector(27 downto 0);
    signal elink_160m_prbsRdy_s  : std_logic_vector(27 downto 0);

    signal reset320_s        : std_logic;
    signal rst_dst_s         : std_logic;
begin

    reset_synch_proc: process(reset_checker_i, ser320_clk_i)
        begin
            if reset_checker_i = '1' then
                rst_dst_s  <= '1';
                reset320_s <= '1';
            elsif rising_edge(ser320_clk_i) then
                rst_dst_s  <= '0';
                reset320_s <= rst_dst_s;
            end if;
    end process;
    
    elink_1g28_s(0)     <= userDataUpLink_i(31 downto 0);
    elink_1g28_s(1)     <= userDataUpLink_i(63 downto 32);
    elink_1g28_s(2)     <= userDataUpLink_i(95 downto 64);
    elink_1g28_s(3)     <= userDataUpLink_i(127 downto 96);
    elink_1g28_s(4)     <= userDataUpLink_i(159 downto 128);
    elink_1g28_s(5)     <= userDataUpLink_i(191 downto 160);
    elink_1g28_s(6)     <= userDataUpLink_i(223 downto 192);

    elink_640m_s(0)     <= userDataUpLink_i(15 downto 0);
    elink_640m_s(1)     <= userDataUpLink_i(31 downto 16) when data_rate_i = '1' else (others => '0');
    elink_640m_s(2)     <= userDataUpLink_i(47 downto 32) when data_rate_i = '1' else userDataUpLink_i(31 downto 16);
    elink_640m_s(3)     <= userDataUpLink_i(63 downto 48) when data_rate_i = '1' else (others => '0');
    elink_640m_s(4)     <= userDataUpLink_i(79 downto 64) when data_rate_i = '1' else userDataUpLink_i(47 downto 32);
    elink_640m_s(5)     <= userDataUpLink_i(95 downto 80) when data_rate_i = '1' else (others => '0');
    elink_640m_s(6)     <= userDataUpLink_i(111 downto 96)  when data_rate_i = '1' else userDataUpLink_i(63 downto 48);
    elink_640m_s(7)     <= userDataUpLink_i(127 downto 112) when data_rate_i = '1' else (others => '0');
    elink_640m_s(8)     <= userDataUpLink_i(143 downto 128)  when data_rate_i = '1' else userDataUpLink_i(79 downto 64);
    elink_640m_s(9)     <= userDataUpLink_i(159 downto 144)  when data_rate_i = '1' else (others => '0');
    elink_640m_s(10)     <= userDataUpLink_i(175 downto 160) when data_rate_i = '1' else userDataUpLink_i(95 downto 80);
    elink_640m_s(11)     <= userDataUpLink_i(191 downto 176) when data_rate_i = '1' else (others => '0');
    elink_640m_s(12)     <= userDataUpLink_i(207 downto 192) when data_rate_i = '1' else userDataUpLink_i(111 downto 96);
    elink_640m_s(13)     <= userDataUpLink_i(223 downto 208) when data_rate_i = '1' else (others => '0');

    elink_320m_s(0)     <= userDataUpLink_i(7 downto 0);
    elink_320m_s(1)     <= userDataUpLink_i(15 downto 8)  when data_rate_i = '1' else (others => '0');
    elink_320m_s(2)     <= userDataUpLink_i(23 downto 16) when data_rate_i = '1' else userDataUpLink_i(15 downto 8);
    elink_320m_s(3)     <= userDataUpLink_i(31 downto 24) when data_rate_i = '1' else (others => '0');
    elink_320m_s(4)     <= userDataUpLink_i(39 downto 32) when data_rate_i = '1' else userDataUpLink_i(23 downto 16);
    elink_320m_s(5)     <= userDataUpLink_i(47 downto 40) when data_rate_i = '1' else (others => '0');
    elink_320m_s(6)     <= userDataUpLink_i(55 downto 48) when data_rate_i = '1' else userDataUpLink_i(31 downto 24);
    elink_320m_s(7)     <= userDataUpLink_i(63 downto 56) when data_rate_i = '1' else (others => '0');
    elink_320m_s(8)     <= userDataUpLink_i(71 downto 64) when data_rate_i = '1' else userDataUpLink_i(39 downto 32);
    elink_320m_s(9)     <= userDataUpLink_i(79 downto 72) when data_rate_i = '1' else (others => '0');
    elink_320m_s(10)     <= userDataUpLink_i(87 downto 80) when data_rate_i = '1' else userDataUpLink_i(47 downto 40);
    elink_320m_s(11)     <= userDataUpLink_i(95 downto 88) when data_rate_i = '1' else (others => '0');
    elink_320m_s(12)     <= userDataUpLink_i(103 downto 96)  when data_rate_i = '1' else userDataUpLink_i(55 downto 48);
    elink_320m_s(13)     <= userDataUpLink_i(111 downto 104) when data_rate_i = '1' else (others => '0');
    elink_320m_s(14)    <= userDataUpLink_i(119 downto 112)  when data_rate_i = '1' else userDataUpLink_i(63 downto 56);
    elink_320m_s(15)     <= userDataUpLink_i(127 downto 120) when data_rate_i = '1' else (others => '0');
    elink_320m_s(16)     <= userDataUpLink_i(135 downto 128) when data_rate_i = '1' else userDataUpLink_i(71 downto 64);
    elink_320m_s(17)     <= userDataUpLink_i(143 downto 136) when data_rate_i = '1' else (others => '0');
    elink_320m_s(18)     <= userDataUpLink_i(151 downto 144) when data_rate_i = '1' else userDataUpLink_i(79 downto 72);
    elink_320m_s(19)     <= userDataUpLink_i(159 downto 152) when data_rate_i = '1' else (others => '0');
    elink_320m_s(20)     <= userDataUpLink_i(167 downto 160) when data_rate_i = '1' else userDataUpLink_i(87 downto 80);
    elink_320m_s(21)     <= userDataUpLink_i(175 downto 168) when data_rate_i = '1' else (others => '0');
    elink_320m_s(22)     <= userDataUpLink_i(183 downto 176) when data_rate_i = '1' else userDataUpLink_i(95 downto 88);
    elink_320m_s(23)     <= userDataUpLink_i(191 downto 184) when data_rate_i = '1' else (others => '0');
    elink_320m_s(24)     <= userDataUpLink_i(199 downto 192) when data_rate_i = '1' else userDataUpLink_i(103 downto 96);
    elink_320m_s(25)     <= userDataUpLink_i(207 downto 200) when data_rate_i = '1' else (others => '0');
    elink_320m_s(26)     <= userDataUpLink_i(215 downto 208) when data_rate_i = '1' else userDataUpLink_i(111 downto 104);
    elink_320m_s(27)     <= userDataUpLink_i(223 downto 216) when data_rate_i = '1' else (others => '0');

    elink_160m_s(0)     <= userDataUpLink_i(3 downto 0);
    elink_160m_s(1)     <= userDataUpLink_i(7 downto 4);
    elink_160m_s(2)     <= userDataUpLink_i(11 downto 8);
    elink_160m_s(3)     <= userDataUpLink_i(15 downto 12);
    elink_160m_s(4)     <= userDataUpLink_i(19 downto 16);
    elink_160m_s(5)     <= userDataUpLink_i(23 downto 20);
    elink_160m_s(6)     <= userDataUpLink_i(27 downto 24);
    elink_160m_s(7)     <= userDataUpLink_i(31 downto 28);
    elink_160m_s(8)     <= userDataUpLink_i(35 downto 32);
    elink_160m_s(9)     <= userDataUpLink_i(39 downto 36);
    elink_160m_s(10)     <= userDataUpLink_i(43 downto 40);
    elink_160m_s(11)     <= userDataUpLink_i(47 downto 44);
    elink_160m_s(12)     <= userDataUpLink_i(51 downto 48);
    elink_160m_s(13)     <= userDataUpLink_i(55 downto 52);
    elink_160m_s(14)    <= userDataUpLink_i(59 downto 56);
    elink_160m_s(15)     <= userDataUpLink_i(63 downto 60);
    elink_160m_s(16)     <= userDataUpLink_i(67 downto 64);
    elink_160m_s(17)     <= userDataUpLink_i(71 downto 68);
    elink_160m_s(18)     <= userDataUpLink_i(75 downto 72);
    elink_160m_s(19)     <= userDataUpLink_i(79 downto 76);
    elink_160m_s(20)     <= userDataUpLink_i(83 downto 80);
    elink_160m_s(21)     <= userDataUpLink_i(87 downto 84);
    elink_160m_s(22)     <= userDataUpLink_i(91 downto 88);
    elink_160m_s(23)     <= userDataUpLink_i(95 downto 92);
    elink_160m_s(24)     <= userDataUpLink_i(99 downto 96);
    elink_160m_s(25)     <= userDataUpLink_i(103 downto 100);
    elink_160m_s(26)     <= userDataUpLink_i(107 downto 104);
    elink_160m_s(27)     <= userDataUpLink_i(111 downto 108);

   
   
    dataCheck_TDCpattern_inst: amt_gtx_decode_320mbps                
            port map (
                      user_rst                                       => reset_checker_i, -- '1' reset
                      RESET_I                                        => '0',         
                      RX_FRAMECLK_I                                  => ser320_clk_i,  -- 40MHz from top
                      -----------------------------------------------------------------------------------           
                      RX_DATA_I                                      => userDataUpLink_i,        
                      RX_EXTRA_DATA_WIDEBUS_I                        => (others => '0'),
                      -----------------------------------------------------------------------------------           
                      GBT_RX_READY_I                                 => '1'
                      );   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
   
    multi_elink_1g28_gen: for i in 0 to 6 generate
        prbs7_32b_checker_inst: prbs7_32b_checker
            port map(
                reset_i          => reset320_s,
                clk_i            => ser320_clk_i,
                clken_i          => ser320_clkEn_i,
                prbs_word_i      => elink_1g28_s(i),
                err_o            => elink_1g28_err_s(i*4),
                rdy_o            => elink_1g28_prbsRdy_s(i*4)
            );
    end generate;

    elink_1g28_prbsRdy_s(3 downto 1) <= "000";
    elink_1g28_prbsRdy_s(7 downto 5) <= "000";
    elink_1g28_prbsRdy_s(11 downto 9) <= "000";
    elink_1g28_prbsRdy_s(15 downto 13) <= "000";
    elink_1g28_prbsRdy_s(19 downto 17) <= "000";
    elink_1g28_prbsRdy_s(23 downto 21) <= "000";
    elink_1g28_prbsRdy_s(27 downto 25) <= "000";

    multi_elink_640mbps_gen: for i in 0 to 13 generate
        prbs7_16b_checker_inst: prbs7_16b_checker
            port map(
                reset_i          => reset320_s,
                clk_i            => ser320_clk_i,
                clken_i          => ser320_clkEn_i,
                prbs_word_i      => elink_640m_s(i),
                err_o            => elink_640m_err_s(i*2),
                rdy_o            => elink_640m_prbsRdy_s(i*2)
            );
    end generate;

    elink_640m_prbsRdy_s(1) <= '0';
    elink_640m_prbsRdy_s(3) <= '0';
    elink_640m_prbsRdy_s(5) <= '0';
    elink_640m_prbsRdy_s(7) <= '0';
    elink_640m_prbsRdy_s(9) <= '0';
    elink_640m_prbsRdy_s(11) <= '0';
    elink_640m_prbsRdy_s(13) <= '0';
    elink_640m_prbsRdy_s(15) <= '0';
    elink_640m_prbsRdy_s(17) <= '0';
    elink_640m_prbsRdy_s(19) <= '0';
    elink_640m_prbsRdy_s(21) <= '0';
    elink_640m_prbsRdy_s(23) <= '0';
    elink_640m_prbsRdy_s(25) <= '0';
    elink_640m_prbsRdy_s(27) <= '0';

    multi_elink_320mbps_gen: for i in 0 to 27 generate
        prbs7_8b_checker_inst: prbs7_8b_checker
            port map(
                reset_i          => reset320_s,
                clk_i            => ser320_clk_i,
                clken_i          => ser320_clkEn_i,
                prbs_word_i      => elink_320m_s(i),
                err_o            => elink_320m_err_s(i),
                rdy_o            => elink_320m_prbsRdy_s(i)
            );
    end generate;

    multi_elink_160mbps_gen: for i in 0 to 27 generate
        prbs7_4b_checker_inst: prbs7_4b_checker
            port map(
                reset_i          => reset320_s,
                clk_i            => ser320_clk_i,
                clken_i          => ser320_clkEn_i,
                prbs_word_i      => elink_160m_s(i),
                err_o            => elink_160m_err_s(i),
                rdy_o            => elink_160m_prbsRdy_s(i)
            );
    end generate;

    err_cnter_gen: for i in 0 to 27 generate

        elink_err_s(i) <= elink_1g28_err_s(i) when elink_config_i(i) = "11" else
                          x"0000" & elink_640m_err_s(i) when elink_config_i(i) = "10" else
                          x"000000" & elink_320m_err_s(i) when elink_config_i(i) = "01" else
                          x"0000000" & elink_160m_err_s(i);

        elink_prbs_rdy(i) <= elink_1g28_prbsRdy_s(i) when elink_config_i(i) = "11" else
                             elink_640m_prbsRdy_s(i) when elink_config_i(i) = "10" else
                             elink_320m_prbsRdy_s(i) when elink_config_i(i) = "01" else
                             elink_160m_prbsRdy_s(i);

        error_detected_o(i)  <= '0' when elink_err_s(i) = x"00000000" else '1';

    end generate;


end rtl;