#include <SDL2/SDL.h>
#include <emscripten.h>
#include <cstdlib>
#include <string>
#include <SDL2/SDL_image.h>

struct context
{
    SDL_Renderer *renderer;
    int iteration;
    SDL_Texture* img;
};

SDL_Surface* gXOut = NULL;

SDL_Texture* loadTexture( SDL_Renderer *renderer, std::string path )
{
    //The final texture
    SDL_Texture* newTexture = NULL;

    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }

        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }

    return newTexture;
}

void mainloop(void *arg)
{
    context *ctx = static_cast<context*>(arg);
    SDL_Renderer *renderer = ctx->renderer;
    
    // red background
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);

    // print image
    SDL_RenderCopy( renderer, ctx->img, NULL, NULL );
    
    // moving blue rectangle
    SDL_Rect r;
    r.x = ctx->iteration % 255;
    r.y = 50;
    r.w = 50;
    r.h = 50;
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect(renderer, &r );

    SDL_RenderPresent(renderer);

    ctx->iteration++;
}

int main()
{
    SDL_Init(SDL_INIT_VIDEO);
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_CreateWindowAndRenderer(255, 255, 0, &window, &renderer);

    context ctx;
    ctx.renderer = renderer;
    ctx.iteration = 0;

    //load image
    ctx.img = loadTexture(renderer, "x.png");

    const int simulate_infinite_loop = 1; // call the function repeatedly
    const int fps = -1; // call the function as fast as the browser wants to render (typically 60fps)
    emscripten_set_main_loop_arg(mainloop, &ctx, fps, simulate_infinite_loop);
    
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}