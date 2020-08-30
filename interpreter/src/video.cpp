#include "video.hpp"
#include <SDL2/SDL.h>

video::video(const char* name, uint32_t window_width, uint32_t window_height, uint32_t texture_width, uint32_t texture_height)
{
	SDL_Init(SDL_INIT_VIDEO);

	window = SDL_CreateWindow(name, 0, 0, window_width, window_height, SDL_WINDOW_SHOWN);

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	texture = SDL_CreateTexture(
		renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, texture_width, texture_height);
}

video::~video()
{
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void video::update(const void* data, uint32_t pitch)
{
	SDL_UpdateTexture(texture, nullptr, data, pitch);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, texture, nullptr, nullptr);
	SDL_RenderPresent(renderer);
}
