#ifndef _FONT_ABSTRACT_H
#define _FONT_ABSTRACT_H
#include <cstdint>

class fontAbstract
{
public:
	//virtual fontAbstract() =0;
	//virtual ~fontAbstract() =0;
	virtual uint8_t height() =0;
	virtual uint8_t width()  =0;
	virtual uint8_t varWidth(int b)  =0;
	virtual const char getcharbyte(int chr,int byte)=0;
	virtual uint8_t bytesHigh()  =0;
	virtual char startchar() =0;
};

#endif