#include <iostream>

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
    cout << "Hello world!" << endl;
    return 0;
}
