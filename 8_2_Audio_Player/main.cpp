#include "mbed.h"
#include <cmath>
#include "DA7212.h"

DA7212 audio;
int16_t waveform[kAudioTxBufferSize];
EventQueue queue(32 * EVENTS_EVENT_SIZE);
Thread t;

int song[42] = 
   {261, 261, 392, 392, 440, 440, 392,
    349, 349, 330, 330, 294, 294, 261,
    392, 392, 349, 349, 330, 330, 294,
    392, 392, 349, 349, 330, 330, 294,
    261, 261, 392, 392, 440, 440, 392,
    349, 349, 330, 330, 294, 294, 261};

int noteLength[42] = {
    1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 2,
    1, 1, 1, 1, 1, 1, 2};

void playNote(int freq)
{
  for (int i = 0; i < kAudioTxBufferSize; i++)
  {
    waveform[i] = (int16_t) (sin((double)i * 2. * M_PI/(double) (kAudioSampleFrequency / freq)) * ((1<<16) - 1));
  }
  // the loop below will play the note for the duration of 1s
  for(int j = 0; j < kAudioSampleFrequency / kAudioTxBufferSize; ++j)
  {
    audio.spk.play(waveform, kAudioTxBufferSize);
  }
}

int main(void)
{
    t.start(callback(&queue, &EventQueue::dispatch_forever));
    // this means there will be totally 42 notes
    for(int i = 0; i < 42; i++) {
        // the length for some specific notes
        int length = noteLength[i];
        // so if lengh = 2, the loop will run 2 times(each time play one second)
        while(length--) {
            queue.call(playNote, song[i]);
            // have to wait more since the calling time may take ?  
            if(length <= 1) 
                wait(1.0);
        }
    }
}