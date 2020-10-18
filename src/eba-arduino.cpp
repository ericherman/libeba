/* SPDX-License-Identifier: LGPL-2.1-or-later */
/* eba-arduino.cpp */
/* Copyright (C) 2020 Eric Herman */
/* https://github.com/ericherman/libeba */

#include <Arduino.h>
#include <HardwareSerial.h>
#include "eba.h"

void print_z(size_t z)
{
	Serial.print(z);
}

void print_s(const char *s)
{
	Serial.print(s);
}

void print_eol(void)
{
	Serial.println();
}

void (*call_null_pointer_to_crash_and_reboot)(void) = NULL;
void die(void)
{
	Serial.println("EXIT FAILURE");
	delay(5 * 1000);
	Serial.println();
	call_null_pointer_to_crash_and_reboot();
}

void eba_arduino_init(void)
{
	eba_debug_print_z = print_z;
	eba_debug_print_s = print_s;
	eba_debug_print_eol = print_eol;
	eba_debug_die = die;
}
