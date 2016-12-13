#include "stdafx.h"
#include "Core.h"
#include "interfaces.h"
#include <vidcap.h>
#include <Ks.h>
#include <KsProxy.h>
#include <KsMedia.h>

#define DETECT_QIC_BY_API		1

/*********************************************************************************************
	Free an existing media type (ie free resources it holds)
	@param
		mt			:	Call by reference of AM_MEDIA_TYPE instance
**********************************************************************************************/
void FreeMediaType(__inout AM_MEDIA_TYPE& mt)
{
	if (mt.cbFormat != 0) {
		CoTaskMemFree((PVOID)mt.pbFormat);

		// Strictly unnecessary but tidier
		mt.cbFormat = 0;
		mt.pbFormat = NULL;
	}
	if (mt.pUnk != NULL) {
		mt.pUnk->Release();
		mt.pUnk = NULL;
	}
}

/*********************************************************************************************
	general purpose function to delete a heap allocated AM_MEDIA_TYPE structure
	which is useful when calling IEnumMediaTypes::Next as the interface
	implementation allocates the structures which you must later delete
	the format block may also be a pointer to an interface to release
	@param
		pmt			:	Address of the pointer variable that receive AM_MEDIA_TYPE instance pointer
	@param
**********************************************************************************************/
void DeleteMediaType(__inout_opt AM_MEDIA_TYPE *pmt)
{
	// allow NULL pointers for coding simplicity
	if (pmt == NULL) {
		return;
	}

	FreeMediaType(*pmt);
	CoTaskMemFree((PVOID)pmt);
}

/*********************************************************************************************
	Return all Video Cpature Device on the computer
	Not identify the Webcam is QIC1822 or not
	Default thinking QIC1822 webcam will be return on the MonikerList[0]
	@param
		MonikerList	:	Address of the pointer variable that receive IMoniker interface pointer
		pm			:	Address of the pointer variable the receive IMoniker interface pointr of QIC1822
	@param
**********************************************************************************************/
HRESULT CoreGetVideoDevice(IMoniker **MonikerList,IMoniker **pMoniker)
{
	//USES_CONVERSION;
	HRESULT hr;
	ICreateDevEnum *pCreateDevEnum;
	IEnumMoniker *pEm;
	ULONG cFetched;
	int idx = 0;
	

    hr = CoCreateInstance(CLSID_SystemDeviceEnum, NULL, CLSCTX_INPROC_SERVER,
        IID_ICreateDevEnum, (void**)&pCreateDevEnum);
	
    hr = pCreateDevEnum->CreateClassEnumerator(CLSID_VideoInputDeviceCategory,
        &pEm, CDEF_DEVMON_PNP_DEVICE);
	
	pEm->Reset();    
    IMoniker *pM;
    while(hr = pEm->Next(1, &pM, &cFetched), hr==S_OK)
	{
		*(MonikerList+idx) = pM;		
		idx++;
    }
    pEm->Release();
	*(MonikerList+idx) = NULL;  

	for(int i=0;i< idx;i++)
	{
#if !DETECT_QIC_BY_API
		LPOLESTR name;
		char szName[256];
		MonikerList[i]->GetDisplayName(0,0,&name);
		WideCharToMultiByte(CP_ACP, 0, name, -1, szName, 256, NULL, NULL);
		
		for (int j=0; j< strlen(szName); j++)
		{
			szName[j] = tolower(szName[j]);
		}

		if((strstr(szName,"vid_0408")!=NULL))
		{
			*pMoniker=MonikerList[i];
			return hr;
		}
#else
		IBaseFilter * _Vcap = NULL;

		hr = MonikerList[i]->BindToObject(0, 0, IID_IBaseFilter, (void**)&_Vcap);
		if(_Vcap)
		{
			if (QicSetDeviceHandle(_Vcap) != QIC_ERR_SUCCESS)
			{
				QicClearDeviceHandle();
				_Vcap->Release();
				continue;
			}
			QicClearDeviceHandle();
			_Vcap->Release();
			
			*pMoniker=MonikerList[i];
			return hr;
		}
#endif
	}

	return hr;
}
/**********************************************************************************************
	Create CaptureGraphBuilder2 and FilterGraph instances for building directshow graph
	@pararm
		pBuilder	:	Address of the pointer variable that receive ICaptureGraphBuilder2 interface pointer
		pFg			:	Address of the pointer variable that receive IGraphBuilder interface pointer
	@param
**********************************************************************************************/

HRESULT CorePrepareFilterGraph(ICaptureGraphBuilder2 **pBuilder,IGraphBuilder **pFg)
{
	HRESULT hr;

	hr = CoCreateInstance((REFCLSID)CLSID_CaptureGraphBuilder2,
        NULL, CLSCTX_INPROC, (REFIID)IID_ICaptureGraphBuilder2,
        (void **)pBuilder);

	hr = CoCreateInstance(CLSID_FilterGraph, NULL, CLSCTX_INPROC,
		IID_IGraphBuilder, (LPVOID *)pFg);
	

	return hr;
}

/**********************************************************************************************
	Create the IBaseFilter instance for building directshow graph
	@pararm
		pmVideo	:	 Address of the IMoniker pointer variable that represent the QIC1822
		pVcap	:	Address of the pointer variable that receive the IBaseFilter pointer of the QIC1822
	@param
**********************************************************************************************/
HRESULT CoreInitFilterGraph(IMoniker* pmVideo, IBaseFilter** pVcap)
{
	HRESULT hr ;

	hr = pmVideo->BindToObject(0, 0, IID_IBaseFilter, (void**)pVcap);
	QicSetDeviceHandle(*pVcap);

	return hr;
}

/**********************************************************************************************
	This method retrieve the IAMStreamConfig interface of the Video Capture Device.
	Using IAMStreming interface can control Video Capture Device through UVC protocol
	@param
		pBuilder	:	Address of the ICaptureGraphBuilder2 variable represent the filter graph itself
		pVap		:	Address of the IBaseFilter variable represent QIC1822
		pVsc		:	Address of the pointer variable that receive IAMStreamingConfig interface 
	@param
**********************************************************************************************/
HRESULT CoreGetVSConfig(ICaptureGraphBuilder2 *pBuilder, IBaseFilter* pVcap, IAMStreamConfig **pVsc)
{
	HRESULT hr;
	hr = pBuilder->FindInterface(&PIN_CATEGORY_CAPTURE,
		&MEDIATYPE_Video, pVcap,
		IID_IAMStreamConfig, (void **)pVsc);
	return hr;
}

/**********************************************************************************************
	Set YUV pin output format of the QIC1822 device through UVC protocol
	@param
		pVsc	:	Address of the IAMStreamingConfig interface of the QIC1822
		width	:	Specifies the width of the YUV pin output format
		height	:	Specifies the height of the YUV pin output format
	@param
**********************************************************************************************/
HRESULT CoreSetVideoDeviceYUVPin(IAMStreamConfig *pVsc,int width,int height)
{
	HRESULT hr;
	VIDEOINFOHEADER *pVih ;	
	int iCount = 0, iSize = 0;

	hr =pVsc->GetNumberOfCapabilities(&iCount, &iSize);
	if (iSize == sizeof(VIDEO_STREAM_CONFIG_CAPS))
	{
		for (int iFormat = 0; iFormat < iCount; iFormat++)
		{
			VIDEO_STREAM_CONFIG_CAPS scc;
			AM_MEDIA_TYPE *pmtConfig;
			hr = pVsc->GetStreamCaps(iFormat, &pmtConfig, (BYTE*)&scc);			
			if (SUCCEEDED(hr))
			{
				pVih=  reinterpret_cast<VIDEOINFOHEADER*>(pmtConfig->pbFormat);
				if ( (pVih->bmiHeader.biHeight == height)  && (pVih->bmiHeader.biWidth == width))	
				{						
					hr = pVsc->SetFormat(pmtConfig);			
					DeleteMediaType(pmtConfig);
					break;
				}								
				DeleteMediaType(pmtConfig);
			}
		}
	}

	return hr;
}

/**********************************************************************************************
	Set H.264 pin output format of the QIC1822 device through USB Extension Unit
	@param
		width	:	Specifies the width of the H.264 pin output format
		height	:	Specifies the height of the H.264 pin output format
		bitrate :	Specifies the bitrate of the H.264 pin output format
		fps		:	Specifies the fps of the H.264 pin output format
	@param
**********************************************************************************************/
HRESULT CoreSetVideoDeviceH264Pin(int width, int height,int bitrate,int fps)
{
	EncoderParams_t params;
	unsigned int gop;

	QicEncoderGetParams (&params);
	params.uiBitrate = bitrate;
	params.usWidth = width;
	params.usHeight = height;
	params.ucFramerate = fps;
	params.ucSlicesize = 0;
	QicEncoderSetParams (&params, FLAG_ENCODER_ALL);

	QicEncoderGetGOP (&gop);
	gop = 0;
	QicEncoderSetGOP (gop);
	return S_OK;
}
/**********************************************************************************************
	Connect two existed IBaseFilters in the filter graph
	The connected direction is from pSrc to pDest.
	@param
		pGraph	:	Address of IGraphBuilder variable represent the filter graph
		pSrc	:	Address of Upstream IBaseFilter varibable
		pDest	:	Address of Downstream IBaseFilter varibable
	@param
**********************************************************************************************/
HRESULT CoreConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc,  IBaseFilter *pDest)
{
    if ((pGraph == NULL) || (pSrc == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an output pin on the first filter.
    IPin *pOut = 0;
    HRESULT hr = CoreGetUnconnectedPin(pSrc, PINDIR_OUTPUT, &pOut);
    if (FAILED(hr)) 
    {
        return hr;
    }
    hr = CoreConnectFilters(pGraph, pOut, pDest);
    pOut->Release();
    return hr;
}


/*************************************
	onnect one output pin and a IBaseFilter in the filter graph
	@pararm
		pGraph	:	Address of IGraphBuilder variable represent the filter graph
		pOut	:	Address of output IPin of the Upstream IBaseFilter 
		pDest	:	Address of Downstream IBaseFilter varibable
	@param
 **************************************/
HRESULT CoreConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest)    
{
    if ((pGraph == NULL) || (pOut == NULL) || (pDest == NULL))
    {
        return E_POINTER;
    }

    // Find an input pin on the downstream filter.
    IPin *pIn = 0;
    HRESULT hr = CoreGetUnconnectedPin(pDest, PINDIR_INPUT, &pIn);
    if (FAILED(hr))
    {
        return hr;
    }
    // Try to connect them.
    hr = pGraph->Connect(pOut, pIn);
    pIn->Release();
    return hr;
}
/**************************************************************************************
	Find an unconnected pin of the pFilter which direction is Pindr
	@param
		pFilter	:	Address of the IBaseFilter variable which will be searched
		Pindir	:	Specific the ppPin direction
		ppPin	:	Address of the IPin pointer variable receive the IPin interface
	@param
 **************************************************************************************/
HRESULT CoreGetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir,IPin **ppPin)           
{
    *ppPin = 0;
    IEnumPins *pEnum = 0;
    IPin *pPin = 0;
    HRESULT hr = pFilter->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }
    while (pEnum->Next(1, &pPin, NULL) == S_OK)
    {
        PIN_DIRECTION ThisPinDir;
        pPin->QueryDirection(&ThisPinDir);
        if (ThisPinDir == PinDir)
        {
            IPin *pTmp = 0;
            hr = pPin->ConnectedTo(&pTmp);
            if (SUCCEEDED(hr))  // Already connected, not the pin we want.
            {
                pTmp->Release();
            }
            else  // Unconnected, this is the pin we want.
            {
                pEnum->Release();
                *ppPin = pPin;
                return S_OK;
            }
        }
        pPin->Release();
    }
    pEnum->Release();
    // Did not find a matching pin.
    return E_FAIL;
}
/*********************************************************************************
	Initilaize the VMR7 Render for preview and specific the preview rectangle
	@param
		pVmr	:	Address ot hte IBaseFilter variable represent the Video Render
		hwnd	:	Specific the previewing on which window hwnd
	@param
*********************************************************************************/

HRESULT CoreInitVideoRender(IBaseFilter *pVmr,HWND hwnd)
{
	HRESULT hr;
	IVMRFilterConfig* pConfig=0;	
	RECT rcDest;

	hr = pVmr->QueryInterface(IID_IVMRFilterConfig, (void**)&pConfig);
    if( SUCCEEDED(hr)) 
	{
		hr = pConfig->SetRenderingMode(VMRMode_Windowless);		
		hr = pConfig->SetNumberOfStreams(1);
		pConfig->Release();
	}
	IVMRWindowlessControl *pWc;
	hr = pVmr->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWc);

	hr = pWc->SetVideoClippingWindow(hwnd);	
	hr = pWc->SetAspectRatioMode (VMR_ARMODE_NONE);
	hr = pWc->SetBorderColor(RGB(0,0,0));    
		
	GetClientRect(hwnd, &rcDest);
	pWc->SetVideoPosition(NULL,&rcDest);
	pWc->Release();
	return hr;
}
/*********************************************************************************
	Runs all the filters in the filter graph. While the graph is running, 
	data moves through the graph and is rendered
	@param
		pFg	:	Address of the IGraphBuilder variable 
	@param
*********************************************************************************/
HRESULT CoreStartPreview(IGraphBuilder *pFg)
{
	HRESULT hr;
	IMediaControl *pMC = NULL;
    hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(SUCCEEDED(hr)) 
	{				
        hr = pMC->Run();
        if(FAILED(hr)) {           
            pMC->Stop();
        }
        pMC->Release();
    }	
	return hr;
}
/*********************************************************************************
	Stop all the filters in the filter graph.
	@param
		pFg	:	Address of the IGraphBuilder variable 
	@param
*********************************************************************************/
HRESULT CoreStopPreview(IGraphBuilder *pFg)
{
	HRESULT hr;
	IMediaControl *pMC = NULL;
    hr = pFg->QueryInterface(IID_IMediaControl, (void **)&pMC);
    if(SUCCEEDED(hr)) 
	{				
		
        hr = pMC->Stop();        
        pMC->Release();
    }	

	return hr;
}

/*********************************************************************************
	Change the specific window to the required size
	@param
		hwnd	:	A handle to the window which presnetation size will be changed
		width	:	Specific the width of window size
		height	:	Specific the height of window size
	@param
*********************************************************************************/
void CoreResizeWindow(HWND hwnd , int width , int height)
{
	RECT rcC;
  
    GetClientRect(hwnd, &rcC);
	rcC.right = width;
	rcC.bottom = height;
	SetWindowPos(hwnd, NULL, 0, 0, rcC.right,
		rcC.bottom , SWP_NOZORDER | SWP_NOMOVE);

}

/*********************************************************************************
	Add a filter to the filter graph by specificing CLSID 
	@param
		pGraph	:	Address of IGraphBuilder variable represent the filter graph
		clsid	:	Specife the filter CLSID you want to add to the filter graph
		wszName	:	Specific the filter name
		ppf		:	Address of the pointer variable receive the IBaseFilter interface
	@param
*********************************************************************************/
HRESULT CoreAddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF)      
{
    if (!pGraph || ! ppF) return E_POINTER;
    *ppF = 0;
    IBaseFilter *pF = 0;
    HRESULT hr = CoCreateInstance(clsid, 0, CLSCTX_INPROC_SERVER,
        IID_IBaseFilter, reinterpret_cast<void**>(&pF));
    if (SUCCEEDED(hr))
    {
        hr = pGraph->AddFilter(pF, wszName);
        if (SUCCEEDED(hr))
            *ppF = pF;
        else
            pF->Release();
    }
    return hr;
}

/*********************************************************************************
	Configure the MPEG2Demultiplexer filter 
	@param
		witdh	:	The width of the H.264 bitstream
		height	:	The height of the H.264 bitstream
		pMeg2Demxu	:	Address of the IBaseFilter variable will be configured
	@param
*********************************************************************************/
HRESULT CoreConfigureQicToAnxb(IBaseFilter *pQicToAnxb)
{
	HRESULT hr;
	IQIC1802Demux *Demux_Control;
	QIC1802_DEMUX_OPTIONS demux_setting;

	hr = pQicToAnxb->QueryInterface(IID_IQIC1802Demux, (void**)&Demux_Control);

	demux_setting.filter_mode = 2;
	demux_setting.timestamp_mode =1;
	demux_setting.minimum_output_interval = 0;
	demux_setting.aes_key_len = 0;
		
	Demux_Control->setDemuxOptions(&demux_setting);
	Demux_Control->Release();

	/* If want to suppout multi-slices frame, please implement below program */
#if 0		
	IQIC1802DemuxEx2 *Demux_Control_Ex2;
	QIC1802_DEMUX_OPTIONS_EX demux_setting_ex;

	hr = pQicToAnxb->QueryInterface(IID_IQIC1802DemuxEx2, (void**)&Demux_Control_Ex2);
	demux_setting_ex.fix_hardware_nal_bug = 1;	
	Demux_Control_Ex2->setDemuxOptionsEx(&demux_setting_ex, sizeof(demux_setting_ex));
	Demux_Control_Ex2->Release();
#endif
	return hr;
}

/*********************************************************************************
	Enumerate a specific filter to find an output pin support MPEG2_TRANSPORT format
	@param
		pFilter	:	Address of the IBaseFilter variable whichi will be searched
		ppPin	:	Address of the IPin pointer variable receive the IPin interface
	@param
*********************************************************************************/
HRESULT	CoreFilerGetMpeg2tsPin (IBaseFilter *pFiler, IPin **ppPin)
{
	IEnumPins *pEnum = NULL;
	IPin	*pPin;
    HRESULT    hr;

    hr = pFiler->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
		if ( (CoreIsPinContainMpeg2ts(pPin) == S_OK) )
		{
			*ppPin = pPin;
			pEnum->Release();
			return S_OK;
		}
        pPin->Release();
    }
    pEnum->Release();
	
	return S_FALSE;
}
/*********************************************************************************
	Determine the pPin support MPEG2_TRANSPORT or not
	@param
		pPin	:	Address of IPin variable which will be tested
	@param
*********************************************************************************/
HRESULT CoreIsPinContainMpeg2ts (IPin *pPin)
{
	IEnumMediaTypes *pEnum = NULL;
	AM_MEDIA_TYPE *pMediaType;
    HRESULT    hr;

    hr = pPin->EnumMediaTypes(&pEnum);
    if (FAILED(hr))
    {
        goto IsPinContainMpeg2ts_Fail;
    }

    while(pEnum->Next(1, &pMediaType, 0) == S_OK)
    {
		if ( (pMediaType->majortype == MEDIATYPE_Stream) && 
			(pMediaType->subtype == MEDIASUBTYPE_MPEG2_TRANSPORT) )
		{
			DeleteMediaType(pMediaType);
			pEnum->Release();
			return S_OK;
		}
		DeleteMediaType(pMediaType);
    }
	pEnum->Release();

IsPinContainMpeg2ts_Fail:
	return S_FALSE;
	
}
/*********************************************************************************
	Remove all downstream filters of pf filter on the filter graph
	@param
		pFg	:	Address of the IGraphBuilder variable
		pf	:	Address of the IBaseFilter variable
	@param
*********************************************************************************/
void CoreNukeDownstream(IGraphBuilder *pFg , IBaseFilter *pf) 
{
	IPin *pP, *pTo;
    ULONG u;
    IEnumPins *pins = NULL;
    PIN_INFO pininfo;
    HRESULT hr = pf->EnumPins(&pins);
    pins->Reset();
    while(hr == NOERROR) {
        hr = pins->Next(1, &pP, &u);
        if(hr == S_OK && pP) {
            pP->ConnectedTo(&pTo);
            if(pTo) {
                hr = pTo->QueryPinInfo(&pininfo);
                if(hr == NOERROR) {
                    if(pininfo.dir == PINDIR_INPUT) {
                        CoreNukeDownstream(pFg,pininfo.pFilter);
                        pFg->Disconnect(pTo);
                        pFg->Disconnect(pP);
                        pFg->RemoveFilter(pininfo.pFilter);
                    }
                    pininfo.pFilter->Release();
                }
                pTo->Release();
            }
            pP->Release();
        }
    }
    if(pins)
        pins->Release();

}


/**********************************************************************************************
	Configure the Profile and Level of QIC1822 H264 Encoder
	@pararm
		iProfile:	The Profile of H264 Encoder. (See EncoderProfile_t)
		iLevel	:	The Level of H264 Encoder. (See EncoderLevel_t)
	@param
**********************************************************************************************/
HRESULT CoreConfigureProfileLevel(int iProfile, int iLevel)
{
	int num_profiles = 0;
	int max_level, level, profile, constraint_flags;

	QicEncoderGetNumberOfProfiles(&num_profiles);

	for (int i = 0; i< num_profiles; i++)
	{	
		QicEncoderGetProfile(i, &max_level, &profile, &constraint_flags);

		if ((profile == iProfile) && (max_level >= iLevel))
		{
			QicEncoderSetProfileAndLevel(iLevel, iProfile, constraint_flags);
			QicEncoderGetProfileAndLevel(&level, &profile, &constraint_flags);
			return S_OK;
		}
	}

	return S_FALSE;
}

/**********************************************************************************************
	Configure the Stream Format of QIC1822 H264 Encoder
	@pararm
		format:	The Stream Format of H264 Encoder. (See EncoderStreamFormat_t)
	@param
**********************************************************************************************/
HRESULT CoreConfigureStreamFormat(int iFormat)
{
	EncoderStreamFormat_t _format;

	if (QicGetStreamFormat(&_format) == QIC_ERR_SUCCESS)
	{	
		if (iFormat)
			_format = FORMAT_STREAM_H264_CONTAINER;
		else
			_format = FORMAT_STREAM_H264_RAW;

		if (QicSetStreamFormat(_format) == QIC_ERR_SUCCESS)
		{
			return S_OK;
		}
	}

	return S_FALSE;
}

/**********************************************************************************************
	Check the Stream Resolution is supported by QIC1822 H264 Encoder
	@pararm
		width:	The Width of Stream Resoltion
		height:	The Height of Stream Resoltion
	@param
**********************************************************************************************/
HRESULT CoreCheckEncoderCapabilities(int width, int height)
{
	int num_capability = 0;
	EncoderCapability_t capability;
	
	if (QicEncoderGetNumberOfCapabilities (&num_capability) == QIC_ERR_SUCCESS)
	{
		for (int i=0; i<num_capability; i++)
		{
			QicEncoderGetStreamCaps(i, &capability);
			if ((capability.usWidth == width) && (capability.usHeight == height))
			{
				return S_OK;
			}
		}	
	}

	return S_FALSE;
}

/*********************************************************************************
	Enumerate a specific filter to find an output pin support MJPG format only.
	In QIC1822, this is encoding pin.
	@param
		pFilter	:	Address of the IBaseFilter variable whichi will be searched
		ppPin	:	Address of the IPin pointer variable receive the IPin interface
	@param
*********************************************************************************/
HRESULT	CoreFilerGetMJPGOnlyPin (IBaseFilter *pFiler, IPin **ppPin)
{
	IEnumPins *pEnum = NULL;
	IPin	*pPin;
    HRESULT    hr;

	if(ppPin) *ppPin = NULL;

	/* make sure this filter is QIC camera source */
	if (QicSetDeviceHandle(pFiler) != QIC_ERR_SUCCESS)
	{
		QicClearDeviceHandle();
		return S_FALSE;
	}

	/* search for MJPG only pin */
    hr = pFiler->EnumPins(&pEnum);
    if (FAILED(hr))
    {
        return hr;
    }

    while(pEnum->Next(1, &pPin, 0) == S_OK)
    {
		IEnumMediaTypes *pEnumTypes = NULL;
		AM_MEDIA_TYPE *pMediaType;
		HRESULT    hr;

		hr = pPin->EnumMediaTypes(&pEnumTypes);
		if (FAILED(hr))
		{
	        pPin->Release();
			pEnum->Release();
			return S_FALSE;
		}

		if(pEnumTypes->Next(1, &pMediaType, 0) == S_OK)
		{
			if ( (pMediaType->majortype == MEDIATYPE_Video) && 
				(pMediaType->subtype == MEDIASUBTYPE_MJPG) )
			{
				*ppPin = pPin;
				DeleteMediaType(pMediaType);
				pEnumTypes->Release();
		        pPin->Release();
				pEnum->Release();
				return S_OK;
			}
			DeleteMediaType(pMediaType);
		}
		pEnumTypes->Release();
        pPin->Release();
    }
    pEnum->Release();
	
	return S_FALSE;
}

BYTE CoreXUExist(IBaseFilter* pFilter, GUID* guid)
{
	// Modify from HRESULT XUProxy::GetDeviceXuNodeId()
	DWORD numberOfNodes;
	HRESULT hr;
	IKsTopologyInfo *pIKsTopologyInfo;
	IKsControl* pKsControl;
	BYTE bMatchedXuId = 0xFF;

	hr = pFilter->QueryInterface(__uuidof(IKsControl),
                               (void **) &pKsControl);
	if (!SUCCEEDED(hr))
	{
		return bMatchedXuId;
	}

	hr = pFilter->QueryInterface(__uuidof(IKsTopologyInfo),
                               (void **) &pIKsTopologyInfo);
	if (!SUCCEEDED(hr))
	{
		return bMatchedXuId;
	}

	hr = pIKsTopologyInfo->get_NumNodes(&numberOfNodes);
	if (SUCCEEDED(hr))
	{
		DWORD i;
		GUID nodeGuid;
		for (i = 0; i < numberOfNodes; i++)
		{
			if (SUCCEEDED(pIKsTopologyInfo->get_NodeType(i, &nodeGuid)))
			{
				if (IsEqualGUID(KSNODETYPE_DEV_SPECIFIC, nodeGuid))
				{ // Found the extension node
					/* TODO: read KSPROPERTY_EXTENSION_UNIT_INFO property to get descriptor */
					KSP_NODE ExtensionProp;
					ULONG ulBytesReturned;

					ExtensionProp.Property.Set = *guid;
					//ExtensionProp.Property.Id = KSPROPERTY_EXTENSION_UNIT_INFO;
					ExtensionProp.Property.Id = 0;
					ExtensionProp.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
					ExtensionProp.NodeId = i;
					
					hr = pKsControl->KsProperty(
						(PKSPROPERTY) &ExtensionProp,
						sizeof(ExtensionProp),
						NULL,
						0,
						&ulBytesReturned);

					if(hr != HRESULT_FROM_WIN32(ERROR_MORE_DATA))
					{
						continue;
					}

					BYTE* buffer = (BYTE*)CoTaskMemAlloc(ulBytesReturned);

					hr = pKsControl->KsProperty(
						(PKSPROPERTY) &ExtensionProp,
						sizeof(ExtensionProp),
						buffer,
						ulBytesReturned,
						&ulBytesReturned);

					bool match = FALSE;
					if(memcmp(buffer, guid, 16) == 0)
					{
						bMatchedXuId = i;
						match = TRUE;
					}else{
						match = FALSE;
					}

					CoTaskMemFree(buffer);

					if(match == FALSE)
						continue;

					pIKsTopologyInfo->Release();
					return bMatchedXuId;
				}
			}
		}
	}
	pIKsTopologyInfo->Release();

	return bMatchedXuId;
}
