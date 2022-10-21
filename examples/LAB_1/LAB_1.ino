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

// V 1.1  200812
// fTemperature now is a global variable and it is initialized to 0 in the setup.
// This is to prevent the MFS.write function to stuck if it is called with a value to large.




//---- AB&T EasyCAT starter kit, slave LAB_1 application example  ----------------------------  


// The EasyCAT shield used in this example has been customized using the Easy Configuration tool. 
// To understand how to do this please see the Easy Configurator user manual.
//
// We use the Hackatronics library to manage the Arduino Multi-Function Shield
// https://www.mpja.com/download/hackatronics-arduino-multi-function-shield.pdf
//
//
// The EtherCAT input variables used in this example are:
//
//		float       Temperature             The temperature from the sensor         
//
// And for the output:
//
//		uint8_t       Alarm                 The alarm state from the master




//*********************************************************************************************

#define CUSTOM                      // Custom mode
#include "LAB_1.h"                  // This file has been created by the Easy Configurator 
                                    // and must be located in the Arduino project folder
                                    //
                                    // There are two others files created by the Easy Configurator:
                                    // LAB_1.bin that must be loaded into the EEPROM.
                                    // LAB_1.xml that can be used, when appropriate, by the EtherCAT master. 
                                    
//*********************************************************************************************


#include "EasyCAT.h"                // EasyCAT library to interface the LAN9252
#include <SPI.h>                    // SPI library

#include <TimerOne.h>               // libraries for the multifunction shield
#include <MultiFuncShield.h>        //


//--- Sensor -----------------------------------------------------------------------

#include <OneWire.h>                                // Dallas DS1820 temperature sensor
#include <DallasTemperature.h>                      //

#define ONE_WIRE_BUS A4                             // sensor is on pin A4

OneWire ds(ONE_WIRE_BUS);                           // setup a oneWire instance to communicate with any OneWire devices  
DallasTemperature Sensor(&ds);                      // pass our oneWire reference to Dallas Temperature.

enum
{
  START_READING,
  WAIT_READING
}TemperatureMach;


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
uint32_t BlinkTime;
uint8_t EcatState;
bool DisplayOn;
float fTemperature;

//--------------------------------------------------------------------------------------------------

#define SAMPLE 100
#define BLINK_CYCLE 500             
#define OPERATIONAL 0x08
 
//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)

  Timer1.initialize();
  MFS.initialize(&Timer1);                                        // initialize multi-function shield library

  Serial.print ("\nEasyCAT - LAB_1 slave\n");                     // print the banner
  MFS.write("Lab1");                                              //
  delay(3000);
                                                                  //---- initialize the EasyCAT board -----
                                                                  
  if (EASYCAT.Init() == true)                                     // initialization succesfully completed
  {                                                               // 
    Serial.println ("initialized");                               //
  }                                                               //
  
  else                                                            // initialization failed   
  {                                                               // the EasyCAT board was not recognized
    Serial.println ("initialization failed");                     //     
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

  Sensor.begin();                                                 // initialize the sensor library
  Sensor.setWaitForConversion(FALSE);                             // function requestTemperature() etc returns immediately
  
  TemperatureMach = START_READING;                                
  fTemperature = 0;
                                                                  
  SampleTime = millis();                                          // initialize variables
  BlinkTime = millis();                                           //
}


//---- main loop ------------------------------------------------------------------------------------
 
void loop()                                                       // In the main loop we must call ciclically the 
{                                                                 // EasyCAT task and our application  
                                                                  //
                                                                  // This allows the bidirectional exachange of the data
                                                                  // between the EtherCAT master and our application
                                                                  //
                                                                  // The EasyCAT cycle and the Master cycle are asynchronous
                                                                  //                                            

  EcatState = EASYCAT.MainTask();                                 // execute the EasyCAT task
  
  Application();                                                  // execute the user applications
}


//---- user application ------------------------------------------------------------------------------

void Application ()                                        
  
{       

  Millis = millis();                                              // in this example we choose  
  if (Millis - SampleTime >= SAMPLE)                              // a cycle time of 100mS
  {                                                               // 
    SampleTime = Millis;                                          //  

    if (EcatState == OPERATIONAL)                                 // if the slave is in operational
    {                                                             // manage the I/O
      switch(TemperatureMach)                                     //------- DS18B20 OneWire temperature sensors -------------------------------
                                                                  //
                                                                  // we read the temperature sensors through a state machine, so
                                                                  // we are not stuck waiting for the acquisition to complete    
      {
        case START_READING:                                       // start DS18B20 acquisition                      
          Sensor.requestTemperatures();                           //
          TemperatureMach = WAIT_READING;                         // advance the state machne                   
        break;                                                    //
  
        case WAIT_READING:                                        // if the conversion is ready       
           if(Sensor.isConversionComplete())                      //
           {                                                      //
              fTemperature = Sensor.getTempCByIndex(0);           // read the result and send
              EASYCAT.BufferIn.Cust.Temperature = fTemperature;   // it to the master
                                                                  //
              TemperatureMach = START_READING;                    // advance the state machine  
           }
        break;
      }
    
      if (EASYCAT.BufferOut.Cust.Alarm == 0x01)                   // check if the master is telling us 
      {                                                           // that we are in alarm               
        if ((Millis - BlinkTime) > BLINK_CYCLE)                   // display blink and beep management
        {                                                         //
          BlinkTime = Millis;                                     //
                                                                  //
          if (DisplayOn)                                          //  
          {                                                       //
            DisplayOn = false;                                    //
            MFS.write("    ");                                    //
          }                                                       //
          else                                                    //
          {                                                       //
            DisplayOn = true;                                     //
            MFS.write(fTemperature);                              //
            MFS.beep();                                           //
          }                                                       //
        }                                                         //
      }                                                           //
      
      else                                                        // normal temperature visualization
      {                                                           //  
          MFS.write(fTemperature);                                //  
      }                                                           //
      
    }
    
    else                                                          // if the slave is not in Operational
    {                                                             // signal it on the display
      MFS.write("Not");                                           //
      delay(1000);                                                //
                                                                  //  
      MFS.write("0p");                                            //
      delay(1000);                                                //
    }                                                             //
  }                                                               //
}  
