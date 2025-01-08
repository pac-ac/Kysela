#include "../../include/gui/gui.h"


void DrawWidget(SDL_Window* window, SDL_Renderer* renderer, SDL_Texture* font, 
		Widget* widget, PlayerData* player_data) {

	//set color, lower opacity if mouse is hovering
	if (widget->hover && widget->actionType != 255) {
		
		SDL_SetRenderDrawColor(renderer, widget->r, widget->g, widget->b, widget->a/2);
	} else {
		SDL_SetRenderDrawColor(renderer, widget->r, widget->g, widget->b, widget->a); 
	}

	//get screen dimensions
	int stretchW, stretchH = 0;
	SDL_GetWindowSize(window, &stretchW, &stretchH);

		
	//offset from edges of window
	if (widget->offsetX) { 
		
		widget->x = stretchW - widget->savex;
	       	if (widget->dividex > 0.0) { widget->x = stretchW * widget->dividex; }
	}
	if (widget->offsetY) { 
		
		widget->y = stretchH - widget->savey; 
	       	if (widget->dividey > 0.0) { widget->y = stretchH * widget->dividey; }
	}


	//stretch to specified dimensions
	if (widget->stretchWidth) { 
		
		widget->w = stretchW - widget->x; 
		if (widget->dividew > 0.0) { widget->w = (stretchW * widget->dividew) - widget->savex; }
	} else {
		if (widget->offsetW) { widget->w = stretchW - (widget->x + widget->savew); }
	}
	
	if (widget->stretchHeight) { 
		
		widget->h = stretchH - widget->y; 
		if (widget->divideh > 0.0) { widget->h = (stretchH * widget->divideh) - widget->savey; }
	} else {
		if (widget->offsetH) { widget->h = stretchH - (widget->y + widget->saveh); }
	}
	
	//create drawing rectangle	
	SDL_Rect rect;
	rect.x = widget->x;
	rect.y = widget->y;
	rect.w = widget->w;
	rect.h = widget->h;
	

	//draw color/image
	if (widget->texture != NULL) {
	
		//draw full texture using
		SDL_RenderCopy(renderer, widget->texture, NULL, &rect);
	} else {
		//draw rectangle with given color, finally
		SDL_RenderFillRect(renderer, &rect);
	}


	//add gradient
	if (widget->shade != 0.0) {
		
		SDL_Rect shadeRect;
		shadeRect.x = rect.x;
		shadeRect.y = rect.y;
		shadeRect.w = 2;
		shadeRect.h = rect.h;
		
		uint16_t shadeW = widget->w*widget->shade;

		if (widget->shade > 0.0) {
		
			for (int i = 0; i < shadeW; i += 2) {
		
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255-(int)(255*((float)i/(float)shadeW)));
				SDL_RenderFillRect(renderer, &shadeRect);
				shadeRect.x += 2;
			}
		} else {
			shadeRect.x = (rect.x+widget->w)-2;
			for (int i = 0; i < shadeW; i += 2) {

				SDL_SetRenderDrawColor(renderer, 0, 0, 0, (int)(255*((float)i/(float)shadeW)));
				SDL_RenderFillRect(renderer, &shadeRect);
				shadeRect.x -= 2;
			}
		}
	}


	//for animations/update every frame
	switch (widget->actionType) {
	
		//list files
		case 0:
			if (widget->list) {
			
				//draw rectangle to contain list
				rect.y += rect.h;
				rect.h = stretchH;
				if (widget->offsetY) { rect.y = 0;		     rect.h = widget->y; 
				} else {	       rect.y = widget->y+widget->h; rect.h = stretchH; }
				SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
				SDL_RenderFillRect(renderer, &rect);
							
				if (widget->offsetY) { rect.y = widget->y-26;
				} else {	       rect.y = widget->y+widget->h; }
				rect.h = 26;
				
				//print files
				for (int i = widget->listOffset; i < player_data->fileCountCopy; i++) {
				
					//highlight currently selected file
					if (i == player_data->fileTarget) {
					
						SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
						SDL_RenderFillRect(renderer, &rect);
					}
					PrintText(player_data->fileListCopy[i], renderer, font, widget->x+widget->textx, rect.y);
				
					if (widget->offsetY) { rect.y -= 26;
					} else {	       rect.y += 26; }
				}
			}
			break;

		//file progress
		case 2:
			widget->slide = (float)((double)player_data->fileIndex / (double)player_data->fileSize);
			break;
	}

	//draw slide
	if (widget->sliderType) {
			
		switch (widget->sliderType) {
		
			//horizontal slide
			case 1:
			case 3:
				rect.w = (int)(widget->slide*((float)widget->w));
				break;
			//vertical slide
			case 2:
			case 4:
				rect.h = (int)(widget->slide*((float)widget->h));
				rect.y += (widget->h - rect.h);
				break;
			//2d slide
			case 5:
				rect.x = (int)(widget->slide*((float)widget->w));
				rect.y = (int)(widget->slide*((float)widget->h));
				rect.w = 10;
				rect.h = 10;
				break;
			default:break;
		}
		
		//use second image for slide, 
		//or draw new rectangle
		if (widget->texture2 != NULL) {
				
			SDL_Rect slideRect = rect;

			switch (widget->sliderType) {
			
				case 3:
					slideRect.x = (rect.x+rect.w) - widget->texture2_w/2;
					slideRect.w = widget->texture2_w;
					SDL_RenderCopy(renderer, widget->texture2, NULL, &slideRect);
					break;
				case 4:
					slideRect.y = (rect.y) - widget->texture2_h/2;
					slideRect.h = widget->texture2_h;
					SDL_RenderCopy(renderer, widget->texture2, NULL, &slideRect);
					break;
				default:
					SDL_RenderCopy(renderer, widget->texture2, &rect, &rect);
					break;
			}
		} else {
			SDL_SetRenderDrawColor(renderer, widget->r2, widget->g2, widget->b2, widget->a2);
			SDL_RenderFillRect(renderer, &rect);
		}
	}
	//draw text last
	PrintText(widget->text, renderer, font, widget->x+widget->textx, widget->y+widget->texty);
}


bool WidgetClicked(int mouseX, int mouseY, Widget* widget) {

	if (mouseX > widget->x && 
		mouseY > widget->y &&
		mouseX < (widget->x+widget->w) && 
		mouseY < (widget->y+widget->h)) {

		return true;
	}
	
	return false;
}

bool ListClicked(int mouseX, int mouseY, Widget* widget, int w, int h) {
	
	
	if (widget->list && 
		mouseX > widget->x && mouseX < (widget->x+widget->w)) {

		if (widget->offsetY == false && 
				mouseY > (widget->y+widget->h) && 
	    			mouseY < ((widget->y+widget->listNum-1)*26)) {
	
				return true;
		}
			
		if (widget->offsetY == true && 
				mouseY < widget->y &&
		   		mouseY > 0) {
		   		//mouseY > h-(widget->h+(widget->listNum*26))) {
	
			return true;
		}
	}
	return false;
}

int ListAction(int mouseX, int mouseY, Widget* widget, PlayerData* player_data) {

	int listSelect = 0;

	//calculate which object in list was selected
	if (widget->offsetY) { listSelect = widget->listOffset-((mouseY-widget->y)/26);
	} else {	       listSelect = ((mouseY-(widget->y+widget->h))/26)+widget->listOffset; }

	switch (widget->actionType) {

		//select file for playback
		case 0:
			//select file for playback
			if (listSelect == player_data->fileTarget) {
		
				//include check if file actually exists or is readable later
				player_data->file = player_data->fileListCopy[listSelect];
			
				//interrupt current file and play the next
				player_data->stop = true;
			} else {
		
				//select file to highlight,
				//if clicked again, its played
				if (listSelect <= player_data->fileCountCopy) {
			
					player_data->fileTarget = listSelect;
				}
			}
		break;
		default:break;
	}
	return player_data->fileTarget;
}


void ButtonAction(int mouseX, int mouseY,
		Widget* widget,	
		AlsaData* alsa_data,
		PlayerData* player_data) {

	
	//determine slider level if there	
	switch (widget->sliderType) {
		
		case 1:
		case 3:
			widget->slide = ((float)(mouseX-widget->x))/((float)widget->w);
			break;
		case 2:
		case 4:
			widget->slide = 1.0 - ((float)(mouseY-widget->y))/((float)widget->h);
			break;
		default:break;
	}

	//what happens when a user clicks on button?
	switch (widget->actionType) {
	
		//open files
		case 0:
			widget->list ^= 1;
			break;

		//pause/play audio
		case 1:
			player_data->playing ^= 1;

			if (player_data->playing) { UpdateWidgetText("Pause ||", widget);
			} else { 		    UpdateWidgetText("Play |>", widget); }

			break;

		//navigate audio
		case 2:
			player_data->fileIndex = (uint32_t)((float)player_data->fileSize * widget->slide);
			break;

		//volume slider
		case 3:
			//y = 0.01*e^(x*4.615) is a nice curve, 
			//subtracting 0.01 makes no slide 0, and full slide very close to 1
			player_data->volume = (0.01 * exp(widget->slide * 4.615)) - 0.01;
			
			//slider is close enough, make it full vol
			if ((1.0 - player_data->volume) < 0.0005) {
			
				player_data->volume = 1.0;
			}

			//slider is close enough, make it mute
			if ((1.0 - player_data->volume) > 0.9995) {
			
				player_data->volume = 0.0;
			}
			break;

		//file search
		case 4:
			for (int i = 0; i < 64; i++) { player_data->fileSearch[i] = '\0'; }
			player_data->fileSearchIndex = 0;
			UpdateWidgetText("File Search", widget);
			break;

		//change audio rate
		case 5:
			for (int i = 0; i < 6; i++) { player_data->rateStr[i] = '\0'; }
			player_data->rateInputIndex = 0;
			player_data->rate = 44100;
			UpdateWidgetText("44100", widget);
			break;
		
		//nonbutton for drawing images
		case 255:break;
		
		default:break;
	}
}


void GraphicalInput(char key, Widget* widget, uint8_t* strIndex, uint8_t maxIndex, char* str) {

	switch (key) {
		
		case '\b':
			if (*strIndex > 0) {
				
				*strIndex -= 1;
				str[*strIndex] = '\0';
			}
			break;
		default:
			if (key >= 32 && key <= 126 && *strIndex < maxIndex) {
				
				str[*strIndex] = key;
				*strIndex += 1;
			}
			break;
	}
	UpdateWidgetText(str, widget);
}


void KeyboardAction(char key, Widget* widget, AlsaData* alsa_data, PlayerData* player_data) {

	switch (widget->actionType) {
	
		//search for files
		case 0:
			break;
		//pause
		case 1:
			if (key == ' ') {
				ButtonAction(widget->x, widget->y, widget, alsa_data, player_data);	
			}
			break;
		//search for files by string
		case 4:
			GraphicalInput(key, widget, &(player_data->fileSearchIndex), 64, player_data->fileSearch);

			//check if file matches search
			if (player_data->fileSearch[0] != '\0') {
				
				player_data->fileCountCopy = 0;
					
				for (int i = 0; i < player_data->fileCount; i++) {

					if (strstr(player_data->fileList[i], player_data->fileSearch) != NULL) {
						
						player_data->fileListCopy[player_data->fileCountCopy] = player_data->fileList[i];
						player_data->fileCountCopy++;
					}
				}
			} else {
				for (int i = 0; i < player_data->fileCount; i++) {
					
					player_data->fileListCopy[i] = player_data->fileList[i];
					player_data->fileCountCopy++;
				}
			}
			break;
		
		//change audio rate
		case 5:
			GraphicalInput(key, widget, &(player_data->rateInputIndex), 6, player_data->rateStr);
			player_data->rate = atoi(player_data->rateStr);
			if (player_data->rate < 1) { player_data->rate = 44100; }
			break;
		default:break;
	}
}


void PrintText(char* str, SDL_Renderer* renderer, SDL_Texture* font, 
		uint16_t textx, uint16_t texty) {

	if (str == NULL) { return; }

	SDL_Rect letter;
	letter.x = textx;
	letter.y = texty;
	letter.w = 22;
	letter.h = 26;

	SDL_Rect crop;
	crop.x = 0;
	crop.y = 0;
	crop.w = 22;
	crop.h = 26;

	for (int i = 0; str[i] != '\0'; i++) {
		
		//newline
		if (str[i] == '\\' && str[i+1] == 'n') {
				
			letter.y += 26;
			i += 2;
			letter.x -= 18;
		}

		//print letter
		if (str[i] >= 32 && str[i] < 127) {
		
			crop.x = (str[i]-32)*22;
		}
		SDL_RenderCopy(renderer, font, &crop, &letter);
		letter.x += 18;
	}
}


void UpdateWidgetText(char* str, Widget* widget) {

	if (widget->text != NULL) { free(widget->text); }
	widget->text = malloc(sizeof(char)*strlen(str)+1);
	strcpy(widget->text, str);
}


//main drawing function
void Draw(SDL_Window* window, SDL_Renderer* renderer, 
	  SDL_Texture* background, SDL_Texture* font,
	  Widget* arr[256], uint8_t widgetNum,
	  char* filename,
	  PlayerData* player_data,
	  AlsaData* data) {

	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderFillRect(renderer, NULL);

	int w, h = 0;
	SDL_GetWindowSize(window, &w, &h);
	player_data->w = w;
	player_data->h = h;


	//get screen offset for wave amp		
	int offset = 0;
	if (player_data->waveAmp > 0) {
		
		if (player_data->waveVert == true) { offset = w/(player_data->waveAmp*2);
		} else {			     offset = h/(player_data->waveAmp*2); }
	} else {
		if (player_data->waveVert == true) { offset = w/2;
		} else {			     offset = h/2; }
	}


	//draw background
	if (player_data->background != NULL) { SDL_RenderCopy(renderer, player_data->background, NULL, NULL);
	} else { 			       SDL_RenderCopy(renderer, background, NULL, NULL); }


	//choose color
	SDL_SetRenderDrawColor(renderer, player_data->r, player_data->g, player_data->b, 255);


	//draw audio wave form
	if (player_data->file != NULL && data->buff != NULL && player_data->waveAmp > 0) {
	
		//vertical wave
		if (player_data->waveVert == true) {
			
			int oldx = (data->buff[0]+offset)*player_data->waveAmp;
			int oldy = 0;
			int drawLimit = data->buff_size/player_data->waveFreq < h ? data->buff_size : h;
	
			//draw wave
			for (int i = player_data->waveFreq; i < drawLimit; i += player_data->waveFreq) {
	
				SDL_RenderDrawLine(renderer, oldx, oldy, (data->buff[i]+offset)*player_data->waveAmp, i);
				oldx = (data->buff[i]+offset)*player_data->waveAmp;
				oldy = i;
			}
		//horizontal wave
		} else {
			int oldx = 0;
			int oldy = (data->buff[0]+offset)*player_data->waveAmp;
			int drawLimit = data->buff_size/player_data->waveFreq < w ? data->buff_size : w;
	
			//draw wave
			for (int i = player_data->waveFreq; i < drawLimit; i += player_data->waveFreq) {
	
				SDL_RenderDrawLine(renderer, oldx, oldy, i, (data->buff[i]+offset)*player_data->waveAmp);
				oldx = i;
				oldy = (data->buff[i]+offset)*player_data->waveAmp;
			}
		}
	}



	//add opacity to wave
	if (player_data->waveFade == true) {
			
		SDL_Rect rect;

		//vertical fade
		if (player_data->waveVert == true) {
			
			rect.y = 0;
			rect.x = offset-(128*player_data->waveAmp);
			rect.h = h;
			rect.w = player_data->waveAmp;

			//fade in/out
			for (int i = 0; i < player_data->waveAmp*256; i++) {
		
				if (i < player_data->waveAmp*128) { SDL_SetRenderDrawColor(renderer, 0, 0, 0, ((player_data->waveAmp*128)-1)-(i*player_data->waveAmp));
				} else {	   		    SDL_SetRenderDrawColor(renderer, 0, 0, 0, ((i-(128*player_data->waveAmp)))); }

				SDL_RenderFillRect(renderer, &rect);
				rect.x += player_data->waveAmp;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
			rect.w = w - rect.x;
			SDL_RenderFillRect(renderer, &rect);
		
			rect.x = 0;
			rect.w = offset-(128*player_data->waveAmp);
		
		//horizontal wave
		} else {
			rect.x = 0;
			rect.y = offset-(128*player_data->waveAmp);
			rect.w = w;
			rect.h = player_data->waveAmp;

			//fade in/out
			for (int i = 0; i < player_data->waveAmp*256; i++) {
		
				if (i < player_data->waveAmp*128) { SDL_SetRenderDrawColor(renderer, 0, 0, 0, ((player_data->waveAmp*128)-1)-(i*player_data->waveAmp));
				} else {	   		    SDL_SetRenderDrawColor(renderer, 0, 0, 0, ((i-(128*player_data->waveAmp)))); }

				SDL_RenderFillRect(renderer, &rect);
				rect.y += player_data->waveAmp;
			}

			SDL_SetRenderDrawColor(renderer, 0, 0, 0, 128);
			rect.h = h - rect.y;
			SDL_RenderFillRect(renderer, &rect);
		
			rect.y = 0;
			rect.h = offset-(128*player_data->waveAmp);
		}
		SDL_RenderFillRect(renderer, &rect);
	}


	//draw given widgets
	for (int i = 0; i < widgetNum; i++) {
	
		if (arr[i] != NULL && arr[i]->active == true) { 
			
			DrawWidget(window, renderer, font, arr[i], player_data); 
		}
	}
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);

	//draw to screen
	SDL_RenderPresent(renderer);
	SDL_RenderClear(renderer);
}
