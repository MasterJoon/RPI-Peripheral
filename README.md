# This project is about Raspberry Pi Peripheral Library in C language.
## This library has some features.
- Not using device module (ex) /dev/ttyS1  
  Instad, using **/dev/mem** and **mmap** fuction, directly access in peripheral register memory.
- About UART, you can set baud rate up to 6,000,000 bps.  
  Also, you can check the CT(transmition complete) flag and handle the DE(Driver Enable) signal in half-duplex serial such as RS-485.
- This library guarantees very regular occupancy time in high priority thread such as real-time task. (suitable for Xenomai project)