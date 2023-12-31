#include <Arduino.h>
#include <MkuUP2424BManager.h>
#include <SoftwareSerial.h>
#include <hardware/gpio.h>

#define EVENT_PIN 10
#define ALWAYS_HIGH_PIN 11
#define ACTIVE_PIN_STATE true

SoftwareSerial sw_serial(1, 0, true);

#if DEBUG
MkuUP2424Manager manager(&sw_serial, true);
#else
MkuUP2424Manager manager(&sw_serial, false);
#endif

bool old_state = !ACTIVE_PIN_STATE;

void setup() {
  #if DEBUG
  while (!Serial) ;

  delay(100);
  #endif

  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);
  manager.Init(9600);

  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);

  gpio_init(EVENT_PIN);
  gpio_set_dir(EVENT_PIN, false);
  gpio_set_pulls(EVENT_PIN, false, true);

  gpio_init(ALWAYS_HIGH_PIN);
  gpio_set_dir(ALWAYS_HIGH_PIN, true);
  gpio_put(ALWAYS_HIGH_PIN, true);

  gpio_init(LED_BUILTIN);
  gpio_set_dir(LED_BUILTIN, true);

  gpio_put(LED_BUILTIN, 1);
  delay(1000);
  gpio_put(LED_BUILTIN, 0);
}

void loop() {
  auto new_state = gpio_get(EVENT_PIN);
  auto start = micros();

  if (new_state != old_state)
  {
    if (new_state == ACTIVE_PIN_STATE)
    {
      manager.TrySetMode(TransmitterMode::TX);
      manager.TrySetMode(TransmitterMode::TX);

      manager.LogMessagePrintf("Mode switch took %f ms", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, (double)(micros() - start) / 1000.0);
    }
    else
    {
      manager.TrySetMode(TransmitterMode::RX);
      manager.TrySetMode(TransmitterMode::RX);

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

  gpio_put(LED_BUILTIN, new_state);
}