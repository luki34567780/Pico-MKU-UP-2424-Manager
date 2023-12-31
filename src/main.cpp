#include <Arduino.h>
#include <MkuUP2424BManager.h>
#include <SoftwareSerial.h>
#include <hardware/gpio.h>

#define EVENT_PIN 10
#define ALWAYS_HIGH_PIN 11
#define ACTIVE_PIN_STATE true

#define SOFTWARE_SERIAL true

#if !SOFTWARE_SERIAL
MkuUP2424Manager manager(&Serial1, 10000, true);
#endif

#if SOFTWARE_SERIAL
SoftwareSerial sw_serial(1, 0, true);
MkuUP2424Manager manager(&sw_serial, 10000, true);
#endif

bool old_state = !ACTIVE_PIN_STATE;

void setup() {
  while (!Serial) ;

  delay(100);

#if SOFTWARE_SERIAL
  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);
#endif

#if !SOFTWARE_SERIAL
  manager.Init(9600, 1, 0);
#endif

#if SOFTWARE_SERIAL
  manager.Init(9600);
  #endif

  delay(100);

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

  auto mode = manager.GetMode();

  manager.LogMessagePrintf("Current Transmitter state: %s", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, nameof::nameof_enum_flag(mode).data());
}

void loop() {
  auto new_state = gpio_get(EVENT_PIN);
  auto start = millis();

  if (new_state != old_state)
  {
    if (new_state == ACTIVE_PIN_STATE)
    {
      bool res = manager.TrySetMode(TransmitterMode::TX);
      
      if (!res)
      {
        res = manager.TrySetMode(TransmitterMode::TX);
      }
      
      if (!res)
      {
        manager.LogMessage("Setting Power mode failed!");
      }

      manager.LogMessagePrintf("Mode switch took %f ms", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, millis() - start);
    }
    else
    {
      bool res = manager.TrySetMode(TransmitterMode::RX);

      if (!res)
      {
        manager.LogMessage("Retrying mode change");

        res = manager.TrySetMode(TransmitterMode::RX);
      }

      if (!res)
      {
        manager.LogMessage("Setting Power mode failed!");
      }

      manager.LogMessagePrintf("Mode switch took %f ms", __builtin_FUNCTION(), __builtin_LINE(), LogLevel::INFO, millis() - start);
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