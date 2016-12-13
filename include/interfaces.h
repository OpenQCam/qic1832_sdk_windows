/*
 * Copyright (c) 2008 Quanta Research Institute.
 */
/*****************************************************************************
 * This file declares the public interface of qicdemux
 *
 *****************************************************************************
 * Authors: s.j.hung@quantatw.com
 *****************************************************************************
 * $Id$ 
 *****************************************************************************/

#ifndef __QIC1802_INTERFACES_H_
#define __QIC1802_INTERFACES_H_

// {CD68278E-B5DD-49cc-A9C1-69C5E025D63E}
DEFINE_GUID(CLSID_QIC1802_Demux, 
0xcd68278e, 0xb5dd, 0x49cc, 0xa9, 0xc1, 0x69, 0xc5, 0xe0, 0x25, 0xd6, 0x3e);

// {4D4E47A6-4BD2-4532-88A6-969EA44C1FCB}
DEFINE_GUID(CLSID_QIC1802_Demux_Static, 
0x4d4e47a6, 0x4bd2, 0x4532, 0x88, 0xa6, 0x96, 0x9e, 0xa4, 0x4c, 0x1f, 0xcb);

// {D935ECB6-BA42-4469-B673-B25242851BA7}
DEFINE_GUID(CLSID_QIC1802_NullPkt_Remover, 
0xd935ecb6, 0xba42, 0x4469, 0xb6, 0x73, 0xb2, 0x52, 0x42, 0x85, 0x1b, 0xa7);

// {BA43D5CC-B101-43f3-B118-EA7BD39C1FE8}
DEFINE_GUID(IID_IQIC1802Demux, 
0xba43d5cc, 0xb101, 0x43f3, 0xb1, 0x18, 0xea, 0x7b, 0xd3, 0x9c, 0x1f, 0xe8);

// {614FCEDC-1C4D-43d3-BE00-B65A74C2BAED}
DEFINE_GUID(IID_IQIC1802DemuxEx, 
0x614fcedc, 0x1c4d, 0x43d3, 0xbe, 0x0, 0xb6, 0x5a, 0x74, 0xc2, 0xba, 0xed);

// {F8936C03-3D52-47dc-9CE7-32D850265D36}
DEFINE_GUID(IID_IQIC1802DemuxEx2, 
0xf8936c03, 0x3d52, 0x47dc, 0x9c, 0xe7, 0x32, 0xd8, 0x50, 0x26, 0x5d, 0x36);


typedef struct
{
	// latency
	float late;					/* late in millisecond */

	// filter out bad frame
	ULONG total_frame_count;	/* number of output frames before filter out bad frames */
	ULONG bad_frame_count;		/* number of frames with incorrect size */
	ULONG bad_ref_frame_count;	/* number of frames dropped becuase last frame incorrect */

	// bit rate
	ULONG input_bytes;			/* number of input bytes */
	ULONG input_frame_size;		/* the size of last input frame */
	ULONG input_frame_count;	/* number of input frames */
	ULONG output_bytes;			/* number of output bytes */
	ULONG output_frame_size;	/* the size of last output frame */
	ULONG output_frame_count;	/* number of output frames */

	// bit rate and fps, note that they are updated by QIC1802 demux every 0.8 sec
	double input_bitrate;		/* input bits per second */
	double input_fps;			/* input frames per second */
	double output_bitrate;		/* output bits per second */
	double output_fps;			/* output frames per second */
}QIC1802_DEMUX_STATUS;

typedef struct
{
	BYTE  filter_mode;				/* 0: disable, 1: bad frame only, 2: bad frame and bad reference frame */
	BYTE  timestamp_mode;			/* Obsoleted */
	DWORD minimum_output_interval;	/* minimum output interval in millisecond */
	BYTE  aes_key[32];
	DWORD aes_key_len;				/* 0: disable aes, 128: 128bits, 192: 192bits, 256: 256bits */
}QIC1802_DEMUX_OPTIONS;

class IQIC1802Demux: public IUnknown
{
public:
	virtual void getDemuxStatus(QIC1802_DEMUX_STATUS* status) = 0;
	virtual void getDemuxOptions(QIC1802_DEMUX_OPTIONS* options) = 0;
	virtual void setDemuxOptions(QIC1802_DEMUX_OPTIONS* options) = 0;
};

typedef struct
{
	// peak bit rate, it records the peak value of input/output bit rate and clear to zero when initialized.
	double peak_input_bitrate;		/* peak value of input bit rate */
	double average_input_bitrate;	/* average value of input bit rate */
	double peak_output_bitrate;		/* peak value of output bit rate */
	double average_output_bitrate;	/* average value of output bit rate */
}QIC1802_DEMUX_STATUS_EX;

class IQIC1802DemuxEx: public IQIC1802Demux
{
public:
	virtual void getDemuxStatusEx(QIC1802_DEMUX_STATUS_EX* status, int status_size) = 0;
};

typedef struct
{
	BYTE  fix_hardware_nal_bug;				/* 0: disable, 1: enable */
}QIC1802_DEMUX_OPTIONS_EX;

class IQIC1802DemuxEx2: public IQIC1802DemuxEx
{
public:
	virtual void getDemuxOptionsEx(QIC1802_DEMUX_OPTIONS_EX* options, int option_size) = 0;
	virtual void setDemuxOptionsEx(QIC1802_DEMUX_OPTIONS_EX* options, int option_size) = 0;
};


#endif /*__QIC1802_INTERFACES_H_*/