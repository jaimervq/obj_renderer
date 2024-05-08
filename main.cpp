/*
* Author: Jaime Rivera
* Date : 2020.04.20
* Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
* Brief: Implementation of a simple obj wireframe-renderer, for quick turntables of obj files.
* Credits: Sean Barrett, author of the STB library, used in this project (https://github.com/nothings/stb)
*/


#include <chrono> 
#include <filesystem>
#include <string>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include "basic_obj_reader.h"
#include "drawing_utils.h"


int main(int argc, char* argv[])
{
	// ------ Input arguments ------ //
	std::chrono::time_point execution_start = std::chrono::high_resolution_clock::now();
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " OBJ_PATH" << std::endl;
		std::cerr << "Example: " << argv[0] << " my_geo_1.obj" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	// ------ Input path analysis ------ //
	std::filesystem::path p = argv[1];
	if (!std::filesystem::exists(p))
	{
		std::cerr << "[ERROR] The specified OBJ file does not exist!";
		std::exit(EXIT_FAILURE);
	}
	if (!(p.extension().string() == ".obj") && !(p.extension().string() == ".OBJ"))
	{
		std::cerr << "[ERROR] The specified file is not an OBJ (.obj/.OBJ) file!";
		std::exit(EXIT_FAILURE);
	}
	std::string obj_filepath = p.string();
	std::string obj_filename = p.filename().string();
	std::string obj_stem = p.stem().string();

	// ------ Output folder ------ //
	std::string output_folder = p.parent_path().string();
	if (!output_folder.empty()) output_folder += "/";
	output_folder += obj_stem + "_turntable/";
	const char *out_folder = output_folder.c_str();
	std::filesystem::create_directory(out_folder);

	// ------ OBJ reading ------ //
	std::cout << "[INFO] Loading OBJ file: " << obj_filename << std::endl;
	ObjReader obj{ obj_filepath };

	// ------ Base image ------ //
	BasicImage out_image= BasicImage::HD_1080();
	out_image.estimate_obj_drawing_params(obj);

	// ------ Drawing colors and brushes ------ //
	BasicColor retro_blue{ 0.2, 0.60, 1.0 };
	BasicColor faded_blue{ 0.1, 0.35, 0.6 };
	BasicColor retro_yellow{ 0.8, 0.57, 0.05 };
	BasicColor retro_orange{ 1.0, 0.35, 0.05 };

	BasicBrush regular_faded_blue_brush{ faded_blue };
	BasicBrush thick_faded_blue_brush{ faded_blue, 4, BasicBrush::SQUARE_TIP_SHAPE };
	BasicBrush regular_yellow_brush{ retro_yellow };
	BasicBrush thick_orange_brush{ retro_orange, 3, BasicBrush::SQUARE_TIP_SHAPE };

	// ------ RPM calculation ------ //
	const int RPM = 9;
	const int FPS = 24;
	double rotation_angle = (double)RPM * 360.0 / 60.0 / (double)FPS;

	// ------ Frames writing ------ //
	printf("[INFO] Drawing frames");
	int frame_count = 0;
	for (double d = 0.0; d < 360.0; d+= rotation_angle, frame_count++)
	{
		// Feedback
		int percentaje = (int)(d / 360.0 * 100);
		printf("\r[INFO] Drawing frames %i%%", percentaje);

		// Backplate
		out_image.clear();
		out_image.draw_solid_line(StraightLine{ -2000, 0, 2000, 0 }, regular_faded_blue_brush);
		out_image.draw_solid_line(StraightLine{ 0, -2000, 0, 2000 }, regular_faded_blue_brush);
		for (double i = -2000.0; i < 2000.0; i += 50.0)
		{
			if (i != 0)
			{
				out_image.draw_dotted_line(StraightLine{ -2000, i, 2000, i }, regular_faded_blue_brush);
				out_image.draw_dotted_line(StraightLine{ i, -2000, i, 2000 }, regular_faded_blue_brush);

				out_image.draw_solid_line(StraightLine{ -15, i, 15, i }, thick_faded_blue_brush);
				out_image.draw_solid_line(StraightLine{ i, -15, i, 15 }, thick_faded_blue_brush);
			}
		}

		StraightLine diagonal_cross{ -1500, 0, 1500, 0 };
		diagonal_cross.rotate(29);
		out_image.draw_solid_line(diagonal_cross, regular_faded_blue_brush);
		diagonal_cross.rotate(122);
		out_image.draw_solid_line(diagonal_cross, regular_faded_blue_brush);

		Circumference circular_frame{ Vect2{0,0}, 700 };
		out_image.draw_dotted_circle(circular_frame, BasicBrush{ faded_blue, 3, BasicBrush::ROUND_TIP_SHAPE });
		
		// Drawing the OBJ
		out_image.draw_obj(obj, d, regular_yellow_brush, thick_orange_brush);

		// Output data text
		const int text_height = 20;
		int text_x = (int)(0.04 * out_image.get_width());
		int text_y = (int)(0.88 * out_image.get_height());
		int line_increment = (int)(out_image.get_line_increment_coef() * text_height);

		int total_faces = obj.count_total_faces();
		int total_verts = obj.count_total_vertices();
		std::string polycount = "faces: " + std::to_string(total_faces) + " / vertices: " + std::to_string(total_verts);

		out_image.draw_text(text_x, text_y, obj_filename + "\n" + polycount, text_height, BasicBrush{retro_blue});
		out_image.draw_frame(text_x - 10, text_y - 10,
			                 text_x - 10 + (int)polycount.size() * text_height + 2 * 10, text_y - 10 + 2 * line_increment + 10,
			                 thick_orange_brush);

		// Writing the output file
		const std::string out_filename_string = output_folder  + obj_stem + "_" + std::to_string(frame_count) + ".png";
		const char *out_filename = out_filename_string.c_str();
		unsigned char *out_pixels = out_image.get_pixels();
		stbi_write_png(out_filename,
			           out_image.get_width(), out_image.get_height(), out_image.get_channels(),
			           out_pixels, out_image.get_width() * out_image.get_channels());
	}

	// ------ Execution end ------ //
	printf("\r[INFO] Drawing frames 100%%\n");
	printf("[INFO] All frames of the turntable written!\n");

	std::chrono::time_point execution_end = std::chrono::high_resolution_clock::now();
	auto duration = std::chrono::duration_cast<std::chrono::seconds>(execution_end - execution_start);
	std::cout << "[INFO] Total execution time: " << duration.count() << " seconds\n";

	return 0;
}