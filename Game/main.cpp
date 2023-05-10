#include <iostream>
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>
#include <cstdlib>
#include <ctime>
#include "LTexture.h"

//Screen dimension constants
const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 800;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//The window renderer
SDL_Renderer* gRenderer;

SDL_Event e;

Mix_Chunk* gMusic = NULL;

Mix_Chunk* gGameover = NULL;

Mix_Chunk* gWin = NULL;

int checkingPicture = 1, live = 2;

bool quit = false;

enum trangthai
{
    dangdong, dangmo, dahuy
};

struct Picture
{
    int xPos, yPos;
    std::string path;
    LTexture crPicture;
    trangthai isOpen = dangdong;

    ~Picture()
    {
        crPicture.~LTexture();
    }

    void openPicture()
    {
        if (crPicture.loadFromFile(gRenderer, path))
            crPicture.render(gRenderer, xPos, yPos);
    }

    void defaultPicture()
    {
        if (crPicture.loadFromFile(gRenderer, "defaultPicture.png"))
            crPicture.render(gRenderer, xPos, yPos);
    }

};

Picture card[8];
LTexture background;
LTexture lives[2];
LTexture Menu;

void randomPicture(Picture card[])
{
    srand(time(NULL));
    int a[8] = {0, 0, 1, 1, 2, 2, 3, 3};
    for (int i = 0; i < 4; i++)
    {
        int ran1 = rand() % 8;
        int ran2 = rand() % 8;
        if (a[ran1] != a[ran2])
        {
            int tg = a[ran1];
            a[ran1] = a[ran2];
            a[ran2] = tg;
        }
    }
    for (int i = 0; i < 8; i++)
    {
        if (a[i] == 0)card[i].path = "Picture1.png";
        if (a[i] == 1)card[i].path = "Picture2.png";
        if (a[i] == 2)card[i].path = "Picture3.png";
        if (a[i] == 3)card[i].path = "Picture4.png";
    }
}

bool inside(int x, int y, Picture card)
{
    if (x >= card.xPos && x <= card.xPos + 200 && y >= card.yPos && y <= card.yPos + 200)return true;
    return false;
}

bool checkwin(Picture card[])
{
    for (int i = 0; i < 8; i++)
    {
        if (card[i].isOpen != dahuy)return false;
    }
    return true;
}


void loadGame()
{
    SDL_RenderClear(gRenderer);
    if (background.loadFromFile(gRenderer, "background.png"))background.render(gRenderer, 0, 0);
    for (int i = 0; i < 2; i++)
    {
        if (lives[i].loadFromFile(gRenderer, "heart.png"))lives[i].render(gRenderer, 120 + 100 * i, 50);
    }
    for (int i = 0; i < 8; i++)card[i].defaultPicture();
    SDL_RenderPresent(gRenderer);
}

void handleEvent(SDL_Event &e)
{
    if (e.type == SDL_MOUSEBUTTONDOWN)
    {
        int x, y;
        SDL_GetMouseState(&x, &y);
        for (int i = 0; i < 8; i++)
        {
            if (inside(x, y, card[i]) == true && card[i].isOpen == dangdong)
            {
                card[i].openPicture();
                card[i].isOpen = dangmo;
                SDL_RenderPresent(gRenderer);
                break;
            }
        }
    }
    if (checkingPicture == 2)
    {
        for (int i = 0; i < 7; i++)
        {
            for (int j = i + 1; j < 8; j++)
            {
                if (card[i].isOpen == dangmo && card[j].isOpen == dangmo)
                {
                    if (card[i].path == card[j].path)
                    {
                        card[i].isOpen = dahuy;
                        card[j].isOpen = dahuy;
                        if (checkwin(card))
                        {
                            SDL_RenderClear(gRenderer);
                            if (background.loadFromFile(gRenderer, "win.png"))background.render(gRenderer, 0, 0);
                            SDL_RenderPresent(gRenderer);
                            Mix_FreeChunk(gMusic);
                            gMusic = NULL;
                            gWin = Mix_LoadWAV("wingame.wav");
                            Mix_PlayChannel(-1, gWin, 0);
                        }
                    }
                    else
                    {
                        card[i].defaultPicture();
                        card[j].defaultPicture();
                        card[i].isOpen = dangdong;
                        card[j].isOpen = dangdong;
                        live--;
                        if (live >= 0)
                        {
                            if (lives[live].loadFromFile(gRenderer, "brokenheart.png"))lives[live].render(gRenderer, 120 + 100 * live, 50);
                        }
                        else
                        {
                            for (int i = 0; i < 8; i++)card[i].isOpen = dahuy;
                            SDL_RenderClear(gRenderer);
                            if (background.loadFromFile(gRenderer, "lose.png"))background.render(gRenderer, 0, 0);
                            SDL_RenderPresent(gRenderer);
                            Mix_FreeChunk(gMusic);
                            gMusic = NULL;
                            gGameover = Mix_LoadWAV("losegame.wav");
                            Mix_PlayChannel(-1, gGameover, 0);
                        }
                        SDL_Delay(500);
                        SDL_RenderPresent(gRenderer);
                    }
                }
            }
        }
        checkingPicture = 1;
    }
    else checkingPicture++;
}

int menu()
{
    int x, y;

    Menu.loadFromFile(gRenderer, "menu.png");

    while(!quit)
    {
        while(SDL_PollEvent(&e) != 0)
        {
            switch(e.type)
            {
                case SDL_QUIT:
                {
                    quit = true;
                    break;
                }
                case SDL_MOUSEBUTTONDOWN:
                {
                    SDL_GetMouseState(&x, &y);

                    if (y >= 535 && y <= 645)
                    {
                        if (x >= 315 && x <= 590)
                            return 0; // Trả về 0 nếu người dùng chọn "Play"

                        if (x >= 700 && x <= 975)
                            return 1; // Trả về 1 nếu người dùng chọn "Quit"

                    }
                    break;
                }
            }

        }

        Menu.render(gRenderer, 0, 0);
        SDL_RenderPresent(gRenderer);
    }

    return 1;
}

int main( int argc, char* args[] )
{
    for (int i = 0 ; i < 8 ; i++)
    {
        card[i].xPos = 120 + 280 * (i % 4);
        card[i].yPos = 200 + (i / 4) * 280;
        card[i].path = "defaultPicture.png";
    }
    randomPicture(card);

	if( !init() )
	{
		printf( "Failed to initialize!\n" );
	}
	else
    {
        int check = menu();
        if(check == 1) quit = true;
        else
        {
            loadGame();
            gMusic = Mix_LoadWAV("maingame.wav");
            Mix_PlayChannel(-1, gMusic, 0);

			//While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
					}
					else
                    {
                        handleEvent(e);
                    }
				}
			}
        }
    }

	//Free resources and close SDL
	close();

	return 0;
}

bool init()
{
    //Initialization flag
    bool success = true;

    //Initialize SDL
    if( SDL_Init( SDL_INIT_VIDEO ) < 0 )
    {
        printf( "SDL could not initialize! SDL Error: %s\n", SDL_GetError() );
        success = false;
    }
    else
    {
        //Set texture filtering to linear
        if( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "1" ) )
        {
            printf( "Warning: Linear texture filtering not enabled!" );
        }

        //Create window
        gWindow = SDL_CreateWindow( "Memory Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN );
        if( gWindow == NULL )
        {
            printf( "Window could not be created! SDL Error: %s\n", SDL_GetError() );
            success = false;
        }
        else
        {
            //Create renderer for window
            gRenderer = SDL_CreateRenderer( gWindow, -1, SDL_RENDERER_ACCELERATED );
            if( gRenderer == NULL )
            {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError() );
                success = false;
            }
            else
            {
                //Initialize renderer color
                SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );

                //Initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if( !( IMG_Init( imgFlags ) & imgFlags ) )
                {
                    printf( "SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError() );
                    success = false;
                }
            }
        }
    }
    if( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) < 0 )
	{
		printf( "SDL_mixer could not initialize! SDL_mixer Error: %s\n", Mix_GetError() );
		success = false;
	}

    return success;
}

void close()
{
	//Destroy window
	SDL_DestroyRenderer( gRenderer );
	SDL_DestroyWindow( gWindow );
	gWindow = NULL;
	gRenderer = NULL;
	for (int i = 0; i < 8; i++)card[i].~Picture();
	Mix_FreeChunk(gWin);
	gWin = NULL;
	Mix_FreeChunk(gGameover);
	gGameover = NULL;

	//Quit SDL subsystems
	IMG_Quit();
	SDL_Quit();
	Mix_Quit();
}
