

#define REG_TX_FIFO_LEVEL 0x08 // TXLVL
#define REG_RXTX_FIFO 00 // RXH, THR
#define REG_FIFO_CONTROL 0x02 // FCR
#define REG_LINE_CONTROL 0x03 // LCR
#define REG_MODEM_CONTROL 0x04 // MCR
#define REG_IO_CONTROL 0x0E // IOControl
#define REG_INTERRUPT_ENABLE 0x01 // IER
#define REG_SIGNALED_INTERRUPT 0x02 // IIR
#define REG_RX_LEVEL 0x09 // RXLVL
#define REG_EXTRA_FEATURES_CONTROL 0x0f // EFCR. 

// Special register only avalabile when LCR[7] == 1
#define REGS_DIVIDER_LOW 0x00  
#define REGS_DIVIDER_HIGH 0x01

// Special registers only available when LCR = 0xBF
#define REGEH_ENHANCED_FEATURES 0x02 // EFR

// Line control register data
#define LCR_ENHANCED_MODE 0xbf // Required for setting some register values. 
#define LCR_EN_DIVISOR_LATCH 0x80
#define LCR_TX_BREAK 0x40
#define LCR_FORCE_PARITY 0x20
#define LCR_ODD_PARITY 0 
#define LCR_EVEN_PARITY 0x10
#define LCR_NO_PARITY 0x00
#define LCR_EN_PARITY 0x08
#define LCR_8_BIT_WORD_1_STOP 0x03

// Modem control register data. 
// Ones that start with MCREH require 'enhanced features' to be enabled. 
#define MCREH_DIVIDE_BY_1 0x00
#define MCREH_DIVIDE_BY_4 0x80 // affects baud rate. 
#define MCREH_IRDA_MODE 0x40
#define MCREH_XON 0x20
#define MCR_LOOPBACK 0x10
#define MCREH_EN_FIFO_TRIGGER 0x08 // enables tcr, tcl. 

// FIFO control register data
#define FCR_RX_TRIGGER_8_CHAR 0x00
#define FCR_RX_TRIGGER_16_CHAR 0x40
#define FCR_RX_TRIGGER_56_CHAR 0x80
#define FCR_RX_TRIGGER_60_CHAR 0xC0
#define FCR_TX_TRIGGER_8_CHAR 0x00
#define FCR_TX_TRIGGER_16_CHAR 0x10
#define FCR_TX_TRIGGER_56_CHAR 0x20
#define FCR_TX_TRIGGER_60_CHAR 0x30
#define FCR_RESET_TX_FIFO 0x04
#define FCR_RESET_RX_FIFO 0x02
#define FCR_ENABLE_FIFOS 0x01

// IO Control register data
#define IOCTRL_SW_RESET 0x08

// Interrupt enable data
#define IER_RX_FIFO_THRESHOLD 0x01 // Data in rx fifo. 
#define IER_TX_FIFO_THRESHOLD 0x02 // Tx fifo needs more data
#define IER_EN_SLEEP_MODE 0x10 // Requires advanced features to be enabled. 
#define IER_RX_STATUS 0x04

// Interrupt signal data
#define IIR_MASK 0x3f
#define IIR_RX_FIFO_THRESHOLD 0x04

#define EFCR_TXDISABLE 0x04
