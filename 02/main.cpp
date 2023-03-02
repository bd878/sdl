#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

class LTexture {
public:
  // initializes variables
  LTexture();

  // deallocates memory
  ~LTexture();

  // loads image from path
  bool loadFromFile(std::string path);

  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);

  // deallocates texture
  void free();

  void setColor(Uint8 red, Uint8 green, Uint8 blue);

  void setBlendMode(SDL_BlendMode blending);

  void setAlpha(Uint8 alpha);

  // render texture at given point
  void render(int x, int y, SDL_Rect* clip = NULL, double angle = 0.0, SDL_Point* center = NULL, SDL_RendererFlip flip = SDL_FLIP_NONE);

  int getWidth();
  int getHeight();

private:
  SDL_Texture* mTexture;

  int mWidth;
  int mHeight;
};

//Starts up SDL and creates window
bool init();

//Loads media
bool loadMedia();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;
  
//The surface contained by the window
SDL_Renderer* gRenderer = NULL;

TTF_Font* gFont = NULL;

LTexture gTextTexture;

LTexture::LTexture() {
  mTexture = NULL;
  mWidth = 0;
  mHeight = 0;
}

LTexture::~LTexture() {
  free();
}

void LTexture::setColor(Uint8 red, Uint8 green, Uint8 blue) {
  SDL_SetTextureColorMod(mTexture, red, green, blue);
}

bool LTexture::loadFromFile(std::string path) {
  free();

  SDL_Texture* newTexture = NULL;
  SDL_Surface* loadedSurface = IMG_Load(path.c_str());
  if (loadedSurface == NULL) {
    printf("unable to load image %s! SDL_image error: %s\n",
      path.c_str(), IMG_GetError());
  } else {
    SDL_SetColorKey(loadedSurface, SDL_TRUE, SDL_MapRGB(loadedSurface->format, 0, 0xFF, 0xFF));
    newTexture = SDL_CreateTextureFromSurface(gRenderer, loadedSurface);
    if (newTexture == NULL) {
      printf("unable to create texture");
    } else {
      mWidth = loadedSurface->w;
      mHeight = loadedSurface->h;
    }

    SDL_FreeSurface(loadedSurface);
  }

  mTexture = newTexture;
  return mTexture != NULL;
}

bool LTexture::loadFromRenderedText(std::string textureText, SDL_Color textColor) {
  free();

  SDL_Surface* textSurface = TTF_RenderText_Solid(gFont, textureText.c_str(), textColor);
  if (textSurface == NULL) {
    printf("unable to render text surface!");
  } else {
    mTexture = SDL_CreateTextureFromSurface(gRenderer, textSurface);
    if (mTexture == NULL) {
      printf("unable to create texture");
    } else {
      mWidth = textSurface->w;
      mHeight = textSurface->h;
    }

    SDL_FreeSurface(textSurface);
  }

  return mTexture != NULL;
}

void LTexture::free() {
  if (mTexture != NULL) {
    SDL_DestroyTexture(mTexture);
    mTexture = NULL;
    mWidth = 0;
    mHeight = 0;
  }
}

void LTexture::render(int x, int y, SDL_Rect* clip, double angle, SDL_Point* center, SDL_RendererFlip flip) {
  SDL_Rect renderQuad = { x, y, mWidth, mHeight};

  if (clip != NULL) {
    renderQuad.w = clip->w;
    renderQuad.h = clip->h;
  }

  SDL_RenderCopyEx(gRenderer, mTexture, clip, &renderQuad, angle, center, flip);
}

int LTexture::getWidth() {
  return mWidth;
}

int LTexture::getHeight() {
  return mHeight;
}

void LTexture::setBlendMode(SDL_BlendMode blending) {
  SDL_SetTextureBlendMode(mTexture, blending);
}

void LTexture::setAlpha(Uint8 alpha) {
  SDL_SetTextureAlphaMod(mTexture, alpha);
}

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
      gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
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

        if( TTF_Init() == -1 )
        {
          printf( "SDL_ttf could not initialize! SDL_ttf Error: %s\n", TTF_GetError() );
          success = false;
        }
      }
    }
  }

  return success;
}

void close()
{
  gTextTexture.free();

  TTF_CloseFont(gFont);
  gFont = NULL;

  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;

  IMG_Quit();
  SDL_Quit();
}

bool loadMedia() {
  bool success = true;

  //Open the font
  gFont = TTF_OpenFont( "lazy.ttf", 28 );
  if( gFont == NULL )
  {
    printf( "Failed to load lazy font! SDL_ttf Error: %s\n", TTF_GetError() );
    success = false;
  }
  else
  {
    //Render text
    SDL_Color textColor = { 0, 0, 0 };
    if( !gTextTexture.loadFromRenderedText( "The quick brown fox jumps over the lazy dog", textColor ) )
    {
      printf( "Failed to render text texture!\n" );
      success = false;
    }
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

      double degrees = 0;

      SDL_RendererFlip flipType = SDL_FLIP_NONE;

      int frame = 0;

      Uint8 a = 255;

      while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
          if (e.type == SDL_QUIT) {
            quit = true;
          }
        }

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        gTextTexture.render( ( SCREEN_WIDTH - gTextTexture.getWidth() ) / 2, ( SCREEN_HEIGHT - gTextTexture.getHeight() ) / 2 );
        //Update screen
        SDL_RenderPresent( gRenderer );
      }
    }
  }

  close();

  return 0;
}