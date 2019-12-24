// set clock to 1MHz; 1 microsecond = 1 clock cycle
#define F_CPU 1000000L

#include <avr/sleep.h>
#include <avr/wdt.h>

// https://code.google.com/p/ebl-arduino/
#include <ButtonEvent.h>
#include <LedControl.h>

#define DEBUG_ENABLED 0
#define DEBUG(x) if(DEBUG_ENABLED){Serial.println(x);}

/**
 * initialize hardware and event handlers
 */
void setup() {
  wdt_disable();
  Serial.begin(9600);
  
  DEBUG("*setting pins");
  pinMode(13,OUTPUT);
  
  DEBUG("*settings events");
  ButtonEvent.addButton(A0, 0, 0, buttonOnDownEvent, NULL, NULL, 100, NULL, 0);
  ButtonEvent.addButton(2, buttonOnDownEvent, NULL, NULL, 100, NULL, 0);
  ButtonEvent.addButton(3, buttonOnDownSleepEvent, NULL, NULL, 100, NULL, 0);
  wdt_enable(WDTO_8S);
  
  DEBUG("*start LED blinking");
  LedControl.startBlink(13, 250);
}

/**
 * permit event handlers to update and pat the dog
 */
void loop() {
  DEBUG("*updating state");
  ButtonEvent.loop();
  LedControl.loop();
  
  DEBUG("*reset watchdog");
  wdt_reset();  
}

/**
 * handle a generic button push
 * \param Sender button event data
 */
void buttonOnDownEvent(ButtonInformation *Sender) {
  DEBUG("*button pressed");
  DEBUG(Sender->pin);
}

/**
 * handle a sleep button push
 * \param Sender button event data
 */
void buttonOnDownSleepEvent(ButtonInformation *Sender) {
  DEBUG("*sleep button pressed");
  DEBUG(Sender->pin);
  sleepNow(Sender->pin);
}

/**
 * put the device to sleep.  Listen for interrupts on interrupt 1,
 * which is digital pin 3.
 * \param pin the pin number from which the event was triggered
 */
void sleepNow(int pin) {
  DEBUG("*event sleep");
  DEBUG(pin);
  
  digitalWrite(13, LOW);
  Serial.flush();
  wdt_disable();
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  attachInterrupt(1,noopEvent, LOW);
  sleep_mode();
  
  DEBUG("*interrupt wakeup");
  sleep_disable();
  detachInterrupt(1);
  wdt_enable(WDTO_8S);
  
  while(digitalRead(pin) == LOW) {;}
  delay(50);
}

/** 
 * do nothing
 */
void noopEvent() {;}
