#include "../../include/audio/alsa.h"


int OpenDevice(AlsaData* data) {
	
	const static char* device = "default";
	snd_output_t* output = NULL;

	int err;

	if ((err = snd_pcm_open(&(data->pcm_handle), device, SND_PCM_STREAM_PLAYBACK, 0)) < 0) {
	
		fprintf(stderr, "Playback open error: %s\n", snd_strerror(err));
		return EXIT_FAILURE;
	}

	if ((err = snd_pcm_set_params(data->pcm_handle, 
		data->format, SND_PCM_ACCESS_RW_INTERLEAVED, 
		data->channels, data->rate, 1, 500000)) < 0) {

		fprintf(stderr, "Playback open error: %s\n", snd_strerror(err));
		return EXIT_FAILURE;
	}
}

void CloseDevice(AlsaData* data) {

	snd_pcm_drain(data->pcm_handle);
	snd_pcm_close(data->pcm_handle);
}


void SetAlsaMasterVolume(AlsaData* data, long int volume) {

	long int min, max;
	snd_mixer_t* handle;
	snd_mixer_selem_id_t* sid;

	const char* card = "default";
	const char* selem_name = "Master";

	snd_mixer_open(&handle, 0);
	snd_mixer_attach(handle, card);
	snd_mixer_selem_register(handle, NULL, NULL);
	snd_mixer_load(handle);

	snd_mixer_selem_id_alloca(&sid);
	snd_mixer_selem_id_set_index(sid, 0);
	snd_mixer_selem_id_set_name(sid, selem_name);
	snd_mixer_elem_t* elem = snd_mixer_find_selem(handle, sid);

	snd_mixer_selem_get_playback_volume_range(elem, &min, &max);
	snd_mixer_selem_set_playback_volume_all(elem, volume*max/100);

	snd_mixer_close(handle);
}



//rate = sample rate (44100) 
//channels 1 = mono, 2 = stero
//format signed 8 bit = 0, unsigned 8 bit = 1
//	signed 16 bit = 2, signed 16 bit LE = 4
void WavPreparePCM(AlsaData* data, int rate, int channels, int format) {


	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&(data->params));
	snd_pcm_hw_params_any(data->pcm_handle, data->params);

	/* Set parameters */
	if (data->pcm = snd_pcm_hw_params_set_access(data->pcm_handle, data->params, SND_PCM_ACCESS_RW_INTERLEAVED) < 0) { 
		
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(data->pcm));
	}

	if (data->pcm = snd_pcm_hw_params_set_format(data->pcm_handle, data->params, format) < 0) {

		printf("ERROR: Can't set format. %s\n", snd_strerror(data->pcm));
	}

	if (data->pcm = snd_pcm_hw_params_set_channels(data->pcm_handle, data->params, channels) < 0) {

		printf("ERROR: Can't set channels number. %s\n", snd_strerror(data->pcm));
	}

	if (data->pcm = snd_pcm_hw_params_set_rate_near(data->pcm_handle, data->params, &rate, 0) < 0) {

		printf("ERROR: Can't set rate. %s\n", snd_strerror(data->pcm));
	}

	/* Write parameters */
	if (data->pcm = snd_pcm_hw_params(data->pcm_handle, data->params) < 0) {
		
		printf("ERROR: Can't set hardware parameters. %s\n", snd_strerror(data->pcm));
	}

	/* Resume information */
	snd_pcm_hw_params_get_channels(data->params, &(data->tmp));
	snd_pcm_hw_params_get_rate(data->params, &(data->tmp), 0);

	//allocate buffer to hold single period
	snd_pcm_hw_params_get_period_size(data->params, &(data->frames), 0);

	//sample size
	data->buff_size = (data->frames)*channels*2;
	data->buff = (char*)(malloc(data->buff_size));
	memset(data->buff, 0, data->buff_size);

	snd_pcm_hw_params_get_period_time(data->params, &(data->tmp), NULL);
}



void WavPlayBuffer(AlsaData* data) {

	//play from sound buffer
	data->pcm = snd_pcm_writei(data->pcm_handle, data->buff, data->frames);

	//check xruns and errors
	if (data->pcm == -EPIPE) {
	
		//fprintf(stderr, "Underrun!\n");
		snd_pcm_prepare(data->pcm_handle);
	
	} else if (data->pcm < 0) {
	
		fprintf(stderr, "Error writing to PCM device: %s\n", snd_strerror(data->pcm));
	}

	//wait until soundcard ready for next buffer
	do { 
		snd_pcm_delay(data->pcm_handle, &(data->g_frames));
		SDL_Delay(1);

	} while (data->g_frames > data->buff_size);
}
