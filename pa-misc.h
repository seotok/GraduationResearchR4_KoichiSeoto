#ifndef pa_misc_h
#define pa_misc_h
#include "portaudio.h"

int pa_prError(PaError err, char* msg);
void pa_showDev();
int pa_dev2idx(char* name);
PaStream* pa_openSimplex(int dev, char rw, int fs, int ch, int blk, PaSampleFormat fmt);
PaStream* pa_openDuplex(int idev, int odev, int fs, int ich, int och, int blk, PaSampleFormat fmt);
int pa_sampleSize(PaSampleFormat fmt);

#endif


