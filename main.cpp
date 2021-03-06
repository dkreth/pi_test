#include <stdlib.h>

#include <iostream>
#include <fstream>

#include <bcm2835.h>

#include<sys/time.h>

#define CTS_A 2
#define STE 8
#define EN_LDO_MSP 27

#define SPI_TEST
//#define MEM_TEST
//#define MEM_BACKWARDS_TEST
#define COMBO_TEST

#define SPI_TEST_FILE "/home/pi/pi_test/test_results/256blocks/SPI_TEST_results/SPI_TEST_results2.txt"
#define MEM_TEST_FILE "/home/pi/pi_test/test_results/256blocks/MEM_TEST_results/MEM_TEST_results2.txt"
#define MEM_BACKWARDS_TEST_FILE "/home/pi/pi_test/test_results/256blocks/MEM_TEST_results/MEM_BACKWARDS_TEST_results2.txt"
#define COMBO_TEST_FILE "/home/pi/pi_test/test_results/256blocks/COMBO_TEST_results/COMBO_TEST_results2.txt"

#define TRANSMITLEN (10000)
#define NUM_OPERATIONS (10000)

#define NUMBER_OF_TRIALS (100)//10 trials takes about 1 minute. 100 trials takes about 10 minutes.
#define SPI_CALL_LENGTH (256)
#define NUM_SPI_CALLS (75000)



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
    cout << "===Starting SPI_TEST===" << endl;
    setup_spi();
    ofstream spi_fileStream;
    spi_fileStream.open(SPI_TEST_FILE,ios::out);
    char send[TRANSMITLEN] = {};
    char received[TRANSMITLEN]={};
    for(int i=0;i<TRANSMITLEN;i++){
        send[i]=i;
    }
    struct timeval spi_start_time;
    struct timeval spi_end_time;

    for (int j=1; j<TRANSMITLEN; j*=2){
        cout << "block size " << j << endl;
        spi_fileStream << "block size " << j << endl;
        gettimeofday(&spi_start_time,NULL);
        for (int i=0; i<NUM_OPERATIONS; i++){
              bcm2835_spi_transfernb(send,received,j);
        }
        gettimeofday(&spi_end_time,NULL);
        float spi_timeElapsed = ((spi_end_time.tv_sec*1000000.0+spi_end_time.tv_usec)-(spi_start_time.tv_sec*1000000.0+spi_start_time.tv_usec))/1000000.0;

        float bitsPerSec = NUM_OPERATIONS*j*8/spi_timeElapsed;

        float megabitsPerSec = bitsPerSec/1000000;
        float megabytesPerSec = megabitsPerSec/8.0;
        cout << " Mb/s: " << megabitsPerSec << endl;
        spi_fileStream << " Mb/s: " << megabitsPerSec << endl;
        cout << " MB/s: " << megabytesPerSec << endl;
        spi_fileStream << " MB/s: " << megabytesPerSec << endl;
        float realToTheoretical = megabitsPerSec/50;
        cout << "realToTheoretical " << realToTheoretical << endl;
        spi_fileStream << "realToTheoretical " << realToTheoretical << endl;
    }

    spi_fileStream.close();
    bcm2835_spi_end();
    bcm2835_close();
    cout << "===SPI_TEST finished===" << endl;
#endif // SPI_TEST

#ifdef MEM_TEST
    cout << "===Starting MEM_TEST===" << endl;
    ofstream mem_resultsFile;
    mem_resultsFile.open(MEM_TEST_FILE,ios::out);
    if(!mem_resultsFile.is_open()){
        cout << "mem_resultsFile failed to open! Exiting program." << endl;
        return 1;
    }
    ofstream mem_fileStream;// for writing binary files

    int file_count_iterator = 0;
    char fileName[64];
    #define BUFSIZE 1024
    char buffer[BUFSIZE];
    for(int i=0;i<BUFSIZE;i++)buffer[i]=i;
    int num_files = 1;
    int file_size=256;
    struct timeval mem_start_time;
    struct timeval mem_end_time;
    for(;num_files<257; num_files*=2,file_size/=2){

        gettimeofday(&mem_start_time,NULL);
        for(int k=0;k<num_files;k++){//number of files
            file_count_iterator++;
            sprintf(fileName,"/media/pi/UPTIMEDRIVE1/test_output_bins/test%d.bin",file_count_iterator);
            mem_fileStream.open(fileName,ios::out|ios::binary);
            if(!mem_fileStream.is_open()){
                cout << "mem_fileStream failed to open! Exiting program." << endl;
                return 1;
            }
            for(int j=0;j<file_size*1024;j++){//size of each file
                mem_fileStream.write(buffer,BUFSIZE);
            }
            mem_fileStream.close();
        }
        gettimeofday(&mem_end_time,NULL);
        float mem_timeElapsed = ((mem_end_time.tv_sec*1000000.0+mem_end_time.tv_usec)-(mem_start_time.tv_sec*1000000.0+mem_start_time.tv_usec))/1000000.0;
        cout << "It took " << mem_timeElapsed << " seconds to write " << num_files << " " << file_size << "MB files, which is " << 256*8/mem_timeElapsed<< "Mbits per second." << endl;
        mem_resultsFile << "It took " << mem_timeElapsed << " seconds to write " << num_files << " " << file_size << "MB files, which is " << 256*8/mem_timeElapsed<< "Mbits per second." << endl;
    }
    mem_resultsFile.close();
    cout << "===MEM_TEST finished===" << endl;
#endif // MEM_TEST

#ifdef MEM_BACKWARDS_TEST
    cout << "===Starting MEM_BACKWARDS_TEST===" << endl;
    mem_resultsFile.open(MEM_BACKWARDS_TEST_FILE,ios::out);
    if(!mem_resultsFile.is_open()){
        cout << "mem_resultsFile for backwards test failed to open! Exiting program." << endl;
        return 1;
    }


    file_count_iterator = 0;
    #define BUFSIZE 1024
    for(int i=0;i<BUFSIZE;i++)buffer[i]=i;
    num_files = 256;
    file_size=1;
    for(;file_size<257; num_files/=2,file_size*=2){

        gettimeofday(&mem_start_time,NULL);
        for(int k=0;k<num_files;k++){//number of files
            file_count_iterator++;
            sprintf(fileName,"/media/pi/UPTIMEDRIVE1/test_output_bins/test%d.bin",file_count_iterator);
            mem_fileStream.open(fileName,ios::out|ios::binary);
            if(!mem_resultsFile.is_open()){
                cout << "memfileStream for backwards test failed to open! Exiting program." << endl;
                return 1;
            }
            for(int j=0;j<file_size*1024;j++){//size of each file
                mem_fileStream.write(buffer,BUFSIZE);
            }
            mem_fileStream.close();
        }
        gettimeofday(&mem_end_time,NULL);
        float mem_timeElapsed = ((mem_end_time.tv_sec*1000000.0+mem_end_time.tv_usec)-(mem_start_time.tv_sec*1000000.0+mem_start_time.tv_usec))/1000000.0;
        cout << "It took " << mem_timeElapsed << " seconds to write " << num_files << " " << file_size << "MB files, which is " << 256*8/mem_timeElapsed<< "Mbits per second." << endl;
        mem_resultsFile << "It took " << mem_timeElapsed << " seconds to write " << num_files << " " << file_size << "MB files, which is " << 256*8/mem_timeElapsed<< "Mbits per second." << endl;
    }
    mem_resultsFile.close();
    cout << "===MEM_BACKWARDS_TEST finished===" << endl;
#endif // MEM_BACKWARDS_TEST

#ifdef COMBO_TEST
    cout << "===Starting COMBO_TEST===" << endl;
    ofstream combo_resultsFile;
    combo_resultsFile.open(COMBO_TEST_FILE,ios::out);
    if(!combo_resultsFile.is_open()){
        cout << "combo_resultsFile failed to open! Exiting program." << endl;
        return 1;
    }
    setup_spi();
    char combo_send[SPI_CALL_LENGTH]={};
    char combo_received[SPI_CALL_LENGTH]={};
    for (int i=0; i<SPI_CALL_LENGTH; i++){//fill the array to send through the SPI
        combo_send[i]=i;
    }
    float speeds[NUMBER_OF_TRIALS];
    char combo_fileName[64];
    struct timeval combo_start_time;
    struct timeval combo_end_time;
    for(int trialNumber=0;trialNumber<NUMBER_OF_TRIALS; trialNumber++){
        gettimeofday(&combo_start_time,NULL);
        ofstream combo_fileStream;
        sprintf(combo_fileName,"/media/pi/UPTIMEDRIVE1/new_output_bins/test%d.bin",trialNumber);
        combo_fileStream.open(combo_fileName,ios::out|ios::binary);
        if(!combo_fileStream.is_open()){
            cout << "combo_fileStream failed to open! Exiting program." << endl;
            return 1;
        }
        for(int i=0;i<NUM_SPI_CALLS;i++){
            bcm2835_spi_transfernb(combo_send, combo_received, SPI_CALL_LENGTH);
            combo_fileStream.write(combo_received,SPI_CALL_LENGTH);
        }
        combo_fileStream.close();
        gettimeofday(&combo_end_time,NULL);
        float combo_timeElapsed = ((combo_end_time.tv_sec*1000000.0+combo_end_time.tv_usec)-(combo_start_time.tv_sec*1000000.0+combo_start_time.tv_usec))/1000000.0;
        float megabitsPerSec = SPI_CALL_LENGTH*NUM_SPI_CALLS/combo_timeElapsed/1024/1024*8;
        cout << "trial " << trialNumber << ": " << megabitsPerSec << " Mb/s" << endl;
        combo_resultsFile << megabitsPerSec << endl;
        speeds[trialNumber] = megabitsPerSec;
    }
    float averageSpeed=0;
    for (int i=0;i<NUMBER_OF_TRIALS;i++){
        averageSpeed+=speeds[i];
    }
    averageSpeed/=NUMBER_OF_TRIALS;
    cout <<"average speed: " << averageSpeed << "Mb/s"<<endl;
    combo_resultsFile <<"average speed: " << averageSpeed << "Mb/s"<<endl;
    combo_resultsFile.close();

    bcm2835_spi_end();
    bcm2835_close();
    cout << "===COMBO_TEST finished===" << endl;
#endif // COMBO_TEST


}
