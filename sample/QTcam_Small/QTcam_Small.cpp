/* 
	This is the sample code to introduce how to implement simple QIC1822 applictaion.
	The YUV and H.264 data will be individually decoded and show on different windows.
	The sample code does not handle some Window Messages which must to be process such as 
	WM_MOVE, WM_SIZE, WM_DISPLAYCHANGE and so on. 
*/

#include "stdafx.h"
#include "objbase.h"
#include "QTcam_Small.h"
#include "Core.h"
#include "qic_h264_filters.h"

#define CODEC_H264		0

#define ICODEC			CODEC_H264

#define MAX_LOADSTRING 100

#define H264_WIDTH		1280
#define H264_HEIGHT		720
#define H264_BITRATE	2000000
#define H264_FRAMERATE	30
#define YUV_WIDTH		384
#define YUV_HEIGHT		216
#define BORDER_HEIGHT	40

// {04FE9017-F873-410e-871E-AB91661A4EF7}
DEFINE_GUID(CLSID_FFDSHOW , 0x04fe9017, 0xf873, 0x410e, 0x87, 0x1e, 0xab, 0x91, 0x66, 0x1a, 0x4e, 0xf7);

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name
int iCodec;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

void Init_QTCam();

HWND ghwnd,ghwnd2;
IBaseFilter *pVcap = NULL;
IBaseFilter *pMpeg2Demux = NULL;
IGraphBuilder *pFg = NULL;
IVMRWindowlessControl *pWcYUV = NULL, *pWcH264 = NULL;

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_QTCAM_SMALL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	iCodec      = ICODEC;

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_QTCAM_SMALL));

	/*Step 1 : COM initialize*/
	CoInitialize(NULL);
	Init_QTCam();

	/**/
	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_QTCAM_SMALL));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= MAKEINTRESOURCE(IDC_QTCAM_SMALL);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;
   HWND hWnd2;

   hInst = hInstance; // Store instance handle in our global variable
   
   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   hWnd2 = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
	
   /**/
   ghwnd = hWnd;
   ghwnd2 = hWnd2;
   /**/
   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);
	
   ShowWindow(hWnd2, nCmdShow);
   UpdateWindow(hWnd2);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam);
		wmEvent = HIWORD(wParam);
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_TIMER:	
		break;
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		CoreStopPreview(pFg);			//Should stop graph and remove all filters when window closing. 
		CoreNukeDownstream(pFg , pVcap);
		CoUninitialize();			//Release COM object
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

void Init_QTCam()
{
	/*Declarate Variable*/
	HRESULT hr;
	IMoniker *monikerList[5];					//store all Video Capture Devices on this PC 
	IMoniker *pmVideo=NULL;						//represent the QIC1822 device
	ICaptureGraphBuilder2 *pBuilder;			//help to construct the filter graph
	IBaseFilter *pVmrH264;						//represent the Video Render for H.264
	IBaseFilter *pVmrYUV;						//represent the Video Render for YUV
	IAMStreamConfig *pVsc;						//represent IAMStreaming fo the QIC1822 for setting YUV format
	IPin *mpeg2Pin = NULL;						//represent the Mpeg2TS output pin of QIC1822
	IBaseFilter *pDecoder = NULL;
	
	/*Step 2 ; Initial graph manager and filter graph*/
	hr = CorePrepareFilterGraph(&pBuilder, &pFg);					if(hr != S_OK) goto exit;
	hr = pBuilder->SetFiltergraph(pFg);								if(hr != S_OK) goto exit;

	/*Step 3 : Enumerate Video Device and find the QIC1822 device*/
	hr = CoreGetVideoDevice(monikerList,&pmVideo);				//enumerate all Video Capture Device on the pc 
	if(pmVideo == NULL)
	{
		MessageBoxA(NULL,"QIC not found","QIC not found",MB_OK);
		PostQuitMessage(0);
	}
	else
	{
		hr = CoreInitFilterGraph(pmVideo , &pVcap);													if(hr != S_OK) goto exit;

		/*Step 4 : Get the IAMStreamingConfig interface*/
		hr = CoreGetVSConfig(pBuilder, pVcap, &pVsc);												if(hr != S_OK) goto exit;
		hr = pFg->AddFilter(pVcap,NULL);															if(hr != S_OK) goto exit;

		// Encoded: H.264
		/*Step 5 : Set the H.264 pin property*/
		if(iCodec == CODEC_H264){
			hr = CoreSetVideoDeviceH264Pin(H264_WIDTH, H264_HEIGHT, H264_BITRATE, H264_FRAMERATE);		if(hr != S_OK) goto exit;
		}
		CoreResizeWindow(ghwnd, H264_WIDTH, H264_HEIGHT + BORDER_HEIGHT);	

		/*Step 6 : Connect filter*/
		if(iCodec == CODEC_H264){
			hr = QicCreateH264TypeTran(&pMpeg2Demux);
			hr = pFg->AddFilter(pMpeg2Demux, L"H264TypeTran");
		}
		hr = CoreFilerGetMJPGOnlyPin (pVcap, &mpeg2Pin);
		hr = CoreConnectFilters(pFg,mpeg2Pin, pMpeg2Demux);

		/*Step 7 : Indicate how to show streams on the screen*/
		hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmrH264);
		hr = CoreInitVideoRender(pVmrH264, ghwnd);	//have to call CoreResizeWindow() function before calling it to ensure paint on the right position
		hr = pVmrH264->QueryInterface(IID_IVMRWindowlessControl, (void**)&pWcH264);
		pFg->AddFilter(pVmrH264, L"VMR7 H.264 Render");
		
		if(iCodec == CODEC_H264){
			hr = CoreAddFilterByCLSID(pFg, CLSID_FFDSHOW, L"ffdshow Video Decoder", &pDecoder);
			if (SUCCEEDED(hr))
			{
				hr = CoreConnectFilters(pFg, pMpeg2Demux, pDecoder);
				hr = CoreConnectFilters(pFg, pDecoder, pVmrH264);
			}
			else
			{
				pDecoder = NULL;
				hr = CoreConnectFilters(pFg, pMpeg2Demux, pVmrH264);
			}
		}

		// Preview: YUV
		/*Step 5 : Set the YUV pin property*/
		hr = CoreSetVideoDeviceYUVPin(pVsc, YUV_WIDTH, YUV_HEIGHT);									if(hr != S_OK) goto exit;
		pVsc->Release();
		CoreResizeWindow(ghwnd2, YUV_WIDTH, YUV_HEIGHT + BORDER_HEIGHT);

		/*Step 7 : Indicate how to show streams on the screen*/
		hr = CoCreateInstance(CLSID_VideoMixingRenderer, NULL,
			CLSCTX_INPROC, IID_IBaseFilter, (void**)&pVmrYUV);										if(hr != S_OK) goto exit;
		hr = CoreInitVideoRender(pVmrYUV, ghwnd2);													if(hr != S_OK) goto exit;
		hr = pFg->AddFilter(pVmrYUV, L"VMR7 YUV Render");											if(hr != S_OK) goto exit;
		hr = pBuilder->RenderStream(&PIN_CATEGORY_CAPTURE, &MEDIATYPE_Video, pVcap, NULL, pVmrYUV);	if(hr != S_OK) goto exit;

		/*Step 8 : Run the filter graph*/
		hr = CoreStartPreview(pFg);																	if(FAILED(hr)) goto exit;
	}

	return;

exit:
	char msg[255];
	sprintf_s(msg, 255, "Error: hr=0x%08x", hr);
	MessageBoxA(NULL,msg,"Error",MB_OK);
}
