//#include <qedit.h>
#include <dshow.h>
#include <Bdaiface.h>
#include "XUControl.h"

#define INITGUID
#include <initguid.h>

DEFINE_GUID(CLSID_MPEG2_SA, 
0x67447292, 0x921e, 0x4d08, 0x9d, 0x1b, 0xc0, 0x5a, 0xdd, 0x20, 0x3e, 0xb8);

HRESULT CoreGetVideoDevice(IMoniker**,IMoniker**);
HRESULT CorePrepareFilterGraph(ICaptureGraphBuilder2 **pBuilder,IGraphBuilder **pFg);
HRESULT CoreInitFilterGraph(IMoniker* pmVideo,IBaseFilter** pVcap);
HRESULT CoreGetVSConfig(ICaptureGraphBuilder2 *pBuilder,IBaseFilter* pVcap,IAMStreamConfig **pVsc);
HRESULT CoreSetVideoDeviceYUVPin(IAMStreamConfig *pVsc,int width,int height);
HRESULT CoreSetVideoDeviceH264Pin(int width, int height,int bitrate,int fps);
HRESULT CoreConnectFilters(IGraphBuilder *pGraph, IBaseFilter *pSrc,  IBaseFilter *pDest);
HRESULT CoreConnectFilters(IGraphBuilder *pGraph, IPin *pOut, IBaseFilter *pDest);
HRESULT CoreGetUnconnectedPin(IBaseFilter *pFilter, PIN_DIRECTION PinDir,IPin **ppPin);
HRESULT CoreInitVideoRender(IBaseFilter *pVmr,HWND hwnd);
HRESULT CoreStartPreview(IGraphBuilder *pFg);
HRESULT CoreStopPreview(IGraphBuilder *pFg);
HRESULT CoreAddFilterByCLSID(IGraphBuilder *pGraph, const GUID& clsid, LPCWSTR wszName, IBaseFilter **ppF);
HRESULT CoreConfigureQicToAnxb(IBaseFilter *pQicToAnxb);
HRESULT	CoreFilerGetMpeg2tsPin (IBaseFilter *pFiler, IPin **ppPin);
HRESULT CoreIsPinContainMpeg2ts (IPin *pPin);
HRESULT CoreConfigureProfileLevel(int iProfile, int iLevel);
HRESULT CoreConfigureStreamFormat(int iFormat);
HRESULT CoreCheckEncoderCapabilities(int width, int height);
void CoreNukeDownstream(IGraphBuilder *pFg , IBaseFilter *pf);
void CoreResizeWindow(HWND hwnd , int width , int height);
HRESULT	CoreFilerGetMJPGOnlyPin(IBaseFilter *pFiler, IPin **ppPin);
BYTE CoreXUExist(IBaseFilter* pFilter, GUID* guid);
