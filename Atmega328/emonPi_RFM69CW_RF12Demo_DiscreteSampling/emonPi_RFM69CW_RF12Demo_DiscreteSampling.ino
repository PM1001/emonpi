/*
  
  emonPi  Discrete Sampling 
  
  If AC-AC adapter is detected assume emonPi is also powered from adapter (jumper shorted) and take Real Power Readings and disable sleep mode to keep load on power supply constant
  If AC-AC addapter is not detected assume powering from battereis / USB 5V AC sample is not present so take Apparent Power Readings and enable sleep mode
  
  Transmitt values via RFM69CW radio
  
   ------------------------------------------
  Part of the openenergymonitor.org project
  
  Authors: Glyn Hudson & Trystan Lea 
  Builds upon JCW JeeLabs RF12 library and Arduino 
  
  Licence: GNU GPL V3

*/

/*Recommended node ID allocation
------------------------------------------------------------------------------------------------------------
-ID-	-Node Type- 
0	- Special allocation in JeeLib RFM12 driver - reserved for OOK use
1-4     - Control nodes 
5-10	- Energy monitoring nodes
11-14	--Un-assigned --
15-16	- Base Station & logging nodes
17-30	- Environmental sensing nodes (temperature humidity etc.)
31	- Special allocation in JeeLib RFM12 driver - Node31 can communicate with nodes on any network group
-------------------------------------------------------b------------------------------------------------------


Change Log:
V1.0 First release 
V1.1 Add sample from both CT's as default and set 110V VRMS for apparent power when US calibration is set e.g '2p'


*/

#define emonTxV3                                                      // Tell emonLib this is the emonPi V3 - don't read Vcc assume Vcc = 3.3V as is always the case on emonPi eliminates bandgap error and need for calibration http://harizanov.com/2013/09/thoughts-on-avr-adc-accuracy/
#define RF69_COMPAT 1                                                 // Set to 1 if using RFM69CW or 0 is using RFM12B

#include <JeeLib.h>                                                   // https://github.com/jcw/jeelib - Tested with JeeLib 3/11/14
#include <avr/pgmspace.h>
#include <util/parity.h>
ISR(WDT_vect) { Sleepy::watchdogEvent(); }                            // Attached JeeLib sleep function to Atmega328 watchdog -enables MCU to be put into sleep mode inbetween readings to reduce power consumption 

#include "EmonLib.h"                                                  // Include EmonLib energy monitoring library https://github.com/openenergymonitor/EmonLib
EnergyMonitor ct1, ct2, ct3, ct4;       

const byte firmware_version = 12;                                    //firmware version x 10 e.g 10 = V1.0 / 1 = V0.1

//----------------------------emonPi Settings---------------------------------------------------------------------------------------------------------------
boolean debug =                   0; 
const unsigned long BAUD_RATE=    38400;

const byte Vrms_EU=               230;                               // Vrms for apparent power readings (when no AC-AC voltage sample is present)
const byte Vrms_USA=              110;                               // USA apparent power VRMS  
const int TIME_BETWEEN_READINGS=  5000;                             // Time between readings (mS)  
const int RF_RESET_PERIOD=        60000;                            // Time (ms) between RF resets (hack to keep RFM60CW alive) 


//http://openenergymonitor.org/emon/buildingblocks/calibration

const float Ical1=                90.9;                             // (2000 turns / 22 Ohm burden) = 90.9
const float Ical2=                90.9;                                 
float Vcal_EU=                    203.77;                             // (230V x 13) / (9V x 1.2) = 276.9 Calibration for UK AC-AC adapter 77DB-06-09 
//const float Vcal=               260;                                // Calibration for EU AC-AC adapter 77DE-06-09 
const float Vcal_USA=             130.0;                              // Calibration for US AC-AC adapter 77DA-10-09
boolean USA=                      0; 


const float phase_shift=          1.7;
const int no_of_samples=          1480; 
const byte no_of_half_wavelengths= 20;
const int timeout=                2000;                               // emonLib timeout 
const int ACAC_DETECTION_LEVEL=   3000;

const byte TEMPERATURE_PRECISION=  12;                                 // 9 (93.8ms),10 (187.5ms) ,11 (375ms) or 12 (750ms) bits equal to resplution of 0.5C, 0.25C, 0.125C and 0.0625C
boolean RF_STATUS=                 0;                                  // Turn RF on and off
//-------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------


//----------------------------emonPi V3 hard-wired connections--------------------------------------------------------------------------------------------------------------- 
const byte LEDpin=                     9;              // emonPi LED - on when HIGH
const byte shutdown_switch_pin =       8;              // Push-to-make - Low when pressed
const byte emonpi_GPIO_pin=            5;              // Connected to Pi GPIO 17, used to activate Pi Shutdown when HIGH
//const byte emonpi_OKK_Tx=              6;            // On-off keying transmission Pin - not populated by default 
//const byte emonPi_RJ45_8_IO=           A6;           // RJ45 pin 8 - Analog 6 (D19) - Aux I/O
const byte emonPi_int1=                1;              // RJ45 pin 6 - INT1 - PWM - Dig 3 - default pulse count input
const byte emonPi_int1_pin=            3;              // RJ45 pin 6 - INT1 - PWM - Dig 3 - default pulse count input
//const byte emonPi_int0=                2;            // Default RFM INT (Dig2) - Can be jumpered used JP5 to RJ45 pin 7 - PWM - D2
//-------------------------------------------------------------------------------------------------------------------------------------------


//-----------------------RFM12B / RFM69CW SETTINGS----------------------------------------------------------------------------------------------------
byte RF_freq=RF12_433MHZ;                                        // Frequency of RF69CW module can be RF12_433MHZ, RF12_868MHZ or RF12_915MHZ. You should use the one matching the module you have.
byte nodeID = 15;                                                 // emonpi node ID
int networkGroup = 210;  

// create JeeLabs RF packet structure - a neat way of packaging data for RF comms
typedef struct { 
int power1;
int power2;
int sampleTime;                                               
int Vrms;                                             
int powerFactor1;                                           
int powerFactor2;                                           
int Irms1;                                         
int Irms2; 
int power3;
int power4;
int powerFactor3;                                           
int powerFactor4;                                           
int Irms3;                                         
int Irms4; 
} PayloadTX;     
PayloadTX emonPi; 

//-------------------------------------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------------------------------------


//Global Variables Energy Monitoring 
double Vcal, vrms;
boolean CT1, CT2,CT3,CT4, ACAC, DS18B20_STATUS;
byte CT_count, Vrms;                                             
unsigned long last_sample=0;                                     // Record millis time of last discrete sample
byte flag;                                                       // flag to record shutdown push button press
volatile byte pulseCount = 0;       
unsigned long last_rf_rest=0;                                  // Record time of last RF reset

// RF Global Variables 
static byte stack[RF12_MAXDATA+4], top, sendLen, dest;           // RF variables 
static char cmd;
static word value;                                               // Used to store serial input
long unsigned int start_press=0;                                 // Record time emonPi shutdown push switch is pressed

const char helpText1[] PROGMEM =                                 // Available Serial Commands 
"\n"
"Available commands:\n"
"  <nn> i     - set node IDs (standard node ids are 1..30)\n"
"  <n> b      - set MHz band (4 = 433, 8 = 868, 9 = 915)\n"
"  <nnn> g    - set network group (RFM12 only allows 212, 0 = any)\n"
"  <n> c      - set collect mode (advanced, normally 0)\n"
"  ...,<nn> a - send data packet to node <nn>, request ack\n"
"  ...,<nn> s - send data packet to node <nn>, no ack\n"
"  ...,<n> p  - Set AC Adapter Vcal 1p = UK, 2p = USA\n"
"  v          - Show firmware version\n"
;

//-------------------------------------------------------------------------------------------------------------------------------------------
// SETUP ********************************************************************************************
//-------------------------------------------------------------------------------------------------------------------------------------------
void setup()
{ 
  delay(100);
  if (USA==1) 
  {
    Vcal = Vcal_USA;                                                       // Assume USA AC/AC adatper is being used, set calibration accordingly 
    Vrms = Vrms_USA;
  }
  else 
  {
    Vcal = Vcal_EU;
    Vrms = Vrms_EU;
  }
  
  emonPi_startup();                                                     // emonPi startup proceadure, check for AC waveform and print out debug
  if (RF_STATUS==1) RF_Setup(); 
  delay(1500);  
  CT_Detect();
  serial_print_startup();

  //emonPi.pulseCount = 0;                                                  // Reset Pulse Count 
   
  ct1.current(1, Ical1);                                     // CT ADC channel 1, calibration.  calibration (2000 turns / 22 Ohm burden resistor = 90.909)
  ct2.current(2, Ical2);                                     // CT ADC channel 2, calibration.
  ct3.current(4, Ical1);                                     // CT ADC channel 3, calibration.  calibration (2000 turns / 22 Ohm burden resistor = 90.909)
  ct4.current(5, Ical2);                                     // CT ADC channel 4, calibration.

  if (ACAC)                                                           //If AC wavefrom has been detected 
  {
    ct1.voltage(0, Vcal, phase_shift);                       // ADC pin, Calibration, phase_shift
    ct2.voltage(0, Vcal, phase_shift);                       // ADC pin, Calibration, phase_shift
    ct3.voltage(0, Vcal, phase_shift);                       // ADC pin, Calibration, phase_shift
    ct4.voltage(0, Vcal, phase_shift);                       // ADC pin, Calibration, phase_shift
  }
 
} //end setup


//-------------------------------------------------------------------------------------------------------------------------------------------
// LOOP ********************************************************************************************
//-------------------------------------------------------------------------------------------------------------------------------------------
void loop()
{
 
  if (USA==1) 
  {
    Vcal = Vcal_USA;                                                       // Assume USA AC/AC adatper is being used, set calibration accordingly 
    Vrms = Vrms_USA;
  }
  else 
  {
    Vcal = Vcal_EU;
    Vrms = Vrms_EU;
  }

  if (digitalRead(shutdown_switch_pin) == 0 ) 
    digitalWrite(emonpi_GPIO_pin, HIGH);                                          // if emonPi shutdown butten pressed then send signal to the Pi on GPIO 11
  else 
    digitalWrite(emonpi_GPIO_pin, LOW);
  
  if (Serial.available()){
    handleInput(Serial.read());                                                   // If serial input is received
    double_LED_flash();
  }                                             
      
  if (RF_STATUS==1){                                                              // IF RF module is present and enabled then perform RF tasks
    if (RF_Rx_Handle()==1) {                                                      // Returns true if RF packet is received                                            
       double_LED_flash(); 
    }
    
    send_RF();                                                                    // Transmitt data packets if needed 
    
    if ((millis()  - last_rf_rest) > RF_RESET_PERIOD) {
      rf12_initialize(nodeID, RF_freq, networkGroup);                             // Periodically reset RFM69CW to keep it alive :-( 
      last_rf_rest=millis();
    }
   
  }
   
  if ((millis() - last_sample) > TIME_BETWEEN_READINGS)
  {
    single_LED_flash();                                                            // single flash of LED on local CT sample
    
    if (ACAC)                                                                      // Read from CT 1
    {
      ct1.calcVI(no_of_half_wavelengths,timeout); 
      emonPi.power1=ct1.realPower;
      emonPi.Vrms=ct1.Vrms*100;
    }
    else 
    {
      emonPi.power1 = ct1.calcIrms(no_of_samples)*Vrms;                               // Calculate Apparent Power 1  1480 is  number of samples
      emonPi.Vrms=Vrms*100;
    }  
    emonPi.Irms1=ct1.Irms*100;
    emonPi.powerFactor1=ct1.powerFactor*100;
  
   if (ACAC)                                                                       // Read from CT 2
   {
     ct2.calcVI(no_of_half_wavelengths,timeout); 
     emonPi.power2=ct2.realPower;
   }
   else 
   {
     emonPi.power2 = ct2.calcIrms(no_of_samples)*Vrms;                               // Calculate Apparent Power 1  1480 is  number of samples
   }
   emonPi.Irms2=ct2.Irms*100;
   emonPi.powerFactor2=ct2.powerFactor*100;

  
   if (ACAC)                                                                       // Read from CT 3
   {
     ct3.calcVI(no_of_half_wavelengths,timeout); 
     emonPi.power3=ct3.realPower;
   }
   else 
   {
     emonPi.power3 = ct3.calcIrms(no_of_samples)*Vrms;                               // Calculate Apparent Power 1  1480 is  number of samples
   }
   emonPi.Irms3=ct3.Irms*100;
   emonPi.powerFactor3=ct3.powerFactor*100;


   if (ACAC)                                                                       // Read from CT 3
   {
     ct3.calcVI(no_of_half_wavelengths,timeout); 
     emonPi.power4=ct4.realPower;
   }
   else 
   {
     emonPi.power3 = ct3.calcIrms(no_of_samples)*Vrms;                               // Calculate Apparent Power 1  1480 is  number of samples
   }
   emonPi.Irms4=ct4.Irms*100;
   emonPi.powerFactor4=ct4.powerFactor*100;


  if (ACAC)                                                                      // Read from transformer
  {
    ct1.calcFreq(6*no_of_half_wavelengths,timeout);
    emonPi.sampleTime = ct1.sampleTime/200;
  }
  else{
      emonPi.sampleTime = 6000;
  }
    
  //if (debug==1) {Serial.print(emonPi.power1); Serial.print(" ");delay(5);}   
   // if (debug==1) {Serial.print(emonPi.power2); Serial.print(" ");delay(5);}  
    
    
    if (pulseCount)                                                       // if the ISR has counted some pulses, update the total count
    {
      cli();                                                              // Disable interrupt just in case pulse comes in while we are updating the count
      //emonPi.pulseCount += pulseCount;
      pulseCount = 0;
      sei();                                                              // Re-enable interrupts
    }     
    
    /*Serial.print(emonPi.power1); Serial.print(" ");
    Serial.print(emonPi.power2); Serial.print(" ");
    Serial.print(emonPi.Vrms); Serial.print(" ");
    Serial.println(emonPi.temp);
    */
    send_emonpi_serial();                                             //Send emonPi data to Pi serial using struct packet structure
    
    last_sample = millis();                                           //Record time of sample  
    
    } // end sample
    
} // end loop---------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void single_LED_flash()
{
  digitalWrite(LEDpin, HIGH);  delay(50); digitalWrite(LEDpin, LOW);
}

void double_LED_flash()
{
  digitalWrite(LEDpin, HIGH);  delay(25); digitalWrite(LEDpin, LOW);
  digitalWrite(LEDpin, HIGH);  delay(25); digitalWrite(LEDpin, LOW);
}




