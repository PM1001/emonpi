// emonPi used 16 x 2 I2C LCD display 

void serial_print_startup(){
  //-------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

  Serial.print("CT 1 Cal: "); Serial.println(Ical1);
  Serial.print("CT 2 Cal: "); Serial.println(Ical2);
  Serial.print("VRMS AC ~");
  Serial.print(vrms); Serial.println("V");

  if (ACAC) 
  {
    Serial.println("AC Wave Detected - Real Power calc enabled");
    if (USA==1) Serial.print("USA mode > "); 
    Serial.print("Vcal: "); Serial.println(Vcal);
    Serial.print("Vrms: "); Serial.print(Vrms); Serial.println("V");
    Serial.print("Phase Shift: "); Serial.println(phase_shift);
  }
  else 
  {
   Serial.println("AC NOT detected - Apparent Power calc enabled");
   if (USA==1) Serial.println("USA mode"); 
   Serial.print("Assuming VRMS: "); Serial.print(Vrms); Serial.println("V");
 }  

  if (CT_count==0) {
    Serial.println("no CT detected");
  }
   else   
   {
    
    if (CT1) {
      Serial.println("CT 1 detect");
    }
    if (CT2) {
      Serial.println("CT 2 detect");
    }     
    if (CT3) {
      Serial.println("CT 3 detect");
    }
    if (CT4) {
      Serial.println("CT 4 detect");
    }
   }
  
  delay(2000);

  	Serial.println("0 DS18B20 detected");

  if (RF_STATUS == 1){
    #if (RF69_COMPAT)
      Serial.println("RFM69CW Init: ");
    #else
      Serial.println("RFM12B Init: ");
    #endif

    Serial.print("Node "); Serial.print(nodeID); 
    Serial.print(" Freq "); 
    if (RF_freq == RF12_433MHZ) Serial.print("433Mhz");
    if (RF_freq == RF12_868MHZ) Serial.print("868Mhz");
    if (RF_freq == RF12_915MHZ) Serial.print("915Mhz"); 
    Serial.print(" Network "); Serial.println(networkGroup);

    showString(helpText1);
  }
  delay(20);  
}


void send_emonpi_serial()  //Send emonPi data to Pi serial /dev/ttyAMA0 using struct JeeLabs RF12 packet structure 
{
  byte binarray[sizeof(emonPi)];
  memcpy(binarray, &emonPi, sizeof(emonPi));
  
  Serial.print("OK ");
  Serial.print(nodeID);
  for (byte i = 0; i < sizeof(binarray); i++) {
    Serial.print(' ');
    Serial.print(binarray[i]);
  }
  Serial.print(" (-0)");
  Serial.println();
  
  delay(10);
}

static void showString (PGM_P s) {
  for (;;) {
    char c = pgm_read_byte(s++);
    if (c == 0)
      break;
    if (c == '\n')
      Serial.print('\r');
    Serial.print(c);
  }
}
