#pragma once

#include <cstdint>


class SDL_Window;
class SDL_Renderer;
class SDL_Texture;


class video
{
public:
	video(const char*, uint32_t, uint32_t, uint32_t, uint32_t);
	~video();
	void update(const void* data, uint32_t);

private:
	SDL_Window* window{};
	SDL_Renderer* renderer{};
	SDL_Texture* texture{};
};
