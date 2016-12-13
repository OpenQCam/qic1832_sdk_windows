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

#include <dshow.h>

/*****************************************************
  Structure Define
******************************************************/
typedef struct {
	unsigned int			uiBitrate;
	unsigned short			usWidth;
	unsigned short			usHeight;
	unsigned char			ucFramerate;
	unsigned char			ucSlicesize;
} EncoderParams_t;

typedef struct {
	unsigned int			uiTimeWindow;		// Not implement, always equal to zero.
	unsigned char			ucOverShoot;		// Not implement, always equal to zero.
	unsigned char			ucMinQP;
	unsigned char			ucMaxQP;
} EncoderQuality_t;

typedef struct {
	unsigned short			usWidth;
	unsigned short			usHeight;
	unsigned int			uiMinBitrate;
	unsigned int			uiMaxBitrate;
	unsigned char			ucMinFramerate;
	unsigned char			ucMaxFramerate;
} EncoderCapability_t;

typedef struct {
	char			szVID[5];
	char			szPID[5];
	char			szREV[5];
} FirmwareVersion_t;

typedef struct {
	unsigned short	usHW;
	unsigned short	usAPI;
	unsigned short	usVID;
	unsigned short	usPID;
	unsigned short	usREV;
	unsigned short	usSVN;
	unsigned char	ucLastErr;
} SystemInfo_t;

typedef struct {
	unsigned int x;
	unsigned int y;
	unsigned int Wx;
	unsigned int Wy;
} CROPRECT;

typedef struct {
	unsigned char bg_alpha;
	unsigned char fg_alpha;
	unsigned char blink_on_frame_count;
	unsigned char blink_off_frame_count;
} OsdMiscell_t;

typedef struct {
	unsigned char color_y;
	unsigned char color_u;
	unsigned char color_v;
} OsdColor_t;

typedef struct {
	unsigned short frame_y_start;
	unsigned short frame_height;
} OsdFrame_t;

typedef struct {
	bool enabled;
	unsigned char char_x_repeat;
	unsigned char char_y_repeat; 
	unsigned short x_start;
	unsigned short y_start;
	unsigned char char_count;
	unsigned char spacing;
} OsdLineAttr_t;

typedef struct {
   bool updated;
   unsigned char fg_color;
   unsigned char bg_color_on;
   unsigned char color_mode;
} OsdCharAttr_t;

typedef struct{
	bool enable;
	unsigned char location;
	unsigned char init_hour;
	unsigned char init_minute;
	unsigned char init_second;
}OsdTimer_t;

typedef void (*EventCallBack_t)();


/*****************************************************
  Arguments Enumeration
******************************************************/
#define FLAG_ENCODER_BITRATE	0x01
#define FLAG_ENCODER_RESOLUTION	0x02
#define FLAG_ENCODER_FRAMERATE	0x04
#define FLAG_ENCODER_SLICESIZE	0x08
#define FLAG_ENCODER_ALL		(FLAG_ENCODER_BITRATE | FLAG_ENCODER_FRAMERATE |	\
								FLAG_ENCODER_RESOLUTION | FLAG_ENCODER_SLICESIZE)

#define FLAG_QUALITY_TIMEWINDOW	0x01
#define FLAG_QUALITY_OVERSHOOT	0x02
#define FLAG_QUALITY_MINQP		0x04
#define FLAG_QUALITY_MAXQP		0x08
#define FLAG_QUALITY_ALL		(FLAG_QUALITY_TIMEWINDOW | FLAG_QUALITY_OVERSHOOT |	\
								FLAG_QUALITY_MINQP | FLAG_QUALITY_MAXQP)

#define OPTION_ENCODER_HOST_DROP_FRAMES		0x01	// Not implement

// EU/XU select layer control macros
#define SELECT_LAYER_STREAM0		(0<<10)
#define SELECT_LAYER_STREAM1		(1<<10)
#define SELECT_LAYER_STREAM2		(2<<10)
#define SELECT_LAYER_STREAM3		(3<<10)
#define SELECT_LAYER_STREAMALL		(7<<10)
#define SELECT_LAYER_TEMPORAL0		(0<<7)
#define SELECT_LAYER_TEMPORAL1		(1<<7)
#define SELECT_LAYER_TEMPORAL2		(2<<7)
#define SELECT_LAYER_TEMPORAL3		(3<<7)
#define SELECT_LAYER_TEMPORALALL	(7<<7)
#define LAYER_OR_VIEW_ID_TO_STREAM(v)	((v>>10)&0x07)
#define LAYER_OR_VIEW_ID_TO_LAYER(v)	((v>>7)&0x07)

typedef enum {
	QIC_ERR_SUCCESS			= 0,
	QIC_WRN_MISMATCH_ARG	= 1,			// The encoder setting is conflicted with profile & level
	QIC_ERR_NO_DEVICE		= -1,			// Device is not found
	QIC_ERR_GEN_FAILURE		= -2,			// Generic failure
	QIC_ERR_INVALID_ARG		= -3,			// Arguments are invalid or out of range
	QIC_ERR_NOT_READY		= -4,			// Device is not ready
	QIC_ERR_NOT_FOUND		= -5,			// Extension unit is not found
	QIC_ERR_NOT_IMPLEMENT	= -6,			// Function is not implemented
	QIC_ERR_TIMEOUT			= -7			// Time out
} QicError_t;	

typedef enum {
	MODE_FLASH_PROGRAM_PP	= 0x00,
	MODE_FLASH_PROGAME_AAI	= 0x01
} FlashProgamMode_t;

typedef enum {
	ENCRYPT_KEY_MODE_128	= 0x00,
	ENCRYPT_KEY_MODE_192	= 0x01,
	ENCRYPT_KEY_MODE_256	= 0x02
} EncyptKeyMode_t;

typedef enum {
	CALIBRATE_NORMAL		= 0x00,
	CALIBRATE_AE_DISABLE	= 0x01,
	CALIBRATE_AWB_DISABLE	= 0x02,
	CALIBRATE_AEAWB_DISABLE	= 0x03
} CalibratMode_t;

typedef enum {
	EFFECT_NONE				= 0x00,
	EFFECT_BLACK_WHITE		= 0x01,
	EFFECT_FALSE_COLOR		= 0x02,
	EFFECT_NEGATIVE			= 0x03,
	EFFECT_POSTERIZATION	= 0x04,
	EFFECT_SEPIA			= 0x05,
	EFFECT_SOLARIZATION1	= 0x06,
	EFFECT_SOLARIZATION2	= 0x07,
	EFFECT_EMBOSS1			= 0x08,
	EFFECT_EMBOSS2			= 0x09,
	EFFECT_SKETCH			= 0x0A
} SpecialEffect_t;

typedef enum {
	ENCODER_PROFILE_BASELINE	= 66,
	ENCODER_PROFILE_MAIN		= 77,
	ENCODER_PROFILE_EXTENDED	= 88,
	ENCODER_PROFILE_HIGH		= 100,
	ENCODER_PROFILE_HIGH_10		= 110,
	ENCODER_PROFILE_HIGH_422	= 122,
	ENCODER_PROFILE_HIGH_444	= 144,
	ENCODER_PROFILE_CAVLC_444	= 44
} EncoderProfile_t;

typedef enum {
	ENCODER_LEVEL_1				= 10,
	ENCODER_LEVEL_1B			= 19,
	ENCODER_LEVEL_1_1			= 11,
	ENCODER_LEVEL_1_2			= 12,
	ENCODER_LEVEL_1_3			= 13,
	ENCODER_LEVEL_2				= 20,
	ENCODER_LEVEL_2_1			= 21,
	ENCODER_LEVEL_2_2			= 22,
	ENCODER_LEVEL_3				= 30,
	ENCODER_LEVEL_3_1			= 31,
	ENCODER_LEVEL_3_2			= 32,
	ENCODER_LEVEL_4				= 40,
	ENCODER_LEVEL_4_1			= 41,
	ENCODER_LEVEL_4_2			= 42,
	ENCODER_LEVEL_5				= 50,
	ENCODER_LEVEL_5_1			= 51
} EncoderLevel_t;

typedef enum {
	ENCODER_ENTROPY_CAVLC		= 0,
	ENCODER_ENTROPY_CABAC		= 1
} EncoderEntropy_t;

typedef enum {
	ENCODER_CONSTRAINT_SET0_FLAG	= 0x01,		// The coded video sequence obeys all constraints in Baseline Profile.
	ENCODER_CONSTRAINT_SET1_FLAG	= 0x02,		// The coded video sequence obeys all constraints in Main Profile.
	ENCODER_CONSTRAINT_SET2_FLAG	= 0x04,		// The coded video sequence obeys all constraints in Extended Profile.
	ENCODER_CONSTRAINT_SET3_FLAG	= 0x08,		// If profile_idc is equal to 66, 77, or 88 and level_idc is equal to 11, this bit indicates level 1b. Else reserved.
	ENCODER_CONSTRAINT_SET4_FLAG	= 0x10,		// Shall be equal to 0.
	ENCODER_RESERVED_ZERO_3BITS		= 0xE0		// Shall be equal to 0.
} EncoderConstraintFlag_t;

typedef enum {
	FORMAT_STREAM_H264_RAW						= 0x00,
	FORMAT_STREAM_H264_CONTAINER 				= 0x01,
	FORMAT_STREAM_H264_RAW_NO_SPSPPS_PADDING	= 0x02,
	FORMAT_STREAM_H264_RAW_NO_ZERO_PADDING		= 0x04,
	FORMAT_STREAM_H264_RAW_NO_ALL_PADDING		= 0x06
} EncoderStreamFormat_t;

// Codec Controls for UVC1.0
typedef enum {
	CODEC_VP8				= 0x00,
	CODEC_VP8_SIMULCAST		= 0x01,
	CODEC_H264				= 0x02,
	CODEC_H264_SIMULCAST	= 0x03
} EuExSelectCodec_t;

typedef enum {
	CameraMotorStatus_Closed	= 0x00,
	CameraMotorStatus_Opened	= 0x01,
	CameraMotorStatus_Moving	= 0x02
} CameraMotorStatus_t;

typedef enum {
	CameraMotorDirection_On		= 0x00,
	CameraMotorDirection_Off	= 0x01,
	CameraMotorDirection_Init	= 0x02,
} CameraMotorDirection_t;

typedef enum {
	CameraMotorAction_Stop		= 0x00,
	CameraMotorAction_Rotate	= 0x01,
} CameraMotorAction_t;

typedef struct {
	unsigned char x; // in number of macro blocks.
	unsigned char y; // in number of macro blocks.
	unsigned char width; // in number of macro blocks.
	unsigned char height; // in number of macro blocks.
} rect_t;

typedef struct {
	unsigned char id; // it is an unique ID value to identify the configure/window
	unsigned char sensitivity; //0: lowest sensitive, 100: highest sensitive
// history
// in number of frames; how long an object needs to be in a region
// before it is considered to be non-moving.
	unsigned char history;
// min object size
// in number of pixels; it is the minimum area of the object.
	unsigned long min_object_size;
	rect_t region_of_intrest;
} md_region_of_interest_t;

typedef struct {
unsigned char number_of_regions;
md_region_of_interest_t region_of_interests[3];
} md_config_t;

typedef struct {
	unsigned long timestamp; // when is it detected
	unsigned char number_of_moving_objects;
	rect_t moving_objects[3];
} md_status_t;

typedef struct {
	CameraMotorStatus_t status; 
} camera_motor_status_t;

/*****************************************************
  Function Prototype
******************************************************/
// Device Handle Controls
QICLIB_API QicError_t QicSetDeviceHandle(IBaseFilter* pVideoCap);
QICLIB_API QicError_t QicClearDeviceHandle();

// AVC Encoder Controls
QICLIB_API QicError_t QicEncoderGetParams(EncoderParams_t *params);
QICLIB_API QicError_t QicEncoderSetParams(EncoderParams_t *params, unsigned char flag);
QICLIB_API QicError_t QicEncoderSetIFrame();
QICLIB_API QicError_t QicEncoderGetGOP(unsigned int *gop);
QICLIB_API QicError_t QicEncoderSetGOP(unsigned int gop);
QICLIB_API QicError_t QicEncoderGetEntropy(unsigned char *entropy);
QICLIB_API QicError_t QicEncoderSetEntropy(unsigned char entropy);

QICLIB_API QicError_t QicEncoderGetNumberOfProfiles(int *num_profiles);
QICLIB_API QicError_t QicEncoderGetProfile(int index, int *max_level, int *profile, int *constraint_flags);
QICLIB_API QicError_t QicEncoderGetProfileAndLevel(int *level, int *profile, int *constraint_flags);
QICLIB_API QicError_t QicEncoderSetProfileAndLevel(int level, int profile, int constraint_flags);

QICLIB_API QicError_t QicEncoderGetNumberOfCapabilities( int *count);
QICLIB_API QicError_t QicEncoderGetStreamCaps (int index, EncoderCapability_t *capability);
QICLIB_API QicError_t QicSetStreamFormat(EncoderStreamFormat_t format);
QICLIB_API QicError_t QicGetStreamFormat(EncoderStreamFormat_t *format);

QICLIB_API QicError_t QicEncoderGetQuality(EncoderQuality_t *quality);							// Not implement
QICLIB_API QicError_t QicEncoderSetQuality(EncoderQuality_t *quality, unsigned char flag);		// Not implement
QICLIB_API QicError_t QicSetEncoderOption( unsigned char encoder_option);						// Not implement

// MMIO Controls
QICLIB_API QicError_t QicMmioWrite(unsigned int addr, unsigned int value);
QICLIB_API QicError_t QicMmioRead(unsigned int addr, unsigned int *value);
QICLIB_API QicError_t QicSetPll(void);
QICLIB_API QicError_t QicReset(void);

// Flash Controls
QICLIB_API QicError_t QicFlashErase(void);
QICLIB_API QicError_t QicFlashSectorErase(int addr);
QICLIB_API QicError_t QicFlashCustom(char *data, int data_size);
QICLIB_API QicError_t QicFlashWrite(int addr, char *data, int data_size);
QICLIB_API QicError_t QicFlashRead(int addr, char *data, int data_size);
QICLIB_API QicError_t QicFlashSetSpiConfig(unsigned int divider, FlashProgamMode_t mode);
QICLIB_API QicError_t QicFlashEraseStatus( unsigned char *erase_status );

// AES Encryptor Controls
QICLIB_API QicError_t QicEncryptorSetKey(char *key, EncyptKeyMode_t key_mode);					// Not implement
QICLIB_API QicError_t QicEncryptorGetKey(char *key, EncyptKeyMode_t key_mode);					// Not implement
QICLIB_API QicError_t QicEncryptorSetConfig(int enable, EncyptKeyMode_t key_mode);				// Not implement

// MISC Controls
QICLIB_API QicError_t QicSetCalibrateMode(CalibratMode_t mode);
QICLIB_API QicError_t QicSetSpecialEffect(SpecialEffect_t effect);
QICLIB_API QicError_t QicSetWBComp(unsigned int r_gain, unsigned int g_gain, unsigned int b_gain);
QICLIB_API QicError_t QicSetExpComp(unsigned int exp_time, unsigned short exp_gain);
QICLIB_API QicError_t QicSetAeSpeed(unsigned short time_step, unsigned char gain_step);
QICLIB_API QicError_t QicSetFlipMode(unsigned char flip_v, unsigned char flip_h);
QICLIB_API QicError_t QicGetFlipMode(unsigned char *flip_v, unsigned char *flip_h);
QICLIB_API QicError_t QicSetLedMode(unsigned char mode);
QICLIB_API QicError_t QicGetLedMode(unsigned char *mode);
QICLIB_API QicError_t QicSetShutterMode(unsigned char mode);
QICLIB_API QicError_t QicGetShutterMode(unsigned char *mode);
QICLIB_API QicError_t QicSetMjpgQFactor(unsigned char qfactor);
QICLIB_API QicError_t QicGetMjpgQFactor(unsigned char *qfactor);
QICLIB_API QicError_t QicSetCroppedImage(unsigned int dev_id, unsigned int crop_switch, CROPRECT crop_window); 

// Custom Controls
QICLIB_API QicError_t QicXuSet (unsigned int ctrlId, unsigned char* pBuf, unsigned int size);
QICLIB_API QicError_t QicXuGet (unsigned int ctrlId, unsigned char* pBuf, unsigned int size, unsigned int* sizeReturned);
QICLIB_API QicError_t QicAutoUpdateInit (EventCallBack_t cb);
QICLIB_API QicError_t QicAutoUpdateUninit ();

// Utility Controls
QICLIB_API QicError_t QicGetFirmwareVersion(FirmwareVersion_t *version);
QICLIB_API QicError_t QicGetSerialNumber(char *serial_number, int data_len, int *bytes_returned);
QICLIB_API QicError_t QicGetSystemInfornation(SystemInfo_t *info);
QICLIB_API QicError_t QicGetiManufacturer(char *serial_number, int data_len, int *bytes_returned);

// Flexible XU API
QICLIB_API QicError_t QicSetAttribute(int attribute, char* buffer, int buffer_size);
QICLIB_API QicError_t QicGetAttribute(int attribute, char* buffer, int buffer_size);

// Codec Controls for UVC1.0
QICLIB_API QicError_t QicEuSetSelectLayer(unsigned short wLayerOrViewID);
QICLIB_API QicError_t QicEuGetSelectLayer(unsigned short *wLayerOrViewID);
QICLIB_API QicError_t QicEuSetProfileAndToolset(unsigned short wProfile, unsigned short wConstrainedToolset, unsigned char bmSettings);
QICLIB_API QicError_t QicEuGetProfileAndToolset(unsigned short *wProfile, unsigned short *wConstrainedToolset, unsigned char *bmSettings);
QICLIB_API QicError_t QicEuSetVideoResolution(unsigned short wWidth, unsigned short wHeight);
QICLIB_API QicError_t QicEuGetVideoResolution(unsigned short *wWidth, unsigned short *wHeight);
QICLIB_API QicError_t QicEuSetMinimumFrameInterval(unsigned int dwFrameInterval);
QICLIB_API QicError_t QicEuGetMinimumFrameInterval(unsigned int *dwFrameInterval);
QICLIB_API QicError_t QicEuSetSliceMode(unsigned short wSliceMode, unsigned short wSliceConfigSetting);
QICLIB_API QicError_t QicEuGetSliceMode(unsigned short *wSliceMode, unsigned short *wSliceConfigSetting);
QICLIB_API QicError_t QicEuSetRateControlMode(unsigned char bRateControlMode);
QICLIB_API QicError_t QicEuGetRateControlMode(unsigned char *bRateControlMode);
QICLIB_API QicError_t QicEuSetAverageBitrateControl(unsigned int dwAverageBitRate);
QICLIB_API QicError_t QicEuGetAverageBitrateControl(unsigned int *dwAverageBitRate);
QICLIB_API QicError_t QicEuSetAverageBitrateControlEx(unsigned int dwAverageBitRate, unsigned int dwCPBsize);
QICLIB_API QicError_t QicEuGetAverageBitrateControlEx(unsigned int *dwAverageBitRate, unsigned int *dwCPBsize);
QICLIB_API QicError_t QicEuSetCpbSizeControl(unsigned int dwCPBsize);
QICLIB_API QicError_t QicEuGetCpbSizeControl(unsigned int *dwCPBsize);
QICLIB_API QicError_t QicEuSetQuantizationParameter(unsigned short wQpPrime_I, unsigned short wQpPrime_P, unsigned short wQpPrime_B);
QICLIB_API QicError_t QicEuGetQuantizationParameter(unsigned short *wQpPrime_I, unsigned short *wQpPrime_P, unsigned short *wQpPrime_B);
QICLIB_API QicError_t QicEuSetSynchronizationAndLongTermReferenceFrame(unsigned char bSyncFrameType, unsigned short wSyncFrameInterval, unsigned char bGradualDecoderRefresh);
QICLIB_API QicError_t QicEuGetSynchronizationAndLongTermReferenceFrame(unsigned char *bSyncFrameType, unsigned short *wSyncFrameInterval, unsigned char *bGradualDecoderRefresh);
QICLIB_API QicError_t QicEuSetLevelIdc(unsigned char bLevelIDC);
QICLIB_API QicError_t QicEuGetLevelIdc(unsigned char *bLevelIDC);
QICLIB_API QicError_t QicEuSetQpRange(unsigned char bMinQp, unsigned char bMaxQp);
QICLIB_API QicError_t QicEuGetQpRange(unsigned char *bMinQp, unsigned char *bMaxQp);
QICLIB_API QicError_t QicEuSetStartOrStopLayer(unsigned char bUpdate);
QICLIB_API QicError_t QicEuGetStartOrStopLayer(unsigned char *bUpdate);
QICLIB_API QicError_t QicEuSetErrorResiliency(unsigned short bmErrorResiliencyFeatures);
QICLIB_API QicError_t QicEuGetErrorResiliency(unsigned short *bmErrorResiliencyFeatures);
QICLIB_API QicError_t QicEuExSetSelectCodec(EuExSelectCodec_t bCodec);
QICLIB_API QicError_t QicEuExGetSelectCodec(EuExSelectCodec_t *bCodec);
QICLIB_API QicError_t QicEuExSetTsvc(unsigned char bTemporalLayer);
QICLIB_API QicError_t QicEuExGetTsvc(unsigned char *bTemporalLayer);

QICLIB_API QicError_t QicOsdStatusSet(unsigned char bStatus);
QICLIB_API QicError_t QicOsdStatusGet(unsigned char *bStatus);
QICLIB_API QicError_t QicOsdMiscellSet(OsdMiscell_t osd_miscell);
QICLIB_API QicError_t QicOsdMiscellGet(OsdMiscell_t *osd_miscell);
QICLIB_API QicError_t QicOsdColorSet(OsdColor_t osd_color[10]);
QICLIB_API QicError_t QicOsdColorGet(OsdColor_t *osd_color);
QICLIB_API QicError_t QicOsdFrameSet(OsdFrame_t osd_frame[8]);
QICLIB_API QicError_t QicOsdFrameGet(OsdFrame_t *osd_frame);
QICLIB_API QicError_t QicOsdLineAttrSet(unsigned char line_id, OsdLineAttr_t line_attr);
QICLIB_API QicError_t QicOsdLineAttrGet(unsigned char line_id, OsdLineAttr_t *line_attr);

QICLIB_API QicError_t QicOsdLineStringSet(unsigned char line_id, unsigned char start_char_index, unsigned char *str, OsdCharAttr_t char_attr);
QICLIB_API QicError_t QicOsdLineStringGet(unsigned char line_id, unsigned char start_char_index, unsigned char *str, OsdCharAttr_t *char_attr);

QICLIB_API QicError_t QicOsdLineArraySet(unsigned char line_id, unsigned char str[32], OsdCharAttr_t char_attr[32]);
QICLIB_API QicError_t QicOsdLineArrayGet(unsigned char line_id, unsigned char *str, OsdCharAttr_t *char_attr);

QICLIB_API QicError_t QicOsdTimerSet(unsigned char line_id, OsdTimer_t osd_timer);

QICLIB_API QicError_t QicOsdLineClear(unsigned char line_id);

/* Motion detection APIs */
QICLIB_API QicError_t QicMDGetVersion(int *major_version, int *minor_version);
QICLIB_API QicError_t QicMDSetEnable(int enable);
QICLIB_API QicError_t QicMDGetEnable(int *enable);
QICLIB_API QicError_t QicMDSetConfiguration(md_config_t* config);
QICLIB_API QicError_t QicMDGetConfiguration(md_config_t* config);
QICLIB_API QicError_t QicMDGetStatus(md_status_t* status);
QICLIB_API QicError_t QicMDSetInterruptMode(int mode);
QICLIB_API QicError_t QicMDGetInterruptMode(int* mode);
QICLIB_API QicError_t QicMDWaitMotion(md_status_t* status);

/* Camera motor control APIs */
QICLIB_API QicError_t QicCameraMotorGetStatus(camera_motor_status_t* status);
QICLIB_API QicError_t QicCameraMotorControl(CameraMotorAction_t act, CameraMotorDirection_t dir, bool autoStop);

#ifdef __cplusplus
}
#endif 