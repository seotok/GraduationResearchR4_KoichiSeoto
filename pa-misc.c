#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "portaudio.h"
#include "pa-misc.h"
#include "hpck.h"

int pa_prError(PaError err, char* msg)
{
    fprintf( stderr, "%s\n", msg);
    fprintf( stderr, "An error occured while using the portaudio stream: \n");
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return -1;
}

int pa_sampleSize(PaSampleFormat fmt)
{
	switch(fmt){
		case paInt8:
		case paUInt8: return 1;
		case paInt16: return 2;
		case paInt24: return 3;
		case paInt32: return 4;
		case paFloat32: return 4;
		default:
			fprintf(stderr, "pa_sampleSize: 指定されたフォーマットは定義されていません。\n");
			return -1;
	}
}

void pa_showDev()
{
    int i;
    const PaDeviceInfo *devInfo;
    printf("Audio Device: \n");
    for(i=0; i<Pa_GetDeviceCount(); i++){
	    devInfo = Pa_GetDeviceInfo(i);
	    printf("%d\t%s\n", i, devInfo->name);
    }
}

int pa_dev2idx(char* name)
{
	int i;
    const PaDeviceInfo *devInfo;
	for(i=0; i<Pa_GetDeviceCount(); i++){
	    devInfo = Pa_GetDeviceInfo(i);
		if(!strcmp(name, devInfo->name)) return i;
	}
	return -1;
}

static void* nullReturn(char* s)
{
	fprintf(stderr, "%s", s);
	return NULL;
}

PaStream* pa_openDuplex(int idev, int odev, int fs, int ich, int och, int blk, PaSampleFormat fmt)
{
	PaStream *stream = (PaStream*)MALLOC(sizeof(PaStream*));
	PaStreamParameters iparam, oparam;
	
	iparam.device = idev;
	iparam.channelCount = ich;
	iparam.sampleFormat = fmt;
	iparam.suggestedLatency = Pa_GetDeviceInfo( iparam.device )->defaultLowInputLatency;
	iparam.hostApiSpecificStreamInfo = NULL;

	oparam.device = odev;
	oparam.channelCount = och;
	oparam.sampleFormat = fmt;
	oparam.suggestedLatency = Pa_GetDeviceInfo( oparam.device )->defaultLowOutputLatency;
	oparam.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
	  &stream, &iparam, &oparam,
	  (double)fs,
	  (unsigned long)blk, /* frames per buffer */
	  paClipOff, NULL, NULL);

	if(err != paNoError) pa_prError(err, "In pa_openDuplex");

	if(!stream)
		return (PaStream*)nullReturn("pa_openDuplex: stream open failed\n"); 
	else
		return stream;
}

PaStream* pa_openSimplex(int dev, char rw, int fs, int ch, int blk, PaSampleFormat fmt)
{
	PaStream *stream = (PaStream*)MALLOC(sizeof(PaStream*));
	PaStreamParameters param;
	
	param.device = dev;
	param.channelCount = ch;
	param.sampleFormat = fmt;
	param.suggestedLatency = Pa_GetDeviceInfo( param.device )->defaultLowInputLatency;
	param.hostApiSpecificStreamInfo = NULL;

	PaError err = Pa_OpenStream(
		&stream,
		(rw=='r' ? &param : NULL),
		(rw=='w' ? &param : NULL),
		(double)fs,
		(unsigned long)blk, /* frames per buffer */
		paClipOff, NULL, NULL);

	if(err != paNoError) pa_prError(err, "In pa_openSimplex");

	if(!stream)
		return (PaStream*)nullReturn("pa_openSimplex: stream open failed\n"); 
	else
		return stream;
}
