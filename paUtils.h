#ifndef _PA_UTIL_H_
#define _PA_UTIL_H_

#ifdef __cplusplus
extern "C" {
#endif

PaStream *startupPa(int inputChanCount, int outputChanCount, 
    int samp_rate, int frames_per_buffer, PaStreamCallback *paCallback, void *data);

void shutdownPa(PaStream *stream);

#ifdef __cplusplus
}
#endif

#endif /* _PA_UTIL_H_ */
