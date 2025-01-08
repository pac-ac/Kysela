#include "../include/types.h"

void CreateWidget(uint16_t x, uint16_t y, 
		 uint16_t w, uint16_t h, 
		 uint32_t color, uint32_t color2, float shade,
		 bool active,
		 bool stretchWidth, bool stretchHeight,
		 bool offsetX, bool offsetY,
		 bool offsetW, bool offsetH,
		 uint8_t dividex, uint8_t dividey,
		 uint8_t dividew, uint8_t divideh,
		 char* text, uint16_t textx, uint16_t texty,
		 char* image, char* image2, 
		 uint8_t actionType, uint8_t sliderType,
		 SDL_Renderer* renderer,
		 Widget* arr[256], uint8_t* widgetNum) {

	//create widget
	Widget* widget = malloc(sizeof(Widget));
	widget->x = x;
	widget->y = y;
	widget->w = w;
	widget->h = h;

	widget->dividex = (float)dividex/100.0;
	widget->dividey = (float)dividey/100.0;
	widget->dividew = (float)dividew/100.0;
	widget->divideh = (float)divideh/100.0;

	widget->savex = x;
	widget->savey = y;
	widget->savew = w;
	widget->saveh = h;

	//select color
	widget->r =  (color >> 16);
	widget->g = ((color >> 8) & 0xff);
	widget->b = (color        & 0xff);
	widget->a = 0 + (255 * (color > 0));
	widget->r2 =  (color2 >> 16);
	widget->g2 = ((color2 >> 8) & 0xff);
	widget->b2 = (color2        & 0xff);
	widget->a2 = 0 + (255 * (color2 > 0));
	
	widget->shade = shade;

	if (image != NULL) {

		SDL_Surface* surface = SDL_LoadBMP(image);	
		widget->texture = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);
	} else {
		widget->texture = NULL;
	}
	free(image);

	
	if (image2 != NULL) {

		SDL_Surface* surface = SDL_LoadBMP(image2);	
		widget->texture2 = SDL_CreateTextureFromSurface(renderer, surface);
		SDL_FreeSurface(surface);

		SDL_QueryTexture(widget->texture2, NULL, NULL, &(widget->texture2_w), &(widget->texture2_h));
	} else {
		widget->texture2 = NULL;
		widget->texture2_w = 0;
		widget->texture2_h = 0;
	}
	free(image2);

	
	widget->text = text;
	widget->textx = textx;
	widget->texty = texty;
	widget->actionType = actionType;
	widget->sliderType = sliderType;
	widget->slide = 0.0;

	widget->active = active;
	widget->hover = false;

	//set drawing options
	widget->stretchWidth = stretchWidth;
	widget->stretchHeight = stretchHeight;
	widget->offsetX = offsetX;
	widget->offsetY = offsetY;
	widget->offsetW = offsetW;
	widget->offsetH = offsetH;

	widget->list = false;
	widget->listNum = 0;
	widget->listOffset = 0;

	//add to array
	arr[*widgetNum] = widget;
	*widgetNum += 1;
}


void DestroyWidget(uint8_t deleteIndex, Widget* arr[256], uint8_t* widgetNum) {

	Widget* widget = arr[deleteIndex];
	
	if (widget->text != NULL) { free(widget->text); }
	if (widget->texture != NULL) { SDL_DestroyTexture(widget->texture); }
	if (widget->texture2 != NULL) { SDL_DestroyTexture(widget->texture2); }

	arr[deleteIndex] = NULL;
	free(widget);

	for (int i = deleteIndex; i < (*widgetNum)-1; i++) {
	
		arr[i] = arr[i+1];
	}
	*widgetNum -= 1;
}


void DestroyWidgets(Widget* arr[256], uint8_t* widgetNum) {

	for (int i = 0; i < *widgetNum; i++) {
	
		if (arr[i] != NULL) {
		
			if (arr[i]->text != NULL) { free(arr[i]->text); }
			if (arr[i]->texture != NULL) { SDL_DestroyTexture(arr[i]->texture); }
			if (arr[i]->texture2 != NULL) { SDL_DestroyTexture(arr[i]->texture2); }
			free(arr[i]);
		}
	}
}


AlsaData* InitAlsaData() {

	AlsaData* data = malloc(sizeof(AlsaData));

	data->PCM_DEVICE = "default";
	data->rate = 44100;
	data->channels = 1;
	data->format = SND_PCM_FORMAT_FLOAT;

	data->buff = NULL;
	data->buff_size = 1;
	data->readfd = 0;
	data->readval = 0;
	
	data->rms = 0;

	return data;
}

void DestroyAlsaData(AlsaData* data) {

	free(data);
}


PlayerData* InitPlayerData() {

	PlayerData* player = malloc(sizeof(PlayerData));
	
	player->w = 640;
	player->h = 360;
	player->quit = false;
	player->stop = false;
	player->playing = true;
	player->delay = 7;
	player->font = NULL;
	player->bgimage = NULL;
	player->background = NULL;


	player->fileIndex = 0;
	player->fileSize = 0;
	player->fileCount = 0;
	player->fileTarget = 0;
	player->fileSearchIndex = 0;
	player->fileCountCopy = 0;

	player->imageNum = 0;

	player->key = 0;
	player->x = 0;
	player->y = 0;

	player->widgetTarget = NULL;
	player->waveFreq = 1;
	player->waveAmp = 1;
	player->waveVert = false;
	player->waveFade = true;

	player->volume = 1.0;
	player->rate = 44100;
	player->rateInputIndex = 0;

	for (int i = 0; i < 1024; i++) { player->fileList[i] = NULL; }
	for (int i = 0; i < 1024; i++) { player->fileListCopy[i] = NULL; }
	for (int i = 0; i < 64; i++) { player->fileSearch[i] = '\0'; }
	for (int i = 0; i < 6; i++) { player->rateStr[i] = '\0'; }
	
	for (int i = 0; i < 128; i++) { player->imageList[i] = NULL; }
	for (int i = 0; i < 128; i++) { player->imageStr[i] = NULL; }


	return player;
}

void DestroyPlayerData(PlayerData* data) {

	for (int i = 0; i < 1024; i++) { 
		
		if (data->fileList[i] != NULL) { free(data->fileList[i]); }
	}


	for (int i = 0; i < 128; i++) {
		
		if (data->imageList[i] != NULL) { free(data->imageList[i]); }
		if (data->imageStr[i] != NULL) { free(data->imageStr[i]); }
	}

	if (data->font != NULL) { free(data->font); }
	if (data->bgimage != NULL) { free(data->bgimage); }
	if (data->background != NULL) { SDL_DestroyTexture(data->background); }

	free(data);
}
