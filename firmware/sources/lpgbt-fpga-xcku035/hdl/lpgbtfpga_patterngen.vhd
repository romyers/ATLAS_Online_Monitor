library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use ieee.numeric_std.all;

entity lpgbtfpga_patterngen is
    port(
        --clk40Mhz_Tx_i      : in  std_logic;
        clk320DnLink_i            : in  std_logic;
        clkEnDnLink_i             : in  std_logic;

        generator_rst_i           : in  std_logic;

        config_group0_i           : in  std_logic_vector(1 downto 0);
        config_group1_i           : in  std_logic_vector(1 downto 0);
        config_group2_i           : in  std_logic_vector(1 downto 0);
        config_group3_i           : in  std_logic_vector(1 downto 0);

        fixed_pattern_i           : in  std_logic_vector(31 downto 0);

        downlink_o                : out std_logic_vector(31 downto 0);

        eport_gen_rdy_o           : out std_logic_vector(15 downto 0)
    );
end lpgbtfpga_patterngen;

architecture rtl of lpgbtfpga_patterngen is

    -- EPORT generators
    component prbs7_2b_generator
        generic(
            INIT_c           : in std_logic_vector(7 downto 0)
        );
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            err_pattern_i    : in  std_logic_vector(7 downto 0);
            rep_delay_i      : in  std_logic_vector(23 downto 0);
            prbs_word_o      : out std_logic_vector(1 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    component prbs7_4b_generator
        generic(
            INIT_c           : in std_logic_vector(7 downto 0)
        );
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            err_pattern_i    : in  std_logic_vector(7 downto 0);
            rep_delay_i      : in  std_logic_vector(23 downto 0);
            prbs_word_o      : out std_logic_vector(3 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    component prbs7_8b_generator
        generic(
            INIT_c           : in std_logic_vector(7 downto 0)
        );
        port (
            reset_i          : in  std_logic;
            clk_i            : in  std_logic;
            clken_i          : in  std_logic;
            err_pattern_i    : in  std_logic_vector(7 downto 0);
            rep_delay_i      : in  std_logic_vector(23 downto 0);
            prbs_word_o      : out std_logic_vector(7 downto 0);
            rdy_o            : out std_logic
        );
    end component;

    type elink_80prbs_arr_T is array (integer range <>) of std_logic_vector(1 downto 0);
    type elink_160prbs_arr_T is array (integer range <>) of std_logic_vector(3 downto 0);
    type elink_320prbs_arr_T is array (integer range <>) of std_logic_vector(7 downto 0);

    signal reset_synch_s     : std_logic;
    signal prbs7_80data_s    : elink_80prbs_arr_T(15 downto 0);
    signal prbs7_160data_s   : elink_160prbs_arr_T(7 downto 0);
    signal prbs7_320data_s   : elink_320prbs_arr_T(3 downto 0);
    signal rst_dst_s         : std_logic;
    
begin

    reset_synch_proc: process(generator_rst_i, clk320DnLink_i)
        begin
            if generator_rst_i = '1' then
                rst_dst_s  <= '1';
                reset_synch_s <= '1';
            elsif rising_edge(clk320DnLink_i) then
                rst_dst_s  <= '0';
                reset_synch_s <= rst_dst_s;
            end if;
    end process;
    
    multi_elink_gen_80mbps: for i in 0 to 15 generate
        prbs7_2b_generator_inst: prbs7_2b_generator
            generic map(
                INIT_c           => x"a4"
            )
            port map(
                reset_i          => reset_synch_s,
                clk_i            => clk320DnLink_i,
                clken_i          => clkEnDnLink_i,
                err_pattern_i    => (others => '0'), -- No error injection
                rep_delay_i      => (others => '0'), -- No error injection
                prbs_word_o      => prbs7_80data_s(i),
                rdy_o            => eport_gen_rdy_o(i)
            );
    end generate;

    multi_elink_gen_160mbps: for i in 0 to 7 generate
        prbs7_2b_generator_inst: prbs7_4b_generator
            generic map(
                INIT_c           => x"a4"
            )
            port map(
                reset_i          => reset_synch_s,
                clk_i            => clk320DnLink_i,
                clken_i          => clkEnDnLink_i,
                err_pattern_i    => (others => '0'), -- No error injection
                rep_delay_i      => (others => '0'), -- No error injection
                prbs_word_o      => prbs7_160data_s(i),
                rdy_o            => eport_gen_rdy_o(i)
            );
    end generate;

    multi_elink_gen_320mbps: for i in 0 to 3 generate
        prbs7_2b_generator_inst: prbs7_8b_generator
            generic map(
                INIT_c           => x"a4"
            )
            port map(
                reset_i          => reset_synch_s,
                clk_i            => clk320DnLink_i,
                clken_i          => clkEnDnLink_i,
                err_pattern_i    => (others => '0'), -- No error injection
                rep_delay_i      => (others => '0'), -- No error injection
                prbs_word_o      => prbs7_320data_s(i),
                rdy_o            => eport_gen_rdy_o(i)
            );
    end generate;

    downlink_o(7 downto 0)   <= prbs7_320data_s(0) when config_group0_i = "11" else
                                prbs7_160data_s(1) & prbs7_160data_s(0) when config_group0_i = "10" else
                                prbs7_80data_s(3) & prbs7_80data_s(2) & prbs7_80data_s(1) & prbs7_80data_s(0) when config_group0_i = "01" else
                                fixed_pattern_i(7 downto 0);

    downlink_o(15 downto 8)  <= prbs7_320data_s(1) when config_group1_i = "11" else
                                prbs7_160data_s(3) & prbs7_160data_s(2) when config_group1_i = "10" else
                                prbs7_80data_s(7) & prbs7_80data_s(6) & prbs7_80data_s(5) & prbs7_80data_s(4) when config_group0_i = "01" else
                                fixed_pattern_i(15 downto 8);

    downlink_o(23 downto 16) <= prbs7_320data_s(2) when config_group2_i = "11" else
                                prbs7_160data_s(5) & prbs7_160data_s(4) when config_group2_i = "10" else
                                prbs7_80data_s(11) & prbs7_80data_s(10) & prbs7_80data_s(9) & prbs7_80data_s(8) when config_group0_i = "01" else
                                fixed_pattern_i(23 downto 16);

    downlink_o(31 downto 24) <= prbs7_320data_s(3) when config_group3_i = "11" else
                                prbs7_160data_s(7) & prbs7_160data_s(6) when config_group3_i = "10" else
                                prbs7_80data_s(15) & prbs7_80data_s(14) & prbs7_80data_s(13) & prbs7_80data_s(12) when config_group0_i = "01" else
                                fixed_pattern_i(31 downto 24);

end rtl;
