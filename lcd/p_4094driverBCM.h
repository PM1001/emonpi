#ifndef _P_4094DRIVER_H_
#define _P_4094DRIVER_H_
extern "C"
{
#include "bcm2835.h"
}
#include <unistd.h>
#include <mutex>
#include <map>
#include <thread>

namespace piHardware{
	class p_4094driverBCM
	{
	public:
		p_4094driverBCM();
		~p_4094driverBCM();
		void SetBitRaw(int bit);
		void ClearBitRaw(int bit);
		void Set(int value,int cs=0);
		void SetBit(int gpo,int cs=0);
		void Clr(int value,int cs=0);
		void ClrBit(int gpo,int cs=0);
		void Toggle(int value,int cs=0);
		void ToggleBit(int gpo,int cs=0);
		void Pulse(int value,int cs=0);
		void PulseBit(int gpo,int cs=0);
		void WriteData(unsigned char,int cs=0);
		void WriteSpiRaw(unsigned char);
		/* data */
		std::unique_ptr<std::thread> pinInterrupt (int pin, int mode, void (*function)(void *), void *pHandlerThis);
		std::unique_ptr<std::thread> pinInterrupt (int pin, int mode, void (*function)(void));

		int waitForInterruptSys (int pin, int mode, int mS);
		enum{ INT_EDGE_SETUP,
			INT_EDGE_FALLING,
			INT_EDGE_RISING,
			INT_EDGE_BOTH,
		};
	private:
	//	int waitForInterruptSys (int pin, int mode, int mS);
		void interruptHandler (int myPin, int mode, void (*function)(void *), void *pHandlerThis);
		void interruptHandler (int myPin, int mode, void (*function)(void));

		std::map<int, unsigned char> data;
    	std::mutex mutex;
	};
}


#endif /*_P_4094DRIVER_H_*/