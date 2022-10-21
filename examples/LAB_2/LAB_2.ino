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



//---- AB&T EasyCAT starter kit, slave LAB_2 application example  ----------------------------  

 
// The EasyCAT shield used in this example has been customized using the Easy Configuration tool. 
// To understand how to do this please see the Easy Configurator user manual.
//
// We use the Hackatronics library to manage the Arduino Multi-Function Shield
// https://www.mpja.com/download/hackatronics-arduino-multi-function-shield.pdf
//
//
// The EtherCAT input variables used in this example are:
//
//		uint16_t      Potentiometer         The multiturn trimmer          
//    uint8_t       Switches              The 3 buttons
//
// And for the output:
//
//		uint8_t       Segments              The 4 middle segments on the display




//*********************************************************************************************

#define CUSTOM                      // Custom mode
#include "LAB_2.h"                  // This file has been created by the Easy Configurator 
                                    // and must be located in the Arduino project folder
                                    //
                                    // There are two others files created by the Easy Configurator:
                                    // LAB_2.bin that must be loaded into the EEPROM.
                                    // LAB_2.xml that can, when appropriate, be used by the EtherCAT master. 
                                    
//*********************************************************************************************


#include "EasyCAT.h"                // EasyCAT library to interface the LAN9252
#include <SPI.h>                    // SPI library

#include <TimerOne.h>               // libraries for the multifunction shield
#include <MultiFuncShield.h>        //

//-----------------------------------------------------------------------------------

 
EasyCAT EASYCAT;                    // EasyCAT istantiation

                                    // The constructor allow us to choose the pin used for the EasyCAT SPI chip select 
                                    // Without any parameter pin 9 will be used 
                   
                                    // We can choose between:
                                    // 8, 9, 10, A5, 6, 7                                    

                                    // On the EasyCAT board the SPI chip select is selected through a bank of jumpers                                                  // For this application we choose a cycle time of 150 mS



//---- global variables ---------------------------------------------------------------------------

uint32_t Millis;
uint32_t SampleTime;
uint8_t Switches; 
float fPotentiometer;
uint8_t EcatState; 

//-------------------------------------------------------------------------------------------------

#define kk 0.90                                                   // analog input filter parameter

#define SAMPLE 20              
#define OPERATIONAL 0x08
 
//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)
  Timer1.initialize();
  MFS.initialize(&Timer1);                                        // initialize multi-function shield library

  Serial.print ("\nEasyCAT - LAB_2 slave\n");                     // print the banner
  MFS.write("Lab2");                                              //
  delay(3000);  
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
 
    while(1)                                                      // print "Init Fail"
    {                                                             // on the display  
      MFS.write("Init");                                          //
      delay(2000);                                                //
      MFS.write("Fail");                                          //
      delay(2000);                                                //
    }                                                             //  
  }  

  Switches = 0x00;                                                // inizialize variabiles
  SampleTime = millis();                                          //
}



//---- main loop ----------------------------------------------------------------------------------------
 
void loop()                                                       // In the main loop we must call ciclically the 
{                                                                 // EasyCAT task and our application  
                                                                  //
                                                                  // This allows the bidirectional exachange of the data
                                                                  // between the EtherCAT master and our application
                                                                  //
                                                                  // The EasyCAT cycle and the Master cycle are asynchronous
                                         
  EcatState = EASYCAT.MainTask();                                 // execute the EasyCAT task
  
  Application();                                                  // execute the user applications
}


//---- user application ------------------------------------------------------------------------------

void Application ()                                        

{       
  char DispBuff[5];
  uint8_t Segments;

  uint8_t i;


  Millis = millis();                                              // in this example we choose
  if (Millis - SampleTime >= SAMPLE)                              // a cycle time of 20 mS 
  {                                                               // 
    SampleTime = Millis;                                          //  

    if (EcatState == OPERATIONAL)                                 // if the slave is in operational
    {                                                             // manage the I/O
                           
      Segments = EASYCAT.BufferOut.Cust.Segments;                 // read the status of the display segments from the master
      
      for (i=0; i<4; i++)                                         // convert it  
      {                                                           //
        if ((Segments & 0x08) == 0x08)                            //
        {                                                         //
          DispBuff[i] = '-';                                      //
        }                                                         //
        else                                                      //
        {                                                         //
          DispBuff[i] = ' ';                                      //
        }                                                         //
        Segments = Segments << 1;                                 //  
      }                                                           //
      
      MFS.write(DispBuff);                                        // and display
                                     
                                                                  // read the potentiometer and filter the value
      fPotentiometer = kk*fPotentiometer + (1.0-kk)*analogRead(POT_PIN);
          
                                                                  // send the value to the master
      EASYCAT.BufferIn.Cust.Potentiometer = (uint16_t)fPotentiometer;
  
      uint8_t Button = MFS.getButton();                           // read the buttons state
                                                            
      if (Button == BUTTON_3_PRESSED)                             // check wich buttons have been pressed
        Switches |= 0x01;                                         //
      if (Button == BUTTON_2_PRESSED)                             //  
        Switches |= 0x02;                                         //
      if (Button == BUTTON_1_PRESSED)                             //
        Switches |= 0x04;                                         //

                                                                  // check wich buttons have been released
      if ((Button == BUTTON_3_SHORT_RELEASE) || (Button == BUTTON_3_LONG_RELEASE))
        Switches &= ~0x01;                                        //
      if ((Button == BUTTON_2_SHORT_RELEASE) || (Button == BUTTON_2_LONG_RELEASE))
        Switches &= ~0x02;                                        //
      if ((Button == BUTTON_1_SHORT_RELEASE) || (Button == BUTTON_1_LONG_RELEASE))
        Switches &= ~0x04;                                        //
  
      EASYCAT.BufferIn.Cust.Switches = Switches;                  // send the buttons state to the master
    }          

    else                                                          // if the slave is not in Operational
    {                                                             // signal it on the display
      MFS.write("Not");                                           //
      delay(1000);                                                //
                                                                  //  
      MFS.write("0p");                                            //
      delay(1000);                                                //
    }                                                             //
  }   
}
