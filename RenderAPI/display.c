#include "display.h"

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

void draw_grid(void)
{
	for (int y = 0; y < window_height; y++)
	{
		for (int x = 0; x < window_width; x++)
		{
			if (y % 10 == 0 || x % 10 == 0)
			{
				color_buffer[(window_width * y) + x] = 0xFF333333;
			}
		}
	}
}

void draw_pixel(int x, int y, color_t color)
{
	if (x >= 0 && x < window_width && y >= 0 && y < window_height)
	{
		color_buffer[(window_width * y) + x] = color;
	}
}

void draw_line(int x0, int y0, int x1, int y1, color_t color)
{
	int delta_x = (x1 - x0);
	int delta_y = (y1 - y0);

	int side_length = abs(delta_x) >= abs(delta_y) ? abs(delta_x) : abs(delta_y);

	// Calculate increment
	float x_inc = delta_x / (float)side_length;
	float y_inc = delta_y / (float)side_length;

	float current_x = x0;
	float current_y = y0;

	for (int i = 0; i <= side_length; i++)
	{
		draw_pixel(round(current_x), round(current_y), color);
		current_x += x_inc;
		current_y += y_inc;
	}
}

void draw_triangle(int x0, int y0, int x1, int y1, int x2, int y2, color_t color)
{
	draw_line(x0, y0, x1, y1, color);
	draw_line(x1, y1, x2, y2, color);
	draw_line(x2, y2, x0, y0, color);
}

void draw_rectangle(int pos_x, int pos_y, int width, int height, color_t color)
{
	for (int y = pos_y; y < pos_y + height; y++)
	{
		for (int x = pos_x; x < pos_x + width; x++)
		{
			draw_pixel(x, y, color);
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

void destroy_window(void)
{
	
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
}