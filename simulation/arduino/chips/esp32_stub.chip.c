#include "wokwi-api.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

typedef enum {
  IDLE,
  START_A,
  START_B,
  START_C,
  BIT_SETUP,
  BIT_SCL_HIGH,
  BIT_SCL_LOW,
  ACK_SETUP,
  ACK_SCL_HIGH,
  ACK_SCL_LOW,
  STOP_A,
  STOP_B,
  STOP_C,
  DONE,
} tx_phase_t;

typedef struct {
  pin_t sda;
  pin_t scl;
  timer_t timer;
  uint32_t max_distance_attr;
  uint8_t bytes[2];
  uint8_t byte_idx;
  int8_t bit_idx;
  tx_phase_t phase;
} chip_state_t;

static inline void release_line(pin_t pin) {
  pin_mode(pin, INPUT_PULLUP);
}

static inline void drive_low(pin_t pin) {
  pin_mode(pin, OUTPUT_LOW);
}

static inline void set_data_bit(chip_state_t *chip, uint8_t bit) {
  if (bit) {
    release_line(chip->sda);
  } else {
    drive_low(chip->sda);
  }
}

static void timer_cb(void *user_data) {
  chip_state_t *chip = (chip_state_t *)user_data;

  if (chip->phase == IDLE) {
    uint16_t max_distance = (uint16_t)attr_read(chip->max_distance_attr);
    chip->bytes[0] = (uint8_t)(max_distance & 0xFF);
    chip->bytes[1] = (uint8_t)((max_distance >> 8) & 0xFF);
    chip->byte_idx = 0;
    chip->bit_idx = 7;
    chip->phase = START_A;
    timer_start(chip->timer, 20, true);
    return;
  }

  switch (chip->phase) {
    case START_A:
      release_line(chip->sda);
      release_line(chip->scl);
      chip->phase = START_B;
      break;

    case START_B:
      drive_low(chip->sda);
      chip->phase = START_C;
      break;

    case START_C:
      drive_low(chip->scl);
      chip->phase = BIT_SETUP;
      break;

    case BIT_SETUP: {
      uint8_t value = chip->bytes[chip->byte_idx];
      set_data_bit(chip, (uint8_t)((value >> chip->bit_idx) & 1));
      chip->phase = BIT_SCL_HIGH;
      break;
    }

    case BIT_SCL_HIGH:
      release_line(chip->scl);
      chip->phase = BIT_SCL_LOW;
      break;

    case BIT_SCL_LOW:
      drive_low(chip->scl);
      chip->bit_idx--;
      if (chip->bit_idx < 0) {
        chip->phase = ACK_SETUP;
      } else {
        chip->phase = BIT_SETUP;
      }
      break;

    case ACK_SETUP:
      release_line(chip->sda);
      chip->phase = ACK_SCL_HIGH;
      break;

    case ACK_SCL_HIGH:
      release_line(chip->scl);
      chip->phase = ACK_SCL_LOW;
      break;

    case ACK_SCL_LOW:
      drive_low(chip->scl);
      chip->byte_idx++;
      if (chip->byte_idx >= 2) {
        chip->phase = STOP_A;
      } else {
        chip->bit_idx = 7;
        chip->phase = BIT_SETUP;
      }
      break;

    case STOP_A:
      drive_low(chip->sda);
      chip->phase = STOP_B;
      break;

    case STOP_B:
      release_line(chip->scl);
      chip->phase = STOP_C;
      break;

    case STOP_C:
      release_line(chip->sda);
      chip->phase = DONE;
      break;

    case DONE:
      timer_stop(chip->timer);
      printf("Esp32Stub sent MAXdistance over I2C\n");
      break;

    default:
      break;
  }
}

void chip_init() {
  chip_state_t *chip = malloc(sizeof(chip_state_t));
  memset(chip, 0, sizeof(chip_state_t));

  chip->sda = pin_init("SDA", INPUT_PULLUP);
  chip->scl = pin_init("SCL", INPUT_PULLUP);
  chip->max_distance_attr = attr_init("maxDistance", 200);

  const timer_config_t timer_config = {
    .callback = timer_cb,
    .user_data = chip,
  };

  chip->phase = IDLE;
  chip->timer = timer_init(&timer_config);
  timer_start(chip->timer, 100000, false);
}
