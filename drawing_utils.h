#pragma once
/*
 * Author: Jaime Rivera
 * Date : 2020.04.20
 * Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
 * Brief: Basic implementations of a brush and image to draw onto
 * Credits: Sean Barrett, author of the STB library, used in this project (https://github.com/nothings/stb)
 */

#include <algorithm>
#include <string>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "basic_color.h"
#include "basic_math.h"
#include "basic_obj_reader.h"
#include "shapes_2D.h"
#include "shapes_3D.h"
#include "text_sprites.h"

// --------- BASIC BRUSH --------- //
class BasicBrush
{
private:
	BasicColor color;
	int tip_width;
	std::string tip_shape;

public:
	// Constructors
	BasicBrush() : color(BasicColor::White), tip_width(1), tip_shape(BasicBrush::ROUND_TIP_SHAPE) {}
	BasicBrush(BasicColor initial_color) : color(initial_color), tip_width(1), tip_shape(BasicBrush::ROUND_TIP_SHAPE) {}
	BasicBrush(BasicColor initial_color, int initial_thickness) : color(initial_color), tip_width(initial_thickness), tip_shape(BasicBrush::ROUND_TIP_SHAPE) {}
	BasicBrush(BasicColor initial_color, int initial_thickness, std::string initial_tip_shape) : color(initial_color), tip_width(initial_thickness), tip_shape(initial_tip_shape) {}

	// Predefined tips (widths and shapes)
	static const int SLIM_TIP_WIDTH;
	static const int THICK_TIP_WIDTH;
	static const std::string SQUARE_TIP_SHAPE;
	static const std::string ROUND_TIP_SHAPE;

	// Get
	BasicColor get_color() { return this->color; }
	int get_tip_width() { return this->tip_width; }
	std::string get_tip_shape() { return this->tip_shape; }

	// Set
	void set_color(BasicColor new_color) { this->color = new_color; }
};
const int BasicBrush::SLIM_TIP_WIDTH = 1;
const int BasicBrush::THICK_TIP_WIDTH = 4;
const std::string BasicBrush::SQUARE_TIP_SHAPE = "SQUARE";
const std::string BasicBrush::ROUND_TIP_SHAPE = "ROUND";

// --------- BASIC IMAGE --------- //
class BasicImage
{
private:
	// Image main properties
	int width, height;
	int channels;
	unsigned char *pixels;
	int max_index;

	// Drawing coeficients
	const double SOLID_LINE_FACTOR = 0.5;
	const double DOTTED_LINE_FACTOR = 8.0;
	const double SOLID_CIRCUMF_FACTOR = 0.5;
	const double DOTTED_CIRCUMF_FACTOR = 3200;

	const double LINE_INCREMENT_COEF = 1.2;

	// OBJ drawing properties
	double z_offset, projection_distance, obj_drawing_scale;

public:
	// Constructors
	BasicImage(int input_width, int input_height, int input_channels) : width(input_width), height(input_height), channels(input_channels), max_index(input_width * input_height * input_channels)
	{
		this->pixels = new unsigned char[max_index];
		this->clear();
	}
	BasicImage(unsigned char *input_pixels, int input_width, int input_height, int input_channels) : pixels(input_pixels), width(input_width), height(input_height), channels(input_channels), max_index(input_width * input_height * input_channels) {}

	// Predefided resolutions
	static BasicImage HD_720();
	static BasicImage HD_1080();
	static BasicImage UHD_4K();

	// Get
	int get_width() { return width; }
	int get_height() { return height; }
	int get_channels() { return channels; }
	unsigned char *get_pixels() { return this->pixels; }

	double get_line_increment_coef() { return this->LINE_INCREMENT_COEF; }

	// Drawing 2D
	void draw_point(Vect2 pos, BasicBrush brush)
	{
		double x = pos.get_x();
		double y = pos.get_y();

		int xi, yi;
		transform_to_image_cords(x, y, xi, yi);

		if (brush.get_tip_width() > 1)
			draw_thick_dot(xi, yi, brush);
		else
			draw_single_pixel(xi, yi, brush);
	}
	void draw_solid_line(StraightLine line, BasicBrush brush)
	{
		double line_len = line.get_length();
		double solid_step = SOLID_LINE_FACTOR / line_len;

		for (double t = 0.0; t < 1.0; t += solid_step)
		{
			Vect2 pixel_pos = line.get_coord_from_t(t);
			draw_point(pixel_pos, brush);
		}
	}
	void draw_dotted_line(StraightLine line, BasicBrush brush)
	{
		double line_len = line.get_length();
		double dotted_step = DOTTED_LINE_FACTOR / line_len;

		for (double t = 0.0; t < 1.0; t += dotted_step)
		{
			Vect2 pixel_pos = line.get_coord_from_t(t);
			draw_point(pixel_pos, brush);
		}
	}
	void draw_solid_circle(Circumference circumf, BasicBrush brush)
	{
		double circumf_length = circumf.get_circumference();
		double solid_step = SOLID_CIRCUMF_FACTOR / circumf_length;

		for (double th = 0.0; th < 360.0; th += solid_step)
		{
			Vect2 pixel_pos = circumf.get_coord_from_theta(th);
			draw_point(pixel_pos, brush);
		}
	}
	void draw_dotted_circle(Circumference circumf, BasicBrush brush)
	{
		double circumf_length = circumf.get_circumference();
		double dotted_step = DOTTED_CIRCUMF_FACTOR / circumf_length;

		for (double th = 0.0f; th < 360.0f; th += dotted_step)
		{
			Vect2 pixel_pos = circumf.get_coord_from_theta(th);
			draw_point(pixel_pos, brush);
		}
	}
	void draw_polygon(Polygon poly, BasicBrush brush)
	{
		for (int i = 1; i < poly.count_vertices(); i++)
		{
			StraightLine line{poly[i - 1], poly[i]};
			draw_solid_line(line, brush);
		}
		StraightLine line_closure{poly[poly.count_vertices() - 1], poly[0]};
		draw_solid_line(line_closure, brush);
	}

	// Drawing 3D
	void draw_edge(Edge e, BasicBrush brush)
	{
		Vect3 v1 = e.get_origin();
		double z1 = abs(v1.get_z() - this->z_offset);
		double x1_flat = (this->projection_distance / z1) * v1.get_x() * this->obj_drawing_scale;
		double y1_flat = (this->projection_distance / z1) * v1.get_y() * this->obj_drawing_scale;

		Vect3 v2 = e.get_end();
		double z2 = abs(v2.get_z() - this->z_offset);
		double x2_flat = (this->projection_distance / z2) * v2.get_x() * this->obj_drawing_scale;
		double y2_flat = (this->projection_distance / z2) * v2.get_y() * this->obj_drawing_scale;

		draw_solid_line(StraightLine{x1_flat, y1_flat, x2_flat, y2_flat}, brush);
	}
	void draw_face(Face f, BasicBrush brush)
	{
		for (Vect3 v : f.get_vertices())
		{
			for (int i = 1; i < f.count_vertices(); i++)
			{
				Edge e{f[i - 1], f[i]};
				draw_edge(e, brush);
			}
			Edge e_closure{f[f.count_vertices() - 1], f[0]};
			draw_edge(e_closure, brush);
		}
	}
	void estimate_obj_drawing_params(ObjReader obj)
	{
		Vect3 tl = obj.get_bb().get_top_left();
		Vect3 br = obj.get_bb().get_bottom_right();

		Matrix3by3 matrix_45 = Matrix3by3::RotationMatrix(45.0, Vect3::YAxis);
		Vect3 tl_45 = mult_matrix_by_vector3(matrix_45, tl);
		Vect3 br_45 = mult_matrix_by_vector3(matrix_45, br);

		Matrix3by3 matrix_90 = Matrix3by3::RotationMatrix(90.0, Vect3::YAxis);
		Vect3 tl_90 = mult_matrix_by_vector3(matrix_90, tl);
		Vect3 br_90 = mult_matrix_by_vector3(matrix_90, br);

		double max_displacement = std::max({
									  abs(tl.get_x()),
									  abs(tl.get_z()),
									  abs(br.get_x()),
									  abs(br.get_z()),
									  abs(tl_45.get_x()),
									  abs(tl_45.get_z()),
									  abs(br_45.get_x()),
									  abs(br_45.get_z()),
									  abs(tl_90.get_x()),
									  abs(tl_90.get_z()),
									  abs(br_90.get_x()),
									  abs(br_90.get_z()),
								  }) *
								  3.0;
		double proj_distance = max_displacement * 1.5;

		double tl_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (tl.get_z() - max_displacement)) * tl.get_x());
		double tl_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (tl.get_z() - max_displacement)) * tl.get_y());
		double br_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (br.get_z() - max_displacement)) * br.get_x());
		double br_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (br.get_z() - max_displacement)) * br.get_y());

		double tl_45_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (tl_45.get_z() - max_displacement)) * tl_45.get_x());
		double tl_45_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (tl_45.get_z() - max_displacement)) * tl_45.get_y());
		double br_45_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (br_45.get_z() - max_displacement)) * br_45.get_x());
		double br_45_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (br_45.get_z() - max_displacement)) * br_45.get_y());

		double tl_90_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (tl_90.get_z() - max_displacement)) * tl_90.get_x());
		double tl_90_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (tl_90.get_z() - max_displacement)) * tl_90.get_y());
		double br_90_scale_x = (this->width * 0.5 * 0.92) / ((proj_distance / (br_90.get_z() - max_displacement)) * br_90.get_x());
		double br_90_scale_y = (this->height * 0.5 * 0.92) / ((proj_distance / (br_90.get_z() - max_displacement)) * br_90.get_y());

		double drawing_scale = std::min({abs(tl_scale_x), abs(tl_scale_y),
										 abs(br_scale_x), abs(br_scale_y),
										 abs(tl_45_scale_x), abs(tl_45_scale_y),
										 abs(br_45_scale_x), abs(br_45_scale_y),
										 abs(tl_90_scale_x), abs(tl_90_scale_y),
										 abs(br_90_scale_x), abs(br_90_scale_y)});

		this->z_offset = max_displacement;
		this->projection_distance = proj_distance;
		this->obj_drawing_scale = drawing_scale;

		printf("[INFO] The parameters used for drawing the OBJ file are:\n"
			   "       - Z Offset: %f\n"
			   "       - Projection distance: %f\n"
			   "       - Drawing scale: %f\n",
			   this->z_offset, this->projection_distance, this->obj_drawing_scale);
	}
	void draw_obj(ObjReader obj, double rot_angle, BasicBrush faces_brush, BasicBrush bb_brush)
	{
		for (Edge e : obj.get_edge_pool())
		{
			e.rotate_around_axis(rot_angle, Vect3::YAxis);
			draw_edge(e, faces_brush);
		}
		for (Face f : obj.get_bb().get_faces())
		{
			f.rotate_around_axis(rot_angle, Vect3::YAxis);
			draw_face(f, bb_brush);
		}
	}

	// Transformations to image coords
	void transform_to_image_cords(double x, double y, int &xi, int &yi)
	{
		xi = static_cast<int>(std::floor(x));
		xi += (width / 2);

		yi = static_cast<int>(std::floor(y));
		yi += (height / 2);
	}
	int get_index_from_coords(int xi, int yi)
	{
		int index_count = ((yi + 1) * width - (width - xi)) * channels;
		return index_count;
	}

	// Drawing in image coords
	void draw_single_pixel(int xi, int yi, BasicBrush brush)
	{
		const bool IS_DEBUGGING = false;
		int index_pos = get_index_from_coords(xi, yi);

		if (index_pos < 0 || index_pos >= max_index || xi < 0 || xi > width - 1 || yi < 0 || yi > height - 1)
		{
			if (IS_DEBUGGING)
				printf("[DEBUG] Coordinates are not valid for drawing! [%i,%i]\n", xi, yi);
			return;
		}

		BasicColor brush_color = brush.get_color();
		BasicColor pixel_color = get_color_at(xi, yi);
		BasicColor blend_color = blend_two_colors(brush_color, BasicColor::over_ID, pixel_color); // TODO have brushes carry blendmode

		pixels[index_pos++] = blend_color.r255();
		pixels[index_pos++] = blend_color.g255();
		pixels[index_pos++] = blend_color.b255();
		if (channels == 4)
			pixels[index_pos++] = blend_color.a255();
	}
	void draw_thick_dot(int xi, int yi, BasicBrush brush)
	{
		int half_tip = (int)(brush.get_tip_width() / 2.0);

		if (brush.get_tip_shape() == BasicBrush::SQUARE_TIP_SHAPE)
		{
			for (int x = -half_tip; x < half_tip + 1; x++)
			{
				for (int y = -half_tip; y < half_tip + 1; y++)
				{
					draw_single_pixel(x + xi, y + yi, brush);
				}
			}
		}
		else if (brush.get_tip_shape() == BasicBrush::ROUND_TIP_SHAPE)
		{
			for (int i = 0; i < half_tip + 1; i++)
			{
				for (double th = 0.0; th < 360.0; th += 1.0)
				{
					int p_x = (int)(i * (rad_cos(th)));
					int p_y = (int)(i * (rad_sin(th)));
					draw_single_pixel(p_x + xi, p_y + yi, brush);
				}
			}
		}
	}
	void draw_frame(int xi1, int yi1, int xi2, int yi2, BasicBrush brush)
	{
		for (int x = xi1; x < xi2 + 1; x++)
		{
			draw_single_pixel(x, yi1, brush);
			draw_single_pixel(x, yi2, brush);
		}
		for (int y = yi1; y < yi2 + 1; y++)
		{
			draw_single_pixel(xi1, y, brush);
			draw_single_pixel(xi2, y, brush);
		}
	}
	void draw_text(int upper_left_x, int upper_left_y, std::string text, int text_height, BasicBrush brush)
	{
		// Text sprite
		const int SPR_CHANNELS = 3;
		const int SPR_COLUMNS = 16;

		int spr_width = 400;
		int spr_height = 150;
		int spr_side = 25;
		uint8_t *s_pixels = s_25_25;

		if (text_height < 15)
		{
			spr_width = 160;
			spr_height = 60;
			spr_side = 10;
			s_pixels = s_10_10;
		}
		else if (text_height < 20)
		{
			spr_width = 240;
			spr_height = 90;
			spr_side = 15;
			s_pixels = s_15_15;
		}
		else if (text_height < 25)
		{
			spr_width = 320;
			spr_height = 120;
			spr_side = 20;
			s_pixels = s_20_20;
		}
		int line_increment = (int)(LINE_INCREMENT_COEF * text_height);

		// Text writing
		int image_x = upper_left_x;
		int image_y = upper_left_y;

		for (char ch : text)
		{
			if (ch == '\n')
			{
				image_x = upper_left_x;
				image_y = upper_left_y + line_increment;
				continue;
			}

			int c = (int)ch;
			int sprite_row = c / SPR_COLUMNS == 0 ? 0 : c / SPR_COLUMNS - 2;
			int sprite_column = c % SPR_COLUMNS;

			int sprite_x = sprite_column * spr_side;
			int sprite_y = sprite_row * spr_side;

			for (int j = 0; j < spr_side; j++)
			{
				for (int i = 0; i < spr_side; i++)
				{
					int s_index = ((sprite_y + 1) * spr_width - (spr_width - sprite_x)) * SPR_CHANNELS;

					if (s_pixels[s_index] != 0)
					{
						draw_single_pixel(image_x, image_y, brush);
					}

					sprite_x++;
					image_x++;
				}

				sprite_x -= spr_side;
				image_x -= spr_side;

				sprite_y++;
				image_y++;
			}

			image_x += spr_side;
			image_y -= spr_side;
		}
	}

	// Get
	BasicColor get_color_at(int xi, int yi)
	{
		int idx = get_index_from_coords(xi, yi);

		double r, g, b;
		double a = 0.0;
		r = (double)pixels[idx++];
		g = (double)pixels[idx++];
		b = (double)pixels[idx++];

		if (this->get_channels() == 4)
		{
			a = pixels[idx++];
		}

		return BasicColor{r, g, b, a};
	}

	// Utility
	void clear()
	{
		for (int i = 0; i < width * height * channels; ++i)
		{
			pixels[i] = 0;
		}
	}

	// Write to file
	void to_file(std::string filename_string)
	{
		const char *filename = (filename_string + ".png").c_str();
		stbi_write_png(filename, width, height, channels, pixels, width * channels);
	}
};
BasicImage BasicImage::HD_720() { return BasicImage{1280, 720, 4}; }
BasicImage BasicImage::HD_1080() { return BasicImage{1920, 1080, 4}; }
BasicImage BasicImage::UHD_4K() { return BasicImage{3840, 2160, 4}; }
