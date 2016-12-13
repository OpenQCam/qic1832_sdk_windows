#pragma once

#if defined (QIC_XUCONTROL_DLL_EXPORTS)
#define QICLIB_API __declspec(dllexport)
#elif defined (QIC_XUCONTROL_DLL_IMPORTS)
#define QICLIB_API __declspec(dllimport)
#else
#define QICLIB_API 
#endif

#ifdef __cplusplus
extern "C" {
#endif 

#include "XUControl.h"

/*****************************************************
  Private Function Prototype - Use for Quanta test only
******************************************************/
typedef enum {
	USB_BOOT_DISABLE		= 0x00,
	USB_BOOT_ENABLE			= 0x01,
	USB_BOOT_JUMP			= 0x02
} UsbBootMode_t;

QICLIB_API QicError_t QicUsbBoot(UsbBootMode_t mode);
QICLIB_API QicError_t QicTestExec(BYTE type, BYTE index, PBYTE arg, BYTE arg_size, PBYTE result, PBYTE result_size);	// Not implement

QICLIB_API QicError_t QicSetEncoderFrameSizeFormat(unsigned char format);
QICLIB_API QicError_t QicGetEncoderFrameSizeFormat(unsigned char *format);
QICLIB_API QicError_t QicSetH264Disable(unsigned char disable);
QICLIB_API QicError_t QicGetH264Disable(unsigned char *disable);

QICLIB_API QicError_t QicSetPanTilt(unsigned short pan, unsigned short tilt);
QICLIB_API QicError_t QicGetPanTilt(unsigned short *pan, unsigned short *tilt);

/*****************************************************
  Private Function Prototype - Use for QIC1806 only (C6 / C7)
******************************************************/
#define FLAG_CALIBRATE_EX_AE		0x01
#define FLAG_CALIBRATE_EX_AWB		0x02
#define FLAG_CALIBRATE_EX_AF		0x04
#define FLAG_CALIBRATE_EX_AFL		0x08
#define FLAG_CALIBRATE_EX_RCA		0x10

typedef struct {
	unsigned char	ucAeStatus;
	unsigned char	ucAwbStatus;
	unsigned char	ucAfStatus;
	unsigned char	ucAflStatus;
} CalibrateModeEx_t;

typedef enum{
	// Specify sequential order only take A address and read data field according THE passed in address and size of data
	ORDER_SEQUENTIAL	= 128, // 1??? ????
	ORDER_ABSOLUTE		= 0,   // 0??? ????
	WAIT_DO			= 64, // 1?? ????
	WAIT_NO			= 0,
	TYPE_QIC		= 0,	//00????
	TYPE_SENSOR		= 16,	// 01????
	TYPE_AUDIO		= 32	// 10????
} BatchMMIO_enum;

typedef struct _BatchMMIO_t{
	unsigned char numReg;
	unsigned char addrSize;
	unsigned char dataSize;
	unsigned char reserved;
	unsigned int dwRegType;
	unsigned int dwWaitSensorVsync;
	unsigned int dwSequential;
	unsigned char* lpAddr;
	unsigned char* lpData;
} BatchMMIO_t, *LPBatchMMIO_t;

#define START_FLASH_ADDRESS_FOR_CUSTOM_SERIAL_NUMBER_API	0x19000
#define SIZE_OF_CUSTOM_SN_FORMAT 32

typedef struct {
	unsigned char sentinel_of_set;
	unsigned char sn_length;
	unsigned char custom_sn[30];
} CustomSNFormat_t;


#define FLAG_SVC_TSVC_ENABLE	0x01
#define FLAG_SVC_SSVC_ENABLE	0x02
#define FLAG_SVC_QSVC_ENABLE	0x04

typedef enum {
	RGB_FORMAT_BYPASS			= 0x00,
	RGB_FORMAT_REC601_RGB565	= 0x01,
	RGB_FORMAT_REC601_RGB888	= 0x02,
	RGB_FORMAT_REC709_RGB565	= 0x03,
	RGB_FORMAT_REC709_RGB888	= 0x04
} RgbFormat_t;

typedef enum {
	RGB_ARRANGEMENT_RGB			= 0x00,
	RGB_ARRANGEMENT_BGR			= 0x01
} RgbArrangement_t;

typedef enum {
	RGB_ENDIAN_BIG_ENDIAN		= 0x00,
	RGB_ENDIAN_LITTLE_ENDIAN	= 0x01
} RgbEndian_t;

typedef enum {
	MIRROR_MODE_DISABLE			= 0x00,
	MIRROR_MODE_PREVIEW_MIRROR	= 0x01,
	MIRROR_MODE_VIDEO_MIRROR	= 0x02,
	MIRROR_MODE_ALL_MIRROR		= 0x03
} MirrorMode_t;

typedef enum {
	TNR_MODE_DISABLE			= 0x00,
	TNR_MODE_PREVIEW_ENABLE		= 0x01,
	TNR_MODE_VIDEO_ENABLE		= 0x02,
	TNR_MODE_ALL_ENABLE			= 0x03
} TnrMode_t;

typedef enum {
	RATE_CONTROL_TYPE_VBR			= 0x00,
	RATE_CONTROL_TYPE_CBR			= 0x01,
	RATE_CONTROL_TYPE_FIXED_QP		= 0x02,
	RATE_CONTROL_TYPE_GLOBAL_VBR	= 0x03
} RateControlType_t;

typedef struct{
	unsigned int time;
	unsigned int msg_addr;
	unsigned int value;
}QicLog_t;

// Custom Serial Number(at 0x19000)
QICLIB_API QicError_t QicSetCustomSerialNumber( CustomSNFormat_t format );
QICLIB_API QicError_t QicGetCustomSerialNumber( CustomSNFormat_t *format );

// Batch MMIO Read/Write
QICLIB_API QicError_t QicBatchMmioWrite(BatchMMIO_t* bat);
QICLIB_API QicError_t QicBatchMmioWriteEx(unsigned char* pData); // Takes binary data directly
QICLIB_API QicError_t QicBatchMmioRead(BatchMMIO_t *bat);
QICLIB_API QicError_t QicBatchMmioReadEx(unsigned char* pData); // Takes binary data directly

// MISC Controls
QICLIB_API QicError_t QicSetCalibrateModeEx(CalibrateModeEx_t mode, unsigned char flag);
QICLIB_API QicError_t QicSetCalibrateRcaLevel(unsigned char level);
QICLIB_API QicError_t QicGetDebugLogs(QicLog_t* logs, unsigned int max_count, unsigned int* returned_count);

// Video Engine Controls
QICLIB_API QicError_t QicVideoSetRGBFormat(unsigned char RGB_format,
										   unsigned char RGB_arrangement,
										   unsigned char RGB_endian);
QICLIB_API QicError_t QicVideoGetRGBFormat(unsigned char *RGB_format,
										   unsigned char *RGB_arrangement,
										   unsigned char *RGB_endian);
QICLIB_API QicError_t QicVideoSetMirror(unsigned char mirror_status);
QICLIB_API QicError_t QicVideoGetMirror(unsigned char *mirror_status);
QICLIB_API QicError_t QicVideoSetTemporalNoiseReduction(unsigned char TNR_status);
QICLIB_API QicError_t QicVideoGetTemporalNoiseReduction(unsigned char *TNR_status);
QICLIB_API QicError_t QicVideoSetRateControl(unsigned char rate_control_type,
											 unsigned char min_QP_value,
											 unsigned char max_QP_value);
QICLIB_API QicError_t QicVideoGetRateControl(unsigned char *rate_control_type,
											 unsigned char *min_QP_value,
											 unsigned char *max_QP_value);
QICLIB_API QicError_t QicVideoSetAutoRefresh(unsigned char refresh_mode,
											 unsigned short I_MB_interval,
											 unsigned char I_Slice_refresh_mode);
QICLIB_API QicError_t QicVideoGetAutoRefresh(unsigned char *refresh_mode,
											 unsigned short *I_MB_interval,
											 unsigned char *I_Slice_refresh_mode);
QICLIB_API QicError_t QicVideoSetRegionOfInterest(unsigned char ROI_status,
												  unsigned long ROI_dp_delta_1_and_2,
												  unsigned long ROI_rectangle_1,
												  unsigned long ROI_rectangle_2);
QICLIB_API QicError_t QicVideoGetRegionOfInterest(unsigned char *ROI_status,
												  unsigned long *ROI_dp_delta_1_and_2,
												  unsigned long *ROI_rectangle_1,
												  unsigned long *ROI_rectangle_2);
QICLIB_API QicError_t QicVideoSetDynamicResolutionScaling(unsigned char DRS_status);
QICLIB_API QicError_t QicVideoGetDynamicResolutionScaling(unsigned char *DRS_status);
QICLIB_API QicError_t QicVideoSetScalableVideoCoding(unsigned char SVC_type,
													 unsigned char TSVC_layer);
QICLIB_API QicError_t QicVideoGetScalableVideoCoding(unsigned char *SVC_type,
													 unsigned char *TSVC_layer);
QICLIB_API QicError_t QicVideoSetSimulcast(unsigned char simulcast_status,
										   unsigned char simulcast_stream_index);
QICLIB_API QicError_t QicVideoGetSimulcast(unsigned char *simulcast_status,
										   unsigned char *simulcast_stream_index);
QICLIB_API QicError_t QicVideoSetStereoView(unsigned char stereo_mode);
QICLIB_API QicError_t QicVideoGetStereoView(unsigned char *stereo_mode);

// Audio Engine Controls
QICLIB_API QicError_t QicAudioSetAcousticNoiseReduction(unsigned char NR_status);
QICLIB_API QicError_t QicAudioGetAcousticNoiseReduction(unsigned char *NR_status);
QICLIB_API QicError_t QicAudioSetEqualizer(unsigned char EQ_status, unsigned char equalizer_type);
QICLIB_API QicError_t QicAudioGetEqualizer(unsigned char *EQ_status, unsigned char *equalizer_type);
QICLIB_API QicError_t QicAudioSetBeamForming(unsigned char BF_status);
QICLIB_API QicError_t QicAudioGetBeamForming(unsigned char *BF_status);

/*****************************************************
  Dell Function Prototype - Use for Dell XU2 only
******************************************************/

typedef struct {
	unsigned short			usDellId;
	unsigned short			usCameraModel;
	unsigned int			uiCapability;
	unsigned char			ucReserved;
} DellDeviceInfo_t;

QICLIB_API QicError_t DellGetDeviceInfo(DellDeviceInfo_t *device_info);
QICLIB_API QicError_t DellGetLed(unsigned char *led_state);
QICLIB_API QicError_t DellSetLed(unsigned char led_state);
QICLIB_API QicError_t DellGetAeMaxGain(char *max_gain);
QICLIB_API QicError_t DellSetAeMaxGain(char max_gain);
QICLIB_API QicError_t DellGetAeMaxTime(unsigned short *max_time_index);
QICLIB_API QicError_t DellSetAeMaxTime(unsigned short max_time_index);

#ifdef __cplusplus
}
#endif 