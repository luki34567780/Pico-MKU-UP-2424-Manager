#include <Arduino.h>
#include <MkuUP2424BManager.h>
#include <SoftwareSerial.h>
#include <hardware/gpio.h>

#define EVENT_PIN 10
#define ALWAYS_HIGH_PIN 11
#define ACTIVE_PIN_STATE true
#define BAUD 115200

// change this from LED_BUILTIN to some pin
// to add a external PTT indicator
// the pin will go to HIGH when the
// transceiver is in TX mode
#define PTT_LED LED_BUILTIN

SoftwareSerial sw_serial(1, 0, true);

#if DEBUG
MkuUP2424Manager manager(&sw_serial, true);
#else
MkuUP2424Manager manager(&sw_serial, false);
#endif

bool old_state = !ACTIVE_PIN_STATE;

void setup() {
  #if DEBUG
  // if DEBUG is true wait for serial port
  // this is to make sure that no logging messages are lost
  while (!Serial) ;

  delay(100);
  #endif

  manager.Init(BAUD);

  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);

  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);

  gpio_init(EVENT_PIN);
  gpio_set_dir(EVENT_PIN, false);
  gpio_set_pulls(EVENT_PIN, false, true);

  gpio_init(ALWAYS_HIGH_PIN);
  gpio_set_dir(ALWAYS_HIGH_PIN, true);
  gpio_put(ALWAYS_HIGH_PIN, true);

  gpio_init(PTT_LED);
  gpio_set_dir(PTT_LED, true);

  gpio_put(PTT_LED, 1);
  delay(1000);
  gpio_put(PTT_LED, 0);
}

void loop() {
  auto new_state = gpio_get(EVENT_PIN);
  auto start = micros();

  if (new_state != old_state)
  {
    if (new_state == ACTIVE_PIN_STATE)
    {
      manager.SetMode(TransmitterMode::TX);
      manager.SetMode(TransmitterMode::TX);

      manager.LogMessagePrintf("Mode switch took %f ms", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, (double)(micros() - start) / 1000.0);
    }
    else
    {
      manager.SetMode(TransmitterMode::RX);
      manager.SetMode(TransmitterMode::RX);

      manager.LogMessagePrintf("Mode switch took %f ms", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, (double)(micros() - start) / 1000.0);
    }

    #ifdef DEBUG
    for (int i = 0; i < 3; i++)
    {
      gpio_put(LED_BUILTIN, true);
      delay(100);
      gpio_put(LED_BUILTIN, false);
      delay(100);
    }
    #endif

    old_state = new_state;
  }

  gpio_put(PTT_LED, new_state);
}