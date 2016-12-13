#ifndef __QIC_H264_FILTERS_H__
#define __QIC_H264_FILTERS_H__

#include "strmif.h"

#ifdef __cplusplus
extern "C"{
#endif

STDAPI QicCreateH264Demux(IBaseFilter** ppF);
STDAPI QicCreateH264TypeTran(IBaseFilter** ppF);

#ifdef __cplusplus
}
#endif

#endif