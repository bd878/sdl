#include <SDL2/SDL.h>
#include <string>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

enum KeyPressSurfaces {
  KEY_PRESS_SURFACE_DEFAULT,
  KEY_PRESS_SURFACE_UP,
  KEY_PRESS_SURFACE_DOWN,
  KEY_PRESS_SURFACE_LEFT,
  KEY_PRESS_SURFACE_RIGHT,
  KEY_PRESS_SURFACE_TOTAL
};

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

SDL_Surface* gKeyPressSurfaces[KEY_PRESS_SURFACE_TOTAL];

SDL_Surface* gCurrentSurface = NULL;

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

  // load default surface
  gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] = loadSurface("press.bmp");
  if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT] == NULL) {
    printf("failed to load default image\n");
    success = false;
  }

  // load up surface
  gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] = loadSurface("up.bmp");
  if (gKeyPressSurfaces[KEY_PRESS_SURFACE_UP] == NULL) {
    printf("failed to load up image\n");
    success = false;
  }

  // load down surface
  gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] = loadSurface("down.bmp");
  if (gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN] == NULL) {
    printf("failed to load down image\n");
    success = false;
  }

  // load left surface
  gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] = loadSurface("left.bmp");
  if (gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT] == NULL) {
    printf("failed to load left image\n");
    success = false;
  }

  // load right surface
  gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] = loadSurface("right.bmp");
  if (gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT] == NULL) {
    printf("failed to load right image\n");
    success = false;
  }

  return success;
}

void close()
{
  //Deallocate surfaces
  for( int i = 0; i < KEY_PRESS_SURFACE_TOTAL; ++i )
  {
    SDL_FreeSurface( gKeyPressSurfaces[ i ] );
    gKeyPressSurfaces[ i ] = NULL;
  }

  SDL_DestroyWindow(gWindow);
  gWindow = NULL;

  SDL_Quit();
}

SDL_Surface* loadSurface(std::string path) {
  SDL_Surface* loadedSurface = SDL_LoadBMP(path.c_str());
  if (loadedSurface == NULL) {
    printf( "Unable to load image %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
  }

  return loadedSurface;
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

      gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];

      while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          } else if (e.type == SDL_KEYDOWN) {
            switch (e.key.keysym.sym) {
              case SDLK_UP:
              gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_UP];
              break;

              case SDLK_DOWN:
              gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DOWN];
              break;

              case SDLK_LEFT:
              gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_LEFT];
              break;

              case SDLK_RIGHT:
              gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_RIGHT];
              break;

              default:
              gCurrentSurface = gKeyPressSurfaces[KEY_PRESS_SURFACE_DEFAULT];
              break;
            }
          }
        }

        SDL_BlitSurface(gCurrentSurface, NULL, gScreenSurface, NULL);

        SDL_UpdateWindowSurface(gWindow);
      }
    }
  }

  close();

  return 0;
}