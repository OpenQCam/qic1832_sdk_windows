#pragma once

#include <dshow.h>
#include <ks.h>
#include <ksproxy.h>
#include "XUControl.h"

const GUID QIC_XU_GUID = 
{0x5D9E1728, 0xAB8D, 0x4fa9, {0x9A, 0xCB, 0xF6, 0xD5, 0x6D, 0xBC, 0x05, 0xA6}};

const GUID DELL_XU_GUID = 
{0x0FB885C3, 0x68C2, 0x4547, {0x90, 0xF7, 0x8F, 0x47, 0x57, 0x9D, 0x95, 0xFC}};

#define CTRL_ID_DEF		1
#define REQ_LEN_MAX		64

typedef struct {
	HANDLE hEvent;
	EventCallBack_t cb;
} ThreadParam_t;

class XUProxy  
{
protected:
	IBaseFilter*	pBaseFilter;
	IKsControl*		pKsControl;
	UINT			uiEnumDevice;
	UINT			uiUnitId;
	UINT			uiXuNodeId;
	UINT			uiCtrlId;
	UINT			uiLength;
	GUID			XuGuid[2];
	KSEVENT			Event;
	KSEVENTDATA		EventData;
	HANDLE			ThreadHandle;
	ThreadParam_t	pData;
	char			szDeviceId[32];
	unsigned char	pValue[REQ_LEN_MAX];
	HRESULT			lErrCode;

public:
	XUProxy();
	~XUProxy();

	bool UnInitialize();
	bool Initialize(IBaseFilter* pVideoCap);
	bool GetDeviceHandle(IBaseFilter** ppVideoCap);
	bool GetDeviceId(char *str);
	bool GetLen(UINT* uiSize);
	bool GetCtrlId(UINT* uiId);
	bool GetUnitId(UINT* uiId);
	bool GetValue(unsigned char *pData);
	bool SetLen(UINT uiLen);
	bool SetCtrlId(UINT uiId);
	bool SetUnitId(UINT uiId);
	bool SetValue(unsigned char *pData);
	bool XUSet();
	bool XUGet();
	bool XUEventInit(EventCallBack_t cb);
	bool XUEventUninit();
	HRESULT GetErrorCode();

private:
	HRESULT GetDeviceEnum(void);
	HRESULT GetDeviceXuNodeId(void);
	bool IsQic (IMoniker *pMoniker);
	bool SaveDeviceId (IMoniker *pMoniker);
	bool SET_CUR(UINT uiPropertyId, UINT uiSize, unsigned char* pValue);
	bool GET_CUR(UINT uiPropertyId, UINT uiSize, unsigned char* pValue);
	bool GET_LEN(UINT uiPropertyId, UINT* uiSize);
};

