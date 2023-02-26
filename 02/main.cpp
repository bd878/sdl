#include <SDL2/SDL.h>
#include <string>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

SDL_Surface* loadSurface(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
  
//The surface contained by the window
SDL_Surface* gScreenSurface = NULL;

SDL_Surface* gStretchedSurface = NULL;

bool init()
{
  bool success = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0)
  {
    printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
    success = false;
  }
  else
  {
    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
      printf("Window could not be created! SDL Error %s\n", SDL_GetError());
    }
    else
    {
      gScreenSurface = SDL_GetWindowSurface(gWindow);
    }
  }

  return success;
}

bool loadMedia()
{
  bool success = true;

  gStretchedSurface = loadSurface("./stretch.bmp");
  if (gStretchedSurface == NULL) {
    printf("failed to load stretch image");
    success = false;
  }

  return success;
}

void close()
{
  SDL_FreeSurface(gStretchedSurface);
  gStretchedSurface = NULL;

  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  SDL_Quit();
}

SDL_Surface* loadSurface(std::string path) {
  SDL_Surface* optimizedSurface = NULL;

  SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
  if (loadedSurface == NULL) {
    printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
  } else {
    optimizedSurface = SDL_ConvertSurface(loadedSurface, gScreenSurface->format, 0);
    if (optimizedSurface == NULL)
    {
      printf("Unable to optimize image");
    }

    SDL_FreeSurface(loadedSurface);
  }

  return optimizedSurface;
}

int main(int argc, char* argv[])
{
  if (!init())
  {
    printf("failed to initialize\n");
  }
  else
  {
    if (!loadMedia())
    {
      printf("Failed ot load media\n");
    }
    else
    {
      bool quit = false;

      SDL_Event e;

      while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }
        }

        SDL_Rect stretchRect;
        stretchRect.x = 0;
        stretchRect.y = 0;
        stretchRect.w = SCREEN_WIDTH;
        stretchRect.h = SCREEN_HEIGHT;
        SDL_BlitScaled(gStretchedSurface, NULL, gScreenSurface, &stretchRect);

        SDL_UpdateWindowSurface(gWindow);
      }
    }
  }

  close();

  return 0;
}