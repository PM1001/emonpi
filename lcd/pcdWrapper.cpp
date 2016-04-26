
#include "PCD8544.h"
#include "p_4094driverBCM.h"
#include <cstdio>
#include <iostream>
#include <ctime>
#include <thread>
#include "fontLiberationSans14x14.h"

#define LCD_STROBE 8
#define LCD_DC 25
#define LCD_RESET 24
using namespace piHardware;
class pcdWrapper{
public:
	pcdWrapper(){
		if (!bcm2835_init())
	    	throw "error initializing bcm2835, are you root? \n";
	    bcm2835_spi_begin();
		std::cout<<"SPI set";
	    bcm2835_spi_setBitOrder(BCM2835_SPI_BIT_ORDER_LSBFIRST);      // The default
	    bcm2835_spi_setDataMode(BCM2835_SPI_MODE0);                   // The default
	    bcm2835_spi_setClockDivider(BCM2835_SPI_CLOCK_DIVIDER_32); // The default
	    bcm2835_spi_chipSelect(BCM2835_SPI_CS1);                      // The default
	    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS1, LOW);      // the default
	    bcm2835_spi_setChipSelectPolarity(BCM2835_SPI_CS2, HIGH);      // the default
		bcm2835_gpio_fsel(LCD_STROBE, BCM2835_GPIO_FSEL_OUTP); //override SPI pin as output - strobe
		bcm2835_gpio_fsel(LCD_RESET, BCM2835_GPIO_FSEL_OUTP); //override SPI pin as output - strobe
		bcm2835_gpio_fsel(LCD_DC, BCM2835_GPIO_FSEL_OUTP); //override SPI pin as output - strobe
	    bcm2835_gpio_set(LCD_STROBE);
	    bcm2835_gpio_clr(LCD_RESET);
	    bcm2835_gpio_set(LCD_DC);

		bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5);
	    bcm2835_pwm_set_clock(BCM2835_PWM_CLOCK_DIVIDER_16);
	    bcm2835_pwm_set_mode(0,1,1);
	    bcm2835_pwm_set_range(0,255);
	    bcm2835_pwm_set_data(0,0);


		std::cout<<"All set, init p_4094driverBCM";
		this->oman4094 = new p_4094driverBCM();
		std::cout<<"init PCD8544";
		this->lcdDriver = new PCD8544(this->oman4094,LCD_STROBE,LCD_DC,LCD_RESET);
		this->lcdDriver->clear();
		this->lcdDriver->setCursor(0,0);
		//this->lcdDriver->print_string("EmonLCD B");
		this->lcdDriver->display();
	    //std::this_thread::sleep_for (std::chrono::milliseconds(100));
	};
	~pcdWrapper(){
		delete this->oman4094;
		delete this->lcdDriver;
	}
	void lcd_display_line(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1){
		this->lcdDriver->drawline(x0,y0,x1,y1,1);
		this->lcdDriver->display();
	}
	void lcd_display_string_big(const char* text,int line){
    	fontLiberationSans14x14 largeFont{};
		this->lcdDriver->fillrect(0,8*line,84,16,0);
		this->lcdDriver->setCursor(0,8*line);
		this->lcdDriver->print_string(text,&largeFont);
		this->lcdDriver->display();
	}
	void lcd_display_string(const char* text,int line){
		this->lcdDriver->fillrect(0,8*line,84,8,0);
		this->lcdDriver->setCursor(0,8*line);
		this->lcdDriver->print_string(text);
		this->lcdDriver->display();
	}
	void backlight(int brightness){
	    	bcm2835_pwm_set_data(0,brightness);
	}
	void intro(){
		this->lcdDriver->setCursor(0,0);
		this->lcdDriver->print_string("EmonLCD");
		for (int i = 0; i < 15; ++i)
		{
			this->lcdDriver->fillcircle(42,24,i+1,1);
			this->lcdDriver->display();
	    	std::this_thread::sleep_for (std::chrono::milliseconds(100));
	    	bcm2835_pwm_set_data(0,60*i);
		}
	}
	void clear(){
		this->lcdDriver->clear();
	}

private:
	p_4094driverBCM* oman4094;
	PCD8544* lcdDriver;	

};

