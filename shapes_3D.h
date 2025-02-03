#pragma once
/*
 * Author: Jaime Rivera
 * Date : 2020.04.20
 * Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
 * Brief: Basic implementations of 3D shapes
 */

#include <vector>

#include "basic_math.h"

// --------- EDGE AND FACE --------- //
class Edge
{
private:
	Vect3 origin_vtx, end_vtx;

public:
	// Constructor
	Edge(Vect3 orig, Vect3 end) : origin_vtx(orig), end_vtx(end) {}

	// Operators
	bool operator==(const Edge &right)
	{
		double const DIST_THRESHOLD = 0.000001;

		double d1 = this->origin_vtx.get_distance(right.origin_vtx);
		double d2 = this->end_vtx.get_distance(right.end_vtx);

		double d3 = this->origin_vtx.get_distance(right.end_vtx);
		double d4 = this->end_vtx.get_distance(right.origin_vtx);

		return (d1 < DIST_THRESHOLD && d2 < DIST_THRESHOLD) || (d3 < DIST_THRESHOLD && d4 < DIST_THRESHOLD); // Disregards direction
	}
	bool operator<(Edge &right)
	{
		return (this->get_lenght() < right.get_lenght());
	}

	// Get
	Vect3 get_origin() { return this->origin_vtx; }
	Vect3 get_end() { return this->end_vtx; }
	double get_lenght() { return this->origin_vtx.get_distance(this->end_vtx); }

	// Transformations
	void move(Vect3 displacement)
	{
		this->origin_vtx = this->origin_vtx + displacement;
		this->end_vtx = this->end_vtx + displacement;
	}
	void rotate_around_axis(double angle, Vect3 axis)
	{
		Matrix3by3 rotation_matrix = Matrix3by3::RotationMatrix(angle, axis);

		this->origin_vtx = mult_matrix_by_vector3(rotation_matrix, this->origin_vtx);
		this->end_vtx = mult_matrix_by_vector3(rotation_matrix, this->end_vtx);
	}
};

class Face
{
private:
	std::vector<Vect3> vertices;

public:
	// Constructors
	Face() {};
	Face(std::vector<Vect3> input_vertices) : vertices(input_vertices) {}
	Face(Vect3 v0, Vect3 v1, Vect3 v2)
	{
		add_vertex(v0);
		add_vertex(v1);
		add_vertex(v2);
	} // For tris
	Face(Vect3 v0, Vect3 v1, Vect3 v2, Vect3 v3)
	{
		add_vertex(v0);
		add_vertex(v1);
		add_vertex(v2);
		add_vertex(v3);
	} // For quads

	// Operator
	Vect3 &operator[](int index)
	{
		return this->vertices[index];
	}

	// Get
	std::vector<Vect3> get_vertices() { return this->vertices; }
	int count_vertices() { return this->vertices.size(); }

	// Transformations
	void move(Vect3 displacement)
	{
		for (Vect3 &v : this->vertices)
		{
			v = v + displacement;
		}
	}
	void rotate_around_axis(double angle, Vect3 axis)
	{
		for (Vect3 &v : this->vertices)
		{
			Matrix3by3 rotation_matrix = Matrix3by3::RotationMatrix(angle, axis);
			v = mult_matrix_by_vector3(rotation_matrix, v);
		}
	}

	// Utility
	void add_vertex(Vect3 new_vertex)
	{
		this->vertices.push_back(new_vertex);
	}
};

// --------- SHAPES --------- //
class Cube
{
protected:
	Vect3 top_left;
	Vect3 bottom_right;
	std::vector<Face> faces;

public:
	// Constructor
	Cube(Vect3 input_top_left, Vect3 input_bottom_right) : top_left(input_top_left), bottom_right(input_bottom_right) { create_faces(); }

	// Get
	std::vector<Face> &get_faces() { return this->faces; }
	Vect3 get_top_left() { return this->top_left; }
	Vect3 get_bottom_right() { return this->bottom_right; }

	// Set
	void set_top_left(Vect3 new_top_left)
	{
		this->top_left = new_top_left;
	}
	void set_bottom_right(Vect3 new_bottom_right)
	{
		this->bottom_right = new_bottom_right;
	}

	// Transformations
	void move(Vect3 displacement)
	{
		for (Face &f : this->faces)
		{
			f.move(displacement);
		}

		this->set_top_left(this->top_left + displacement);
		this->set_bottom_right(this->bottom_right + displacement);
	}
	void rotate_around_axis(double angle, Vect3 axis)
	{
		for (Face &q : this->get_faces())
		{
			q.rotate_around_axis(angle, axis);
		}
	}

	// Utility
	void create_faces()
	{
		Vect3 vert0 = top_left;
		Vect3 vert1{bottom_right.get_x(), top_left.get_y(), top_left.get_z()};
		Vect3 vert2{bottom_right.get_x(), bottom_right.get_y(), top_left.get_z()};
		Vect3 vert3{top_left.get_x(), bottom_right.get_y(), top_left.get_z()};
		Vect3 vert4{top_left.get_x(), top_left.get_y(), bottom_right.get_z()};
		Vect3 vert5{bottom_right.get_x(), top_left.get_y(), bottom_right.get_z()};
		Vect3 vert6 = bottom_right;
		Vect3 vert7{top_left.get_x(), bottom_right.get_y(), bottom_right.get_z()};

		Face face0{vert0, vert1, vert2, vert3};
		faces.push_back(face0);
		Face face1{vert4, vert5, vert6, vert7};
		faces.push_back(face1);
		Face face2{vert0, vert4, vert5, vert1};
		faces.push_back(face2);
		Face face3{vert3, vert7, vert6, vert2};
		faces.push_back(face3);
		Face face4{vert7, vert4, vert0, vert3};
		faces.push_back(face4);
		Face face5{vert6, vert5, vert1, vert2};
		faces.push_back(face5);
	}
	Vect3 get_center()
	{
		return top_left.get_midpoint(bottom_right);
	}
};

// --------- SPECIAL SHAPES --------- //
class BoundingBox : public Cube
{
public:
	// Constructor
	BoundingBox() : Cube(Vect3{0.0, 0.0, 0.0}, Vect3{0.0, 0.0, 0.0}) { this->faces.clear(); }

	// Utility
	void expand(Vect3 new_point)
	{
		double p_x = new_point.get_x(), p_y = new_point.get_y(), p_z = new_point.get_z();

		if (p_x < this->top_left.get_x())
			this->top_left.set_x(p_x);
		if (p_x > this->bottom_right.get_x())
			this->bottom_right.set_x(p_x);
		if (p_y > this->top_left.get_y())
			this->top_left.set_y(p_y);
		if (p_y < this->bottom_right.get_y())
			this->bottom_right.set_y(p_y);
		if (p_z > this->top_left.get_z())
			this->top_left.set_z(p_z);
		if (p_z < this->bottom_right.get_z())
			this->bottom_right.set_z(p_z);
	}
};