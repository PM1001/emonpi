#include "p_4094driverBCM.h"
#include <ctime>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>

#include <poll.h>
#include <cstring>
#include <cstdio>
#include <iostream>

using namespace piHardware;


p_4094driverBCM::p_4094driverBCM(){


}

p_4094driverBCM::~p_4094driverBCM(){    
}

void p_4094driverBCM::ClearBitRaw(int bit){
	bcm2835_gpio_clr(bit);
}

void p_4094driverBCM::SetBitRaw(int bit){
	bcm2835_gpio_set(bit);
}

void p_4094driverBCM::Set(int value,int cs){
	int data = 0;
	if(this->data.count(cs)){
		data = this->data[cs];
	}
	this->WriteData(data|((unsigned char)value),cs);
}

void p_4094driverBCM::SetBit(int gpo,int cs){
	this->Set(1<<gpo,cs);
}

void p_4094driverBCM::Clr(int value,int cs){
	int data = 0;
	if(this->data.count(cs)){
		data = this->data[cs];
	}
	this->WriteData(data&(((unsigned char)value)^0xFF),cs);
}

void p_4094driverBCM::ClrBit(int gpo,int cs){
	this->Clr(1<<gpo,cs);	
}

void p_4094driverBCM::Toggle(int value,int cs){
	int data = 0;
	if(this->data.count(cs)){
		data = this->data[cs];
	}
	this->WriteData(data^(((unsigned char)value)),cs);
}

void p_4094driverBCM::ToggleBit(int gpo,int cs){
	this->Toggle(1<<gpo,cs);	
}

void p_4094driverBCM::Pulse(int value,int cs){
	this->Toggle(value,cs);
	usleep(100);
	this->Toggle(value,cs);
}

void p_4094driverBCM::PulseBit(int gpo,int cs){
	this->Pulse(1<<gpo,cs);	
}

void p_4094driverBCM::WriteData(unsigned char data,int gpio){
    std::lock_guard<std::mutex> guard(mutex);

	std::this_thread::sleep_for (std::chrono::microseconds(20));
   	//bcm2835_gpio_clr(8);
	bcm2835_spi_transfer(data);

   	bcm2835_gpio_set(gpio);
	std::this_thread::sleep_for (std::chrono::microseconds(20));
   	bcm2835_gpio_clr(gpio);
	this->data[gpio] = data;
}

void p_4094driverBCM::WriteSpiRaw(unsigned char data){
    std::lock_guard<std::mutex> guard(mutex);
	bcm2835_spi_transfer(data);
	//std::this_thread::sleep_for (std::chrono::microseconds(1));
}
int p_4094driverBCM::waitForInterruptSys (int pin,int mode, int mS)
{

    int level =  bcm2835_gpio_lev(pin);
    while(level == bcm2835_gpio_lev(pin))
  		std::this_thread::sleep_for (std::chrono::microseconds(mS));

    int newLevel =  bcm2835_gpio_lev(pin);
    if (level==newLevel){
    	return -1;
    }

    if (mode==INT_EDGE_FALLING){
    	if ((level==1)&&(newLevel==0))
    		return 1;
    }
    else if (mode==INT_EDGE_RISING){
    	if ((level==0)&&(newLevel==1))
    		return 1;
    }
    else
    {
    	return 1;
    }
 	return -1 ;
}


void p_4094driverBCM::interruptHandler (int myPin, int mode, void (*function)(void*), void *pHandlerThis)
{  	
	std::cout<<"interrupt handler: "<<myPin<<'\n';
  	for (;;)
    	if (p_4094driverBCM::waitForInterruptSys (myPin,mode, 100) > 0){
#ifndef NDEBUG
    		std::cout<<"interrupt calling function for pin: "<<myPin<<'\n';
#endif
      		(*function)(pHandlerThis) ;
    	}

  	return ;
}
void p_4094driverBCM::interruptHandler (int myPin, int mode, void (*function)(void))
{

  	std::cout<<"interrupt handler: "<<myPin<<'\n';
  	for (;;)
    	if (p_4094driverBCM::waitForInterruptSys (myPin,mode, 100) > 0){
#ifndef NDEBUG
    		std::cout<<"interrupt calling function for pin: "<<myPin<<'\n';
#endif
      		(*function)() ;
    	}

  	return ;
}


std::unique_ptr<std::thread> p_4094driverBCM::pinInterrupt (int pin, int mode, void (*function)(void*), void *pHandlerThis)
{

	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
#ifndef NDEBUG
  	std::cout<<"Starting thread "<<pin<<':'<<mode<<'\n';
#endif
  //	std::unique_ptr<std::thread> interruptThread{new std::thread(&p_4094driverBCM::interruptHandler,this, pin,mode,function,pHandlerThis)} ;
#ifndef NDEBUG
  	std::cout<<"Thread launched "<<pin<<':'<<mode<<'\n';
#endif
  //std::this_thread::sleep_for (std::chrono::milliseconds(1));
  	//interruptThread.detach();
  return nullptr;//interruptThread;
}

std::unique_ptr<std::thread> p_4094driverBCM::pinInterrupt (int pin, int mode, void (*function)(void))
{

	bcm2835_gpio_fsel(pin, BCM2835_GPIO_FSEL_INPT);
#ifndef NDEBUG
  	std::cout<<"Starting thread "<<pin<<':'<<mode<<'\n';
#endif
  //	std::unique_ptr<std::thread> interruptThread{new std::thread(&p_4094driverBCM::interruptHandler,this, pin,mode,function)} ;
#ifndef NDEBUG
  	std::cout<<"Thread launched "<<pin<<':'<<mode<<'\n';
#endif
  //std::this_thread::sleep_for (std::chrono::milliseconds(1));
  	//interruptThread.detach();
  return nullptr;// interruptThread;
}

#if 0
int p_4094driverBCM::waitForInterruptSys (int pin, int mS)
{
  int fd, x ;
  uint8_t c ;
  struct pollfd polls ;
  int sysFd;

// Setup poll structure

  polls.fd     = fd ;
  polls.events = POLLPRI ;	// Urgent data!

// Wait for it ...

  std::cout<<"waiting for pin "<<pin<<'\n';
  x = poll (&polls, 1, mS) ;

// Do a dummy read to clear the interrupt
//	A one character read appars to be enough.

  std::cout<<"intterrupt: "<<pin<<'\n';
  char fName   [64] ;
  std::sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
  if ((sysFd = open (fName, O_RDWR)) < 0)
    return -1 ;
  (void)read (fd, &c, 1) ;

  return x ;
}


void p_4094driverBCM::interruptHandler (int myPin, void (*function)(void))
{

  std::cout<<"intterrupt handler: "<<myPin<<'\n';
  for (;;)
    if (p_4094driverBCM::waitForInterruptSys (myPin, -1) > 0)
      (*function)() ;

  return ;
}



int p_4094driverBCM::pinInterrupt (int pin, int mode, void (*function)(void))
{
  pthread_t threadId ;
  char fName   [64] ;
  char *modeS ;
  char  pinS [8] ;
  pid_t pid ;
  int   count, i ;
  uint8_t c ;
  int sysFd =-1;

  pin &= 63 ;

// Now export the pin and set the right edge
//	We're going to use the gpio program to do this, so it assumes
//	a full installation of wiringPi. It's a bit 'clunky', but it
//	is a way that will work when we're running in "Sys" mode, as
//	a non-root user. (without sudo)

  if (mode != INT_EDGE_SETUP)
  {
    /**/ if (mode == INT_EDGE_FALLING)
      modeS = "falling" ;
    else if (mode == INT_EDGE_RISING)
      modeS = "rising" ;
    else
      modeS = "both" ;

    std::sprintf (pinS, "%d", pin) ;

    if ((pid = fork ()) < 0){	// Fail
  	  std::cout<<"No fork "<<pinS<<':'<<modeS<<'\n';
      return pid ;
    }

    if (pid == 0)	// Child, exec
    {
      execl ("/usr/local/bin/gpio", "gpio", "edge", pinS, modeS, (char *)NULL) ;
  	  std::cout<<"execl failure /usr/local/bin/gpio"<<pinS<<':'<<modeS<<" errno: "<<errno<<':'<<strerror(errno)<<'\n';
      return -1 ;	// Failure ...
    }
    else		// Parent, wait
  	  std::cout<<"Parent wait "<<pinS<<':'<<modeS<<'\n';
      wait (NULL) ;
  }

// Now pre-open the /sys/class node - it may already be open if
//	we are in Sys mode, but this will do no harm.

  std::sprintf (fName, "/sys/class/gpio/gpio%d/value", pin) ;
  if ((sysFd = open (fName, O_RDWR)) < 0){
  	std::cout<<"No open: "<<fName<<'\n';
    return -1 ;
  }

// Clear any initial pending interrupt

  std::cout<<"ioctl "<<pinS<<':'<<modeS<<'\n';
  ioctl (sysFd, FIONREAD, &count) ;
  for (i = 0 ; i < count ; ++i)
    read (sysFd, &c, 1) ;

  std::cout<<"Starting thread "<<pinS<<':'<<modeS<<'\n';
  std::thread interruptThread(&p_4094driverBCM::interruptHandler,this, pin,function) ;

  std::cout<<"Detaching thread "<<pinS<<':'<<modeS<<'\n';
  //std::this_thread::sleep_for (std::chrono::milliseconds(1));
  interruptThread.detach();
  return 1 ;
}
#endif