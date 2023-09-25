library ieee;
use ieee.std_logic_1164.all;
use ieee.std_logic_unsigned.all;
use ieee.numeric_std.all;

entity prbs7_32b_checker is
    port (
        reset_i          : in  std_logic;
        clk_i            : in  std_logic;
        clken_i          : in  std_logic;
        prbs_word_i      : in  std_logic_vector(31 downto 0);
        err_o            : out std_logic_vector(31 downto 0);
        rdy_o            : out std_logic
    );
end prbs7_32b_checker;

architecture rtl of prbs7_32b_checker is
    signal feedback_reg         : std_logic_vector(31 downto 0);
    signal err_s                : std_logic_vector(31 downto 0);
    
    type checker_state_T is (waitForLock, Locked);
    signal status : checker_state_T;
                                
    constant STATS_CONFIG_c     : integer := 10;
    signal cnt_stats            : integer range 0 to STATS_CONFIG_c;

begin

    checker_fsm_proc: process(reset_i, clk_i)
    begin
        if reset_i = '1' then
            status <= waitForLock;
            cnt_stats <= 0;
            
        elsif rising_edge(clk_i) then
            case status is
                when waitForLock =>                                
                    if cnt_stats= STATS_CONFIG_c then
                        status <= Locked;
                    else
                        if err_s = x"00000000" and feedback_reg /= x"00000000" then
                            cnt_stats <= cnt_stats + 1;
                        else
                            cnt_stats <= 0;
                        end if;
                    end if;
                
                when Locked => null;
            end case;
        end if;
    end process;
    
    -- PRBS7 equation: x^7 + x^6 + 1
    -- LSB first
    prbs7_proc: process(reset_i, clk_i)
        variable cnter : integer range 0 to 1;
    begin

        if reset_i = '1' then
            feedback_reg <= (others => '0');
            err_s        <= x"00000000";

        elsif rising_edge(clk_i) then

            if clken_i = '1' then
                
                err_s(0) <= prbs_word_i(0) xor (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                err_s(1) <= prbs_word_i(1) xor (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                err_s(2) <= prbs_word_i(2) xor (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                err_s(3) <= prbs_word_i(3) xor (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                err_s(4) <= prbs_word_i(4) xor (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                err_s(5) <= prbs_word_i(5) xor (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                err_s(6) <= prbs_word_i(6) xor (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                err_s(7) <= prbs_word_i(7) xor (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                err_s(8) <= prbs_word_i(8) xor (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                err_s(9) <= prbs_word_i(9) xor (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                err_s(10) <= prbs_word_i(10) xor (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                err_s(11) <= prbs_word_i(11) xor (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                err_s(12) <= prbs_word_i(12) xor (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                err_s(13) <= prbs_word_i(13) xor (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                err_s(14) <= prbs_word_i(14) xor (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                err_s(15) <= prbs_word_i(15) xor (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                err_s(16) <= prbs_word_i(16) xor (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                err_s(17) <= prbs_word_i(17) xor (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                err_s(18) <= prbs_word_i(18) xor (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                err_s(19) <= prbs_word_i(19) xor (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                err_s(20) <= prbs_word_i(20) xor (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                err_s(21) <= prbs_word_i(21) xor (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                err_s(22) <= prbs_word_i(22) xor (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                err_s(23) <= prbs_word_i(23) xor (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                err_s(24) <= prbs_word_i(24) xor (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                err_s(25) <= prbs_word_i(25) xor (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                err_s(26) <= prbs_word_i(26) xor (feedback_reg(1) xor feedback_reg(0));
                err_s(27) <= prbs_word_i(27) xor (feedback_reg(2) xor feedback_reg(1));
                err_s(28) <= prbs_word_i(28) xor (feedback_reg(3) xor feedback_reg(2));
                err_s(29) <= prbs_word_i(29) xor (feedback_reg(4) xor feedback_reg(3));
                err_s(30) <= prbs_word_i(30) xor (feedback_reg(5) xor feedback_reg(4));
                err_s(31) <= prbs_word_i(31) xor (feedback_reg(6) xor feedback_reg(5));
                
                if status /= Locked then                    
                    feedback_reg <= prbs_word_i;
                else
                    feedback_reg(0) <= (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(1) <= (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                    feedback_reg(2) <= (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                    feedback_reg(3) <= (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                    feedback_reg(4) <= (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                    feedback_reg(5) <= (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                    feedback_reg(6) <= (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(7) <= (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                    feedback_reg(8) <= (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                    feedback_reg(9) <= (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                    feedback_reg(10) <= (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                    feedback_reg(11) <= (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                    feedback_reg(12) <= (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(13) <= (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                    feedback_reg(14) <= (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                    feedback_reg(15) <= (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                    feedback_reg(16) <= (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                    feedback_reg(17) <= (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                    feedback_reg(18) <= (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(19) <= (feedback_reg(1) xor feedback_reg(0) xor feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                    feedback_reg(20) <= (feedback_reg(2) xor feedback_reg(1) xor feedback_reg(1) xor feedback_reg(0));
                    feedback_reg(21) <= (feedback_reg(3) xor feedback_reg(2) xor feedback_reg(2) xor feedback_reg(1));
                    feedback_reg(22) <= (feedback_reg(4) xor feedback_reg(3) xor feedback_reg(3) xor feedback_reg(2));
                    feedback_reg(23) <= (feedback_reg(5) xor feedback_reg(4) xor feedback_reg(4) xor feedback_reg(3));
                    feedback_reg(24) <= (feedback_reg(6) xor feedback_reg(5) xor feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(25) <= (feedback_reg(0) xor feedback_reg(6) xor feedback_reg(5));
                    feedback_reg(26) <= (feedback_reg(1) xor feedback_reg(0));
                    feedback_reg(27) <= (feedback_reg(2) xor feedback_reg(1));
                    feedback_reg(28) <= (feedback_reg(3) xor feedback_reg(2));
                    feedback_reg(29) <= (feedback_reg(4) xor feedback_reg(3));
                    feedback_reg(30) <= (feedback_reg(5) xor feedback_reg(4));
                    feedback_reg(31) <= (feedback_reg(6) xor feedback_reg(5));
                end if;

                
            end if;
            
        end if;

    end process;
    
    err_o <= err_s;
    rdy_o <= '1' when status = Locked else '0';
    
end rtl;