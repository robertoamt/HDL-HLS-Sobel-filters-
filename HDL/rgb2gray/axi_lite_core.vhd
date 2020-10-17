-- ==============================================================
-- Vivado(TM) HLS - High-Level Synthesis from C, C++ and SystemC v2019.1 (64-bit)
-- Copyright 1986-2019 Xilinx, Inc. All Rights Reserved.
-- ==============================================================
library IEEE;
use IEEE.STD_LOGIC_1164.all;
use IEEE.NUMERIC_STD.all;

entity axi_lite_core is
generic (
    C_S_AXI_ADDR_WIDTH    : INTEGER := 4;
    C_S_AXI_DATA_WIDTH    : INTEGER := 32);
port (
    ACLK                  :in   STD_LOGIC;
    ARESET                :in   STD_LOGIC;
    AWADDR                :in   STD_LOGIC_VECTOR(C_S_AXI_ADDR_WIDTH-1 downto 0);
    AWVALID               :in   STD_LOGIC;
    AWREADY               :out  STD_LOGIC;
    WDATA                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH-1 downto 0);
    WSTRB                 :in   STD_LOGIC_VECTOR(C_S_AXI_DATA_WIDTH/8-1 downto 0);
    WVALID                :in   STD_LOGIC;
    WREADY                :out  STD_LOGIC;
    BRESP                 :out  STD_LOGIC_VECTOR(1 downto 0);
    BVALID                :out  STD_LOGIC;
    BREADY                :in   STD_LOGIC
);
end entity axi_lite_core;

architecture behave of axi_lite_core is

type axi_lite_rcv_fsm is (IDLE,RCV,RSP);
signal axi_lite_rcv : axi_lite_rcv_fsm;

begin

BRESP <= "00";

axi_lite_rcv_process: process(ACLK)
begin
    if rising_edge(ACLK) then
        if ARESET = '0' then
            AWREADY <= '0';  
            WREADY <= '0';  
        else
            case axi_lite_rcv is
                when IDLE => 
                    AWREADY <= '0';  
                    WREADY <= '0'; 
                    BVALID <= '0';
                    axi_lite_rcv <= RCV;
                when RCV =>
                    if AWVALID = '1' and WVALID = '1' then
                        AWREADY <= '1';  
                        WREADY <= '1';
                        BVALID <= '0';
                        axi_lite_rcv <= RSP;
                    end if;
                when RSP =>
                    if AWVALID = '1' and WVALID = '1' then
                        AWREADY <= '0';  
                        WREADY <= '0';
                        BVALID <= '1';
                        axi_lite_rcv <= IDLE;
                    end if;
            end case;
        end if;    
    end if;
end process;

end architecture behave;
