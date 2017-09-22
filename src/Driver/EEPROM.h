/**
 * @file EEPROM.h
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

#ifndef EEPROM_H
#define EEPROM_H

#include "Storage.h"

/**
 * Storage device driver for the AVR internal EEPROM.
 */
class EEPROM : public Storage {
public:
  /**
   * Construct EEPROM device driver with storage size.
   */
  EEPROM() : Storage(E2END) {}

  /**
   * @override{Storage}
   * Read eeprom block with the given size into the buffer from the
   * source address. Return number of bytes read or negative error
   * code.
   * @param[in] dst buffer to read from eeprom.
   * @param[in] src address in eeprom to read from.
   * @param[in] count number of bytes to read.
   * @return number of bytes or negative error code.
   */
  virtual int read(void* dst, uint32_t src, size_t count)
  {
    eeprom_read_block(dst, (const void*) src, count);
    return (count);
  }

  /**
   * @override{Storage}
   * Write eeprom block at given destination address with the contents
   * from the buffer. Return number of bytes written or negative error
   * code.
   * @param[in] dst address in eeprom to read write to.
   * @param[in] src buffer to write to eeprom.
   * @param[in] count number of bytes to write.
   * @return number of bytes or negative error code.
   */
  virtual int write(uint32_t dst, const void* src, size_t count)
  {
    eeprom_update_block(src, (void*) dst, count);
    return (count);
  }
};
#endif
