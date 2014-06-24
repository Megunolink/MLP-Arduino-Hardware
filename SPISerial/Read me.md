SPI Serial
==========

This library is for the SC16IS750 UART operating in SPI mode. It uses interrupts for receiving data but, currently not sending data. The chip has a 64 byte FIFO (for both reading and sending) so it doesn't really seem worth doing interrupt handling for transmitting. 

A break-out board is available from [SparkFun](https://www.sparkfun.com/products/9981). Note that as of 2014-06-24, the SparkFun board has a mistake. Although the IRQ pin is available at the edge of the board, it isn't actually connected. You need to add a small wire from the resistor near the /IRQ to the connection itself. The wire should connect the /IRQ output to the side of the resistor nearest the chip. The other side of the resistor is V+ so that won't help. 

