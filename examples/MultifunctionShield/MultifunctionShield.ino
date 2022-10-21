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



//---- AB&T EasyCAT with Multifunction shield application example  ----------------------------  
//
// We use the Hackatronics library to manage the Arduino Multi-Function Shield
//
// https://www.mpja.com/download/hackatronics-arduino-multi-function-shield.pdf
//
//
// The EasyCAT board is used in "Standard mode" so we have 32 bytes in output and 32 bytes in input
// The variables exchanghed with the EtherCAT master are mapped as follows:
//
// Input variables
//
//		Byte[0]       Temperature             The temperature from the sensor         
//    Byte[1]       Potentiometer           The analog value from the potentiometer  
//    Byte[2]       Buttons                 The state of the three buttons  
//
// Output variables
//
//	 Byte[0]        Beep                    The state of the buzzer
//   Byte[1]        Display                 The number shown on the display 


// To make your own application with custom names and data types you have to use the "Custom mode"
// and configure the EasyCAT using the "Easy Configurator".
//
// https://www.youtube.com/watch?v=JO5wMEIHAF4
//
// https://www.bausano.net/images/arduino-easycat/EasyConfigurator_UserManual.pdf


                                    
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
uint8_t EcatState;
uint8_t Switches;

//--------------------------------------------------------------------------------------------------

#define SAMPLE 50           
#define OPERATIONAL 0x08
 
//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)

  Timer1.initialize();
  MFS.initialize(&Timer1);                                        // initialize multi-function shield library

  Serial.print ("\nEasyCAT - Slave with Multifunction shield \n");// print the banner

                                                                  //---- initialize the EasyCAT board -----
                                                                  
  if (EASYCAT.Init() == true)                                     // initialization succesfully completed
  {                                                               // 
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

  Sensor.begin();                                                 // initialize the sensor library
  Sensor.setWaitForConversion(FALSE);                             // function requestTemperature() etc returns immediately
  
  TemperatureMach = START_READING;                                
                                                                  
  SampleTime = millis();                                          // initialize variables
  Switches = 0x00;
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
  uint8_t Temperature;
  uint16_t Potentiometer;
  uint8_t Buttons;  
  uint8_t Display;      


  Millis = millis();                                              // in this example we choose  
  if (Millis - SampleTime >= SAMPLE)                              // a cycle time of 50mS
  {                                                               // 
    SampleTime = Millis;                                          //  

    if (EcatState == OPERATIONAL)                                 // if the slave is in operational
    {                                                             // manage the I/O



                              //**** inputs management ****************************************************************************************
                              

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
              Temperature = Sensor.getTempCByIndex(0);            // read the result
                                                             
              EASYCAT.BufferIn.Byte[0] = Temperature;             // send it to the master using Input Byte[0]
                                                         
              TemperatureMach = START_READING;                    // advance the state machine  
           }                                                      //
        break;                                                    //  
      }                                                           //



                                                                  //-------- Potentiometer --------------------------------------------------
                                                                  //                                                             
      Potentiometer = (analogRead(POT_PIN));                      // read the potentiometer
      Potentiometer = Potentiometer >> 2;                         // normalize it to 8 bits
                                                                   
      EASYCAT.BufferIn.Byte[1] = (uint8_t)Potentiometer;          // send the value to the master using Input Byte[1]


                                                                  //-------- Buttons -------------------------------------------------------
                                                                  //
      Buttons = MFS.getButton();                                  // read the buttons
                                                                  //
      if (Buttons == BUTTON_3_PRESSED)                            // check wich buttons have been pressed
        Switches |= 0x01;                                         //
      if (Buttons == BUTTON_2_PRESSED)                            //  
        Switches |= 0x02;                                         //
      if (Buttons == BUTTON_1_PRESSED)                            //
        Switches |= 0x04;                                         //

                                                                  // check wich buttons have been released
      if ((Buttons == BUTTON_3_SHORT_RELEASE) || (Buttons == BUTTON_3_LONG_RELEASE))
        Switches &= ~0x01;                                        //
      if ((Buttons == BUTTON_2_SHORT_RELEASE) || (Buttons == BUTTON_2_LONG_RELEASE))
        Switches &= ~0x02;                                        //
      if ((Buttons == BUTTON_1_SHORT_RELEASE) || (Buttons == BUTTON_1_LONG_RELEASE))
        Switches &= ~0x04;                                        //

      EASYCAT.BufferIn.Byte[2] = Switches;                        // send the buttons state to the master using Input Byte[2]




                              //**** outputs management **************************************************************************************
                              

                                                                  //--------- Beep -----------------------------------------------------------
                                                                  //  
      if (EASYCAT.BufferOut.Byte[0] == 0x01)                      // check Output Byte[0] to see if the master
      {                                                           // is telling us to beep the buzzer
          MFS.beep();                                             //    
      }                                                           //

                                                                  //-------- Display --------------------------------------------------------
                                                                  //  
      Display = EASYCAT.BufferOut.Byte[1];                        // read Output Byte[1]
                                                                  //
      MFS.write(Display);                                         // and show its value on the display
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
