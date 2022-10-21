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


// !!! Important Note !!!
// Please read the application note "EasyCAT and Adafruit TFT display AN003"
// downloadable from the AB&T website http://www.bausano.net



//---- AB&T EasyCAT shield simple HMI application example --- V.2_0 --------------------------------------------------  


//V.2_0  Fixed the analog bars for negative values 


// This application demonstrates a simple EtherCAT HMI (human machine interface) built by:
//
// an EasycatShield                                            http://www.bausano.net/en/hardware/ethercat-e-arduino/easycat.html
// an Arduino UNO                                              https://www.arduino.cc/en/Main/ArduinoBoardUno
// an Adafruit 2.8" TFT capacitive touch shield for Arduino    https://www.adafruit.com/product/1947

// Many thanks to the Arduino Team and to Adafruit for their cool products  :-)


//--------------------------------------------------------------------------------------------------------------------



#include "EasyCAT.h"                                      // EasyCAT library

#include <Adafruit_GFX.h>                                 // Core graphics library

#include <SPI.h>                                          // this is needed for display
#include <Adafruit_ILI9341.h>                             //

#include <Wire.h>                                         // this is needed for FT6206
#include <Adafruit_FT6206.h>                              //

#include <Fonts/FreeMonoBold24pt7b.h>
#include <Fonts/FreeMonoBold12pt7b.h>
                                                
Adafruit_FT6206 ctp = Adafruit_FT6206();                  // The FT6206 uses hardware I2C (SCL/SDA)

#define TFT_CS 10                                         // The display also uses hardware SPI, plus #9 & #10
#define TFT_DC 9                                          //
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);  //


#define	BLACK   0x0000                    // assign human-readable names to some common 16-bit color values
#define	BLUE    0x001F                    //
#define	RED     0xF800                    //    
#define	GREEN   0x07E0                    //  
#define CYAN    0x07FF                    //
#define MAGENTA 0xF81F                    //
#define YELLOW  0xFFE0                    //
#define WHITE   0xFFFF                    //
                                          //
#define ORANGE  0xFC00                    //



EasyCAT EASYCAT(8);         // Instantiate the EasyCAT library (we need version 1.4 or higher)
                            // choosing pin 8 as SPI chip select not to interfere with the LCD                                
                            //
                            // !!! Remember to set pin 8 also on the EasyCAT board, using the jumper,   
                            // if you have rev B, or the 0 ohm resistor, if you have rev A !!! 


//---- global variables ---------------------------------------------------------------------------

unsigned long Millis, PreviousMillis;

int16_t Analog_X =0, Analog_X_prev = 1;
int16_t Analog_Y =0, Analog_Y_prev = 1;

bool FirstRound = true;
bool Blink = true;
bool Running = false;
unsigned char N_Cycles = 0;

unsigned char Conta;

unsigned char EcatState;

unsigned char LedStatus = 0;
unsigned char TouchStatus_prev = 0;



//---- setup ---------------------------------------------------------------------------------------
 
void setup()
{ 
  Serial.begin(9600);                                         // Initialize the debug serial line                       
          
  Serial.print ("\nEasyCAT HMI\n");                           // Print the banner
  Serial.print ("\nAB&T www.bausano.net\n");                  //
  Serial.print ("\nMade in ITALY\n");                         //      

  
  tft.begin();                                                // Initialize the LCD                            
    
  if (! ctp.begin(40))                                        // Initialize the touch screen
  {                                                           // pass in 'sensitivity' coefficient
    Serial.println("Couldn't start FT6206 touchscreen controller");
    while (1);
  }
  Serial.println("Capacitive touchscreen started");
    
 
  tft.fillScreen(BLACK);                                      // Clear the LCD
  tft.setRotation(1);                                         // Set the orientation to horizontal
   
  tft.setFont(&FreeMonoBold24pt7b);                           // Set large font
  tft.setTextSize(1);                                         // Set text size 
 
  tft.setTextColor(GREEN);                                    // Print the first part of the banner on the LCD  
  tft.setCursor(105, 60);                                     //
  tft.print ("AB&T");                                         //

  tft.setFont(&FreeMonoBold12pt7b);                           // Set small font
      
  tft.setCursor(60, 130);                                     // Print the second part of the banner on the LCD     
  tft.print ("www.bausano.net");                              //
  tft.setCursor(70, 190);                                     //
  tft.print ("Made in ITALY");                                //
  
  delay (3000);                                               // Wait a while  
  tft.fillScreen(BLACK);                                      // clear the screen

                                                              // Initialize the EasyCAT board

  if (EASYCAT.Init() == true)                                 //---- Initialization succeded -----------  
  {                                                           //  
    Serial.print ("inizialized");                             //  
                                                              //
    tft.setFont(&FreeMonoBold24pt7b);                         // Print the "OK" banner on the LCD
                                                              //
    tft.fillScreen(BLACK);                                    // clear the screen
                                                              //                                                             
    tft.setTextColor(ORANGE);                                 //  
    tft.setCursor(60, 50);                                    //
    tft.print ("EasyCAT");                                    //  
                                                              //
    tft.setFont(&FreeMonoBold12pt7b);                         //
                                                              //
    tft.setCursor(78, 110);                                   //
    tft.print ("inizialized!");                               //
    PrintSmile (160, 180, GREEN);                             //     
  }           
  
  else                                                        //---- Initialization failed -------------         
  {                                                           //
    Serial.print ("inizialization failed");                   //
                                                              //  
    tft.setFont(&FreeMonoBold24pt7b);                         // Print the "KO" banner on the LCD
                                                              //
    tft.fillScreen(BLACK);                                    // clear the screen
                                                              //   
    tft.setTextColor(ORANGE);                                 // The EasyCAT board is not recognized   
    tft.setCursor(63, 50);                                    // The most common reason is that the SPI 
    tft.print ("EasyCAT");                                    // chip select choosen on the board doesn't  
                                                              // match the one choosen by the firmware
    tft.setFont(&FreeMonoBold12pt7b);                         //
                                                              //
    tft.setCursor(78, 110);                                   //
    tft.print ("init failed!");                               //
    PrintSad (160, 180, GREEN);                               //  

    pinMode(13, OUTPUT);                                      //  
                                                                
    while(1)                                                  // Stay in loop for ever, blinking the Arduino led 
    {                                                         // 
      digitalWrite (13, LOW);                                 //
      delay(500);                                             //
      digitalWrite (13, HIGH);                                //  
      delay(500);                                             // 
    }                                                         // 
  }            

  delay (3000);                                               // Wait a while  
  tft.fillScreen(BLACK);                                      // Clear the screen

                                                              // ---- Print on the LCD the fixed texts and drawing of the HMI ---
  tft.setTextColor(YELLOW);                                   //
  tft.setFont(&FreeMonoBold24pt7b);                           //
                                                             
  tft.setCursor(85, 30);                                      // Analog input "X"      
  tft.print("X=");                                            //
  tft.drawRect(60, 40, 257, 15, YELLOW);                      //     
                                                              
  tft.setCursor(85, 110);                                     // Analog input "Y"
  tft.print("Y=");                                            //
  tft.drawRect(60, 123,  257, 15, YELLOW);                    //
  
  tft.setFont(&FreeMonoBold12pt7b);                           // Leds  
                                                              //
  tft.setCursor(8, 235);                                      // 
  tft.print("led1  led2  led3  led4");                        //
                                                              //
  tft.drawCircle(35, 185, 18, GREEN);                         //
  tft.drawCircle(120, 185, 18, GREEN);                        //
  tft.drawCircle(205, 185, 18, GREEN);                        //
  tft.drawCircle(290, 185, 18, GREEN);                        //  
  
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


//---- user application ----------------------------------------------------------------------------------------------------------

void Application ()                                        

{       
  Millis = millis();                                    // For this application we choose a cycle time of 150 mS
  if (Millis - PreviousMillis >= 150)                   // that is suitable for an HMI 
  {                                                     // 
    PreviousMillis = Millis;                            //

  
//                             -------------------------------- output data management ---------------------------------------------
    
                                                                          // We read the analog values from the EtherCAT output buffer,
                                                                          // i.e. from the data that come from the Master,
                                                                          // and visualize them on the LCD
                                                                                
    Analog_X =  EASYCAT.BufferOut.Byte[0] | (EASYCAT.BufferOut.Byte[1] << 8);   // Analog_X is mapped to output Bytes 0 and 1
    Analog_Y =  EASYCAT.BufferOut.Byte[2] | (EASYCAT.BufferOut.Byte[3] << 8);   // Analog_Y is mapped to output Bytes 2 and 3
  
    Analog_X = Analog_X >> 3;                                             // normalize the values on 12 bit i.e. from 0 t0 4095        
    Analog_Y  = Analog_Y >> 3;                                            //
  
    if ((Analog_X != Analog_X_prev) || (FirstRound == true))              // If the Analog_X is changed from the previous reading, or this  
    {                                                                     // is the first time we pass here, refresh the visualization
                                                                          //
      PrintValueRight (Analog_X, Analog_X_prev, 305, 30, ORANGE);         // Print the signed value on the LCD, in numerical form, right justified 
      PrintValueBar (abs(Analog_X), abs(Analog_X_prev), 61, 41, ORANGE);  // Print the absolute value on the LCD, as a bar 
                                                                          //  
      Analog_X_prev = Analog_X;                                           // Remember the value for the next cycle    
    }  
  
  
    if ((Analog_Y != Analog_Y_prev) || (FirstRound == true))              // The same for Analog_Y ...
    {                                                                     //  
      PrintValueRight (Analog_Y, Analog_Y_prev, 305, 110, ORANGE);        //
      PrintValueBar (abs(Analog_Y), abs(Analog_Y_prev), 61, 124, ORANGE); // 
                                                                          //
      Analog_Y_prev = Analog_Y;                                           //
    }  
  
    FirstRound = false;                                                   // Remember that we already passed here 
  
  
//                             -------------------------------- input data management ----------------------------------------------
  
                                                                          // We read the data from the touch, process them, and write
                                                                          // the result to the EtherCAT input buffer, i.e. to the data
                                                                          // that will be sent to the Master.
                                                                          
    ProcessTouch(&LedStatus);                                             // Read and process the data from the touch
                                                                          // and visualize the led status on the LCD  
  
    EASYCAT.BufferIn.Byte[0] = LedStatus;                                 // The status of the leds is mapped to Byte input 0
    
    EASYCAT.BufferIn.Byte[31] = Conta++;                                  // We also increase Conta every cycle and put it in 
                                                                          // input Byte 31, just for test pourpose   
  
  
  
//                             ------------------------------------- HALT/RUN visualization ---------------------------------------
  
    if (EcatState == 0x08)                                                // If the EasyCAT is in Operational State
    {                                                                     //
      if (!Running)                                                       // and the communication is running
      {                                                                   //
        PrintRun();                                                       // we print a green "RUN" on the LCD
        Running = true;                                                   //
      }                                                                   //
    }
  
    else                                                                  // Otherwise we print a blinking red "HALT" 
    {                                                                     //     
      Running = false;                                                    //
                                                                          //  
      if (N_Cycles++ > 4)                                                 //
      {                                                                   //
        N_Cycles = 0;                                                     //  
        if (Blink)                                                        //
        {                                                                 //
          PrintHalt();                                                    //  
          Blink = false;                                                  //
        }                                                                 //  
        else                                                              //  
        {                                                                 //
          tft.fillRect(27, 5, 15, 79, BLACK);                             //
          Blink = true;                                                   //
        }                                                                 //
      }    
    }
  }    
}   


//----- print a value between 0 and 4095, right justified  ------------------------------------------------------------

void PrintValueRight (int Value, int Value_prev, unsigned int x, unsigned int y, unsigned int Color)
{ 
  
  #define Width 32                                                  // space between characters, in pixel
  
  unsigned char i;
  unsigned char Length, Length_prev;
  unsigned char N_Clear, N_Print;
  char AsciiString [8];
  char AsciiString_prev [8];

                                                                    // We use the current and the previous value
                                                                    // to clear and redraw only the needed characters,
                                                                    // to avoid flickering
                                                            
  itoa (Value, AsciiString, 10  );                                  // Convert the current value and the previous value
  itoa (Value_prev, AsciiString_prev, 10  );                        // into ASCII strings
    
  Length = strlen(AsciiString);                                     // Calculate the strings length
  Length_prev = strlen(AsciiString_prev);                           //
    
  if (Length != Length_prev)                                        // If the string length is changed
  {                                                                 // we clear and redraw everything
    N_Clear = 5;                                                    //
    N_Print = Length;                                               //
  }                                                                 //

  else                                                              // Else we compute how many characters
  {                                                                 // to clear and redraw  
    for (i=0; i<Length; i++)                                        //
    {                                                               //
      if(AsciiString[i] != AsciiString_prev[i])                     //    
        break;                                                      //
    }                                                               //  
    N_Clear = Length - i;                                           //
    N_Print = N_Clear;                                              //
  }                                                                 //  

  tft.setFont(&FreeMonoBold24pt7b);                                 // Set large font

  tft.fillRect(x-(N_Clear*Width), y-30, N_Clear*Width, 32, BLACK);  // Clear the area

  x -= Width;                                                       // Set the X for the first character on the right
                    
  for (i=0; i<N_Print; i++)                                         // Print the string, from right to left 
  {                                                                 //
    tft.drawChar(x, y, AsciiString[Length-1-i], Color, 0, 1);       //
    x -= Width;                                                     //
  }                                                                 // 
}


//----- print a value between 0 and 4095, as a bar  ------------------------------------------------------------

void PrintValueBar (unsigned int Value, unsigned int Value_prev, unsigned int x, unsigned int y, unsigned int Color)
{ 
  
  #define Tickness 13                                               // Bar tickness, in pixel

  unsigned char Value_8, Value_8_prev;
                                                                    // We use the current and the previous value
                                                                    // to clear and redraw only the needed pixel,
                                                                    // to avoid flickering
                                                                    
  Value_8 = Value >> 4;                                             // We normalize the current and the previous      
  Value_8_prev = Value_prev >> 4;                                   // value to 8 bits, i.e. from 0 to 255

  if (Value_8 > Value_8_prev)
  {
   tft.fillRect(x + Value_8_prev, y, (Value_8 - Value_8_prev), Tickness, Color);  
  }  

  if (Value_8 < Value_8_prev)
  {
    tft.fillRect(x + Value_8, y, (Value_8_prev - Value_8), Tickness, BLACK);   
  }
}



//--------- print "Smile" emoticon -------------------------------------------------------------------------

void PrintSmile(unsigned int x, unsigned int y, unsigned int Color)

{ 
    tft.drawCircle(x, y-6, 21, Color);   
    tft.drawCircle(x, y-6, 20, Color); 
    tft.drawCircle(x, y-6, 19, Color);  
 
    tft.fillRect(x-30, y-30, 60, 38, BLACK);    
  
    tft.drawCircle(x, y, 30, Color);                          
    tft.drawCircle(x, y, 29, Color);  
    tft.drawCircle(x, y, 28, Color);      

    tft.fillCircle(x - 10, y- 8, 3, Color);
    tft.fillCircle(x + 10, y- 8, 3, Color);     
}  


//--------- print "Sad" emoticon--------------------------------------------------------------------------------

void PrintSad(unsigned int x, unsigned int y, unsigned int Color)

{ 
    tft.drawCircle(x, y+29, 21, Color);   
    tft.drawCircle(x, y+29, 20, Color); 
    tft.drawCircle(x, y+29, 19, Color);  
 
    tft.fillRect(x-30, y+16, 60, 35, BLACK);    
  
    tft.drawCircle(x, y, 30, Color);                          
    tft.drawCircle(x, y, 29, Color);  
    tft.drawCircle(x, y, 28, Color);      

    tft.fillCircle(x - 10, y- 8, 3, Color);
    tft.fillCircle(x + 10, y- 8, 3, Color);     
}  


//--------- print a vertical green "RUN" ----------------------------------------------------------------------

void PrintRun (void)

{
  tft.fillRect(27, 5, 15, 79, BLACK);   
  tft.setFont(&FreeMonoBold12pt7b);
  tft.setTextColor(GREEN);  
  tft.setCursor(27, 18);                                         
  tft.print("R"); 
  tft.setCursor(27, 39);                                         
  tft.print("U"); 
  tft.setCursor(27, 61);                                         
  tft.print("N"); 
}


//--------- print a vertical red "HALT" ----------------------------------------------------------------------

void PrintHalt (void)
{
  tft.fillRect(27, 5, 15, 79, BLACK);     
  tft.setFont(&FreeMonoBold12pt7b);  
  tft.setTextColor(RED);  
  tft.setCursor(27, 18);                                         
  tft.print("H"); 
  tft.setCursor(28,39);                                         
  tft.print("A"); 
  tft.setCursor(28, 61);                                         
  tft.print("L"); 
  tft.setCursor(28, 83);                                         
  tft.print("T");     
}


//--- process data from the Touch and draw led status on the LCD ------------------------------------------- 

void ProcessTouch(unsigned char* LedStatus)

{
  unsigned char LedToggle;
  unsigned char TouchStatus = 0x00;
                                                                         
  if (ctp.touched())                                                    // If the touch has been tapped, we process the data
  {
    TS_Point p = ctp.getPoint();                                        // Retrive the tapped point
    
   // Serial.print("X = "); Serial.print(p.x);                          // Print out raw data from the touch controller
   // Serial.print("\tY = "); Serial.println(p.y);                      // for debug pourpose  

    if (p.x > (160))                                                    // If the tapped point belong the led area at the 
                                                                        // bottom of the screen, find out wich led is and
                                                                        // set the corrisponding flag in TouchStatus                                                                       
    {                                                                                 
      if (p.y > (260))                                                  // led 1
      {                                                                 //
        TouchStatus |= 0x01;                                            //
//      Serial.println("1");                                            //        
      }                                                                 //

      if ((p.y < (225)) && (p.y > 170))                                 // led 2
      {                                                                 //
        TouchStatus |= 0x02;                                            //
//      Serial.println("2");                                            //        
      }                                                                 //

       if ((p.y < (140)) && (p.y > 85))                                 // led 3
      {                                                                 //
        TouchStatus |= 0x04;                                            //
//      Serial.println("3");                                            //        
      }                                                                 //
           
      if (p.y < (60))                                                   // led 4   
      {                                                                 //  
        TouchStatus |= 0x08;                                            //
//      Serial.println("4");                                            //        
      }                                                                 //

      LedToggle = ~TouchStatus_prev & TouchStatus;                      // If there is a rising edge in some bit of TouchStatus
      *LedStatus = *LedStatus ^ LedToggle;                              // we toggle the corrisponding bit in LedStatus 

      DrawLed(*LedStatus);                                              // Draw the leds on the LCD
    }  
  }

  TouchStatus_prev = TouchStatus;                                       // Remember if one led has been tapped    
}


//---- draw the leds on the LCD -----------------------------------------------------------------------------------------

void DrawLed (unsigned char LedStatus)

{
      if (LedStatus & 0x01)                                             // Draw the leds according to LedStatus
        tft.fillCircle(35, 185, 16, GREEN);                             //
      else                                                              //
        tft.fillCircle(35, 185, 16, BLACK);                             //
                                                                        //
      if (LedStatus & 0x02)                                             //  
        tft.fillCircle(120, 185, 16, GREEN);                            //
      else                                                              //
        tft.fillCircle(120, 185, 16, BLACK);                            //  
                                                                        //
      if (LedStatus & 0x04)                                             //
        tft.fillCircle(205, 185, 16, GREEN);                            //
      else                                                              //
        tft.fillCircle(205, 185, 16, BLACK);                            //
                                                                        //
      if (LedStatus & 0x08)                                             //
        tft.fillCircle(290, 185, 16, GREEN);                            //
      else                                                              //  
        tft.fillCircle(290, 185, 16, BLACK);                            // 
}



