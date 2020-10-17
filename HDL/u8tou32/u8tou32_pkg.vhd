library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;


package u8tou32_pkg is

----------------------------Type declaration-------------------------------

type u8tou32_fsm_type is (START,RCV,JOIN,WAIT_READY,MISSED,SEND);
type FIFO_U32_TYPE is array (natural range <>) of std_logic_vector(31 downto 0);
type FIFO_U8_TYPE is array (natural range<>) of std_logic_vector(7 downto 0);

----------------------------Component declaration--------------------------
component axi_lite_core is
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
end component;

end package u8tou32_pkg;

-------------------------------------------------

package body u8tou32_pkg is



end package body u8tou32_pkg;
