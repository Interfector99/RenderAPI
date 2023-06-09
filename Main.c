#include <stdio.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

bool initialize_window(void)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Create SDL window
	window = SDL_CreateWindow
	(
		NULL,					// window name and border	
		SDL_WINDOWPOS_CENTERED,	// window position on the monitor in x axis
		SDL_WINDOWPOS_CENTERED,	// window position on the monitor in y axis
		800,					// window size in pixel in x axis
		600,					// window size in pixel in y axis
		SDL_WINDOW_BORDERLESS	// flags: fullscreen, border, shadow
	);
	if (!window)
	{
		fprintf(stderr, "Error creating window.\n");
		return false;
	}

	// Create SDL renderer
	renderer = SDL_CreateRenderer
	(
		window,	// which window does this renderer belong to
		-1,		// what is the default display device, -1 means get the first default
		0		// flags: 0 means nothing

	);
	if (!renderer)
	{
		fprintf(stderr, "Error creating renderer.\n");
		return false;
	}

	return true;
}

void setup(void)
{

}

void process_input(void)
{
	SDL_Event event;
	SDL_PollEvent(&event);

	switch (event.type)
	{
		case SDL_QUIT: 
			is_running = false;
		break;

		case SDL_KEYDOWN:
			if (event.key.keysym.sym == SDLK_ESCAPE)
			{
				is_running = false;
			}
			break;
	}
}

void update(void)
{

}

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
	SDL_RenderClear(renderer);

	// ...

	SDL_RenderPresent(renderer);
}

int main(int argc, char* args[])
{
	is_running = initialize_window();

	setup();

	while (is_running)
	{
		process_input();
		update();
		render();
	}
	return 0;
}