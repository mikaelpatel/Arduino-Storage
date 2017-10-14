/**
 * @file MC23LCXXX.h
 * @version 1.0
 *
 * @section License
 * Copyright (C) 2017, Mikael Patel
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 */

#ifndef MC23LCXXX_H
#define MC23LCXXX_H

#include "SPI.h"
#include "Storage.h"

/**
 * Storage device driver for Microchip 23LCXXX, SPI Serial SRAM.
 * @param[in] KBYTE size.
 * @param[in] SS_PIN slave select board pin.
 * @param[in] FREQ max bus frequency for device (default 16 MHz).
 */
template<uint16_t KBYTE, BOARD::pin_t SS_PIN, uint32_t FREQ = 16000000L>
class MC23LCXXX : public Storage,
  protected SPI::Device<0, MSBFIRST, FREQ, SS_PIN>
{
public:
  /** Maximum device clock frequency. */
  static const uint32_t MAX_FREQ = 16000000L;

  /**
   * Construct and initiate device driver with given slave select
   * board pin, max bus frequency, and bus manager.
   * @param[in] spi bus manager.
   */
  MC23LCXXX(SPI& spi) :
    Storage(KBYTE * 1024UL),
    SPI::Device<0, MSBFIRST, FREQ, SS_PIN>(spi)
  {}

  /**
   * @override{Storage}
   * Read given count number of bytes from SRAM source address to
   * destination buffer. Returns number of bytes read, or negative
   * error code.
   * @param[in] dst destination buffer pointer.
   * @param[in] src source memory address on device.
   * @param[in] count number of bytes to read from device.
   * @return number of bytes read or negative error code.
   */
  virtual int read(void* dst, uint32_t src, size_t count)
  {
    uint8_t* sp = (uint8_t*) &src;
    header_t header;
    size_t size;
    header.cmd = READ;
    if (KBYTE > 64) {
      header.addr[0] = sp[2];
      header.addr[1] = sp[1];
      header.addr[2] = sp[0];
      size = sizeof(header);
    }
    else {
      header.addr[0] = sp[1];
      header.addr[1] = sp[0];
      size = sizeof(header) - 1;
    }
    acquire();
    write(&header, size);
    read(dst, count);
    release();
    return (count);
  }

  /**
   * @override{Storage}
   * Write given count number of bytes to SRAM destination address
   * from source buffer. Returns number of bytes written, or negative
   * error code.
   * @param[in] dst destination memory address on device.
   * @param[in] src source buffer pointer.
   * @param[in] count number of bytes to write to device.
   * @return number of bytes written or negative error code.
   */
  virtual int write(uint32_t dst, const void* src, size_t count)
  {
    uint8_t* dp = (uint8_t*) &dst;
    header_t header;
    size_t size;
    header.cmd = WRITE;
    if (KBYTE > 64) {
      header.addr[0] = dp[2];
      header.addr[1] = dp[1];
      header.addr[2] = dp[0];
      size = sizeof(header);
    }
    else {
      header.addr[0] = dp[1];
      header.addr[1] = dp[0];
      size = sizeof(header) - 1;
    }
    acquire();
    write(&header, size);
    write(src, count);
    release();
    return (count);
  }

protected:
  /** Command and address header. */
  struct header_t {
    uint8_t cmd;		//!< Command code.
    uint8_t addr[3];		//!< 16/24-bit address in MSB order.
  } __attribute__((packed));

  /** Command codes. */
  enum {
    READ = 0x03,		//!< Read data from memory
    WRITE = 0x02,		//!< Write data to memory
    RDMR = 0x05,		//!< Read mode register
    WRMR = 0x01			//!< Write mode register
  };

  using SPI::Device<0,MSBFIRST,FREQ,SS_PIN>::acquire;
  using SPI::Device<0,MSBFIRST,FREQ,SS_PIN>::transfer;
  using SPI::Device<0,MSBFIRST,FREQ,SS_PIN>::read;
  using SPI::Device<0,MSBFIRST,FREQ,SS_PIN>::write;
  using SPI::Device<0,MSBFIRST,FREQ,SS_PIN>::release;
};

/**
 * Storage device driver for Microchip 23LC512, 512 Kbit SPI Serial
 * SRAM.
 * @param[in] SS_PIN slave select board pin.
 * @param[in] FREQ max bus frequency for device (default 16 MHz).
 * @section Circuit
 * @code
 *                          23LC512
 *                       +------------+
 * (SS)----------------1-|CS   U   VCC|-8----------------(VCC)
 * (MISO)--------------2-|SO      HOLD|-7---------(VCC/PULLUP)
 * (VCC/PULLUP)--------3-|NU       SCK|-6----------------(SCK)
 * (GND)---------------4-|VSS       SI|-5---------------(MOSI)
 *                       +------------+
 * @endcode
 */
template<BOARD::pin_t SS_PIN, uint32_t FREQ = 16000000L>
class MC23LC512 : public MC23LCXXX<64, SS_PIN, FREQ> {
public:
  /**
   * Construct and initiate device driver with given slave select
   * board pin, max bus frequency, and bus manager.
   * @param[in] spi bus manager.
   */
  MC23LC512(SPI& spi) : MC23LCXXX<64, SS_PIN, FREQ>(spi) {}
};

/**
 * Storage device driver for Microchip 23LC1024, 1M bit SPI Serial
 * SRAM.
 * @param[in] SS_PIN slave select board pin.
 * @param[in] FREQ max bus frequency for device (default 16 MHz).
 * @section Circuit
 * @code
 *                          23LC1024
 *                       +------------+
 * (SS)----------------1-|CS   U   VCC|-8----------------(VCC)
 * (MISO)--------------2-|SO      HOLD|-7---------(VCC/PULLUP)
 * (VCC/PULLUP)--------3-|NU       SCK|-6----------------(SCK)
 * (GND)---------------4-|VSS       SI|-5---------------(MOSI)
 *                       +------------+
 * @endcode
 */
template<BOARD::pin_t SS_PIN, uint32_t FREQ = 16000000L>
class MC23LC1024 : public MC23LCXXX<128, SS_PIN, FREQ>
{
public:
  /**
   * Construct and initiate device driver with given slave select
   * board pin, max bus frequency, and bus manager.
   * @param[in] spi bus manager.
   */
  MC23LC1024(SPI& spi) : MC23LCXXX<128, SS_PIN, FREQ>(spi) {}
};

#endif
