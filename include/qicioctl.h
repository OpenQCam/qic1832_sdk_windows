#ifndef __QIC_IOCTL_H_
#define __QIC_IOCTL_H_

#include <ks.h>
#include <ksmedia.h>

#define QIC_PROPERTY_ID_GET_PICTURE		6

typedef struct{
	BYTE*							PreviewImage;
	ULONG 							PreviewImageSize;
	KS_DATAFORMAT_VIDEOINFOHEADER2*	PreviewImageFormat;
}QIC_PREVIEW_IMAGE;

#endif
