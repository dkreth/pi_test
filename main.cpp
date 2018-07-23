#include <stdlib.h>

#include <iostream>
#include <string>
#include <algorithm>
#include <fstream>

#include <climits>
#include <numeric>
#include <functional>

#include <wiringPi.h>
#include <bcm2835.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <errno.h>
#include <stdio.h>
#include <vector>

#include <sstream>
#include <string.h>

#include<iomanip>
#include<time.h>
#include<sys/time.h>

#define CTS_A 2
#define STE 8
#define EN_LDO_MSP 27
#define SPI_CLK 11

#define PI_BUFFER_SIZE 65536*16
#define COMMAND_BUFFER_SIZE 16

#define SERIAL_NUMBER_FILE "/home/pi/serial.txt"

#define SPI_TEST

#define TRANSMITLEN (10000)
#define NUM_OPERATIONS (10000)



void setup_spi(){
    bcm2835_init();
    bcm2835_spi_begin();
    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);
    bcm2835_spi_chipSelect(BCM2835_SPI_CS0);
    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_8);

    bcm2835_gpio_fsel(CTS_A, BCM2835_GPIO_FSEL_INPT);
    bcm2835_gpio_fsel(STE, BCM2835_GPIO_FSEL_INPT);

    // Allows for Pi to connect to radio board pins
    bcm2835_gpio_fsel(EN_LDO_MSP, BCM2835_GPIO_FSEL_OUTP);
    bcm2835_gpio_set(EN_LDO_MSP);
}

using namespace std;

int main()
{

#ifdef SPI_TEST
    setup_spi();
    ofstream fileStream;
    fileStream.open("/media/pi/UPTIMEDRIVE1/pi_test/SPI_TEST_results.txt",ios::out);
    char send[TRANSMITLEN] = {};
    char received[TRANSMITLEN]={};
    for(int i=0;i<TRANSMITLEN;i++){
        send[i]=i;
    }
    struct timeval spi_start_time;
    struct timeval spi_end_time;

    for (int j=1; j<TRANSMITLEN; j*=2){
        fileStream << "block size " << j << endl;
        gettimeofday(&spi_start_time,NULL);
        for (int i=0; i<NUM_OPERATIONS; i++){
              bcm2835_spi_transfernb(send,received,j);
        }
        gettimeofday(&spi_end_time,NULL);
        float spi_timeElapsed = ((spi_end_time.tv_sec*1000000.0+spi_end_time.tv_usec)-(spi_start_time.tv_sec*1000000.0+spi_start_time.tv_usec))/1000000.0;

        float bitsPerSec = NUM_OPERATIONS*j*8/spi_timeElapsed;

        float megabitsPerSec = bitsPerSec/1000000;
        float megabytesPerSec = megabitsPerSec/8.0;
        fileStream << " Mb/s: " << megabitsPerSec << endl;
        fileStream << " MB/s: " << megabytesPerSec << endl;
        float realToTheoretical = megabitsPerSec/50;
        fileStream << "realToTheoretical " << realToTheoretical << endl;
    }

    fileStream.close();
    bcm2835_spi_end();
    bcm2835_close();
#endif // SPI_TEST

}
