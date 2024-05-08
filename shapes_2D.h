#pragma once
/*
* Author: Jaime Rivera
* Date : 2020.04.20
* Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
* Brief: Basic implementations of 2D shapes
*/


#include <vector>

#include "basic_math.h"


// --------- SHAPES --------- //
class StraightLine
{
private:
	Vect2 origin;
	Vect2 end;

public:
	// Constructors
	StraightLine() : origin(0.0, 0.0), end(0.0, 0.0) {}
	StraightLine(double x1, double y1, double x2, double y2) : origin(x1, y1), end(x2, y2) {}
	StraightLine(Vect2 given_origin, Vect2 given_end)
	{
		this->origin = given_origin;
		this->end = given_end;
	}

	// Transformations
	void move(Vect2 displacement)
	{
		this->origin = this->origin + displacement;
		this->end = this->end + displacement;
	}
	void move(double dx, double dy)
	{
		this->origin = this->origin + Vect2{dx, dy};
		this->end = this->end + Vect2{dx, dy};
	}
	void rotate(double angle)
	{
		Vect2 mid_point{ (origin.get_x() + end.get_x()) / 2 , (origin.get_y() + end.get_y()) / 2 };
		this->origin.rotate_around(mid_point, angle);
		this->end.rotate_around(mid_point, angle);

	}

	// Utility
	Vect2 get_coord_from_t (double t)
	{
		double p_x, p_y;
		double x1 = origin.get_x();   double y1 = origin.get_y();
		double x2 = end.get_x();      double y2 = end.get_y();

		if      (t > 1.0)  t = 1.0;
		else if (t < 0.0)  t = 0.0;

		p_x = (1.0 - t) * x1 + t * x2;
		p_y = (1.0 - t) * y1 + t * y2;

		Vect2 param_point{ floor(p_x), floor(p_y) };
		return param_point;
	}
	double get_length()
	{
		return origin.get_distance(end);
	}
};

class Circumference
{
private:
	Vect2 center;
	double radius;

public:
	// Constructors
	Circumference() : center(0.0,0.0), radius(1.0) {}
	Circumference(Vect2 given_center, double given_radius)
	{
		this->center = given_center;
		this->radius = given_radius;
	}
	Circumference(double x1, double y1, double x2, double y2)
	{
		double c_x = (x1 + x2) / 2.0;
		double c_y = (y1 + y2) / 2.0;
		this->center = Vect2{ c_x, c_y };
		this->radius = y1 - c_y;

	}

	// Transformations
	void move_center(Vect2 increment_point)
	{
		this->center = this->center + increment_point;
	}
	void incremet_radius(double radius_delta)
	{
		this->radius += radius_delta;
	}

	// Utility
	Vect2 get_coord_from_theta(double theta)
	{
		double p_x = this->radius * (rad_cos(theta));
		double p_y = this->radius * (rad_sin(theta));

		Vect2 pure_coord{ p_x, p_y };
		return this->center + pure_coord;
	}
	double get_circumference()
	{
		return 2 * M_PI * this->radius;
	}
};

class Polygon
{
private:
	Vect2 translation{ 0.0, 0.0 };
	std::vector<Vect2> vertices;

public:
	// Constructor
	Polygon() {};
	Polygon(Vect2 initial_vertex, int number_of_vertices)
	{
		add_by_rotation(initial_vertex, number_of_vertices);
	}
	Polygon(double circunscribed_radius, int number_of_vertices)
	{
		add_by_rotation(Vect2{0, circunscribed_radius}, number_of_vertices);
	}

	//Operator
	Vect2 &operator[](int index)
	{
		return this->vertices[index];
	}

	// Get
	int count_vertices()
	{
		return this->vertices.size();
	}

	// Transform
	void move(Vect2 displacement)
	{
		this->translation = this->translation + displacement;

		for (Vect2 &vtx : this->vertices)
		{
			vtx = vtx + displacement;
		}
	}
	void scale(double factor)
	{
		for (Vect2 &vtx : this->vertices)
		{
			vtx = vtx + this->translation.invert();
			vtx = vtx * factor;
			vtx = vtx + this->translation;
		}
	}
	void rotate(double angle)
	{
		for (Vect2 &vtx : this->vertices)
		{
			vtx = vtx + this->translation.invert();
			vtx.rotate(angle);
			vtx = vtx + this->translation;
		}
	}

	// Utility
	void add_vertex(Vect2 vtx)
	{
		this->vertices.push_back(vtx);
	}
	void add_by_rotation(Vect2 first_vtx, int number_of_rotations)
	{
		double rotation_angle = 360.0 / number_of_rotations;
		for (int i = 0; i < number_of_rotations; i++)
		{
			Vect2 new_vertex{ first_vtx.get_x(), first_vtx.get_y() };
			this->vertices.push_back(first_vtx);
			first_vtx.rotate(rotation_angle);
		}
	}

};

class Rectangle:
	public Polygon
{
public:
	Rectangle() { add_by_rotation(Vect2{ -1,-1 }, 4); }
};

class Triangle :
	public Polygon
{
public:
	Triangle() { add_by_rotation(Vect2{ 0, -1 }, 3); }
};
