#ifndef ALSA_H
#define ALSA_H

#include <stdio.h>
#include <stdlib.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <alsa/pcm.h>
#include <math.h>

#include "../../include/types.h"



//open/close devices
int OpenDevice(AlsaData* data);
void CloseDevice(AlsaData* data);


//various hw functions to prepare/play/modify sound
void SetAlsaMasterVolume(AlsaData* data, long int volume);

void WavPreparePCM(AlsaData* alsa_data, int rate, int channels, int format);

void WavPlayBuffer(AlsaData* alsa_data);

#endif
