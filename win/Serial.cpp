#include "Common.h"
#include "Serial.h"
#include <windows.h>
#include "PCOMM.h"
#include <iostream>

using namespace std;


int OpenSerial(int comNumb) {
  int ret;

  ret = sio_open (comNumb);
  if (ret == SIO_OK) {
	//sio_ioctl (comNumb, B9600, P_NONE | BIT_8 | STOP_1 );
	sio_ioctl (comNumb, B115200, P_NONE | BIT_8 | STOP_1 );
	return comNumb;
  }

  return -1;
}
int WriteSerialCommand(int hSerial, char *command, int size) {
  int ret;

  ret = sio_write (hSerial, command, size);
	
  if (ret == size) {
	DEBUG_TAG("[W]:");
	for (int i = 0; i < size; i++) {
	  cout << hex << (command[i]&0xff) << " ";
	  //printf("[%02X] ", command[i]&0xff);
	}
	cout << endl;
	return ret;
  }

  return -1;
}
int ReadSerialRespond(int hSerial, char *respond, int size, int wait, int timeout) {
  int ret;

  ret = sio_SetReadTimeouts (hSerial, wait, timeout);
  ret = sio_read (hSerial, respond, size);
	
  return ret;
}
int CloseSerial(int comNumb) {
  int ret;

  ret = sio_close(comNumb);

  return ret;
}
