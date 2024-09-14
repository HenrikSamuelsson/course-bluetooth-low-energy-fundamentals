/*
 * Copyright (c) 2023 Nordic Semiconductor ASA
 *
 * SPDX-License-Identifier: LicenseRef-Nordic-5-Clause
 */

#include <dk_buttons_and_leds.h>
#include <zephyr/bluetooth/addr.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/bluetooth/gap.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

/* Create the advertising parameter for connectable advertising. */
static struct bt_le_adv_param *adv_param =
    BT_LE_ADV_PARAM((BT_LE_ADV_OPT_CONNECTABLE | BT_LE_ADV_OPT_USE_IDENTITY),
                    800,   /*Min Advertising Interval 500ms (800*0.625ms). */
                    801,   /*Max Advertising Interval 500.625ms (801*0.625ms).*/
                    NULL); /* Set to NULL for undirected advertising.*/

LOG_MODULE_REGISTER(Lesson2_Exercise3, LOG_LEVEL_INF);

#define DEVICE_NAME CONFIG_BT_DEVICE_NAME
#define DEVICE_NAME_LEN (sizeof(DEVICE_NAME) - 1)

#define RUN_STATUS_LED DK_LED1
#define RUN_LED_BLINK_INTERVAL 1000

static const struct bt_data ad[] = {
    /* Set the flags, discovery mode flag is enabled to act as peripheral. */
    BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_GENERAL | BT_LE_AD_NO_BREDR)),
    /* Include the device name in the advertising pocket. */
    BT_DATA(BT_DATA_NAME_COMPLETE, DEVICE_NAME, DEVICE_NAME_LEN),

};

static const struct bt_data sd[] = {
    /* Include the UUID of the LED Button Service in the scan response packet */
    BT_DATA_BYTES(
        BT_DATA_UUID128_ALL,
        BT_UUID_128_ENCODE(0x00001523, 0x1212, 0xefde, 0x1523, 0x785feabcd123)),
};

int main(void) {
  int blink_status = 0;
  int err;

  LOG_INF("Starting Lesson 2 - Exercise 3 \n");

  err = dk_leds_init();
  if (err) {
    LOG_ERR("LEDs init failed (err %d)\n", err);
    return -1;
  }

  /* Setup the random static address. */
  bt_addr_le_t addr;
  err = bt_addr_le_from_str("FF:EE:DD:CC:BB:AA", "random", &addr);
  if (err) {
    printk("Invalid BT address (err %d)\n", err);
  }

  err = bt_id_create(&addr, NULL);
  if (err < 0) {
    printk("Creating new ID failed (err %d)\n", err);
  }

  err = bt_enable(NULL);
  if (err) {
    LOG_ERR("Bluetooth init failed (err %d)\n", err);
    return -1;
  }

  LOG_INF("Bluetooth initialized\n");

  /* Start the advertising, with our advertising data and scan response. */
  err = bt_le_adv_start(adv_param, ad, ARRAY_SIZE(ad), sd, ARRAY_SIZE(sd));
  if (err) {
    LOG_ERR("Advertising failed to start (err %d)\n", err);
    return -1;
  }

  LOG_INF("Advertising successfully started\n");

  for (;;) {
    dk_set_led(RUN_STATUS_LED, (++blink_status) % 2);
    k_sleep(K_MSEC(RUN_LED_BLINK_INTERVAL));
  }
}
