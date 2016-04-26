/**
 * @file PCD8544.h
 * @brief Hitachi PCD8544 Driver (Header file)
 *
 */

#ifndef __PCD8544_H__
#define __PCD8544_H__
#include "p_4094driverBCM.h"
#include "fontAbstract.h"
 
#include <ctime>

namespace piHardware{
	class PCD8544
	{
	public:
		PCD8544(p_4094driverBCM* outDriv_,int strobeLine, int dcLine, int resetLine);
		~PCD8544();

		void init(uint8_t contrast);
		void updateBoundingBox(uint8_t xmin, uint8_t ymin, uint8_t xmax, uint8_t ymax);
		void drawbitmap(uint8_t x, uint8_t y,const uint8_t *bitmap, uint8_t w, uint8_t h,uint8_t color);
		void drawstring(uint8_t x, uint8_t y, const char *c);
		void drawchar(uint8_t x, uint8_t y, char c);
		uint8_t drawchar(uint8_t x, uint8_t y, char c,fontAbstract* font_current);
		void write(const char c,fontAbstract* font=nullptr);
		void setCursor(uint8_t x, uint8_t y);
		void drawline(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t color);
		void fillrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h,  uint8_t color);
		void drawrect(uint8_t x, uint8_t y, uint8_t w, uint8_t h, uint8_t color);
		void drawcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
		void fillcircle(uint8_t x0, uint8_t y0, uint8_t r, uint8_t color);
		void setPixel(uint8_t x, uint8_t y, uint8_t color);
		void togglePixel(uint8_t x, uint8_t y);
		uint8_t getPixel(uint8_t x, uint8_t y);
		void spiWrite(uint8_t LCDout);
		inline void command(uint8_t c);
		inline void data(uint8_t c);
		void setContrast(uint8_t val);
		void display(void);
		void clear(void);

		inline void print_byte(unsigned char b,fontAbstract* font=nullptr){this->write(b,font);}
		inline void print_string(const char c[],fontAbstract* font=nullptr){while(*c){this->write(*c++,font);}}
		
		void printTime(time_t t,fontAbstract* font=nullptr);
		void printRunTime(time_t t,time_t end,fontAbstract* font=nullptr);
		void printInt(int num,int pad=0);
	private:
		inline void swap(uint8_t a, uint8_t b) { uint8_t t = a; a = b; b = t; }
		int strobe;
		int dc;
		int reset;
		unsigned char contrast;
		unsigned char col;
		unsigned char row;
		int cursor_y;
		int cursor_x;
		unsigned char textsize;
		unsigned char textcolor;
 
		p_4094driverBCM* outDriv;

#ifdef enablePartialUpdate
		uint8_t xUpdateMin, xUpdateMax, yUpdateMin, yUpdateMax;
#endif

		static const unsigned char LCDWIDTH  = 84;
		static const unsigned char LCDHEIGHT = 48;

		// the memory buffer for the LCD
		uint8_t displayBuffer[LCDWIDTH * LCDHEIGHT / 8] = {};
				// commands
		static const unsigned char BLACK = 1;
		static const unsigned char WHITE = 0;

		static const unsigned char PCD8544_POWERDOWN = 0x04;
		static const unsigned char PCD8544_ENTRYMODE = 0x02;
		static const unsigned char PCD8544_EXTENDEDINSTRUCTION = 0x01;

		static const unsigned char PCD8544_DISPLAYBLANK     = 0x0;
		static const unsigned char PCD8544_DISPLAYNORMAL    = 0x4;
		static const unsigned char PCD8544_DISPLAYALLON     = 0x1;
		static const unsigned char PCD8544_DISPLAYINVERTED  = 0x5;

		// H = 0
		static const unsigned char PCD8544_FUNCTIONSET   = 0x20;
		static const unsigned char PCD8544_DISPLAYCONTROL  = 0x08;
		static const unsigned char PCD8544_SETYADDR = 0x40;
		static const unsigned char PCD8544_SETXADDR = 0x80;

		// H = 1
		static const unsigned char PCD8544_SETTEMP  = 0x04;
		static const unsigned char PCD8544_SETBIAS  = 0x10;
		static const unsigned char PCD8544_SETVOP   = 0x80;

				// font bitmap
		static const unsigned char  font[];
	};
}
#endif // __H__
