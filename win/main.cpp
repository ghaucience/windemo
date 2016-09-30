/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file main.cpp.
* @brief 
* 
* @version 1.0
* @author au
* @date 2015/08/27-16:30:37
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2015/08/27by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#include <stdio.h>
#include <stdlib.h>

#include <iostream>
#include <list>
#include <sstream>
#include <map>
using namespace std;

#include <windows.h>

#include "PCOMM.H"
#include "Serial.h"
#include "Proto.h"
#include "KDLock.h"

#define DLG_MAIN 1000
#define BTN_INNET 1001
#define BTN_RESET 1002
#define TXT_INNSET_RESULT 1009
#define BTN_DOOR_STATUS 1003
#define BTN_ALERT_INFO 1004

typedef void (*RSP_FUNC)(char *buf, int len);


void MainLoop();
INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam);
DWORD WINAPI SerialRead (LPVOID lpParam);
void Init();

int DoInNet();
int DoReset();
int DoAlertInfo();
int DoDoorStatus();

void viewbuf(char *buf, int len);

int    g_com = 0;
HANDLE g_hSerialWriteEvent = NULL;
HANDLE g_hSerialWrite = NULL;
HANDLE g_hSerialRead = NULL;
HWND   g_hWnd = NULL;
map<uint16, RSP_FUNC> g_rspfuncs;

int    g_innet = 0;
KDLock *g_kdlock = NULL;

int main(int argc, char *argv[]) {
  g_com = OpenSerial(atoi(argv[1]));
  if (g_com < 0) {
	cout << "open serial failed" << endl;
	exit(0);
  }

  g_hSerialRead = CreateThread(NULL, 0, SerialRead, NULL, 0, NULL);	

  Init();

  int ret = DialogBox(NULL, MAKEINTRESOURCE(DLG_MAIN), NULL, DialogProc);
  if (ret == -1) {
	cout << "Error is " << GetLastError() << endl;
  }

  return 0;
}
DWORD WINAPI SerialRead (LPVOID lpParam) { 
  int ret = 0;
  static uint8 buffer[4096];
  static uint32 size = 0;

  uint8 buf[1024];
  while (1) {
	ret = ReadSerialRespond(g_com, (char*)buf, sizeof(buf), 
							2000,
							80);
	if (ret <= 0) {
	  continue;
	}
	DEBUG_TAG("Read------------------->:");
	viewbuf((char *)buf, ret);
	DEBUG_TAG("Read------------------->:");
	_append_data((uint8*)buffer, size, buf, ret);
	uint8 frame[256 + 16];
	uint8 len;
	while (_get_frame(buffer, size, frame, len)) {
	  DEBUG_TAG("=========Serial Frame:=========");
	  viewbuf((char*)frame, len);
	  frame_t f;
	  _parse_frame(frame, len, f);
	  int type = _get_type(f);
	  uint8* data = _get_data(f);
	  int len  = _get_len(f);
	  
	  DEBUG_TAG("type:" << hex << type);

	  map<uint16, RSP_FUNC>::iterator it =
		g_rspfuncs.find(type);
	  if (it != g_rspfuncs.end()) {
		if (g_kdlock != NULL && data[0] != 0x80) {
		  uint8 ack[32];
		  uint16 ack_len;
		  g_kdlock->Ack(ack, ack_len);
		  SerialWriteCommand(g_com, 0x5F, 0x80, NULL, 0);
		}
		it->second((char*)data, len);
	  } else {
		DEBUG_TAG("unsupport cmd: " << type);
	  }
	}
  }
}

INT_PTR CALLBACK DialogProc(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam) {
  switch (uMsg) {
  case WM_INITDIALOG:
	g_hWnd = hwndDlg;
	return TRUE;
  case WM_CLOSE:
	EndDialog(hwndDlg, 0);
	return TRUE;
  case WM_COMMAND:
	{
	  int wmId    = LOWORD(wParam); 
	  int wmEvent = HIWORD(wParam); 
	  switch (wmId) {
	  case BTN_INNET:
		DoInNet();
		break;
	  case BTN_RESET:
		DoReset();
		break;
	  case BTN_DOOR_STATUS:
		DoDoorStatus();
		break;
	  case BTN_ALERT_INFO:
		DoAlertInfo();
		break;
	  }
	}
	return TRUE;
  }
  return FALSE;
}

/////////////////////////////////////////////////////////////
int DoInNet() {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return 0;
  }
  uint8 req[256];
  uint16 req_len;
  g_kdlock->Reset(req, req_len, 0x01);
  SerialWriteCommand(g_com, 0xf5, 0xb0, req, req_len&0xff);
  return 0;
}
int DoReset() {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return 0;
  }
  uint8 req[256];
  uint16 req_len;
  g_kdlock->Reset(req, req_len, 0x00);
  SerialWriteCommand(g_com, 0xf5, 0xb0, req, req_len&0xff);
  return 0;
}
int DoAlertInfo() {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return 0 ;
  }
  uint8 req[256];
  uint16 req_len;
  g_kdlock->AlertInfo(req, req_len);
  SerialWriteCommand(g_com, 0xf5, 0xb2, req, req_len&0xff);
  return 0;
}
int DoDoorStatus() {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return 0 ;
  }
  uint8 req[256];
  uint16 req_len;
  g_kdlock->ReportStatus(req, req_len);
  SerialWriteCommand(g_com, 0xf5, 0xb1, req, req_len&0xff);
  return 0;
}

void rsp_ReadLockInfo(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->ReadLockInfo((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc1, rsp, rsp_len&0xff);
}
void rsp_OpenLock(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->OpenLock((uint8*)buf, len, rsp, rsp_len);
  viewbuf((char*)rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc2, rsp, rsp_len&0xff);
}
void rsp_ReadRecord(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->ReadRecord((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc3, rsp, rsp_len&0xff);
}
void rsp_SetMode(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->SetMode((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc4, rsp, rsp_len&0xff);  
}
void rsp_TimeCalibration(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->TimeCalibration((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc5, rsp, rsp_len&0xff);  
}
void rsp_AddDelKey(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->AddDelKey((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc6, rsp, rsp_len&0xff);    
}
void rsp_ReadKeyInfo(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
#if 0
  g_kdlock->ReadKeyInfo((uint8*)buf, len, rsp, rsp_len);
  Sleep(1000);
#else
  rsp_len = 0x1C;
  rsp[0] = 0x80;
  rsp[1] = 0x80;
  rsp[2] = 0x80;
  memset(rsp + 3, 0x00, rsp_len - 3);
#endif
  SerialWriteCommand(g_com, 0x5f, 0xc7, rsp, rsp_len&0xff);    
}
void rsp_ModifyKey(char *buf, int len) {
  DEBUG_TAG("");
  if (g_kdlock == NULL) {
	return;
  }
  uint8 rsp[256];
  uint16 rsp_len;
  g_kdlock->ModifyKey((uint8*)buf, len, rsp, rsp_len);
  SerialWriteCommand(g_com, 0x5f, 0xc8, rsp, rsp_len&0xff);
}

void rsp_ack(char *buf, int len)  {
  DEBUG_TAG("Ack!!");
}

void Init() {
  g_kdlock = new KDLock();
  g_rspfuncs[0xc1] = rsp_ReadLockInfo;
  g_rspfuncs[0xc2] = rsp_OpenLock;
  g_rspfuncs[0xc3] = rsp_ReadRecord;
  g_rspfuncs[0xc4] = rsp_SetMode;
  g_rspfuncs[0xc5] = rsp_TimeCalibration;
  g_rspfuncs[0xc6] = rsp_AddDelKey;
  g_rspfuncs[0xc7] = rsp_ReadKeyInfo;
  g_rspfuncs[0xc8] = rsp_ModifyKey;
  g_rspfuncs[0x80] = rsp_ack;
}

void viewbuf(char *buf, int len) {
  for (int i = 0; i < len; i++) {
	cout << hex << (buf[i]&0xff) << " ";
  }
  cout << endl;
}
