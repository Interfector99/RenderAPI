#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <SDL.h>

bool is_running = false;
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

int window_width = 800;
int window_height = 600;

bool initialize_window(void)
{
	// Initialize SDL
	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		fprintf(stderr, "Error initializing SDL.\n");
		return false;
	}

	// Use SDL to query what is the fullscreen max width and height
	SDL_DisplayMode display_mode;
	SDL_GetCurrentDisplayMode(0, &display_mode);
	window_width = display_mode.w;
	window_height = display_mode.h;
	// this is only borderless windowed yet, not real fullscreen
	// we can change the pixelcount, SDL interpolates to different sizes


	// Create SDL window
	window = SDL_CreateWindow
	(
		NULL,					// window title
		SDL_WINDOWPOS_CENTERED,	// window position on the monitor in x axis
		SDL_WINDOWPOS_CENTERED,	// window position on the monitor in y axis
		window_width,			// window size in pixel in x axis
		window_height,			// window size in pixel in y axis
		SDL_WINDOW_BORDERLESS	// extra flags (fullscreen, border, shadow)
	);
	if (!window)
	{
		fprintf(stderr, "Error creating window.\n");
		return false;
	}

	// Create SDL renderer
	renderer = SDL_CreateRenderer
	(
		window,	// the window context
		-1,		// index of the rendering device, (-1 means get the first default)
		0		// extra flags (0 means nothing)

	);
	if (!renderer)
	{
		fprintf(stderr, "Error creating renderer.\n");
		return false;
	}

	// Change to real fullscreen
	// SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);

	return true;
}

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

void draw_grid(void)
{
	for (int y = 0; y < window_height; y+=10)
	{
		for (int x = 0; x < window_width; x+=10)
		{
			color_buffer[(window_width * y) + x] = 0xFF333333;
		}
	}
}

void render_color_buffer(void)
{
	// Update the content of the texture
	SDL_UpdateTexture
	(
		color_buffer_texture,				// the texture to update
		NULL,								// an SDL_Rect structure representing the area to update (NULL means entire texture)
		color_buffer,						// raw pixel data to copy
		(window_width * sizeof(uint32_t))	// the number of bytes in a row of pixel data, including padding between lines
	);

	SDL_RenderCopy
	(
		renderer,				// the rendering context
		color_buffer_texture,	// the source texture
		NULL,					// the source SDL_Rect structure of NULL for entire texture
		NULL					// the destination SDL_Rect structure or NULL for entire rendering target (stretched if needed)
	);
}

void clear_color_buffer(uint32_t color)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			color_buffer[(window_width * y) + x] = color;
		}
	}
}

void render(void)
{
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
	SDL_RenderClear(renderer);

	draw_grid();

	render_color_buffer();
	clear_color_buffer(0x01010100);

	SDL_RenderPresent(renderer);
}

void destroy_window(void)
{
	free(color_buffer);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
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