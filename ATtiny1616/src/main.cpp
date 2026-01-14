#include <Arduino.h>
#include <avr/sleep.h>

// ATtiny 1616
#define PIN_UPDI PIN_PA0
#define PIN_OVP PIN_PA7

#define CH1_FS PIN_PA2
#define CH1_PIN_RELAY_PLUS PIN_PA4
#define CH1_PIN_RELAY_MINUS PIN_PA5
#define CH1_LED PIN_PA6

#define CH2_PIN_RELAY_PLUS PIN_PC0
#define CH2_PIN_RELAY_MINUS PIN_PC1
#define CH2_LED PIN_PC2
#define CH2_FS PIN_PC3

#define DEBOUNCE_TIME 200 // Debounce time in ms

struct Channel {   // Structure declaration
  int pinSwitch;
  int pinRelayMinus;
  int pinRelayPlus;
  int pinLed;
  bool state;
  long last_interrupt_time;
};

struct Channel channel1 = {
  .pinSwitch = CH1_FS,
  .pinRelayMinus = CH1_PIN_RELAY_MINUS,
  .pinRelayPlus = CH1_PIN_RELAY_PLUS,
  .pinLed = CH1_LED,
  .state = LOW,    // start with relay off
  .last_interrupt_time = 0
};

struct Channel channel2 = {
  .pinSwitch = CH2_FS,
  .pinRelayMinus = CH2_PIN_RELAY_MINUS,
  .pinRelayPlus = CH2_PIN_RELAY_PLUS,
  .pinLed = CH2_LED,
  .state = LOW,    // start with relay off
  .last_interrupt_time = 0
};

void setLed(Channel channel) {
  digitalWrite(channel.pinLed, channel.state);
};

void ackLed(Channel channel) {
  digitalWrite(channel.pinLed, HIGH);
  delay(250);
  digitalWrite(channel.pinLed, LOW);
  delay(250);
  digitalWrite(channel.pinLed, HIGH);
  delay(250);
  digitalWrite(channel.pinLed, LOW);
  delay(250);
};

void setRelay(Channel channel) {
  if (channel.state) {
    digitalWrite(channel.pinRelayPlus, HIGH);
    delay(200);
    digitalWrite(channel.pinRelayPlus, LOW);
  } else {
    digitalWrite(channel.pinRelayMinus, HIGH);
    delay(200);
    digitalWrite(channel.pinRelayMinus, LOW);
  }
}

void setChannel(Channel channel) {
  setRelay(channel);
  setLed(channel);
}

void interruptSwitch1() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    channel1.state = !channel1.state;
    setChannel(channel1);
    last_interrupt_time = interrupt_time;
  }
}

void interruptSwitch2() {
  static unsigned long last_interrupt_time = 0;
  unsigned long interrupt_time = millis();
  if (interrupt_time - last_interrupt_time > DEBOUNCE_TIME) {
    channel2.state = !channel2.state;
    setChannel(channel2);
    last_interrupt_time = interrupt_time;
  }
}

void setupChannel(Channel channel) {
  pinMode(channel.pinLed, OUTPUT);
  digitalWrite(channel.pinLed, LOW);
  pinMode(channel.pinRelayMinus, OUTPUT);
  digitalWrite(channel.pinRelayMinus, LOW);
  pinMode(channel.pinRelayPlus, OUTPUT);
  digitalWrite(channel.pinRelayPlus, LOW);
  pinMode(channel.pinSwitch, INPUT_PULLUP);
};

void setup() {
  cli();
  setupChannel(channel1);
  attachInterrupt(channel1.pinSwitch, interruptSwitch1, LOW);
  setupChannel(channel2);
  attachInterrupt(channel2.pinSwitch, interruptSwitch2, LOW);
  sei();
  ackLed(channel1);
  setChannel(channel1);
  ackLed(channel2);
  setChannel(channel2);
}

void loop() {
  set_sleep_mode(SLEEP_MODE_IDLE);
  sleep_mode();
}