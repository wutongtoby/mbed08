#include "mbed.h"
#include <cmath>
#include "DA7212.h"

int idC, i;
Thread t;
DA7212 audio;
// the build in class for playing music
InterruptIn button(SW2);
int16_t waveform[kAudioTxBufferSize]; 
// kAudioTxBufferSize is a macro
// the waveform array store the data we want to give to DAC in one period
// and in this int16 1 is seem as 1/2^16-1 for DAC
// so int16 max equal to 1, int16 min equal to -1(approxiamate)
EventQueue queue(32 * EVENTS_EVENT_SIZE);


// the function will first get the output table
// then it will play the sound
void playNote(int freq)
{
    // since the data will be multiplied by 2^16(in DAC), we first divided it by 2^16
    // waveform[i] = sin(2 * pi *  i *  (freq / kAudioSampleFrequency))     /      ((1<<16) - 1)
    for (i = 0; i < kAudioTxBufferSize; i++)
        waveform[i] = (int16_t) (sin((double)i * 2. * M_PI / (double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));

    // the loop below will play the note for the duration of 1s
    for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
        audio.spk.play(waveform, kAudioTxBufferSize);
}

// since the playnote will only play it for 1 second,
// we call it every 1 second to get continunous sound
void playNoteC(void) {idC = queue.call_every(1, playNote, 261);}
void stopPlayNoteC(void) {
    queue.cancel(idC);
    printf("yes\n");
}

int main(void)
{
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    button.fall(queue.event(playNoteC));
    button.rise(queue.event(stopPlayNoteC));
}
