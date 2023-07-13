#include <stdio.h>
#include <stdint.h>
#include <stdbool.h> 
#include "display.h"
#include "vector.h"
#include "mesh.h"
#include "array.h"
#include "matrix.h"
#include "light.h"
#include "triangle.h"
#include "texture.h"

triangle_t* triangles_to_render = NULL;

bool is_running = false;
int previous_frame_time = 0;

vec3_t camera_position = { 0, 0, 0 };
mat4_t proj_matrix;

void setup(void)
{
	render_method = RENDER_WIRE;
	cull_method = CULL_BACKFACE;
	projection_method = PROJECTION_PERSPECTIVE;
	// Allocate the required memory to hold the color buffer
	color_buffer = (color_t*) malloc(sizeof(color_t) * window_width * window_height); // sizeof() is a language operator, not a function
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

	// Init perspective projection matrix
	float fov = M_PI / 3; // 60 degrees
	float aspect_ratio = (float)window_height / (float)window_width;
	float znear = 0.1f;
	float zfar = 100.0f;
	proj_matrix = mat4_make_perspective(fov, aspect_ratio, znear, zfar);
	//proj_matrix = mat4_make_orthographic(0.1f, 100.0f, 5.0f, -5.0f, -5.0f, 5.0f);

	// manually load texture data from static array
	mesh_texture = (uint32_t*)REDBRICK_TEXTURE;
	texture_width = 64;
	texture_height = 64;


	load_cube_mesh_data();
	//load_obj_file_data("../assets/cube.obj");
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
			if (event.key.keysym.sym == SDLK_1)
			{
				render_method = RENDER_WIRE_VERTEX;
			}
			if (event.key.keysym.sym == SDLK_2)
			{
				render_method = RENDER_WIRE;
			}
			if (event.key.keysym.sym == SDLK_3)
			{
				render_method = RENDER_FILL_TRIANGLE;
			}
			if (event.key.keysym.sym == SDLK_4)
			{
				render_method = RENDER_FILL_TRIANGLE_WIRE;
			}
			if (event.key.keysym.sym == SDLK_5)
			{
				render_method = RENDER_TEXTURED;
			}
			if (event.key.keysym.sym == SDLK_6)
			{
				render_method = RENDER_TEXTURED_WIRE;
			}
			if (event.key.keysym.sym == SDLK_c)
			{
				cull_method = CULL_BACKFACE;
			}
			if (event.key.keysym.sym == SDLK_d)
			{
				cull_method = CULL_NONE;
			}
			if (event.key.keysym.sym == SDLK_p)
			{
				projection_method = PROJECTION_PERSPECTIVE;
				float fov = M_PI / 3; // 60 degrees
				float aspect_ratio = (float)window_height / (float)window_width;
				float znear = 0.1f;
				float zfar = 100.0f;
				proj_matrix = mat4_make_perspective(fov, aspect_ratio, znear, zfar);
			}
			if (event.key.keysym.sym == SDLK_o)
			{
				projection_method = PROJECTION_ORTHOGONAL;
				proj_matrix = mat4_make_orthographic(0.1f, 100.0f, 5.0f, -5.0f, -5.0f, 5.0f);
			}
			break;
	}
}

void update(void)
{
	// Wait some time until the reach the target frame time in milliseconds
	int time_to_wait = FRAME_TARGET_TIME - (SDL_GetTicks() - previous_frame_time);

	// Only delay execution if we are running too fast
	if (time_to_wait > 0 && time_to_wait <= FRAME_TARGET_TIME) {
		SDL_Delay(time_to_wait);
	}
	previous_frame_time = SDL_GetTicks();

	// Initialize the array of triangles to render
	triangles_to_render = NULL;

	// Transformations
	/*mesh.rotation.x += 0.01;
	mesh.rotation.y += 0.01;
	mesh.rotation.z += 0.01;*/
	//mesh.scale.x += 0.002;
	//mesh.scale.y += 0.001;
	//mesh.translation.x += 0.01;
	mesh.translation.z = 5.0;

	// Create matrices
	mat4_t scale_matrix = mat4_make_scale(mesh.scale.x, mesh.scale.y, mesh.scale.z);
	mat4_t rotation_matrix_x = mat4_make_rotation_x(mesh.rotation.x);
	mat4_t rotation_matrix_y = mat4_make_rotation_y(mesh.rotation.y);
	mat4_t rotation_matrix_z = mat4_make_rotation_z(mesh.rotation.z);
	mat4_t translation_matrix = mat4_make_translation(mesh.translation.x, mesh.translation.y, mesh.translation.z);

	// Loop all triangle faces
	int num_faces = array_length(mesh.faces);
	for (int i = 0; i < num_faces; i++)
	{
		face_t mesh_face = mesh.faces[i];

		vec3_t face_vertices[3];
		face_vertices[0] = mesh.vertices[mesh_face.a - 1];
		face_vertices[1] = mesh.vertices[mesh_face.b - 1];
		face_vertices[2] = mesh.vertices[mesh_face.c - 1];

		vec4_t transformed_vertices[3];

		// Loop all three vertices of this current face and apply transformations
		for (int j = 0; j < 3; j++)
		{
			vec4_t transformed_vertex = vec4_from_vec3(face_vertices[j]);

			// World matrix
			mat4_t world_matrix = mat4_identity();

			world_matrix = mat4_mult_mat4(scale_matrix, world_matrix);
			world_matrix = mat4_mult_mat4(rotation_matrix_x, world_matrix);
			world_matrix = mat4_mult_mat4(rotation_matrix_y, world_matrix);
			world_matrix = mat4_mult_mat4(rotation_matrix_z, world_matrix);
			world_matrix = mat4_mult_mat4(translation_matrix, world_matrix);

			transformed_vertex = mat4_mult_vec4(world_matrix, transformed_vertex);

			// Save transformed vertex
			transformed_vertices[j] = transformed_vertex;
		}
		
		vec3_t vector_a = vec3_from_vec4(transformed_vertices[0]); /*   A   */
		vec3_t vector_b = vec3_from_vec4(transformed_vertices[1]); /*  / \  */
		vec3_t vector_c = vec3_from_vec4(transformed_vertices[2]); /* C---B */

		vec3_t vector_ab = vec3_sub(vector_b, vector_a);
		vec3_t vector_ac = vec3_sub(vector_c, vector_a);

		vector_ab = vec3_normalize(vector_ab);
		vector_ac = vec3_normalize(vector_ac);

		vec3_t normal = vec3_cross(vector_ab, vector_ac);

		normal = vec3_normalize(normal);

		vec3_t camera_ray = vec3_sub(camera_position, vector_a);

		float face_angle = vec3_dot(camera_ray, normal);

		// Backface Culling (clockwise, left-handed system)
		if (cull_method == CULL_BACKFACE)
		{
			// Bypass the faces that are looking away from the camera
			if (face_angle < 0)
			{
				continue;
			}
		}

		vec4_t projected_points[3];

		// Projection
		for (int j = 0; j < 3; j++)
		{
			projected_points[j] = mat4_mul_vec4_project(proj_matrix, transformed_vertices[j]);

			// Scale into the view
			projected_points[j].x *= (window_width / 2.0f);
			projected_points[j].y *= (window_height / 2.0f);

			// Invert y values to account for flipped screen y coordinates
			projected_points[j].y *= -1;

			// Translate the projected point to the middle of the screen
			projected_points[j].x += (window_width / 2.0f);
			projected_points[j].y += (window_height / 2.0f);
		}

		// Calculate average depth for each face
		float avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z + transformed_vertices[2].z) / 3.0;

		float light_intensity_factor = -vec3_dot(normal, light.direction);

		color_t triangle_color = light_apply_intensity(mesh_face.color, light_intensity_factor);
		triangle_t projected_triangle =
		{
			.points =
			{
				{ projected_points[0].x, projected_points[0].y, projected_points[0].z, projected_points[0].w },
				{ projected_points[1].x, projected_points[1].y, projected_points[1].z, projected_points[1].w },
				{ projected_points[2].x, projected_points[2].y, projected_points[2].z, projected_points[2].w },
			},
			.texcoords = 
			{
				{ mesh_face.a_uv.u, mesh_face.a_uv.v },
				{ mesh_face.b_uv.u, mesh_face.b_uv.v },
				{ mesh_face.c_uv.u, mesh_face.c_uv.v },
			},
			.color = triangle_color,
			.avg_depth = avg_depth
		};

		// Save the projected triangle in the array of triangles to render
		array_push(triangles_to_render, projected_triangle);
	}

	// sort triangles to render
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		for (int j = i; j < num_triangles; j++)
		{
			if (triangles_to_render[i].avg_depth < triangles_to_render[j].avg_depth)
			{
				triangle_t temp = triangles_to_render[i];
				triangles_to_render[i] = triangles_to_render[j];
				triangles_to_render[j] = temp;
			}
		}
	}
}

void render(void)
{
	draw_grid();

	// Loop all projected triangles and render them
	int num_triangles = array_length(triangles_to_render);
	for (int i = 0; i < num_triangles; i++)
	{
		triangle_t triangle = triangles_to_render[i];

		// Draw filled triangles
		if (render_method == RENDER_FILL_TRIANGLE || render_method == RENDER_FILL_TRIANGLE_WIRE)
		{

			draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
								 triangle.points[1].x, triangle.points[1].y,
								 triangle.points[2].x, triangle.points[2].y,
								 triangle.color);
		}

		// Draw textured triangles
		if (render_method == RENDER_TEXTURED || render_method == RENDER_TEXTURED_WIRE)
		{
			draw_textured_triangle(
				triangle.points[0].x, triangle.points[0].y, triangle.points[0].z, triangle.points[0].w, triangle.texcoords[0].u, triangle.texcoords[0].v,
				triangle.points[1].x, triangle.points[1].y, triangle.points[1].z, triangle.points[1].w, triangle.texcoords[1].u, triangle.texcoords[1].v,
				triangle.points[2].x, triangle.points[2].y, triangle.points[2].z, triangle.points[2].w, triangle.texcoords[2].u, triangle.texcoords[2].v,
				mesh_texture);
		}

		// Draw wires
		if (render_method == RENDER_WIRE || render_method == RENDER_WIRE_VERTEX || render_method == RENDER_FILL_TRIANGLE_WIRE || render_method == RENDER_TEXTURED_WIRE)
		{
			draw_triangle(triangle.points[0].x, triangle.points[0].y,
						  triangle.points[1].x, triangle.points[1].y,
						  triangle.points[2].x, triangle.points[2].y,
						  0xFFFFFFFF);
		}

		// Draw vertex points
		if (render_method == RENDER_WIRE_VERTEX)
		{
			draw_rectangle(triangle.points[0].x - 3, triangle.points[0].y - 3, 6, 6, 0xFFFF0000);
			draw_rectangle(triangle.points[1].x - 3, triangle.points[1].y - 3, 6, 6, 0xFFFF0000);
			draw_rectangle(triangle.points[2].x - 3, triangle.points[2].y - 3, 6, 6, 0xFFFF0000);
		}
	}

	render_color_buffer();

	clear_color_buffer(0x01010100);

	SDL_RenderPresent(renderer);
}

void free_resources(void)
{
	free(color_buffer);
	array_free(mesh.faces);
	array_free(mesh.vertices);

	array_free(triangles_to_render);
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
	free_resources();

	return 0;
}