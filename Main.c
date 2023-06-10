#include "display.h"

bool is_running = false;

void setup(void)
{
	// Allocate the required memory to hold the color buffer
	color_buffer = (uint32_t*) malloc(sizeof(uint32_t) * window_width * window_height); // sizeof() is a language operator, not a function
	// TODO: check for malloc error

	// Creating SDl texture that is used to display the color buffer
	color_buffer_texture = SDL_CreateTexture
	(
		renderer,						// the rendering context
		SDL_PIXELFORMAT_ARGB8888,		// pixel format
		SDL_TEXTUREACCESS_STREAMING,	// texture access
		window_width,					// texture width
		window_height					// texture_height
	);
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
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	//draw_grid();
	draw_rectangle(200,200, 400,200, 0xFF336699);

	render_color_buffer();
	clear_color_buffer(0x01010100);

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

	destroy_window();

	return 0;
}