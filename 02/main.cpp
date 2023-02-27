#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
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

SDL_Texture* loadTexture(std::string path);

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
  
//The surface contained by the window
SDL_Renderer* gRenderer = NULL;

SDL_Texture* gTexture = NULL;

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
    if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1")) {
      printf("Warning: linear texture filtering not enabled");
    }

    gWindow = SDL_CreateWindow( "SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (gWindow == NULL)
    {
      printf("Window could not be created! SDL Error %s\n", SDL_GetError());
    }
    else
    {
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED);
      if (gRenderer == NULL) {
        printf("renderer could not be created: %s\n", SDL_GetError());
        success = false;
      } else {
        // init renderer color
        SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);

        // init png loading
        int imgFlags = IMG_INIT_PNG;
        if (!(IMG_Init(imgFlags) & imgFlags)) {
          printf("SDL_Image could not initialize!\n");
          success = false;
        }
      }
    }
  }

  return success;
}

void close()
{
  SDL_DestroyTexture(gTexture);
  gTexture = NULL;

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  IMG_Quit();
  SDL_Quit();
}

SDL_Texture* loadTexture(std::string path) {
  SDL_Texture* newTexture = NULL;

  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("unable to load image");
  } else {
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("unable to create texture");
    }

    // get rid of old surface
    SDL_FreeSurface(loadedSurface);
  }

  return newTexture;
}

bool loadMedia() {
  bool success = true;

  gTexture = loadTexture("./texture.png");
  if (gTexture == NULL) {
    printf("Failed to load texture\n");
    success = false;
  }

  return success;
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

        SDL_RenderClear(gRenderer);

        SDL_RenderCopy(gRenderer, gTexture, NULL, NULL);

        SDL_RenderPresent(gRenderer);
      }
    }
  }

  close();

  return 0;
}