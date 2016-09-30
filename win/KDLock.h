/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
* COPYRIGHT NOTICE. 
* Copyright (c), 2015 Lierda.
* All rights reserved.
* 
* @file KDLock.h.
* @brief 
* 
* @version 1.0
* @author au
* @date 2016/09/21-11:38:02
* @email dongliang@lierda.com
* 
* @revision:
*  - 1.0 2016/09/21by au.
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
#ifndef __KDLOCK_H_
#define __KDLOCK_H_
#include "Common.h"

#ifdef __cplusplus
extern "C" {
#endif

#pragma pack(push)
#pragma pack(1)

  class KDLock {
  public:
	KDLock();
	virtual ~KDLock();
	
	int Ack(uint8 *ack, uint16 &len);

	int Reset(uint8 *req, uint16 &len, uint8 in_out);
	int ReportStatus(uint8 *req, uint16 &len); 
	int AlertInfo(uint8 *req, uint16 &len);

	int ReadLockInfo(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int OpenLock(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int ReadRecord(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int SetMode(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int TimeCalibration(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int AddDelKey(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int ReadKeyInfo(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
	int ModifyKey(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len);
  private:
  };
  KDLock::KDLock() {
  }
  KDLock::~KDLock() {
  }
  int KDLock::Ack(uint8 *ack, uint16 &len) {
	//0x80：正确  0x81：错误   0x92：ID号错误  0x93：未知命令       其他：错误
	*ack = 0x80;
	len = 1;
	return 0;
  }

  int KDLock::Reset(uint8 *req, uint16 &len, uint8 _in_out) {
	struct {
	  /* 0x00：入网  0x01：退网 */
	  uint8 in_out;
	} buf = {
	  _in_out,
	};
	len = sizeof(buf);
	memcpy(req, &buf, len);
	return 0;	
  }
  int KDLock::ReportStatus(uint8 *req, uint16 &len) {
	struct {
	  /* Bit0：1为开锁，0为关锁
		 Bit1：1为添加密钥通知，0为没有通知
		 Bit2：1为删除密钥通知，0为没有通知
		 Bit3：初始化标示，1为初始化，0为没有初始化
		 Bit4：1为反锁，0为解除反锁
		 Bit5：1为布防，0为取消布防
		 Bit6：1为门磁关，0为门磁开
		 Bit7：保留
	  */
	  uint8 status; 
	  /* 1：密码 2：指纹 3：卡片 4：机械钥匙5：遥控开锁 6：一键开启（12V开锁信号开锁）7：APP开启 */
 	  uint8 key_1_type;
	  /* 密码编号范围：0-9 指纹编号范围：0-99 卡片编号范围：0-99 机械钥匙编号：
		 100 遥控开锁编号：101 一键开启编号：102 APP开启编号：103
	  */
	  uint8 key_1_num;
	  /* 0x01：密码 
		 0x02：指纹 
		 0x03：卡片
	  */
	  uint8 key_2_type;
	  /* 密码编号范围：0-9
		 指纹编号范围：0-99
		 卡片编号范围：0-99
	  */
	  uint8 key_2_num;
	  /* 开锁时间（年月日时分秒），时间采用BCD码。
		 比如：14 04 09 13 13 10（十六进制数）表示2014年4月9日 13时13分10秒
	  */
	  uint8 openlock_time[6];
	  /* BIT0-BIT6为电量1-100，比如：1 = 1%  33 = 33%    100 = 100% */
	  uint8 battery;
	} buf = {
	  (1 << 0) | ( 1 << 3),
	  7,
	  103,
	  0xff,
	  0xff,
	  {0x14, 0x04, 0x09, 0x13, 0x13, 0x10},
	  33,
	};
	len = sizeof(buf);
	memcpy(req, &buf, len);
	return 0;
  }
  int KDLock::AlertInfo(uint8 *req, uint16 &len) {
	struct {
	  /* Bit0：1为错误报警，0为正常。（输入错误密码或指纹或卡片超过10次就报警）
		 Bit1：1为劫持报警 ，0为正常。（输入防劫持密码或防劫持指纹开锁就报警）
		 Bit2：1为防撬报警  ，0为正常。（锁被撬开）
		 Bit3：1为机械钥匙报警，0为正常。（使用机械钥匙开锁）
		 Bit4：1为低电压报警，0为正常（电池电量不足）
		 Bit5-7：保留
	  */
	  uint8 alertinfo;
	  /* BIT0-BIT6为电量1-100，比如：1 = 1%  33 = 33%    100 = 100%
	   */
	  uint8 battery;
	} buf = {
	  0x00,
	  33,
	};
	len = sizeof(buf);
	memcpy(req, &buf, len);
	return 0;
  }

  int KDLock::ReadLockInfo(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct {
	  /* 0x80：成功  0x81：失败   其他：失败*/
	  uint8 ret;
	  /*
		BIT0安全模式标志（0：正常模式，1：安全模式）
		BIT1语音模式标志（0：静音模式，1：语音模式）
		BIT2-BIT3语言模式标志（0：中文，1：英文， 其他：保留）
		BIT4-BIT7 保留（填充0）
	  */
	  uint8 workMode;
	  /* 低电压志+电量值，BIT7为低电压标志，BIT0-BIT6为电量1-100，
		 比如：1 = 1%  33 = 33%    100 = 100%
	  */
	  uint8 lowpower;
	  /* BIT0:表示此锁支持密码输入 （1：支持 0：不支持）
		 BIT1:表示此锁支持卡片     （1：支持 0：不支持）
		 BIT2:表示此锁支持指纹输入 （1：支持 0：不支持）
		 BIT3-7 :保留（填充0）
	  */
	  uint8 lockFunction;
	  /* 比如：字符串“F6113”(最多7个ASCII字符)，不足7个字符填充空字符
	   */
	  uint8 lockType[7];
	  /* 比如：字符串“V1.0”（5个ASCII字符），不足5个字符填充空字符
	   */
	  uint8 lockVersion[5];
	  /* 锁当前时间（年月日时分秒），时间采用BCD码，差30秒才进行更新。
		 比如：14 04 09 13 13 10（十六进制数）
		 表示2014年4月9日 13时13分10秒
	  */
	  uint8 currentTime[6];
	} buf = {
	  0x80,
	  (1 << 0) | (1 << 1) | (0 << 2) ,
	  33,
	  (1 << 0) | (1 << 1) | (1 << 2),
	  {'F', '6', '1', '1', '3'},
	  {'V', '1', '.', '0'},
	  {0x14, 0x04, 0x09, 0x13, 0x13, 0x10},
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;
  }
  int KDLock::OpenLock(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct oc_lock_t{
	  /* BIT0:密码开锁（1：使用该密钥开锁  0：不使用）
		 BIT1:指纹开锁
		 BIT2:刷卡开锁
		 BIT3:APP开锁
		 BIT4-7 :保留（填充0）
	  */
	  uint8 ocMode;
	  uint8 key[12];
	} *preq = (oc_lock_t*)req;
	struct {
	  uint8 ret;
	} buf = {
	  /* 0x80：成功  0x81：失败  0x8C：开锁失败，为安全模式   其他：失败
	   */
	  0x80,
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;
  }
  int KDLock::ReadRecord(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct read_record_t {
	  /* 0x00：读取编号为00-19开门记录
		 0x01：读取编号为20-39开门记录
		 如此类推，
		 0x09：读取编号为180-199开门记录
		 0xFF：读取编号全部开门记录
	  */
	} *preq = (read_record_t*)req;
	struct {
	  /* 0x80：成功 0x81：失败 0x82：没有数据 其他：失败*/
	  uint8 rsp;
	  /* MAX=200 */
	  uint8 current_num;
	  /* 1字节，从0开始 */
	  uint8 pktidx;
	  uint8 record[10];
	}buf = {
	  0x80,
	  0x01,
	  0x00,
	  {0x07, 0x00, 0x07, 0x00,
	   0x14, 0x04, 0x09, 0x13, 0x13, 10},
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;
  }
  int KDLock::SetMode(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct sm_t {
	  /* 0x00：正常模式，0x01：安全模式 ，0xFF：不修改 */
	  uint8 securityMode;
	  /* 0x00：静音模式，0x01：语音模式，0xFF：不修改 */
	  uint8 voiceMode;
	  /* 0x00：中文，0x01：英文，0xFF：不修改 */
	  uint8 languageMode;
	} *preq = (sm_t*)req;
	struct {
	  /* 0x80：成功   0x81：失败   
		 0x91：没添加两个不同类型密钥         
		 0x94：没有权限    其他：失败
	   */
	  uint8 ret;
	} buf = {
	  0x80,
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;
  }
  int KDLock::TimeCalibration(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct tc_t {
	  uint8 time[6];
	} *preq = (tc_t*)req;
	struct {
	  /* 0x80：成功  0x81：失败  0x94：没有权限   其他：失败 */
	  uint8 ret;
	} buf = {
	  0x80
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;	
  }
  int KDLock::AddDelKey(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct adk_t {
	  /* 1.新增     2.删除 */
	  uint8 op;
	  /* 1.密码     2. 指纹     3.卡片 */
	  uint8 key_type;
	  /* 密码编号范围：00-09
		 指纹编号范围：00-99
		 卡片编号范围：00-99
	  */
	  uint8 key_num;
	  /* 1．密钥字节数：12字节
		 2．非密码类型填0x00
		 3.  密码删除填0xFF
	  */
	  uint8 key[12];
	} *preq = (adk_t*)req;
	struct {
	  /* 0x80：成功   0x81：失败  0x83：密钥已存在  
		 0x84：密钥不存在   0x85：配满   0x86：编号已存在
		 0x87：编号不存在 0x88：无此功能  0x8D：编号超出范围  
		 0x8E：密码过于简单  0x8F：密码无效  
		 0x90：删除失败，为安全模式   0x94：没有权限    其他：失败 
	   */
	  uint8 ret;
	} buf = {
	  0x80,
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;	
  }
  int KDLock::ReadKeyInfo(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct rki_t {
	  /* 0x01：密码  0x02：指纹  0x03：卡片  
		 0xFF：读取所有密钥\
	  */
	  uint8 key_type;
	} *preq = (rki_t*)req;
	struct {
	  /* 0x80：成功 0x81：失败 0x82：没有数据 其他：失败*/
	  uint8 rsp;
	  /* Max=10 */
	  uint8 key_num;
	  /* Max=100*/
	  uint8 finger_num;
	  /* Max = 100 */
	  uint8 card_num;
	  /* 1字节，从0开始 */
	  uint8 pktidx;
	  /* 1密码 2. 指纹 3. 卡片  */
	  uint8 key_type;
	  uint8 key_idx;
	} buf = {
	  0x80,
	  1, 0, 0,
	  0, 
	  1,
	  0,
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;	
  }

  int KDLock::ModifyKey(uint8 *req, uint16 qlen, uint8 *rsp, uint16 &len) {
	struct mk_t {
	  /* 1：管理密码  2：用户密码 */
	  uint8 key_type;
	  /* 编号范围：00-09 （修改管理密码时，此字节填0XFF） */
	  uint8 key_num;
	  /* 有效位数6-12位，不足12位填充0xFF */
	  uint8 key[12];
	}*preq = (mk_t*)req;
	struct {
	  /* 0x80：成功   0x81：失败  0x83：密钥已存在 
		 0x8E：密码过于简单(失败)  0x8F：密码无效   
		 0x94：没有权限    其他：失败 
	  */
	  uint8 ret;
	} buf = {
	  0x00,
	};
	len = sizeof(buf);
	memcpy(rsp, &buf, len);
	return 0;
  }

#pragma pack(pop)
#ifdef __cplusplus
}
#endif
#endif
