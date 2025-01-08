#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <SDL2/SDL.h>
#include <alsa/asoundlib.h>
#include <alsa/mixer.h>
#include <alsa/pcm.h>
#include <string.h>
#include <math.h>
#include <time.h>



#include "../include/types.h"
#include "../include/player.h"
#include "../include/audio/alsa.h"
#include "../include/gui/gui.h"


#define WAV_HEADER_LENGTH 44



void ProcessInput(AlsaData* alsa_data, PlayerData* player_data, Widget* arr[256], uint8_t widgetNum) {

	int mouseX, mouseY = 0;
	
	//stop if a widget was already clicked
	bool clicked = false;
	static bool mouse_down = false;

	while (SDL_PollEvent(&(player_data->event)) && player_data->quit == false) {

		//quit window
		player_data->quit = (player_data->event.type == SDL_QUIT);

		//mouse input
		if (player_data->event.type == SDL_MOUSEBUTTONDOWN 
			|| player_data->event.type == SDL_MOUSEBUTTONUP
			|| player_data->event.type == SDL_MOUSEMOTION
			|| player_data->event.type == SDL_MOUSEWHEEL) {
		
			SDL_GetMouseState(&mouseX, &mouseY);
			if (mouseX < 0 || mouseY < 0) { return; }

			for (int i = widgetNum-1; i >= 0; i--) {
			
				//check if contains coordinate
				if (WidgetClicked(mouseX, mouseY, arr[i])) {
				
					clicked = true;
					arr[i]->hover = true;
					player_data->widgetTarget = arr[i];
					
					switch (player_data->event.type) {
					
						//action
						case SDL_MOUSEBUTTONDOWN:
							
							mouse_down = true;
							ButtonAction(mouseX, mouseY, arr[i], alsa_data, player_data);
							break;

						//action if held down and slide
						case SDL_MOUSEMOTION:
							
							if (mouse_down == true && arr[i]->sliderType != 0) {
							
								ButtonAction(mouseX, mouseY, arr[i], alsa_data, player_data);
							}
							break;
						
						//mouse up
						case SDL_MOUSEBUTTONUP:
							mouse_down = false;
							break;
						default:break;
					}
				} else {
					arr[i]->hover = false;
					
					if (player_data->event.type == SDL_MOUSEBUTTONUP) {
					
						mouse_down = false;
					}
				}

				//check if list was clicked
				if (ListClicked(mouseX, mouseY, arr[i], player_data->w, player_data->h) && clicked == false) {
					
					switch (player_data->event.type) {
						
						//list clicked action
						case SDL_MOUSEBUTTONDOWN:
							ListAction(mouseX, mouseY, arr[i], player_data);
							break;
						//scroll
						case SDL_MOUSEWHEEL:
								
							if (arr[i]->list && arr[i]->actionType == 0 
									&& arr[i]->listOffset < arr[i]->listNum) {
						
								if ((player_data->event.wheel.y >= 0) ^ arr[i]->offsetY) {
								
									arr[i]->listOffset -= 1 * (arr[i]->listOffset > 0);
								} else {
									arr[i]->listOffset += 1 * (arr[i]->listOffset < arr[i]->listNum-1);
								}
							}
							break;
						default:break;
					}
				}
			}
		}

		//keyboard input
		if (player_data->widgetTarget != NULL) {
			
			switch (player_data->event.type) {
	
				//keyboard input to widget target
				case SDL_KEYDOWN:
					if (player_data->event.key.keysym.mod & (KMOD_SHIFT | KMOD_CAPS)) {
				
						KeyboardAction((char)(player_data->event.key.keysym.sym) - 32, 
								player_data->widgetTarget, alsa_data, player_data);
					} else {
						KeyboardAction((char)(player_data->event.key.keysym.sym), 
								player_data->widgetTarget, alsa_data, player_data);
					}
					break;	
				case SDL_KEYUP:
					break;	
				default:break;
			}
		}
	}
}



int main(int argc, char* argv[]) {

	//data for soundcard
	AlsaData* alsa_data = InitAlsaData();
	
	//program data and init
	PlayerData* player_data = InitPlayerData();


	//print version
	SDL_version ver;
	SDL_GetVersion(&ver);
	printf("Kysela v1.0\n");
	printf("Made with ALSA v%s and ", SND_LIB_VERSION_STR);
	printf("SDL v%d.", ver.major);
	printf("%d.\n", ver.minor);

	
	//create sdl window and renderer
	const int WIDTH = player_data->w;
	const int HEIGHT = player_data->h;
	
	SDL_Window* window = NULL;
	SDL_Renderer* renderer = NULL;
	SDL_Surface* bgimage = NULL;
	SDL_Texture* bgtexture = NULL;
	SDL_Surface* font = NULL;
	SDL_Texture* fonttexture = NULL;
	SDL_Surface* icon = NULL;
	

	//initialize window	
	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
	
		printf("ERROR: SDL couldn't be initialized. %s\n", SDL_GetError());
		return EXIT_FAILURE;
	} else {
		window = SDL_CreateWindow("Kysela", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, 
					  WIDTH, HEIGHT, SDL_WINDOW_RESIZABLE);
		if (window == NULL) {
		
			printf("ERROR: SDL window couldn't be created. %s\n", SDL_GetError());
			return EXIT_FAILURE;
		} else {
			//create renderer
			renderer = SDL_CreateRenderer(window, -1, 0);
		
			if (renderer == NULL) {

				printf("ERROR: SDL renderer couldn't be created. %s\n", SDL_GetError());
				return EXIT_FAILURE;
			}
			SDL_RenderSetScale(renderer, 1, 1);
			
			//set window icon
			icon = SDL_LoadBMP("icon.bmp");
			SDL_SetWindowIcon(window, icon);
			
			//do this for alpha color blending
			SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
		}
	}
	//we dont really need these anymore
	SDL_FreeSurface(bgimage);
	SDL_FreeSurface(font);

		
	//gui buttons and stuff
	//(fixed array for simplicity, prob won't need more than 256 anyway lol)
	Widget* widgetArr[256];
	uint8_t widgetNum = 0;
	for (int i = 0; i < 256; i++) { widgetArr[i] = NULL; }

	//load files and gui settings from config file
	if (argc > 1) { LoadFiles(argv[1], renderer, player_data, widgetArr, &widgetNum); }
	else { 		LoadFiles("kysela.config", renderer, player_data, widgetArr, &widgetNum); }
	SDL_SetWindowSize(window, player_data->w, player_data->h);	

	//ms delay from settings
	const int DELAY = player_data->delay;

	//copy list of files for searching
	for (int i = 0; i < player_data->fileCount; i++) {
		
		player_data->fileListCopy[i] = player_data->fileList[i];
		player_data->fileCountCopy++;
	}
			
	//load background image
	bgimage = SDL_LoadBMP(player_data->bgimage);
	bgtexture = SDL_CreateTextureFromSurface(renderer, bgimage);
			
	//text and font
	font = SDL_LoadBMP(player_data->font);
	fonttexture = SDL_CreateTextureFromSurface(renderer, font);

	//open pcm device
	if (OpenDevice(alsa_data)) { player_data->quit = true; }


	//put everything below here into a loop for playlists
	while (!(player_data->quit)) {

		//loop for when player_data.file isnt playing
		while (player_data->file == NULL) {
			
			//SDL_WindowFlags winInfo = SDL_GetWindowFlags(window);
			
			//draw window
			Draw(window, renderer, bgtexture, fonttexture, 
				widgetArr, widgetNum, 
				NULL, player_data, alsa_data);
			//user sdl input
			ProcessInput(alsa_data, player_data, widgetArr, widgetNum);
			if (player_data->quit) { break; }
			
			SDL_Delay(DELAY);
		}
		
		//read player_data.file
		alsa_data->readfd = open(player_data->file, O_RDONLY);

		if (alsa_data->readfd < 0) {

			if (player_data->quit == false) {
				
				printf("ERROR: The file \"%s\" couldn't be read or doesn't exist.\n", player_data->file);
				player_data->file = NULL;
			}
			break;
		}
		
		//prepare audio device to play player_data.file
		//default params WavPreparePCM(alsa_data, 44100, 2, 2);
		WavPreparePCM(alsa_data, player_data->rate, 2, 2);
	
		//get player_data.file size relative to buffer size
		FILE* ftype;
		ftype = fopen(player_data->file, "r");
		fseek(ftype, 0, SEEK_END);
		player_data->fileSize = ftell(ftype) / alsa_data->buff_size;
		fseek(ftype, 0, SEEK_SET);
		fclose(ftype);
	
		//update widget with text for current file name and size
		char sizeStr[20];
		sprintf(sizeStr, "%d", player_data->fileSize);

		for (int i = 0; i < widgetNum; i++) {

			switch (widgetArr[i]->actionType) {
			
				case 0:UpdateWidgetText(player_data->file, widgetArr[i]);break;
				case 2:UpdateWidgetText(sizeStr, widgetArr[i]);		 break;
				default:break;
			}
		}

		//unhide other buttons
		for (int i = 1; i < widgetNum; i++) { widgetArr[i]->active = true; }

		//update background
		UpdateBackgroundImage(renderer, player_data->file, player_data);


		
		//main loop for playing audio
		do {
			//playing the actual audio from player_data.file
			if (player_data->playing == true) {
			
				//this works?????
				//read data and control volume
				int16_t val = 0;

				for (int i = player_data->fileIndex == 0 ? WAV_HEADER_LENGTH : 0; 
						i < alsa_data->buff_size; i += 2) {
			
					alsa_data->readval = pread(alsa_data->readfd, &val, sizeof(int16_t), 
							(player_data->fileIndex*alsa_data->buff_size) + i);
				
					val *= player_data->volume;
				
					alsa_data->buff[i+1] = val >> 8;
					alsa_data->buff[i] = val & 0xff;
				}
	
				if (alsa_data->readval > 0) {

					//volume scaling
					//Volume(&player_data, alsa_data);

					//play actual sound
					WavPlayBuffer(alsa_data);
				
					//move offset for next buffer data in player_data.file
					player_data->fileIndex++;
				} else {
					//printf("An error occured while reading from the player_data.file. Exiting.\n");
					break;
				}
			}
			
			//SDL_WindowFlags winInfo = SDL_GetWindowFlags(window);
			
			//draw window
			Draw(window, renderer, bgtexture, fonttexture, 
					widgetArr, widgetNum, 
					player_data->file, player_data, alsa_data);
			//user sdl input
			ProcessInput(alsa_data, player_data, widgetArr, widgetNum);
			if (player_data->quit) { break; }

			SDL_Delay(DELAY);
		
		} while (alsa_data->readval > 0 && player_data->stop == false);


		//close current player_data.file
		close(alsa_data->readfd);
		player_data->fileIndex = 0;
		player_data->fileSize = 0;
		
		//file ended, go to next file in list
		//this check also prevents file from
		//changing when its not supposed to
		if (player_data->stop == false) {
		
			player_data->fileTarget++;
			
			if (player_data->fileTarget < player_data->fileCount) {
			
				player_data->file = player_data->fileList[player_data->fileTarget];
			} else { 
				player_data->file = NULL; 
			}
		}
		player_data->stop = false;
			
	//bottom of loop where program truly exits
	}
	
	//cleanup gui
	DestroyWidgets(widgetArr, &widgetNum);

	//destroy window
	SDL_DestroyTexture(bgtexture);
	SDL_DestroyTexture(fonttexture);
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	

	//clean up and close
	CloseDevice(alsa_data);
	DestroyAlsaData(alsa_data);
	printf("See you next time.\n");
	DestroyPlayerData(player_data);
	

	return 0;
}
