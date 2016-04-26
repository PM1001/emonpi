#include "pcdWrapper.cpp"
#include <string.h>
#include <cstdio>

extern "C"{
	pcdWrapper* NewLCD() {std::cerr<<"Attempting to create pcdWrapper2\n";try{pcdWrapper* lcd = new pcdWrapper{};std::cerr<<"P: "<<lcd<<"\n";return lcd;}
		catch(const std::runtime_error& re){std::cerr << "Runtime error: " << re.what() << std::endl;}
		catch(const std::exception& ex){std::cerr << "Error occurred: " << ex.what() << std::endl;}
		catch(...){std::cerr << "Unknown failure occured. Possible memory corruption" << std::endl;}}	
	void lcd_display_line(pcdWrapper* lcd,uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
		{lcd->lcd_display_line( x0,y0, x1, y1);}
	void lcd_display_string_big(pcdWrapper* lcd,const char* text,int line)
		{lcd->lcd_display_string_big(text,line);}
	void lcd_display_string(pcdWrapper* lcd,const char* text,int line)
		{lcd->lcd_display_string(text,line);}
	void backlight(pcdWrapper* lcd,int brightness)
		{lcd->backlight(brightness);}
}