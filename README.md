# pi_test
This report aims show the feasibility of using a Raspberry Pi to capture SPI data and write it to a disk without any data loss, and measure the speed at which it can do so. The report is broken down into 3 sections:
1) Measuring speed of capturing data with the SPI interface with varying block sizes
2) Measuring speed of writing data to the Raspberry Pi with different file sizes
3) Measuring speed of doing both operations—SPI capture and write to disk—in series
## Specs
* Compiler: GNU GCC Compiler
* Hardware: Raspberry Pi 3 B+, OS installed on 16GB SanDisk microSD card, 8GB USB A drive
* OS: Raspbian
* Libraries used: stdlib.h, iostream, fstream, bcm2835.h, sys/time.h
## Summary of Results
* Maximum clock speed that could reliably be used was the 8x clock divider (50MHz on Raspberry Pi 3). Any faster would make the Pi glitch or miss data
* Max speed of SPI transfer was approximately 32 Mb/s and could be accomplished with blocks of 256 bytes. Larger blocks increased latency without affecting speed. CPU usage for SPI_TEST was 20-25%.
* No clear difference in write speed could be determined between writing files of 1MB and 256 MB. CPU usage for MEM_TEST was 5-8%, with occassional fluctuations to 15%.
* Performing a SPI transfer followed by writing the data to a binary file can be performed reliably (without data loss) at a rate of about 23.6 Mb/s. CPU usage for COMBO_TEST was 20-25%.
