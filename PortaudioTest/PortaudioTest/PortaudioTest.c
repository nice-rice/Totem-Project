/** @file paex_sine.c
	@ingroup examples_src
	@brief Play a sine wave for several seconds.
	@author Ross Bencina <rossb@audiomulch.com>
    @author Phil Burk <philburk@softsynth.com>
*/
/*
 * $Id: paex_sine.c 1752 2011-09-08 03:21:55Z philburk $
 *
 * This program uses the PortAudio Portable Audio Library.
 * For more information see: http://www.portaudio.com/
 * Copyright (c) 1999-2000 Ross Bencina and Phil Burk
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files
 * (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

/*
 * The text above constitutes the entire PortAudio license; however, 
 * the PortAudio community also makes the following non-binding requests:
 *
 * Any person wishing to distribute modifications to the Software is
 * requested to send the modifications to the original developer so that
 * they can be incorporated into the canonical version. It is also 
 * requested that these non-binding requests be included along with the 
 * license above.
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>

#include <sndfile.h>
#include "portaudio.h"

#include "pa_util.h"
#include "pa_ringbuffer.h"

#define FILE_NAME "BingAndRuth.wav"

#define NUM_WRITES_PER_BUFFER   (4)
#define NUM_OF_CHANNELS 1
#define PA_SAMPLE_TYPE paFloat32 
#define NUM_SECONDS   (1200)
#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (1024)
#define RING_BUFFER_SIZE (4096)

#ifndef M_PI
#define M_PI  (3.14159265)
#endif

#ifndef min
#define min(a, b) (((a) < (b)) ? (a) : (b)) 
#endif

#define TABLE_SIZE   (128)
typedef struct
{
    PaUtilRingBuffer ring_buffer;
	float *sample;
	SNDFILE* fileHandle;
	long file_size;
	int file_marker;
	float *file_data;
	
}
paTestData;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                            unsigned long framesPerBuffer,
                            const PaStreamCallbackTimeInfo* timeInfo,
                            PaStreamCallbackFlags statusFlags,
                            void *userData )
{
    paTestData *data = (paTestData*)userData;
    float *out = (float*)outputBuffer;
    unsigned long i;
	static unsigned long j = 0;
	float read[FRAMES_PER_BUFFER];

	  ring_buffer_size_t elementsToPlay = PaUtil_GetRingBufferReadAvailable(&data->ring_buffer);
      ring_buffer_size_t elementsToRead = min(elementsToPlay, (ring_buffer_size_t)(framesPerBuffer));

    (void) timeInfo; /* Prevent unused variable warnings. */
    (void) statusFlags;
    (void) inputBuffer;
   	
	PaUtil_ReadRingBuffer(&data->ring_buffer, out, elementsToRead);
	//for(i = 0; i < FRAMES_PER_BUFFER; ++i){
	//	*out++ = read[i];
	//} 
	
    return paContinue;
}

/*
 * This routine is called by portaudio when playback is done.
 */
static void StreamFinished( void* userData )
{
   paTestData *data = (paTestData *) userData;
   printf( "Stream Completed\n");
}



void *WriteThread(void* ptr){
	
	paTestData* data = (paTestData*)ptr;
	data->file_marker = 0;
	long i = 0;
	float sample;

	while(1){
		ring_buffer_size_t numOfElements = PaUtil_GetRingBufferWriteAvailable(&data->ring_buffer);
		
		
		if(numOfElements >= FRAMES_PER_BUFFER){
			/*memcpy(data->write, data->file_data+data->file_marker, sizeof(float) * TABLE_SIZE );*/
			PaUtil_WriteRingBuffer(&data->ring_buffer, data->file_data+data->file_marker, FRAMES_PER_BUFFER);		
			data->file_marker+=FRAMES_PER_BUFFER;
		
		}

		if(data->file_marker+FRAMES_PER_BUFFER > data->file_size)
			data->file_marker = 0;

		/*for(i = 0; i < numOfElements; ++i){ 
			sample = data->file_data[data->file_marker];
			PaUtil_WriteRingBuffer(&data->ring_buffer, &sample, 1);		
			++data->file_marker;
		}*/


	}
	return 0;
}




/*******************************************************************/
int main(void);
int main(void)
{
    PaStreamParameters outputParameters;
    PaStream *stream;
    PaError err;
    paTestData data;
    int i;

	SF_INFO sfInfo;
	SNDFILE* fileHandle;
	long readcount;

    printf("PortAudio Test: output sine wave. SR = %d, BufSize = %d\n", SAMPLE_RATE, FRAMES_PER_BUFFER);
    


/******* Open Sound File *******/

	/*Open the sound file and check for errrors*/
	if(!(fileHandle = sf_open (FILE_NAME, SFM_READ, &sfInfo)) )
	{
		printf("Unable to open file %s\n", FILE_NAME);
		puts(sf_strerror(NULL));
		return 1;
	}
	if (sfInfo.format != (SF_FORMAT_WAV | SF_FORMAT_PCM_16)) {
		fprintf(stderr, "Input should be 16bit Wav\n");
		sf_close(fileHandle);
		return 1;
	}

	data.file_data = (float*) malloc(sfInfo.frames * sizeof(float));
	if (data.file_data == NULL) {
		fprintf(stderr, "Could not allocate memory for data\n");
		sf_close(fileHandle);
		return 1;
	}

	/*Read file into data structure*/
    readcount = sf_readf_float(fileHandle, data.file_data, sfInfo.frames);	
	
	data.file_size = sfInfo.frames;
	sf_close(fileHandle);
	//for(i = 0; i < sfInfo.frames; ++i){
	//	file_data[i] = (float) sin( ((double)i/(double)TABLE_SIZE) * M_PI * 2. );
	//
	//}
	data.file_marker = 0;


/******** Set up Audio Stream *********/

	/*Allocate memory for ring buffer (sample type * number of elements)*/
	data.sample = (float*) malloc((RING_BUFFER_SIZE) * sizeof(float*));
	if(data.sample == NULL)
	{
		printf("Could not allocate Ringbuffer data\n"); 
		return 1;
	}
	/*Initialise ringbuffer to match the previously allocated memory*/
	PaUtil_InitializeRingBuffer(&data.ring_buffer, sizeof(float*), (RING_BUFFER_SIZE), data.sample);

    err = Pa_Initialize();
    if( err != paNoError ) goto error;

    outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
    if (outputParameters.device == paNoDevice) {
      fprintf(stderr,"Error: No default output device.\n");
      goto error;
    }
    outputParameters.channelCount = NUM_OF_CHANNELS;       /* stereo output */
    outputParameters.sampleFormat = PA_SAMPLE_TYPE; /* 32 bit floating point output */
    outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
    outputParameters.hostApiSpecificStreamInfo = NULL;

    err = Pa_OpenStream(
              &stream,
              NULL, /* no input */
              &outputParameters,
              sfInfo.samplerate,
              FRAMES_PER_BUFFER,
              paClipOff,      /* we won't output out of range samples so don't bother clipping them */
              patestCallback,
              &data );
    if( err != paNoError ) goto error;

	


/******Create thread to write sound file data to ring buffer**********/
	pthread_t write_thread;
	int thread_return = pthread_create(&write_thread, NULL, WriteThread, (void*) &data); 
	if(thread_return)
		printf("Write Thread Failed\n");

	Pa_Sleep(2000);

/****Start the Stream ********/
    err = Pa_StartStream( stream );
    if( err != paNoError ) goto error;

    printf("Play for %d seconds.\n", NUM_SECONDS );
    Pa_Sleep( NUM_SECONDS * 1000 );

    err = Pa_StopStream( stream );
    if( err != paNoError ) goto error;

    err = Pa_CloseStream( stream );
    if( err != paNoError ) goto error;

    Pa_Terminate();
    printf("Test finished.\n");
    




    return err;
error:
	
	Pa_Terminate();
	if(data.sample)
		free(data.sample);

    fprintf( stderr, "An error occured while using the portaudio stream\n" );
    fprintf( stderr, "Error number: %d\n", err );
    fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
    return err;
}
