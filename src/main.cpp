#include <Arduino.h>
#include <MkuUP2424BManager.h>
#include <SoftwareSerial.h>

#define EVENT_PIN 10
#define ALWAYS_HIGH_PIN 11
#define ACTIVE_PIN_STATE true

//MkuUP2424Manager manager(&sw_serial, 100, true);

SoftwareSerial sw_serial(0, 1);
MkuUP2424Manager manager(&Serial1, 100, true);

bool old_state = !ACTIVE_PIN_STATE;

void setup() {
  while (!Serial) ;

  delay(100);

  gpio_set_pulls(0, true, false);
  gpio_set_pulls(1, true, false);

  // manager.Init(9600, 1, 0);
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

  auto online = manager.IsPoweredOn();

  Serial.printf("Powered on: %s\n", online ? "true" : "false");

  auto mode = manager.GetMode();

  Serial.printf("Mode: %s\n", nameof::nameof_enum_flag(mode).data());

  if (manager.TrySetMode(TransmitterMode::TX))
  {
    Serial.printf("Setting to TX mode worked!\n");
  }
  else
  {
    Serial.printf("Setting To TX mode failed!\n");
  }

  mode = manager.GetMode();

  Serial.printf("Mode: %s\n", nameof::nameof_enum_flag(mode).data());
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
      
      Serial.printf("Setting Power mode failed!\n");
    }
    else
    {
      bool res = manager.TrySetMode(TransmitterMode::TX);

      if (!res)
      {
        res = manager.TrySetMode(TransmitterMode::TX);
      }

      Serial.printf("Setting Power mode failed!\n");
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