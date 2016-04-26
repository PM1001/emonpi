
#ifndef FONT_LIBERATION_SANS14X14_ALPHA_H_
#define FONT_LIBERATION_SANS14X14_ALPHA_H_
#include "fontAbstract.h"


class fontLiberationSans14x14:public fontAbstract
{
public:
	 fontLiberationSans14x14(){;}
	~fontLiberationSans14x14(){;}

	uint8_t height()	{return 16;}
	uint8_t width()		{return 14;}
    uint8_t varWidth(int b) {return (this->getcharbyte(b,-1));}
	const char getcharbyte(int chr,int byte)	{
                const uint8_t bytes_per_char = this->width() * this->bytesHigh() + 1; /* The +1 is the width byte at the start */
                return fontLiberationSans14x14::fontMem[(chr-this->startchar()) * bytes_per_char+byte+1];}
    uint8_t bytesHigh(){
        if ((this->height() % 8) > 0){
                return((this->height() / 8) + 1);
        }
        else{
                return(this->height() / 8);
        }}
    char startchar() {return (48);}
private:
	static const char  fontMem[];
};


#endif
