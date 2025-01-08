#include "../include/player.h"



//update background iamge by assigning
//new texture in imageList if the
//corresponding index in imageStr
//is found in the current file path
void UpdateBackgroundImage(SDL_Renderer* renderer, char* file, PlayerData* data) {
			
	if (data->background != NULL) { SDL_DestroyTexture(data->background); }

	for (int i = 0; i < data->imageNum; i++) {
	
		if (strstr(file, data->imageStr[i]) != NULL) {
				
			SDL_Surface* surface = SDL_LoadBMP(data->imageList[i]);
			data->background = SDL_CreateTextureFromSurface(renderer, surface);
			SDL_FreeSurface(surface);
			return;
		}
	}
	SDL_Surface* surface = SDL_LoadBMP(data->bgimage);
	data->background = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
}


void AddImageToPlayer(char* imagePathStr, char* imageStrMatch, PlayerData* data) {

	if (data->imageNum < 128) {

		data->imageList[data->imageNum] = imagePathStr;
		data->imageStr[data->imageNum] = imageStrMatch;
		data->imageNum++;
	}
}


int IdentifyConfigCommand(char* str) {
	
	char* mainStr = "$MAIN";
	char* buttonStr = "$BUTTON";
	char* imageStr = "$IMAGE";
	char* waveStr = "$WAVE";
	char* commandList[4] = { mainStr, buttonStr, imageStr, waveStr };

	for (int i = 0; i < 4; i++) {
	
		char* checkStr = commandList[i];

		int j = 0;
		for (j; checkStr[j] != '\0'; j++) {
			
			if (str[j] != checkStr[j]) { break; }
		}
		if (j == strlen(checkStr)) { return i; }
	}
	return -1;
}




int LoadFiles(char* config_path, SDL_Renderer* renderer, PlayerData* player_data, Widget* arr[256], uint8_t* widgetNum) {

	//file and dir paths
	FILE* file;
	char line[256];
	char* filePath = NULL;

	//user created widgets
	bool WidgetSpace = false;

	
	file = fopen(config_path, "r");
	
	if (file == NULL) { 
		
		printf("Config file '%s' could not be found or opened.\n", config_path);
		return EXIT_FAILURE; 
	}


	while (fgets(line, sizeof(line), file)) {

		//if not commented out
		if (line[0] != '#' && line[0] != '\n') {
		
			//go through line to check for shit
			struct stat buffer;
			int dirLen = strlen(line);
			line[dirLen-1] = '\0';

			//check for button/image/wave command
			int configCheck = IdentifyConfigCommand(line);
			int8_t configInitIndex = -1;
			char* arg = NULL;
			char* rest = line;
			

			//main config for window
			if (configCheck == 0) {
			
				while (arg = strtok_r(rest, ";", &rest)) {
				
					int val = atoi(arg);
					
					switch (configInitIndex) {
					
						case 0:player_data->w = val;break;
						case 1:player_data->h = val;break;
						case 2:
						       if (strcmp(arg, "null")) {
						       		
							       player_data->bgimage = malloc(sizeof(char)*strlen(arg));
							       strcpy(player_data->bgimage, arg);
						       }
						       break;
						case 3:
						       if (strcmp(arg, "null")) {
						       		
							       player_data->font = malloc(sizeof(char)*strlen(arg));
							       strcpy(player_data->font, arg);
						       }
						case 4:player_data->delay = val;break;
						default:break;
					}
					configInitIndex++;
				}

			//create custom widget
			} else if (configCheck == 1) {
		
				uint8_t actionType = 0; uint8_t sliderType = 0;
				uint16_t x = 0; uint16_t y = 0;
				uint16_t w = 0; uint16_t h = 0;
				bool xoffset = false; bool yoffset = false;
				bool woffset = false; bool hoffset = false;
				bool wstretch = false;bool hstretch = false;
				uint8_t xdivide = 0; uint8_t ydivide = 0;
				uint8_t wdivide = 0; uint8_t hdivide = 0;
				uint32_t color = 0; uint32_t color2 = 0; float shade = 0;
				char* widgetImage = NULL; char* widgetImage2 = NULL;
				char* text = NULL;
				uint16_t textx = 0; uint16_t texty = 0;

				
				while (arg = strtok_r(rest, ";", &rest)) {
	
					int val = atoi(arg);

					switch (configInitIndex) {
					
						case 0:actionType = val;break;
						case 1:sliderType = val;break;
						case 2:x = val;break;
						case 3:y = val;break;
						case 4:w = val;break;
						case 5:h = val;break;
						case 6:color = val;break;
						case 7:color2 = val;break;
						case 8:shade = ((float)val)/100.0;break;
						case 9:
						       if (strcmp(arg, "null")) {

							       widgetImage = malloc(sizeof(char)*strlen(arg));
							       strcpy(widgetImage, arg);
						       }
						       break;
						case 10:
						       if (strcmp(arg, "null")) {
						       		
							       widgetImage2 = malloc(sizeof(char)*strlen(arg));
							       strcpy(widgetImage2, arg);
						       }
						       break;
						
						case 11:wstretch = (bool)(val);break;
						case 12:hstretch = (bool)(val);break;
						case 13:xoffset = (bool)(val);break;
						case 14:yoffset = (bool)(val);break;
						case 15:woffset = (bool)(val);break;
						case 16:hoffset = (bool)(val);break;
						case 17:xdivide = val;break;
						case 18:ydivide = val;break;
						case 19:wdivide = val;break;
						case 20:hdivide = val;break;
						
						case 21:
						       if (strcmp(arg, "null")) {
						       		
							       text = malloc(sizeof(char)*strlen(arg));
							       strcpy(text, arg);
						       }
						       break;
						case 22:textx = val;break;
						case 23:texty = val;break;
						default:break;
					}
					configInitIndex++;
				}

				//if all args passed, create custom widget	
				if (configInitIndex >= 23) {
			
					CreateWidget(x, y, w, h, color, color2, shade, true, 
							wstretch, hstretch, 
							xoffset, yoffset, woffset, hoffset,
							xdivide, ydivide, wdivide, hdivide,
							text, textx, texty, 
							widgetImage, widgetImage2,
							actionType, sliderType, 
							renderer, arr, widgetNum);
				}

			//add custom image
			} else if (configCheck == 2) {
					
				char* image = NULL;
				char* strMatch = NULL;
				
				while (arg = strtok_r(rest, ";", &rest)) {
				
					switch (configInitIndex) {
					
						case 0:
							if (strcmp(arg, "null")) {

								image = malloc(sizeof(char)*strlen(arg));
								strcpy(image, arg);
							}
							break;
						case 1:
							if (strcmp(arg, "null")) {
							
								strMatch = malloc(sizeof(char)*strlen(arg));
								strcpy(strMatch, arg);
							}
							break;
						default:break;
					}
					configInitIndex++;
					if (configInitIndex >= 128) { break; }
				}
				AddImageToPlayer(image, strMatch, player_data);

			//wave config
			} else if (configCheck == 3) {

				while (arg = strtok_r(rest, ";", &rest)) {
	
					int val = atoi(arg);

					switch (configInitIndex) {
					
						case 0:player_data->r = val;break;
						case 1:player_data->g = val;break;
						case 2:player_data->b = val;break;
						case 3:player_data->waveVert = (bool)val;break;
						case 4:player_data->waveFade = (bool)val;break;
						case 5:player_data->waveAmp = val;break;
						case 6:player_data->waveFreq = val;break;
						default:break;
					}
					configInitIndex++;
				}	

			//if file/directory exists
			} else if (stat(line, &buffer) == 0) {

				//check for directory
				DIR* dir;
				struct dirent* ent;

				if ((dir = opendir(line)) != NULL) {
				
					//read files in directory
					while ((ent = readdir(dir)) != NULL) {
					
						int fileLen = strlen(ent->d_name);
						
						//allocate memory and copy directory with concat file
						char path[dirLen+fileLen];
						strcpy(path, line);
						strcat(path, ent->d_name);

						//add to file list of player
						player_data->fileList[player_data->fileCount] = (char*)malloc(sizeof(char)*(fileLen+dirLen));
						strcpy(player_data->fileList[player_data->fileCount], path);
						player_data->fileCount++;

						if (player_data->fileCount >= 1024) { break; }
					}
					closedir(dir);

				//add as an individual file
				} else {
					//allocate memory and copy string
					player_data->fileList[player_data->fileCount] = (char*)malloc(sizeof(char)*dirLen);
					strcpy(player_data->fileList[player_data->fileCount], line);
					player_data->fileCount++;

					if (player_data->fileCount >= 1024) { break; }
				}
			} else  {
				printf("Couldn't read file or directory '%s'.\n", line);
			}
		}
	}
	fclose(file);


	//widget inits
	//(put this in own function sometime)
	for (int i = 0; i < *widgetNum; i++) {
	
		switch (arr[i]->actionType) {
		
			case 0:
				arr[i]->listNum = player_data->fileCount;
				if (arr[i]->offsetY) {
		
					arr[i]->listOffset = arr[i]->listNum-1;
				}
				break;
			case 3:
				arr[i]->slide = 1.0;
				break;
			
			default:break;
		}

	}
	return 0;
}
