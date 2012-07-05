// Copyright 2012 Florian Brandner
//
// This file is part of the newlib C library for the Patmos processor.
//
// This file is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This code is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public License
// (COPYING3.LIB). If not, see <http://www.gnu.org/licenses/>.
//
// Some Patmos specific defines (e.g., concerning the UART coming with Patmos).

#ifndef __PATMOS__H
#define __PATMOS__H

/// Address to access the status register of the UART coming with Patmos
#define __PATMOS_UART_STATUS_ADDR 0x80000000

/// Address to access the data register of the UART coming with Patmos
#define __PATMOS_UART_DATA_ADDR 0x80000001


/// Bit mask for the transmit-ready bit (TRE)
#define __PATMOS_UART_TRE 1

/// Bit mask for the data-available bit (DAV)
#define __PATMOS_UART_DAV 2

/// Bit mask for the parity-error bit (PAE)
#define __PATMOS_UART_PAE 4


/// Macro to read the UART's status register
#define __PATMOS_UART_STATUS(res) asm volatile ("lbl %0 = [%1];;" : "=r" (res) : "r" (__PATMOS_UART_STATUS_ADDR));

/// Macro to read the UART's data register
#define __PATMOS_UART_RD_DATA(res) asm volatile ("lbl %0 = [%1];;" : "=r" (res) : "r" (__PATMOS_UART_DATA_ADDR));

/// Macro to write the UART's data register
#define __PATMOS_UART_WR_DATA(data) asm volatile ("sbl [%0] = %1;;" : : "r" (__PATMOS_UART_DATA_ADDR), "r" (data));

#endif // __PATMOS__H