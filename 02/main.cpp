#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <stdio.h>

//Screen dimension constants
const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

const int BUTTON_WIDTH = 300;
const int BUTTON_HEIGHT = 200;
const int TOTAL_BUTTONS = 54;

enum LButtonSprite {
  BUTTON_SPRITE_MOUSE_OUT = 0,
  BUTTON_SPRITE_MOUSE_OVER_MOTION = 1,
  BUTTON_SPRITE_MOUSE_DOWN = 2,
  BUTTON_SPRITE_MOUSE_UP = 3,
  BUTTON_SPRITE_TOTAL = 4
};

class LTexture {
public:
  // initializes variables
  LTexture();

  // deallocates memory
  ~LTexture();

  // loads image from path
  bool loadFromFile(std::string path);

#if defined(SDL_TTF_MAJOR_VERSION)
  bool loadFromRenderedText(std::string textureText, SDL_Color textColor);
#endif

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

class LButton {
public:
  LButton();

  void setPosition(int x, int y);

  void handleEvent(SDL_Event* e);

  void render();
private:
  SDL_Point mPosition;

  LButtonSprite mCurrentSprite;
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


//Mouse button sprites
SDL_Rect gSpriteClips[ BUTTON_SPRITE_TOTAL ];
LTexture gButtonSpriteSheetTexture;

//Buttons objects
LButton gButtons[ TOTAL_BUTTONS ]; 

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

#if defined(SDL_TTF_MAJOR_VERSION)
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
#endif

LButton::LButton() {
  mPosition.x = 0;
  mPosition.y = 0;

  mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
}

void LButton::setPosition(int x, int y) {
  mPosition.x = x;
  mPosition.y = y;
}

void LButton::handleEvent(SDL_Event* e) {
  if (e->type == SDL_MOUSEMOTION || e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP) {
    int x, y;
    SDL_GetMouseState(&x, &y);

    bool inside = true;
    if (x < mPosition.x) {
      inside = false;
    } else if (x > mPosition.x + BUTTON_WIDTH) {
      inside = false;
    } else if (y < mPosition.y) {
      inside = false;
    } else if (y > mPosition.y + BUTTON_HEIGHT) {
      inside = false;
    }

    if (!inside) {
      mCurrentSprite = BUTTON_SPRITE_MOUSE_OUT;
    } else {
      switch (e->type) {
        case SDL_MOUSEMOTION:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_OVER_MOTION;
        break;

        case SDL_MOUSEBUTTONDOWN:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_DOWN;
        break;

        case SDL_MOUSEBUTTONUP:
        mCurrentSprite = BUTTON_SPRITE_MOUSE_UP;
        break;
      }
    }
  }
}

void LButton::render() {
  gButtonSpriteSheetTexture.render(mPosition.x, mPosition.y, &gSpriteClips[mCurrentSprite]);
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

  //Load sprites
  if( !gButtonSpriteSheetTexture.loadFromFile( "button.png" ) )
  {
    printf( "Failed to load button sprite texture!\n" );
    success = false;
  }
  else
  {
    //Set sprites
    for( int i = 0; i < BUTTON_SPRITE_TOTAL; ++i )
    {
      gSpriteClips[ i ].x = 0;
      gSpriteClips[ i ].y = i * 200;
      gSpriteClips[ i ].w = BUTTON_WIDTH;
      gSpriteClips[ i ].h = BUTTON_HEIGHT;
    }

    //Set buttons in corners
    gButtons[ 0 ].setPosition( 0, 0 );
    gButtons[ 1 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, 0 );
    gButtons[ 2 ].setPosition( 0, SCREEN_HEIGHT - BUTTON_HEIGHT );
    gButtons[ 3 ].setPosition( SCREEN_WIDTH - BUTTON_WIDTH, SCREEN_HEIGHT - BUTTON_HEIGHT );
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

          for (int i = 0; i < TOTAL_BUTTONS; ++i) {
            gButtons[i].handleEvent(&e);
          }
        }

        SDL_SetRenderDrawColor( gRenderer, 0xFF, 0xFF, 0xFF, 0xFF );
        SDL_RenderClear( gRenderer );

        for (int i = 0; i < TOTAL_BUTTONS; ++i) {
          gButtons[i].render();
        }

        SDL_RenderPresent( gRenderer );
      }
    }
  }

  close();

  return 0;
}