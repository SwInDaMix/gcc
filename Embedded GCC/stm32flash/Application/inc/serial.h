/*
  stm32flash - Open Source ST STM32 flash program for *nix
  Copyright (C) 2010 Geoffrey McRae <geoff@spacevs.com>

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/


#ifndef _SERIAL_H
#define _SERIAL_H

#include <stdint.h>

typedef struct serial serial_t;

typedef enum {
	SERIAL_PARITY_NONE,
	SERIAL_PARITY_EVEN,
	SERIAL_PARITY_ODD,

	SERIAL_PARITY_INVALID
} serial_parity_t;

typedef enum {
	SERIAL_BITS_5,
	SERIAL_BITS_6,
	SERIAL_BITS_7,
	SERIAL_BITS_8,

	SERIAL_BITS_INVALID
} serial_bits_t;

typedef enum {
	SERIAL_BAUD_1200,
	SERIAL_BAUD_1800,
	SERIAL_BAUD_2400,
	SERIAL_BAUD_4800,
	SERIAL_BAUD_9600,
	SERIAL_BAUD_19200,
	SERIAL_BAUD_38400,
	SERIAL_BAUD_57600,
	SERIAL_BAUD_115200,
	SERIAL_BAUD_128000,
	SERIAL_BAUD_230400,
	SERIAL_BAUD_256000,
	SERIAL_BAUD_460800,
	SERIAL_BAUD_500000,
	SERIAL_BAUD_576000,
	SERIAL_BAUD_921600,
	SERIAL_BAUD_1000000,
	SERIAL_BAUD_1500000,
	SERIAL_BAUD_2000000,

	SERIAL_BAUD_INVALID
} serial_baud_t;

typedef enum {
	SERIAL_STOPBIT_1,
	SERIAL_STOPBIT_2,

	SERIAL_STOPBIT_INVALID
} serial_stopbit_t;

typedef enum {
	SERIAL_ERR_OK = 0,

	SERIAL_ERR_SYSTEM,
	SERIAL_ERR_UNKNOWN,
	SERIAL_ERR_INVALID_BAUD,
	SERIAL_ERR_INVALID_BITS,
	SERIAL_ERR_INVALID_PARITY,
	SERIAL_ERR_INVALID_STOPBIT,
	SERIAL_ERR_NODATA
} serial_err_t;

typedef enum {
	GPIO_RTS = 1,
	GPIO_DTR,
	GPIO_BRK,
} serial_gpio_t;

serial_t *   serial_open (char const *device);
void         serial_close(serial_t *h);
void         serial_flush(serial_t const *h);
serial_err_t serial_setup(serial_t *h, serial_baud_t const baud, serial_bits_t const bits, serial_parity_t const parity, serial_stopbit_t const stopbit);
serial_err_t serial_write(serial_t const *h, void const *buffer, unsigned int len);
serial_err_t serial_read (serial_t const *h, void *buffer, unsigned int len);
char const * serial_get_setup_str(serial_t const *h);
serial_err_t serial_gpio (serial_t const *h, serial_gpio_t n, int level);

/* common helper functions */
serial_baud_t      serial_get_baud       (unsigned int const baud);
unsigned int serial_get_baud_int   (serial_baud_t const baud);
serial_bits_t      serial_get_bits       (char const *mode);
unsigned int serial_get_bits_int   (serial_bits_t const bits);
serial_parity_t    serial_get_parity     (char const *mode);
char         serial_get_parity_str (serial_parity_t const parity);
serial_stopbit_t   serial_get_stopbit    (char const *mode);
unsigned int serial_get_stopbit_int(serial_stopbit_t const stopbit);

#endif