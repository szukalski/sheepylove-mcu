#include <Arduino.h>
#include <avr/sleep.h>

#define PA0 5 // PA0 == pin 6 == UPDI
#define PA1 2 // PA1 == pin 4 == LED
#define PA2 3 // PA2 == pin 5 == Relay -
#define PA3 4 // PA3 == pin 7 == Footswitch
#define PA6 0 // PA6 == pin 2 == Relay +
#define PA7 1 // PA7 == pin 3 == OVP warning
#define DEBOUNCE_TIME 200 // Debounce time in ms

struct FootSwitch {
  int pinSwitch;
};

struct Channel {   // Structure declaration
  int pinRelayMinus;
  int pinRelayPlus;
  int pinLed;
  bool state;
};

struct FootSwitch footSwitch = {
  .pinSwitch = PA3,
};

struct Channel channel = {
  .pinRelayMinus = PA2,
  .pinRelayPlus = PA6,
  .pinLed = PA1,
  .state = LOW,    // start with relay off
};

void setLed(Channel channel) {
  digitalWrite(channel.pinLed, channel.state);
}

void ackLed(Channel channel) {
  digitalWrite(channel.pinLed, HIGH);
  delay(250);
  digitalWrite(channel.pinLed, LOW);
  delay(250);
  digitalWrite(channel.pinLed, HIGH);
  delay(250);
  digitalWrite(channel.pinLed, LOW);
  delay(250);
}

void setRelay(Channel channel) {
  if (channel.state) {
    digitalWrite(channel.pinRelayPlus, HIGH);
    delay(10);
    digitalWrite(channel.pinRelayPlus, LOW);
  } else {
    digitalWrite(channel.pinRelayMinus, HIGH);
    delay(10);
    digitalWrite(channel.pinRelayMinus, LOW);
  }
}

void setChannel(Channel channel) {
  setRelay(channel);
  setLed(channel);
}

void interruptSwitch() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    channel.state = !channel.state;
    setChannel(channel);
    last_interrupt_time = interrupt_time;
  }
}

void setup() {
  cli();
  pinMode(channel.pinLed, OUTPUT);
  digitalWrite(channel.pinLed, LOW);
  pinMode(channel.pinRelayMinus, OUTPUT);
  digitalWrite(channel.pinRelayMinus, LOW);
  pinMode(channel.pinRelayPlus, OUTPUT);
  digitalWrite(channel.pinRelayPlus, LOW);
  pinMode(footSwitch.pinSwitch, INPUT_PULLUP);
  attachInterrupt(footSwitch.pinSwitch, interruptSwitch, LOW);
  sei();
  ackLed(channel);
  setChannel(channel);
}

void loop() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
}
