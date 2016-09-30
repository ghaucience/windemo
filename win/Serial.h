/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file Serial.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/07/13-22:17:56
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/07/13by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __SERIAL_H_
#define __SERIAL_H_
#ifdef __cplusplus
extern "C" {
#endif

int OpenSerial(int comNumb) ;
int CloseSerial(int comNumb) ;
int ReadSerialRespond(int hSerial, char *respond, 
					  int size, int wait, int timeout) ;
int WriteSerialCommand(int hSerial, char *command, int size) ;

#ifdef __cplusplus
}
#endif
#endif

