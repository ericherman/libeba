/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba_test_arduino.ino */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libefloat */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "eba-arduino.h"
#include "echeck-arduino.h"

int eba_test_get(int verbose);
int eba_test_new(int verbose);
int eba_test_rotate(int verbose);
int eba_test_set_all(int verbose);
int eba_test_set(int verbose);
int eba_test_shift_fill(int verbose);
int eba_test_shift_left(int verbose);
int eba_test_shift_right(int verbose);
int eba_test_swap(int verbose);
int eba_test_to_string(int verbose);
int eba_test_toggle(int verbose);

/* globals */
uint32_t loop_count;

void setup(void)
{
	Serial.begin(115200);

	eba_arduino_init();
	echeck_arduino_serial_log_init();

	delay(50);

	Serial.println("Begin");

	loop_count = 0;
}

void loop(void)
{
	int failures = 0;
	int verbose = 1;
	Serial.println("=================================================");
	++loop_count;
	Serial.print("Starting run #");
	Serial.println(loop_count);
	Serial.println("=================================================");

	int run_shift_tests = ((loop_count % 2) == 0);
	if (run_shift_tests) {
		Serial.println("Starting SHIFT tests");
		failures += eba_test_shift_left(verbose);
		failures += eba_test_shift_right(verbose);
		failures += eba_test_shift_fill(verbose);
	}

	failures += eba_test_get(verbose);
	failures += eba_test_new(verbose);
	failures += eba_test_rotate(verbose);
	failures += eba_test_set_all(verbose);
	failures += eba_test_set(verbose);
	failures += eba_test_swap(verbose);
	failures += eba_test_to_string(verbose);
	failures += eba_test_toggle(verbose);

	Serial.println("=================================================");
	if (failures) {
		Serial.print("failures = ");
		Serial.println(failures);
		Serial.println("FAIL");
	} else {
		Serial.println("SUCCESS");
	}
	Serial.println("=================================================");

	uint32_t delay_ms = 10 * 1000;
	delay(delay_ms);
}
