#include "Common.h"
#include "Serial.h"
#include "Proto.h"
#include "string.h"
extern void viewbuf(char *buf, int len);
void SerialWriteCommand(int com, 
						uint8 stx,
						uint8 cmd,
						uint8 *buf, 
						uint8 size) {
  if (com < 0) {
	return;
  }
  uint8 *data = (uint8*)new char[size + 5];
  if (data == NULL) {
	return;
  }
  data[0] = stx&0xff;
  uint16 sum = uart_calc_fcs(buf, size) + cmd;
  data[1] = sum&0xff;
  data[2] = (sum>>8)&0xff;
  data[3] = size+1;
  data[4] = cmd&0xff;
  if (size > 0) {
	memcpy(data + 5, buf,size);
  }
  WriteSerialCommand(com, (char*)data, size + 5);  
  delete[] data;
}

uint16 uart_calc_fcs(uint8 *data, uint8 len) {
  uint16 check = 0;
  uint16 i = 0;
  
  //viewbuf((char*)data, len);
  
  for (i = 0; i< len; i++) {
    check += data[i]&0xff;
  }
  return check;
}

void _append_data(uint8 *buffer, uint32 &size, 
				  uint8 *_buf, uint32 _len) {
  if (_len > 0) {
	memcpy(buffer + size, _buf, _len);
	size += _len;
  }
}
bool _get_frame(uint8 *buffer, uint32 &size,
				uint8 *_frame,
				uint8 &_len) {
  static char except = AU_START;
  uint32 i = 0;
  int frame_len = 0;
  int len = 0;
  int rlen = 0;
  int start = 0;

  while (i < size) {
	char ch = buffer[i++];
	switch(except) {
	case AU_START:
	  if ( (ch&0xff) == AU_START_CHAR ||
		   (ch&0xff) == AU_START_CHAR_1) {
		start = i-1;
		frame_len = 0;
		_frame[frame_len++] = ch;
		except = AU_SUM_LOW;
	  } else {
		;
	  }
	  break;
	case AU_SUM_LOW:
	  _frame[frame_len++] = ch;
	  except = AU_SUM_HIGH;
	  break;
	case AU_SUM_HIGH:
	  _frame[frame_len++] = ch;
	  except = AU_LEN;
	  break;
	case AU_LEN:
	  _frame[frame_len++] = ch;
	  len = ch;
	  except = AU_CMD_ACK;
	  rlen = 0;
	  break;
	case AU_CMD_ACK:
	  _frame[frame_len++] = ch;
	  rlen++;
	  if (rlen < len) {
		except = AU_DATA;
	  } else {
		uint16 fcs = uart_calc_fcs(_frame+4, frame_len-4);
		uint16 fff = ((_frame[2] << 8) | (_frame[1]))&0xffff;
		if (fff == fcs) {
		  size = size - frame_len - start;
		  memcpy(buffer, buffer + start + frame_len, size);
		  except = AU_START;
		  _len = frame_len;
		  return true;
		} else {
		  cout << "fcs error:" << "fcs:" << fcs << ",fff" << fff<< endl;
		  
		  viewbuf((char*)_frame, frame_len);
		  except = AU_START;
		  len = rlen = 0;
		}		
	  }
	  break;
	case AU_DATA:
	  _frame[frame_len++] = ch;
	  rlen++;
	  if (rlen < len) {
		except = AU_DATA;
	  } else {
		uint16 fcs = uart_calc_fcs(_frame+4, frame_len-4);
		uint16 fff = ((_frame[2] << 8) | _frame[1]);
		if (fff == fcs) {
		  size = size - frame_len - start;
		  memcpy(buffer, buffer + start + frame_len, size);
		  except = AU_START;
		  _len = frame_len;
		  return true;
		} else {
		  cout << "fcs error:" << "fcs:" << fcs << ",fff" << fff<< endl;
		  viewbuf((char*)_frame, frame_len);
		  except = AU_START;
		  len = rlen = 0;
		}		
	  }
	  break;
	default:
	  except = AU_START;
	  len = rlen = 0;
	  break;
	}
  }

  size = 0;
  except = AU_START;
  _len = 0;
  return false;
}

void _parse_frame(uint8 *_frame, uint32 _len, frame_t &_f) {
  _f.sof = _frame[0];
  _f.sum = (_frame[2] << 8) | (_frame[1]);
  _f.len = _frame[3];
  _f.data = (uint8*)&_frame[4];
}
int _get_type(frame_t &f) {
  return f.data[0];
}
uint8* _get_data(frame_t &f) {
  return f.data;
}
int _get_len(frame_t &f) {
  return f.len;
}
