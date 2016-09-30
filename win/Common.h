/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file Common.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/07/13-22:28:22
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/07/13by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __COMMON_H_
#define __COMMON_H_
#include <iostream>
using namespace std;

#ifdef __cplusplus
extern "C" {
#endif

#ifndef uint8 
#define uint8 unsigned char
#endif
#ifndef uint16
#define uint16 unsigned short
#endif
#ifndef uint32
#define uint32 unsigned int
#endif

#ifndef int8 
#define int8 char
#endif

#ifndef int16
#define int16 short
#endif

#ifndef int32
#define int32 int
#endif

#ifndef NULL
#define NULL 0
#endif

#define DEBUG_TAG(x) (cout << "[" << __FILE__ << "$" << dec << "@" << __LINE__ << "@" << __FUNCTION__  << "]" << x << endl)

#define HI_UINT16(a) ((uint8)(((a) >> 8) & 0xFF))
#define LO_UINT16(a) ((uint8)(((a) & 0xFF)))
#define BUILD_UINT16(loByte, hiByte)  ((uint16)(((loByte) & 0x00FF) + (((hiByte) & 0x00FF) << 8)))

#ifdef __cplusplus
}
#endif
#endif
