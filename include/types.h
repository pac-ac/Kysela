#ifndef TYPES_H
#define TYPES_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <alsa/pcm.h>


//define structs and functions for creating/destroying


typedef struct {

	//dimensions
	uint16_t x;
	uint16_t y;
	uint16_t w;
	uint16_t h;
	float dividex;
	float dividey;
	float dividew;
	float divideh;
	
	uint16_t savex;
	uint16_t savey;
	uint16_t savew;
	uint16_t saveh;

	//color
	uint8_t r;
	uint8_t g;
	uint8_t b;
	uint8_t a;
	uint8_t r2;
	uint8_t g2;
	uint8_t b2;
	uint8_t a2;
	float shade;

	//image if provided
	SDL_Texture* texture;
	SDL_Texture* texture2;
	int texture2_w;
	int texture2_h;

	//what happens when you
	//click on button
	uint8_t actionType;
	uint8_t sliderType;
	float slide;

	//text
	char* text;
	uint16_t textx;
	uint16_t texty;

	//flags n shit
	bool active;
	bool hover;
	bool stretchWidth;
	bool stretchHeight;
	bool offsetX;
	bool offsetY;
	bool offsetW;
	bool offsetH;

	//stuff for lists/menus
	bool list;
	uint16_t listNum;
	uint16_t listOffset;

} Widget;

typedef struct {

	char* PCM_DEVICE;
	snd_pcm_t* pcm_handle;
	snd_pcm_sframes_t g_frames;
	snd_pcm_format_t format;
	int rate;
	int channels;

	unsigned int pcm, tmp, dir;
	snd_pcm_hw_params_t* params;
	snd_pcm_uframes_t frames;
	int8_t* buff;
	int buff_size;
	int readfd;
	int readval;
	int rms;

} AlsaData;

typedef struct {

	SDL_Event event;

	//player
	char* file;
	bool quit;
	bool stop;
	bool playing;
	uint16_t delay;
	SDL_Texture* background;
	char* font;
	char* bgimage;

	//files
	uint32_t fileIndex;
	uint32_t fileSize;
	uint32_t fileCount;
	uint32_t fileTarget;
	char* fileList[1024];
	uint32_t fileCountCopy;
	char* fileListCopy[1024];
	char fileSearch[64];
	uint8_t fileSearchIndex;
	
	//images	
	uint16_t imageNum;
	char* imageList[128];
	char* imageStr[128];

	//input
	uint8_t key;
	int x;
	int y;
	Widget* widgetTarget;

	//graphical stuff
	uint16_t w;
	uint16_t h;
	uint8_t waveFreq;
	uint8_t waveAmp;
	bool waveVert;
	bool waveFade;
	uint8_t r;
	uint8_t g;
	uint8_t b;

	//audio
	float volume;
	uint32_t rate;
	uint8_t rateInputIndex;
	char rateStr[6];

} PlayerData;



void CreateWidget(uint16_t x, uint16_t y, 
		uint16_t w, uint16_t h,
		uint32_t color, uint32_t color2, float shade,
		bool active, bool stretchWidth, bool stretchHeight,
		bool offsetX, bool offsetY, bool offsetW, bool offsetH,
		uint8_t dividex, uint8_t dividey, uint8_t dividew, uint8_t divideh,
		char* text, uint16_t textx, uint16_t texty,
		char* image, char* image2,
		uint8_t actionType, uint8_t sliderType,
		SDL_Renderer* renderer,
		Widget* arr[256], uint8_t* widgetNum);
void DestroyWidget(uint8_t deleteIndex, Widget* arr[256], uint8_t* widgetNum);
void DestroyWidgets(Widget* arr[256], uint8_t* widgetNum);


AlsaData* InitAlsaData();
void DestroyAlsaData(AlsaData* data);


PlayerData* InitPlayerData();
void DestroyPlayerData(PlayerData* data);



#endif
