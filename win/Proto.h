/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file Proto.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/07/13-22:20:26
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/07/13by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __PROTO_H_
#define __PROTO_H_
#ifdef __cplusplus
#include "Common.h"

extern "C" {
#endif
  struct frame_t {
	uint8 sof;
	unsigned short sum;
	uint8 len;
	uint8 *data;
  };
  /* 
   * STX - 1 byte,  0xF5
   * SUM - 2 bytes(LOW | HIGH), CMD + DATA1 + DATA2...
   * LEN - 1 byte, len(cmd, data1, ...)
   * CMD - 1 byte,
   * DAT - 1 ~ 31 bytes
   */
  enum {
	AU_START      = 0,
	AU_SUM_LOW    = 1,
	AU_SUM_HIGH   = 2,
	AU_LEN        = 3,
	AU_CMD_ACK    = 4,
	AU_DATA       = 5,
  };
#define AU_START_CHAR 0xF5
#define AU_START_CHAR_1  0x5F
  void SerialWriteCommand(int com, 
						  uint8 stx,
						  uint8 cmd,
						  uint8 *buf, 
						  uint8 size);
  uint16 uart_calc_fcs(uint8 *data, 
					 uint8 len);
  void _append_data(uint8 *buffer, uint32 &size, 
					uint8 *_buf, uint32 _len);
  bool _get_frame(uint8 *buffer, uint32 &size,
				  uint8 *_frame,
				  uint8 &_len);

  void _parse_frame(uint8 *_frame, uint32 len, frame_t &_f) ;
  int _get_type(frame_t &f) ;
  uint8* _get_data(frame_t &f) ;
  int _get_len(frame_t &f);

#ifdef __cplusplus
}
#endif
#endif
