#include <Arduino.h>
#include <MkuUP2424BManager.h>
#include <SoftwareSerial.h>

#define EVENT_PIN 10
#define ALWAYS_HIGH_PIN 11
#define ACTIVE_PIN_STATE true

#define SOFTWARE_SERIAL false

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

  Serial1.

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

  auto online = manager.IsPoweredOn();

  manager.LogMessagePrintf("Powered on: %s", __builtin_FUNCTION(), __builtin_LINE(), online ? "true" : "false");

  while (!online)
  {
    manager.LogMessage("Please power on and try again!");
    
    delay(100);

    online = manager.IsPoweredOn();
  }

  auto mode = manager.GetMode();

  manager.LogMessagePrintf("Current Transmitter state: %s", __builtin_FUNCTION(), __builtin_LINE(), nameof::nameof_enum_flag(mode).data());

  if (manager.TrySetMode(TransmitterMode::TX))
  {
    manager.LogMessage("Setting to TX mode worked!");
  }
  else
  {
    manager.LogMessage("Setting To TX mode failed!");
  }

  mode = manager.GetMode();

  manager.LogMessagePrintf("Current transmitter state: %s\n", __builtin_FUNCTION(), __builtin_LINE(), nameof::nameof_enum_flag(mode).data());
}

void loop() {
  auto new_state = gpio_get(EVENT_PIN);
  if (new_state != old_state)
  {
    if (new_state == ACTIVE_PIN_STATE)
    {
      bool res = manager.TrySetMode(TransmitterMode::TX);
      
      if (!res)
      {
        res = manager.TrySetMode(TransmitterMode::TX);
      }
      
      manager.LogMessage("Setting Power mode failed!");
    }
    else
    {
      bool res = manager.TrySetMode(TransmitterMode::TX);

      if (!res)
      {
        res = manager.TrySetMode(TransmitterMode::TX);
      }

      manager.LogMessage("Setting Power mode failed!");
    }

    // todo: remove after testing is finished
    for (int i = 0; i < 3; i++)
    {
      gpio_put(LED_BUILTIN, true);
      delay(100);
      gpio_put(LED_BUILTIN, false);
      delay(100);
    }

    old_state = new_state;
  }

  gpio_put(LED_BUILTIN, new_state);
}