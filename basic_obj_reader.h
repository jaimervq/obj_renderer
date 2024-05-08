#pragma once
/*
* Author: Jaime Rivera
* Date : 2020.04.20
* Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
* Brief: Basic implementation of a obj files reader
*/


#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "shapes_3D.h"


class ObjReader
{
private:
	// Reading
	std::string source_file;
	bool invert_y;

	// Geometry
	std::vector<Face> faces;
	BoundingBox bounding_box;

	// Edge pool (just for drawing purposes)
	std::vector<Edge> edge_pool;

	// Polycount and general feedback
	int face_count;
	int vertex_count;

public:
	// Constructor
	ObjReader(std::string input_file) : source_file(input_file), invert_y(true), face_count(0), vertex_count(0)
	{
		read_from_file();
		clear_edge_pool();
		calculate_bb();
		to_center();
	}

	// Read from file
	void read_from_file()
	{
		std::ifstream f{ this->source_file };
		if (!f.is_open()) return;

		std::vector <Vect3> temp_vertices;
		while (!f.eof())
		{
			char line[512];
			f.getline(line, 512);

			std::stringstream s;
			s << line;

			char type;

			if (line[0] == 'v' && line[1] == ' ')
			{
				double vx, vy, vz;
				s >> type >> vx >> vy >> vz;
				if (invert_y) vy *= -1;
				Vect3 vert{ vx, vy, vz };
				temp_vertices.push_back(vert);

				this->vertex_count++;
			}
			else if (line[0] == 'f')
			{
				Face f;
				std::string face_data;
				s >> type;

				while (s >> face_data)
				{
					std::string face_index;
					for (char c : face_data)
					{
						if (c == '/') break;
						face_index += c;
					}

					int idx = std::stoi(face_index) - 1;
					f.add_vertex(temp_vertices[idx]);
				}
				this->faces.push_back(f);
				this->face_count ++;

				//Edges
				for (int i = 1; i < f.count_vertices(); i++)
				{
					Edge e{ f[i - 1], f[i] };
					this->edge_pool.push_back(e);
				}
				Edge e_closure{ f[f.count_vertices() - 1], f[0] };
				this->edge_pool.push_back(e_closure);
			}
		}

		f.close();

		// Printing feedback
		printf("[INFO] Total faces: %i, Total vertices: %i\n", this->face_count, this->vertex_count);
	}

	// Get
	std::vector <Face> & get_faces() { return this->faces; }
	BoundingBox & get_bb() { return this->bounding_box; }
	int count_total_faces() { return this->face_count; }
	int count_total_vertices() { return this->vertex_count; }

	// Utility for drawing
	std::vector <Edge> get_edge_pool() { return this->edge_pool; }

	// Transformations
	void to_center()
	{
		// Calculating displacement
		Vect3 displacement = bounding_box.get_center().get_inverted();
		
		if (displacement.get_magnitude() > 0.0)
		printf("[WARNING] The center of the obj's bounding box was off-center.\n"
			   "          Displacing it back to center {%f, %f, %f}\n", displacement.get_x(), displacement.get_y(), displacement.get_z());

		// Moving faces and BB
		for (Face f : this->faces){ f.move(displacement); }
		this->bounding_box.move(displacement);

		// Moving edge pool
		for (Edge &e : this->edge_pool){ e.move(displacement); }

	}
	void rotate_around_axis(double angle, Vect3 axis)
	{
		for (Face &f : this->get_faces())
		{
			f.rotate_around_axis(angle, axis);
		}
		for (Face &f : this->get_bb().get_faces())
		{
			f.rotate_around_axis(angle, axis);
		}
	}

	// Utility
	void clear_edge_pool()
	{
		std::sort(this->edge_pool.begin(), this->edge_pool.end());
		this->edge_pool.erase(std::unique(this->edge_pool.begin(), this->edge_pool.end()), this->edge_pool.end());
	}
	void calculate_bb()
	{
		Vect3 first_v = this->get_faces()[0].get_vertices()[0];
		this->bounding_box.set_top_left(first_v);
		this->bounding_box.set_bottom_right(first_v);

		for (Face &f : faces)
		{
			std::vector<Vect3> vertices = f.get_vertices();
			for (Vect3 v : vertices)
			{
				bounding_box.expand(v);
			}
		}

		this->bounding_box.create_faces();
	}
};