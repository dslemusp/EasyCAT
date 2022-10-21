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



//---- AB&T EasyCAT shield loopback test V.2_0  ----------------------------------------------  


// In this example the output bytes received by the EasyCAT are transmitted back to 
// the master, for test pourpose
//
// The EasyCAT must be set in STANDARD MODE (default setting)




#include "EasyCAT.h"                // EasyCAT library to interface the LAN9252
#include <SPI.h>                    // SPI library

 
EasyCAT EASYCAT;                    // EasyCAT istantiation



//--- sanity check to see if the EasyCAT is set in STANDARD MODE --------------------------------------

#ifdef CUST_BYTE_NUM_OUT
  #error "The EasyCAT must be set in STANDARD MODE !!!"
#endif 

#ifdef CUST_BYTE_NUM_IN
  #error "The EasyCAT must be set in STANDARD MODE !!!"
#endif 


//---- global variables ---------------------------------------------------------------------------

unsigned long Millis = 0;
unsigned long PreviousMillis = 0;

//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{
  Serial.begin(9600);                                             // serial line initialization
                                                                  //(used only for debug)
           
  Serial.print ("\nEasyCAT - Generic EtherCAT slave\n");          // print the banner

 
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

void Application ()                                    // the received bytes for the outputs                                    
                                                       // are transmitted back, simulating the inputs
{
  uint8_t Index;
    
  Millis = millis();                                    // As an example for this application 
  if (Millis - PreviousMillis >= 10)                    // we choose a cycle time of 10 mS 
  {                                                     // 
    PreviousMillis = Millis;                            //
                              

    for (Index=0; Index < BYTE_NUM; Index++)                       // BYTE_NUM is a constant declared in the library file "EasyCAT.h"
                                                                   // it defines the number of bytes for the input and for the output                                           
                                                                   // when the EasyCAT is configured in STANDARD MODE (default setting)
                                                                                                                                                                               
    {
      EASYCAT.BufferIn.Byte[BYTE_NUM - 1 - Index] =  EASYCAT.BufferOut.Byte[Index] ^ 0xFF; // the data are crossed and complemented
    
      //EASYCAT.BufferIn.Byte[Index] =  EASYCAT.BufferOut.Byte[Index];                       // the data are trasmitted back as they are  
    }
  }    
}   





