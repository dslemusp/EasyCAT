//********************************************************************************************
//                                                                                           *
// AB&T Tecnologie Informatiche - Ivrea Italy                                                *
// http://www.bausano.net                                                                    *
// https://www.ethercat.org/en/products/791FFAA126AD43859920EA64384AD4FD.htm                 *
//                                                                                           *  
//********************************************************************************************   
 
//********************************************************************************************    
//                                                                                           *
// This software is distributed as an example, "AS IS", in the hope that it could            *
// be useful, WITHOUT ANY WARRANTY of any kind, express or implied, included, but            *
// not limited,  to the warranties of merchantability, fitness for a particular              *
// purpose, and non infringiment. In no event shall the authors be liable for any            *    
// claim, damages or other liability, arising from, or in connection with this software.     *
//                                                                                           *
//******************************************************************************************** 



//---- AB&T EasyCAT shield --- V.2_0------------------------------------------------------------------------------
//---- Multi I/O EtherCAT slave application example -------------------------------------------------------------  
//
//---- This example requires an Arduino Mega board --------------------------------------------------------------




#include "EasyCAT.h"                // EasyCAT library
#include <SPI.h>                    // SPI library - The EasyCAT shield is connected to the SPI bus

 
EasyCAT EASYCAT;                    // EasyCAT istantiation 
                                    // For this application we must use the
                                    // default SPI chip select = pin 9   (check your board setting!)

                                    // For others applications the constructor allow    
                                    // us to choose the chip select pin: 
                                                                      
                                    // for EasyCAT board REV_A we can choose between:
                                    // 8, 9, 10 
                                    //  
                                    // for EasyCAT board REV_B we can choose between:
                                    // 8, 9, 10, A5, 6, 7                                    

//---------------------------------------------------------------------------------------------------

#include <OneWire.h>                // OneWire library

OneWire  ds(2);                     // We have 2 OneWire DS18B20 temperature sensors connected on pin 2 
                                    //
                                    // The sensors have been programmed for 9 bits resolution   
                                    //
                                    // These are the unique 64-bit codes that identify the sensors
                                    //
unsigned char Address_Temp0[] = {0x28, 0x41, 0xD5, 0xD3, 0x07, 0x00, 0x00, 0x23};
unsigned char Address_Temp1[] = {0x28, 0x7E, 0x96, 0xD3, 0x07, 0x00, 0x00, 0x4D};


//-----------------------------------------------------------------------------------------------------

#include <Wire.h>                   // I2C library

                                    // We have 2 APDS-9300 luminosity sensors connected to the I2C bus
                                    
#define LIGHT_ADD_0 0x39            // Address of the first sensor
#define LIGHT_ADD_1 0x29            // Address of the second sensor


//---- pins declaration ------------------------------------------------------------------------------

const int Ana0 = A0;                // Analog inputs declaration
const int Ana1 = A1;                // 
const int Ana2 = A2;                //
const int Ana3 = A3;                //
const int Ana4 = A4;                // 
const int Ana5 = A5;                // 
const int Ana6 = A6;                //
const int Ana7 = A7;                // 
const int Ana8 = A8;                // 
const int Ana9 = A9;                // 
const int Ana10 = A10;              //
const int Ana11 = A11;              //
const int Ana12 = A12;              // 
const int Ana13 = A13;              // 
const int Ana14 = A14;              // 
const int Ana15 = A15;              //

const int Pwm0 = 4;                 // PWM declaration
const int Pwm1 = 5;                 //
const int Pwm2 = 6;                 //
const int Pwm3 = 7;                 //
const int Pwm4 = 8;                 //
const int Pwm5 = 10;                //
const int Pwm6 = 11;                //
const int Pwm7 = 12;                //

const int BitOut0 = 22;             // Digital outputs declaration
const int BitOut1 = 23;             //
const int BitOut2 = 24;             //
const int BitOut3 = 25;             //
const int BitOut4 = 26;             // 
const int BitOut5 = 27;             //
const int BitOut6 = 28;             // 
const int BitOut7 = 29;             // 
const int BitOut8 = 30;             //
const int BitOut9 = 31;             //
const int BitOut10 = 32;            // 
const int BitOut11 = 33;            // 
const int BitOut12 = 34;            //
const int BitOut13 = 35;            // 
const int BitOut14 = 36;            // 
const int BitOut15 = 37;            // 

const int BitIn0 = 49;              // Digital inputs declaration
const int BitIn1 = 48;              //
const int BitIn2 = 47;              // 
const int BitIn3 = 46;              // 
const int BitIn4 = 45;              //
const int BitIn5 = 44;              //
const int BitIn6 = 43;              // 
const int BitIn7 = 42;              // 
const int BitIn8 = 41;              //
const int BitIn9 = 40;              // 
const int BitIn10 = 39;             //
const int BitIn11 = 38;             // 
const int BitIn12 = 17;             //
const int BitIn13 = 16;             // 
const int BitIn14 = 15;             // 
const int BitIn15 = 14;             // 

const int LedRed = 13;              // HALT/RUN led pins declaration  
const int LedGreen = 3;             //




//---- global variables ---------------------------------------------------------------------------

unsigned long Millis, PreviousMillis = 0;

unsigned char EcatState;
unsigned char N_Cycles = 0;
bool Blink = true;
UWORD Raw;

enum
{
  START_TEMP_0,
  WAIT_TEMP_0,
  START_TEMP_1,
  WAIT_TEMP_1
}TemperatureMach;


//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
 
  Serial.begin(9600);                                             // Serial line initialization
                                                                  // (Used only for debug)
       
  Serial.print ("\nEasyCAT - Multi I/O EtherCAT slave\n");        // Print the banner

  Wire.begin();                                                   // Inizialize the I2C library
                                                                  
  Wire.beginTransmission(LIGHT_ADD_0);                            // Power up the first luminosity sensor
  Wire.write(0xC0);                                               //
  Wire.write(0x03);                                               //
  Wire.endTransmission();                                         //
                                                                  //
  Wire.beginTransmission(LIGHT_ADD_0);                            // Set the integration time to 101 mS
  Wire.write(0xC1);                                               //
  Wire.write(0x01);                                               //
  Wire.endTransmission();                                         //

  Wire.beginTransmission(LIGHT_ADD_1);                            // The same as above for the second sensor ...
  Wire.write(0xC0);                                               //
  Wire.write(0x03);                                               //
  Wire.endTransmission();                                         //
                                                                  //
  Wire.beginTransmission(LIGHT_ADD_1);                            //    
  Wire.write(0xC1);                                               //
  Wire.write(0x01);                                               //
  Wire.endTransmission();                                         //

  
  TemperatureMach = START_TEMP_0;                                 // Initialize the temperature sensors state machine

  pinMode(BitOut0, OUTPUT);                                       // Digital outputs setting
  pinMode(BitOut1, OUTPUT);                                       // 
  pinMode(BitOut2, OUTPUT);                                       // 
  pinMode(BitOut3, OUTPUT);                                       // 
  pinMode(BitOut4, OUTPUT);                                       // 
  pinMode(BitOut5, OUTPUT);                                       // 
  pinMode(BitOut6, OUTPUT);                                       // 
  pinMode(BitOut7, OUTPUT);                                       //   
  pinMode(BitOut8, OUTPUT);                                       // 
  pinMode(BitOut9, OUTPUT);                                       // 
  pinMode(BitOut10, OUTPUT);                                      // 
  pinMode(BitOut11, OUTPUT);                                      // 
  pinMode(BitOut12, OUTPUT);                                      // 
  pinMode(BitOut13, OUTPUT);                                      // 
  pinMode(BitOut14, OUTPUT);                                      // 
  pinMode(BitOut15, OUTPUT);                                      //     

  pinMode(Pwm0, OUTPUT);                                          // PWM setting  
  pinMode(Pwm1, OUTPUT);                                          //   
  pinMode(Pwm2, OUTPUT);                                          //   
  pinMode(Pwm3, OUTPUT);                                          //       
  pinMode(Pwm4, OUTPUT);                                          //   
  pinMode(Pwm5, OUTPUT);                                          //   
  pinMode(Pwm6, OUTPUT);                                          //   
  pinMode(Pwm7, OUTPUT);                                          //  

  pinMode(BitIn0, INPUT_PULLUP);                                  // Digital inputs setting
  pinMode(BitIn1, INPUT_PULLUP);                                  // 
  pinMode(BitIn2, INPUT_PULLUP);                                  //
  pinMode(BitIn3, INPUT_PULLUP);                                  //
  pinMode(BitIn4, INPUT_PULLUP);                                  // 
  pinMode(BitIn5, INPUT_PULLUP);                                  //
  pinMode(BitIn6, INPUT_PULLUP);                                  //
  pinMode(BitIn7, INPUT_PULLUP);                                  //
  pinMode(BitIn8, INPUT_PULLUP);                                  // 
  pinMode(BitIn9, INPUT_PULLUP);                                  //
  pinMode(BitIn10, INPUT_PULLUP);                                 //
  pinMode(BitIn11, INPUT_PULLUP);                                 //
  pinMode(BitIn12, INPUT_PULLUP);                                 // 
  pinMode(BitIn13, INPUT_PULLUP);                                 //
  pinMode(BitIn14, INPUT_PULLUP);                                 //
  pinMode(BitIn15, INPUT_PULLUP);                                 //

  pinMode(LedRed, OUTPUT);                                        // HALT/RUN led pins setting
  pinMode(LedGreen, OUTPUT);                                      //
                                                                  //
  digitalWrite (LedRed, LOW);                                     //
  digitalWrite (LedGreen, LOW);                                   //
 
                                                                  //---- initialize the EasyCAT board -----
                                                                  
  if (EASYCAT.Init() == true)                                     // Initialization
  {                                                               // succesfully completed
    Serial.print ("initialized");                                 //
  }                                                               //
  
  else                                                            // Initialization failed   
  {                                                               // The EasyCAT board was not recognized
    Serial.print ("initialization failed");                       //     
                                                                  // The most common reason is that the SPI 
                                                                  // chip select choosen on the board doesn't 
                                                                  // match the one choosen by the firmware
                                                                  
                                                                  // Stay in loop for ever
                                                                  // with the red led blinking fast
    while(1)                                                      //
    {                                                             //   
      digitalWrite (LedRed, LOW);                                 // 
      delay(100);                                                 //   
      digitalWrite (LedRed, HIGH);                                //  
      delay(100);                                                 // 
    }                                                             // 
  }    

  PreviousMillis = millis();  
}


//---- main loop ----------------------------------------------------------------------------------------
 
void loop()                                             // In the main loop we must call ciclically the 
{                                                       // EasyCAT task and our application
                                                        //
                                                        // This allows the bidirectional exachange of the data
                                                        // between the EtherCAT master and our application
                                                        //
                                                        // The EasyCAT cycle and the Master cycle are asynchronous
                                                        //   

  EcatState = EASYCAT.MainTask();                       // execute the EasyCAT task
  
  Application();                                        // user applications
}


//---- user application ------------------------------------------------------------------------------

void Application ()                                   // Here we manage the I/O resources of the Arduino Mega board                                 

{
  UWORD Analog;

  unsigned char i;
  unsigned char Temp, Val;

  Millis = millis();                                    // For this application we choose a cycle time of 50 mS
  if (Millis - PreviousMillis >= 50)                    // 
  {                                                     // 
    PreviousMillis = Millis;                            //

                                                   
    for (i=0; i<16; i++)                                //------ 16 analog inputs -------------------------------------------------      
    {                                                   //
      Analog.Word = analogRead(i+54);                   // Read one analog input
      Analog.Word >>= 2;                                // Normalize it on 8 bits
      EASYCAT.BufferIn.Byte[i] = Analog.Byte[0];        // And put the result into the EtherCAT buffer  
    }
  
                                                        //------- 16 digital inputs -----------------------------------------------
                                                        //
    EASYCAT.BufferIn.Byte[16] = PINL;                   // Read the digital inputs from 0 to 7 (they are connected to PORT L)
                                                        // and put the result into the EtherCAT buffer
  
    Temp = PING & 0x07;                                 // The digital inputs from 8 to 15 are scattered on PORT G, D, H and J
                                                        // so we have to collect and reorder them
    Val = PIND & 0x80;                                  //
    Val = Val >> 4;                                     // 
    Temp |= Val;                                        //
                                                        //
    Val = PINH & 0x03;                                  //    
    Val = Val << 4;                                     //
    Temp |= Val;                                        //
                                                        //
    Val = PINJ & 0x03;                                  //    
    Val = Val << 6;                                     //
    Temp |= Val;                                        //  
                                                        //
    EASYCAT.BufferIn.Byte[17] = Temp;                   // Put the result into the EtherCAT buffer  
  
  
    switch(TemperatureMach)                             //------- 2 DS18B20 OneWire temperature sensors -------------------------------
                                                        //
                                                        // We read the temperature sensors through a state machine, so
                                                        // we are not stuck waiting for the acquisition to complete    
    {
      case START_TEMP_0:                                // Start DS18B20 acquisition
        ds.reset();                                     //
        ds.select(Address_Temp0);                       //
        ds.write(0x44,0);                               //
        TemperatureMach = WAIT_TEMP_0;                  // Advance the state machne                   
      break;                                            //
  
      case WAIT_TEMP_0:                                 // Is the acquisition completed?
        if (ds.read_bit());                             //
        {                                               //
          ds.reset();                                   // Yes: read the temperature
          ds.select(Address_Temp0);                     //
          ds.write(0xBE,0);                             //
          Raw.Byte[0] = ds.read();                      //
          Raw.Byte[1] = ds.read();                      //
          Raw.Word = Raw.Word >> 3;                     //
                                                        // 
          if (Raw.Byte[1] & 0x01)                       // We limit the scale between 0 and 63.5 degree (0 - 255)                   
              Raw.Byte[0] = 0x00;                       //      
          if (Raw.Byte[0] & 0x80)                       // (We use only 7 bits)                  
              Raw.Byte[0] = 0x7F;                       //                                                       
                                                        //         
          EASYCAT.BufferIn.Byte[18] = Raw.Byte[0];      // Put the result into the EtherCAT buffer                
          TemperatureMach = START_TEMP_1;               // Advance the state machine   
        }                                               //
      break;                                            //
  
      case START_TEMP_1:                                // The same as above, for the second sensor ...
        ds.reset();                                     //
        ds.select(Address_Temp1);                       //
        ds.write(0x44,0);                               //
        TemperatureMach = WAIT_TEMP_1;                  //                   
      break;                                            //
  
      case WAIT_TEMP_1:                                 //
        if (ds.read_bit());                             //
        {                                               //
          ds.reset();                                   //
          ds.select(Address_Temp1);                     //
          ds.write(0xBE,0);                             //
          Raw.Byte[0] = ds.read();                      //
          Raw.Byte[1] = ds.read();                      //
          Raw.Word = Raw.Word >> 3;                     //
                                                        // 
          if (Raw.Byte[1] & 0x01)                       //                 
              Raw.Byte[0] = 0x00;                       //      
          if (Raw.Byte[0] & 0x80)                       //                  
              Raw.Byte[0] = 0x7F;                       //                                                       
                                                        //  
          EASYCAT.BufferIn.Byte[19] = Raw.Byte[0];      //              
          TemperatureMach = START_TEMP_0;               //    
        }     
      break;
    }
  
                                                        //------- 2 APDS-9300 I2C luminosity sensors ----------------------------------
                                                        
    Wire.beginTransmission(LIGHT_ADD_0);                // Read the luminosity from the first sensor
    Wire.write(0xEC);                                   //
    Wire.endTransmission(0);                            //  
                                                        //
    Wire.requestFrom(LIGHT_ADD_0,2,1);                  //    
    Raw.Byte[0] = Wire.read();                          //
    Raw.Byte[1] = Wire.read();                          //
                                                        //
    Raw.Word = Raw.Word >> 2;                           // Scale it down
    if (Raw.Byte[1] != 0)                               // and limit the full scale to 255
      Raw.Byte[0] = 0xFF;                               //
    EASYCAT.BufferIn.Byte[20] = Raw.Byte[0];            //
      
    Wire.beginTransmission(LIGHT_ADD_1);                // The same as above for the second sensor ...
    Wire.write(0xEC);                                   //
    Wire.endTransmission(0);                            //
                                                        //
    Wire.requestFrom(LIGHT_ADD_1,2,1);                  //    
    Raw.Byte[0] = Wire.read();                          //
    Raw.Byte[1] = Wire.read();                          //
                                                        //
    Raw.Word = Raw.Word >> 2;                           //
    if (Raw.Byte[1] != 0)                               //
      Raw.Byte[0] = 0xFF;                               //
    EASYCAT.BufferIn.Byte[21] = Raw.Byte[0];            //
  
  
                                                        //------- 16 digital outputs --------------------------------------------
                                                        //                                                    
    PORTC = EASYCAT.BufferOut.Byte[0];                  // Write the digital outputs from 0 to 7  (they are connected to PORT C)
    PORTA = EASYCAT.BufferOut.Byte[1];                  // Write the digital outputs from 8 to 15 (they are connected to PORT A)
  
  
                                                        //------- 8 PWM outputs -------------------------------------------------
                                                        //  
    analogWrite (Pwm0, EASYCAT.BufferOut.Byte[6]);      // Read the values from the EtherCAT buffer
    analogWrite (Pwm1, EASYCAT.BufferOut.Byte[7]);      // and write them to the PWM pins
    analogWrite (Pwm2, EASYCAT.BufferOut.Byte[8]);      //
    analogWrite (Pwm3, EASYCAT.BufferOut.Byte[9]);      //
    analogWrite (Pwm4, EASYCAT.BufferOut.Byte[10]);     //
    analogWrite (Pwm5, EASYCAT.BufferOut.Byte[11]);     //
    analogWrite (Pwm6, EASYCAT.BufferOut.Byte[12]);     //
    analogWrite (Pwm7, EASYCAT.BufferOut.Byte[13]);     //  
  
  
                                                        //------ HALT/RUN visualization ---------------------------------------
  
    if (EcatState == 0x08)                              // If the EasyCAT is in Operational State
    {                                                   // and the communication is running
      {                                                 //
        digitalWrite(LedRed, LOW);                      // we switch on the green led
        digitalWrite(LedGreen, HIGH);                   //
      }                                                 //
    }
  
    else                                                // Otherwise we blink the red led 
    {                                                   //     
      digitalWrite(LedGreen, LOW);                      //                                                        
      if (N_Cycles++ > 4)                               //
      {                                                 //
        N_Cycles = 0;                                   //  
        if (Blink)                                      //
        {                                               //
          digitalWrite(LedRed, HIGH);                   //
          Blink = false;                                //
        }                                               //  
        else                                            //  
        {                                               //
          digitalWrite(LedRed, LOW);                    //
          Blink = true;                                 //
        }                                               //
      }                                                 //
    }                                                   //
  }                                                     //    
}                                                       //











