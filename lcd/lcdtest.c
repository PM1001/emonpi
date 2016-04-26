#include "pcdWrapper.cpp"
#include <string.h>
#include <cstdio>
#include <chrono>
#include <ctime>
    using std::chrono::system_clock;

extern "C"{
	pcdWrapper* NewLCD() {std::cerr<<"Attempting to create pcdWrapper2\n";try{return new pcdWrapper{};}
		catch(const std::runtime_error& re){std::cerr << "Runtime error: " << re.what() << std::endl;}
		catch(const std::exception& ex){std::cerr << "Error occurred: " << ex.what() << std::endl;}
		catch(...){std::cerr << "Unknown failure occured. Possible memory corruption" << std::endl;}}
	void lcd_display_string(pcdWrapper* lcd,const char* text,int line)
		{lcd->lcd_display_string(text,line);}
	void backlight(pcdWrapper* lcd,int brightness)
		{lcd->backlight(brightness);}
}

int main(int argc, char const *argv[])
{
	while(1){
	pcdWrapper* lcd = NewLCD();
	lcd->clear();
	lcd->intro();
		std::time_t tt = system_clock::to_time_t (system_clock::now());
		struct std::tm * ptm = std::localtime(&tt);	ptm->tm_sec+=2;
		std::this_thread::sleep_until (system_clock::from_time_t (mktime(ptm)));
	lcd->clear();
	lcd_display_string(lcd,"Testing 1",0);
	lcd_display_string(lcd,"Testing 2",1);
	lcd_display_string(lcd,"Testing 3",2);
	lcd_display_string(lcd,"Testing 4",3);
	lcd_display_string(lcd,"Testing 5",4);
	lcd_display_string(lcd,"Testing 6",5);
		tt= system_clock::to_time_t (system_clock::now());
		ptm = std::localtime(&tt);	ptm->tm_sec+=5;
		std::this_thread::sleep_until (system_clock::from_time_t (mktime(ptm)));
	delete lcd;
	}
	std::cerr<<"testing complete\n";
	return 0;
}

