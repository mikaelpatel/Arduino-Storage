/**
 * @file Storage.h
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

#ifndef STORAGE_H
#define STORAGE_H

/**
 * External memory storage interface, data block read/write and
 * streaming class.
 */
class Storage {
public:
  /**
   * Create storage manager.
   */
  Storage() : m_addr(0) {}

  /**
   * Read count number of bytes from storage address to buffer.
   * @param[in] dest destination buffer pointer.
   * @param[in] src source memory address on device.
   * @param[in] count number of bytes to read from device.
   * @return number of bytes read or negative error code.
   */
  virtual int read(void* dest, uint32_t src, size_t count) = 0;

  /**
   * Write count number of bytes to storage address from buffer.
   * @param[in] dest destination memory address on device.
   * @param[in] src source buffer pointer.
   * @param[in] count number of bytes to write to device.
   * @return number of bytes written or negative error code.
   */
  virtual int write(uint32_t dest, const void* src, size_t count) = 0;

  /**
   * Allocate block with given number of bytes on storage.
   * @param[in] count number of bytes.
   * @return address of allocated block.
   */
  uint32_t alloc(size_t count)
  {
    uint32_t res = m_addr;
    m_addr += count;
    return (res);
  }

  /**
   * Storage Block for data; temporary or persistent external storage
   * of data.
   */
  class Block {
  public:
    /**
     * Construct block on given storage device at the given start
     * address, local buffer and size.
     * @param[in] mem storage device for block.
     * @param[in] buf buffer address.
     * @param[in] size number of bytes.
     */
    Block(Storage &mem, void* buf, size_t size, size_t nmemb = 1) :
      m_mem(mem),
      m_addr(m_mem.alloc(size)),
      m_size(size),
      m_nmemb(nmemb),
      m_buf(buf)
    {
    }

    /**
     * Returns storage address.
     * @return address.
     */
    uint32_t addr()
    {
      return (m_addr);
    }

    /**
     * Read storage block to buffer.
     * @param[in] ix element index (default 0):
     * @return number of bytes read or negative error code.
     */
    int read(size_t ix = 0)
    {
      if (ix < m_nmemb)
	return (m_mem.read(m_buf, m_addr + (ix * m_size), m_size));
      return (-1);
    }

    /**
     * Write buffer to storage block.
     * @param[in] ix element index (default 0):
     * @return number of bytes written or negative error code.
     */
    int write(size_t ix = 0)
    {
      if (ix < m_nmemb)
	return (m_mem.write(m_addr + (ix * m_size), m_buf, m_size));
      return (-1);
    }

  protected:
    Storage& m_mem;
    const uint32_t m_addr;
    const size_t m_size;
    const size_t m_nmemb;
    void* m_buf;
  };

  /**
   * Stream of given size on given storage. Write/print intermediate
   * data to the stream that may later be read and transfered.
   * Multiple stream may be created on the same device by assigning
   * start address and size.
   */
  class Stream : public ::Stream {
  public:
    /**
     * Construct stream on given storage device at the given start
     * address. The storage size of the stream is given as a template
     * parameter.
     * @param[in] mem storage device for stream.
     * @param[in] size number of bytes in stream.
     */
    Stream(Storage &mem, size_t size) :
      m_mem(mem),
      m_addr(m_mem.alloc(size)),
      m_put(0),
      m_get(0),
      m_count(0),
      m_size(size)
    {
    }

    /**
     * Returns storage address.
     * @return address.
     */
    uint32_t addr()
    {
      return (m_addr);
    }

    /**
     * @override{Stream}
     * Write given byte to stream. Return number of bytes written,
     * zero if full.
     * @param[in] byte to write.
     * @return number of bytes written(1).
     */
    virtual size_t write(uint8_t byte)
    {
      if (m_count == m_size) return (0);
      m_mem.write(m_addr + m_put, &byte, sizeof(byte));
      m_count += 1;
      m_put += 1;
      if (m_put == m_size) m_put = 0;
      return (sizeof(byte));
    }

    /**
     * @override{Stream}
     * Write given buffer and numbe of bytes to stream. Return number
     * of bytes written.
     * @param[in] bufffer to write.
     * @param[in] size number of byets to write.
     * @return number of bytes.
     */
    virtual size_t write(const uint8_t *buffer, size_t size)
    {
      uint16_t room = m_size - m_count;
      if (room == 0) return (0);
      if (size > room) size = room;
      size_t res = size;
      room = m_size - m_put;
      if (size > room) {
	m_mem.write(m_addr + m_put, buffer, room);
	buffer += room;
	size -= room;
	m_count += room;
	m_put = 0;
      }
      m_mem.write(m_addr + m_put, buffer, size);
      m_count += size;
      m_put += size;
      return (res);
    }

    /**
     * @override{Stream}
     * Returns number of bytes available for read().
     * @return bytes available.
     */
    virtual int available()
    {
      return (m_count);
    }

    /**
     * @override{Stream}
     * Return next byte to read if available otherwise negative error
     * code(-1).
     * @return next byte or negative error code.
     */
    virtual int peek()
    {
      if (m_count == 0) return (-1);
      uint8_t res = 0;
      m_mem.read(&res, m_addr + m_get, sizeof(res));
      return (res);
    }

    /**
     * @override{Stream}
     * Return next byte if available otherwise negative error
     * code(-1).
     * @return next byte or negative error code.
     */
    virtual int read()
    {
      if (m_count == 0) return (-1);
      uint8_t res = 0;
      m_mem.read(&res, m_addr + m_get, sizeof(res));
      m_count -= 1;
      m_get += 1;
      if (m_get == m_size) m_get = 0;
      return (res);
    }

    /**
     * @override{Stream}
     * Flush all data and reset stream.
     */
    virtual void flush()
    {
      m_put = 0;
      m_get = 0;
      m_count = 0;
    }

  protected:
    Storage& m_mem;
    const uint32_t m_addr;
    uint16_t m_put;
    uint16_t m_get;
    uint16_t m_count;
    size_t m_size;
  };

protected:
  uint32_t m_addr;
};
#endif
