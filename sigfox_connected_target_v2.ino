
// ---------------------------------------------------------------------
// This file is a example for use the library armapi with
// The AirBoard using the SigBee shield module
//
// This software send periodically the digital value of the pin D3~ to Sigfox network.
//
// Created date: 06.09.2016
// Author: Louis Moreau
// Adapted from: https://github.com/atim-radiocommunications/armapi
// ---------------------------------------------------------------------

// ---------------------------------------------------------------------
// Include
// ---------------------------------------------------------------------
#include <arm.h>
#include <avr/sleep.h>
#include <TheAirBoard.h>
// ---------------------------------------------------------------------
// Define
// ---------------------------------------------------------------------
#define GREEN  5             // GREEN LED
#define BLUE   6              // BLUE LED
#define RED    9            // RED LED
#define PIEZO  3             //Digital Pin 
#define TIMEINTERVALL 900000  // 900 sec -> 15 min sleep time

// ---------------------------------------------------------------------
// Global variables
// ---------------------------------------------------------------------
//Instance of  the class Arm
Arm SigFox;

//Instance of the air board class
TheAirBoard board;

bool debug = false;

//The message to send at sigfox - Set the size of your message in bytes within the brackets
uint8_t msg[2];
//downlink received message
uint8_t data[8];

bool blue = false;
bool sigfoxOK = false;
bool trigger = false; 
int downlinkCounter;
int points = 0;
int count = 0;
//Only if you want some info on the Atim module
/*
uint8_t rev;
armType_t armType;
*/


// ---------------------------------------------------------------------
// Implemented functions
// ---------------------------------------------------------------------
void wakeUpNow()        // here the interrupt is handled after wakeup
{
  points++;
}

void setup()
{
  //Pin modes
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(PIEZO, INPUT);

  resetLeds();

  if(debug){
    Serial.begin(57600);
    digitalWrite(GREEN, HIGH);
  }else{
    //Init Arm and set LED to on if error
  	if (SigFox.Init(&Serial) != ARM_ERR_NONE){
  		digitalWrite(RED, HIGH);
      delay(1000);
      resetLeds();
  	}else{
      digitalWrite(GREEN, HIGH);
      delay(1000);
      resetLeds();
      //SigFox.Info(&armType, &rev, 0, 0, 0);
      sigfoxOK=true; 
      SigFox.SetMode(ARM_MODE_SFX);
      //Set Sigfox mode in uplink.
      SigFox.SfxEnableDownlink(false);
      //If downlink
      /*
      if(SigFox.SfxEnableDownlink(true)!= ARM_ERR_NONE){
        digitalWrite(RED, HIGH);
      }else{
        digitalWrite(GREEN, HIGH);
      }
      */
      SigFox.UpdateConfig();
  	}
  }
  points = 0;
  count = 0;
  delay(1000);
  resetLeds();
  attachInterrupt(1, wakeUpNow, RISING);
}

void sleepNow()         // here we put the device to sleep
{
    set_sleep_mode(SLEEP_MODE_PWR_DOWN);   // sleep mode is set here
 
    sleep_enable();          // enables the sleep bit in the mcucr register
                             // so sleep is possible. just a safety pin
 
    attachInterrupt(1, wakeUpNow, RISING);
 
    sleep_mode();            // here the device is actually put to sleep!!
                             // THE PROGRAM CONTINUES FROM HERE AFTER WAKING UP
 
    sleep_disable();         // first thing after waking from sleep:
                             // disable sleep...
    //detachInterrupt(digitalPinToInterrupt(PIEZO)); // disables interrupt so the
                             // wakeUpNow code will not be executed
                             // during normal running time.
 
}

void loop()
{ 
    if(debug){
      Serial.print("Awake for ");
      Serial.print(count);
      Serial.println(" sec");
      Serial.print("Digital value :");
      Serial.println(digitalRead(PIEZO));
      Serial.print("Points :");
      Serial.println(points);
    }
    blinkBlue();
    delay(500);
    count++;
    if(count>10){
      resetLeds();
      if(points>1){ //to avoid sending message when switching on the device + to avoid false positive
        sendMessage();
      }else{
        digitalWrite(RED, HIGH);
      }
      delay(1000);
      resetLeds();
      count = 0;
      points = 0;
      sleepNow();// sleep function called here
    }
  
  
}

void sendMessage(){
  if(sigfoxOK){    
      unsigned int i;
      digitalWrite(BLUE, HIGH);
      msg[0] = board.batteryChk() * 10; //We multiply the battery result by 10 so that it is easier to fill it in a byte as an integer. It should be between 2,9 and 4,2V thus 29 and 42
      msg[1] = points;
      //Only if you want to check the software revision
      /*
      msg[2] = rev;
      msg[3] = armType;
      */
      //Send the message to sigfox
      if(!debug){
        SigFox.Send(msg, sizeof(msg));
        resetLeds();
        //@TODO Ask Atim why downlink doesn't work
        /*SigFox.Receive(&data, sizeof(data), 60000);
        if(data[i]){
          digitalWrite(GREEN, HIGH);
        }else{
          digitalWrite(RED, HIGH);
        }*/
      }
      resetLeds();  
    }else{
      digitalWrite(RED, HIGH);
    } 
}

void blinkBlue(){
  if(blue){
    digitalWrite(BLUE, LOW);
    blue = false;
  }else{
    digitalWrite(BLUE, HIGH);
    blue = true;
  }
}

void resetLeds(){
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
  digitalWrite(RED, LOW);
}




