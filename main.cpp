/* Program for a sinewave generation synthesizer
 * which can play simultaneous tones
 * 
 * Demonstrates use of NCurses and PortAudio (also libsnd for debugging file) 
 */


#include <stdio.h>
#include <stdlib.h>     /* atof */
#include <string.h>		/* memset() */
#include <sndfile.h>	/* libsndfile */
#include <portaudio.h>	/* portaudio */
#include <stdatomic.h>  /* permits write/read of "cmd" to be atomic */
#include <unistd.h>     /* for usleep(ms) */
#include "key_tables.h" /* key to freq functions */
#include "synth.h"      /* Class Synth */
#include "paUtils.h"    /* portaudio function prototypes */
#include "main.h"

#define NUM_CHAN    2

#if(USE_NCURSES)
#include <ncurses.h>    /* This library is for getting input without hitting return */
void user_io_ncurses(Synth *ps);
#else
void user_io_plain(Synth *ps);
#endif

/* PortAudio callback function protoype */
static int paCallback( 
    const void *inputBuffer, 
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData );

/* callback structure */
typedef struct {
    Synth *ps;
#if(DB_WAV_OUT)
    SNDFILE *sndfile;
#endif
} Buf;

int main(int argc, char *argv[])
{
    /* instantiate instance of Synth class */
    Synth synth;
    Synth *ps = &synth;
    /* instantiate instance of portaudio callback data structure */
	Buf buf;
	/* PortAudio data stream */
    PaStream *stream;
#if (DB_WAV_OUT)
    /* libsndfile data structures */
    SF_INFO sfinfo;
#endif

    /* initialize tables */
    init_key2index();

    /* initialize Synth */
    ps->init_synth(SAMP_RATE);

    /* initialize struct for callback */
    buf.ps = ps;

    /* start up Port Audio */
    stream = startupPa(1, NUM_CHAN, 
        SAMP_RATE, FRAMES_PER_BUFFER, paCallback, &buf);

#if (DB_WAV_OUT)
    /* open debug output file */
    memset(&sfinfo, 0, sizeof(sfinfo));
    sfinfo.samplerate = SAMP_RATE;
	sfinfo.channels = NUM_CHAN;
	sfinfo.format = SF_FORMAT_WAV | SF_FORMAT_PCM_16;
    if ( (buf.sndfile = sf_open ("test_file.wav", SFM_WRITE, &sfinfo)) == NULL ) {
        fprintf (stderr, "Error: could not open test wav file\n");
        return -1;
    }



#endif

#if(USE_NCURSES)
    user_io_ncurses(ps);
#else
    user_io_plain(ps);
#endif

#if (DB_WAV_OUT)
    /* close debugging output wav file */
    sf_close (buf.sndfile);
#endif

    /* shut down Port Audio */
    shutdownPa(stream);

    return 0;
}


/* This is the callback routine invoked by the PortAudio engine when audio is needed.
 * It will be called in the "real-time" thread, so don't do anything
 * in the routine that requires significant time or resources.
 */
//Your code here
static int paCallback( 
    const void *inputBuffer, 
    void *outputBuffer,
    unsigned long framesPerBuffer,
    const PaStreamCallbackTimeInfo* timeInfo,
    PaStreamCallbackFlags statusFlags,
    void *userData )
{
    double *po;
    Buf *p = (Buf *)userData; 
    float *output = (float *)outputBuffer;

    //get mono output from synth
    po = p->ps->synth_block(framesPerBuffer);
    
    //interleave stereo callback output buffer from mono
    for (int i = 0; i < framesPerBuffer; i++){
        output[NUM_CHAN*i] = po[i];
        output[NUM_CHAN*i + 1] = po[i];
    }
    sf_write_float(p->sndfile, output, framesPerBuffer * NUM_CHAN);

    return 0;
}