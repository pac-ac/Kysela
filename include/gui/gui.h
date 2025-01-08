#ifndef GUI_H
#define GUI_H

#include <unistd.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <math.h>

#include "../../include/audio/alsa.h"
#include "../../include/player.h"


bool WidgetClicked(int mouseX, int mouseY, Widget* widget);

bool ListClicked(int mouseX, int mouseY, Widget* widget, int w, int h);

int ListAction(int mouseX, int mouseY, Widget* widget, PlayerData* player_data);

void UpdateWidgetText(char* str, Widget* widget);

void ButtonAction(int mouseX, int mouseY, Widget* widget, AlsaData* alsa_data, PlayerData* player_data);

void GraphicalInput(char key, Widget* widget, uint8_t* strIndex, uint8_t maxIndex, char* str);

void KeyboardAction(char key, Widget* widget, AlsaData* alsa_data, PlayerData* player_data);

void PrintText(char* str, SDL_Renderer* renderer, SDL_Texture* font, 
		uint16_t textx, uint16_t texty);

void DrawWidget(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* font, Widget* widget, 
		PlayerData* player_data);

void Draw(SDL_Window* window, SDL_Renderer* renderer, 
	  SDL_Texture* background, SDL_Texture* font,
	  Widget* arr[256], uint8_t widgetNum,
	  char* filename,
	  PlayerData* player_data,
	  AlsaData* alsa_data);


#endif
