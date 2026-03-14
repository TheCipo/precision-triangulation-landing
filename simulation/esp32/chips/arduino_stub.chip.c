#include "wokwi-api.h"
#include <stdio.h>
#include <string.h>
#include "sample_data.h"

typedef struct {
  i2c_dev_t i2c;
  uint8_t tx_buf[6];
  uint8_t tx_idx;
  uint8_t rx_buf[2];
  uint8_t rx_idx;
  uint16_t sample_idx;
  int16_t max_distance;
  bool reading;
} chip_state_t;

static bool on_i2c_connect(void *user_data, uint32_t address, bool read) {
  chip_state_t *chip = (chip_state_t *)user_data;
  chip->reading = read;
  chip->tx_idx = 0;
  chip->rx_idx = 0;

  if (read) {
    const int16_t *row = SAMPLE_DATA[chip->sample_idx];
    chip->tx_buf[0] = (uint8_t)(row[0] & 0xFF);
    chip->tx_buf[1] = (uint8_t)((row[0] >> 8) & 0xFF);
    chip->tx_buf[2] = (uint8_t)(row[1] & 0xFF);
    chip->tx_buf[3] = (uint8_t)((row[1] >> 8) & 0xFF);
    chip->tx_buf[4] = (uint8_t)(row[2] & 0xFF);
    chip->tx_buf[5] = (uint8_t)((row[2] >> 8) & 0xFF);
  }

  (void)address;
  return true;
}

static uint8_t on_i2c_read(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  if (chip->tx_idx >= sizeof(chip->tx_buf)) {
    return 0;
  }
  return chip->tx_buf[chip->tx_idx++];
}

static bool on_i2c_write(void *user_data, uint8_t data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  if (chip->rx_idx < sizeof(chip->rx_buf)) {
    chip->rx_buf[chip->rx_idx++] = data;
    if (chip->rx_idx == 2) {
      chip->max_distance = (int16_t)((chip->rx_buf[1] << 8) | chip->rx_buf[0]);
      printf("ArduinoStub MAXdistance=%d\n", chip->max_distance);
    }
  }
  return true;
}

static void on_i2c_disconnect(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;
  if (chip->reading && chip->tx_idx >= sizeof(chip->tx_buf)) {
    chip->sample_idx = (uint16_t)((chip->sample_idx + 1) % SAMPLE_COUNT);
  }
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  memset(chip, 0, sizeof(chip_state_t));

  const i2c_config_t i2c_config = {
    .address = 10,
    .scl = pin_init("SCL", INPUT_PULLUP),
    .sda = pin_init("SDA", INPUT_PULLUP),
    .connect = on_i2c_connect,
    .read = on_i2c_read,
    .write = on_i2c_write,
    .disconnect = on_i2c_disconnect,
    .user_data = chip,
  };

  chip->i2c = i2c_init(&i2c_config);
}
