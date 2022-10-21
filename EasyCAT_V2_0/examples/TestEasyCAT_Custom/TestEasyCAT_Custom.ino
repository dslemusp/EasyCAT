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



//---- AB&T EasyCAT shield custom application example  ---------------------------------------  


// This is the legacy "TestEasyCAT" example but the variables have been 
// customized using the Easy Configuration tool. 
// To understand how to do this please see the Easy Configurator user manual.
//
//
// The input variables used in this example are:
//
//		uint16_t    Analog_0                The first analog input         
//		uint16_t    Analog_1                The second analog input
//		uint8_t     DipSwitches             The four dip switches
//		uint8_t     Bit8_FallingTestRamp    A falling test ramp
//		uint16_t    Bit16_RisingTestRamp    A rising test ramp
//
// And the output:
//
//		uint8_t     Leds;                   The four leds




//*********************************************************************************************

#define CUSTOM                      // Custom mode
#include "TestEasyCAT_Custom.h"     // This file has been created by the Easy Configurator 
                                    // and must be located in the Arduino project folder
                                    //
                                    // There are two others files created by the Easy Configurator:
                                    // TestEasyCAT_Custom.bin that must be loaded into the EEPROM.
                                    // TestEasyCAT_Custom.xml that must be used by the EtherCAT master. 
                                    
//*********************************************************************************************




#include "EasyCAT.h"                // EasyCAT library to interface the LAN9252
#include <SPI.h>                    // SPI library

 
EasyCAT EASYCAT;                    // EasyCAT istantiation

                                    // The constructor allow us to choose the pin used for the EasyCAT SPI chip select 
                                    // Without any parameter pin 9 will be used 
                   
                                    // We can choose between:
                                    // 8, 9, 10, A5, 6, 7                                    

                                    // On the EasyCAT board the SPI chip select is selected through a bank of jumpers                Millis = millis();                                    // For this application we choose a cycle time of 150 mS

                                    // (The EasyCAT board REV_A allows only pins 8, 9, 10 through 0 ohm resistors)

 //EasyCAT EASYCAT(8);              // example:                                  
                                    // pin 8 will be used as SPI chip select
                                    // The chip select chosen by the firmware must match the setting on the board  


//---- pins declaration ------------------------------------------------------------------------------

const int Ana0 = A0;                // analog input 0
const int Ana1 = A1;                // analog input 1

const int BitOut0 = A2;             // digital output bit 0
const int BitOut1 = A3;             // digital output bit 1
const int BitOut2 = A4;             // digital output bit 2
const int BitOut3 = A5;             // digital output bit 3

const int BitIn0 = 3;               // digital input  bit 0
const int BitIn1 = 5;               // digital input  bit 1
const int BitIn2 = 6;               // digital input  bit 2
const int BitIn3 = 7;               // digital input  bit 3



//---- global variables ---------------------------------------------------------------------------

uint16_t ContaUp;                      // used for sawthoot test generation
uint8_t  ContaDown;                    //

unsigned long Millis = 0;
unsigned long PreviousMillis = 0;
unsigned long PreviousSaw = 0;
unsigned long PreviousCycle = 0;


//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)
           
  Serial.print ("\nEasyCAT - Generic EtherCAT slave\n");          // print the banner

  pinMode(BitOut0, OUTPUT);                                       // digital output pins setting
  pinMode(BitOut1, OUTPUT);                                       // 
  pinMode(BitOut2, OUTPUT);                                       //
  pinMode(BitOut3, OUTPUT);                                       //

  pinMode(BitIn0, INPUT_PULLUP);                                  // digital input pins setting
  pinMode(BitIn1, INPUT_PULLUP);                                  // 
  pinMode(BitIn2, INPUT_PULLUP);                                  //
  pinMode(BitIn3, INPUT_PULLUP);                                  //

  ContaUp = 0x0000;                                               //
  ContaDown = 0x00;                                               //
 
                                                                  //---- initialize the EasyCAT board -----
                                                                  
  if (EASYCAT.Init() == true)                                     // initialization
  {                                                               // succesfully completed
    Serial.print ("initialized");                                 //
  }                                                               //
  
  else                                                            // initialization failed   
  {                                                               // the EasyCAT board was not recognized
    Serial.print ("initialization failed");                       //     
                                                                  // The most common reason is that the SPI 
                                                                  // chip select choosen on the board doesn't 
                                                                  // match the one choosen by the firmware
                                                                  
    pinMode(13, OUTPUT);                                          // stay in loop for ever
                                                                  // with the Arduino led blinking
    while(1)                                                      //
    {                                                             //   
      digitalWrite (13, LOW);                                     // 
      delay(500);                                                 //   
      digitalWrite (13, HIGH);                                    //  
      delay(500);                                                 // 
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

  EASYCAT.MainTask();                                   // execute the EasyCAT task
  
  Application();                                        // user applications

}



//---- user application ------------------------------------------------------------------------------

void Application ()                                        

{
  uint16_t Analog0;
  uint16_t Analog1;
       
  Millis = millis();                                    // As an example for this application 
  if (Millis - PreviousMillis >= 10)                    // we choose a cycle time of 10 mS 
  {                                                     // 
    PreviousMillis = Millis;                            //  
   
                                                        // --- analog inputs management ---
      
    Analog0 = analogRead(Ana0);                         // read analog input 0
    Analog0 = Analog0 << 6;                             // normalize it on 16 bits
    EASYCAT.BufferIn.Cust.Analog_0 = Analog0;           // and put the result into
                                                        // input Byte 0 

    Analog1 = analogRead(Ana1);                         // read analog input 1
    Analog1 = Analog1 << 6;                             // normalize it on 16 bits  
    EASYCAT.BufferIn.Cust.Analog_1 = Analog1;           // and put the result into
    

                                                        // --- four output bits management ----
                                                        //                          
    if (EASYCAT.BufferOut.Cust.Leds & (1<<0))           // the four output bits are mapped to the 
      digitalWrite (BitOut0, HIGH);                     // lower nibble of output Byte 0
    else                                                // 
      digitalWrite (BitOut0, LOW);                      // we read each bit and write it
                                                        // to the corrisponding pin
    if (EASYCAT.BufferOut.Cust.Leds & (1<<1))           // 
      digitalWrite (BitOut1, HIGH);                     //
    else                                                //
      digitalWrite (BitOut1, LOW);                      // 
                                                        //
    if (EASYCAT.BufferOut.Cust.Leds & (1<<2))           //   
      digitalWrite (BitOut2, HIGH);                     //
    else                                                //
      digitalWrite (BitOut2, LOW);                      // 
                                                        //  
    if (EASYCAT.BufferOut.Cust.Leds & (1<<3))           // 
      digitalWrite (BitOut3, HIGH);                     //
    else                                                //
      digitalWrite (BitOut3, LOW);                      // 

                                                        //--- four input bits management ---
                                                        //  
    if (digitalRead(BitIn0))                            // the four input pins are mapped to the
      EASYCAT.BufferIn.Cust.DipSwitches |= (1<<0);      // lower nibble of input Byte 6
    else                                                //
      EASYCAT.BufferIn.Cust.DipSwitches &= ~(1<<0);     // we read each pin and write it 
                                                        // to the corresponding bit
    if (digitalRead(BitIn1))                            //
      EASYCAT.BufferIn.Cust.DipSwitches |= (1<<1);      //  
    else                                                //
      EASYCAT.BufferIn.Cust.DipSwitches &= ~(1<<1);     //
                                                        //
    if (digitalRead(BitIn2))                            //
      EASYCAT.BufferIn.Cust.DipSwitches |= (1<<2);      //   
    else                                                //
      EASYCAT.BufferIn.Cust.DipSwitches &= ~(1<<2);     //  
                                                        //  
    if (digitalRead(BitIn3))                            //
      EASYCAT.BufferIn.Cust.DipSwitches |= (1<<3);      // 
    else                                                //
      EASYCAT.BufferIn.Cust.DipSwitches &= ~(1<<3);     //

                                                        // --- test sawtooth generation --- 
                                                        //
    Millis = millis();                                  // each 100 mS
    if (Millis - PreviousSaw >= 100)                    // 
    {                                                   // 
      PreviousSaw = Millis;                             //
                                                        //
      ContaUp += 256 ;                                  // we increment the variable ContaUp  
      ContaDown--;                                      // and decrement ContaDown
    }                                                   //
                                                           
    EASYCAT.BufferIn.Cust.Bit16_RisingTestRamp = ContaUp;   // medium speed rising slope
    EASYCAT.BufferIn.Cust.Bit8_FallingTestRamp = ContaDown; // medium speed falling slope    
  }   
}




