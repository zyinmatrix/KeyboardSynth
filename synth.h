#ifndef _SYNTH_H_
#define _SYNTH_H_

#include <stdatomic.h>  /* permits write/read of "cmd" to be atomic */

/* other defines */
#define KEYS_VOICED         4   /* number of simultaneouse keys voiced */
#define SAMP_RATE           48000
#define FRAMES_PER_BUFFER   1024
#define FS_AMPL             0.5 /* -6 dB FS */
//#define ATTACK_FACTOR       0.99980 /* attack time constant of 100 ms */
//#define ATTACK_FACTOR       0.99960 /* attack time constant of 50 ms */
#define ATTACK_FACTOR       0.99800 /* attack time constant of 10 ms */
#define DECAY_FACTOR        0.99998 /* decay time constant of 1.0 sec */
#define DROP_LEVEL          0.001  /* -60 dBFS */
#define PI                  3.14159265358979323846

/* commands */
enum {
    CMD_ADD_KEY = 1,
    CMD_RM_KEY
};

/* for each tone */
typedef struct {
    int key; /* index into freq array */   
    double f0; /* frequency associated with key */ 
    double phase_inc; /* phase increment per sample to realize freq */
    double phase; /* save phase value for next sample */
    double attack_factor;
    double decay_factor;
    double attack_amp; /* save attack amplitude for next sample */
    double decay_amp; /* save decay amplitude for next sample */
} Tone;

class Synth {
public:
    Synth(void); //constructor
    ~Synth(void); //destructor
    /* public member functions */
    void init_synth(int samp_rate);
    void execute_cmd(int cmd);
    double *synth_block(int len);
    void add_key(double freq);
    void rm_key(void);
    void init_key(double new_freq);
    bool invalid_key(int key);
    void shift_keys(void);

    /* public class data */
    atomic_int cmd; //command for synth
    double new_freq;//freq associated with new key
    int num_keys;   //number of keys voiced
    int num_chan;   //output channels
    int samp_rate;  //sampling rate of output
    Tone tone[KEYS_VOICED];
    double ampl;
    double attack_factor;
    double decay_factor;
    double drop_level;
    double output[FRAMES_PER_BUFFER];
#if (DB_WAV_OUT)
    SNDFILE *sndfile; //for debug file
#endif
};

#endif /* _SYNTH_H_ */