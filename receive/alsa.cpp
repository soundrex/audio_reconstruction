#include <iostream>
#include "alsa.h"

using namespace std;

atomic<int> buf_size(0);
snd_pcm_t *handle;

void thread_fn(chrono::time_point<chrono::steady_clock> time) {
	while (true) {
		this_thread::sleep_until(time += duration);
		if (buf_size > 0) {
			buf_size--;
		} else {
			packet_result_t({packet_result_type::reader_waiting}).log();
		}
	}
}

void write_samples(void const *samples, size_t len) {
	snd_pcm_sframes_t frames = snd_pcm_writei(handle, samples, len);
	if (frames < 0)
		frames = snd_pcm_recover(handle, frames, 0);

	if (frames < 0) {
		printf("snd_pcm_writei failed: %s\n", snd_strerror(frames));
	} else if (frames < len) {
		printf("Short write (expected %li, wrote %li)\n", len, frames);
	}
}

void init_pcm() {
	int err;
	if ((err = snd_pcm_open(&handle, device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
		cerr << "Playback open error: " << snd_strerror(err) << endl;
		exit(1);
	}

	if ((err = snd_pcm_set_params(handle,
		                      SND_PCM_FORMAT_S16_LE,
		                      SND_PCM_ACCESS_RW_INTERLEAVED,
		                      2,
		                      48000,
		                      1,
		                      20000)) < 0) {   /* 20ms */
		cerr << "Parameter setting error: " << snd_strerror(err) << endl;
		exit(1);
	}

	thread(thread_fn, chrono::steady_clock::now());
}

