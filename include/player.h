#ifndef PLAYER_H
#define PLAYER_H

#include <unistd.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <dirent.h>
#include <math.h>

#include "../include/types.h"
#include "../include/audio/alsa.h"


void Volume(PlayerData* player_data, AlsaData* alsa_data);


void UpdateBackgroundImage(SDL_Renderer* renderer, char* file, PlayerData* data);

void AddImageToPlayer(char* imagePathStr, char* imageStrMatch, PlayerData* player_data);

int IdentifyConfigCommand(char* str);

int LoadFiles(char* config_path, SDL_Renderer* renderer, PlayerData* player_data, Widget* widgetArr[256], uint8_t* widgetNum);

#endif
