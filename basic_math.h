#pragma once
/*
 * Author: Jaime Rivera
 * Date : 2020.04.20
 * Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
 * Brief: Basic implementations of math objects for 2D and 3D transformations
 */

#define _USE_MATH_DEFINES
#include <math.h>

// --------- TRIGONOMETRY --------- //
double rad_sin(double a) { return sin(a * M_PI / 180.0); }
double rad_cos(double a) { return cos(a * M_PI / 180.0); }

// --------- 3D SPACE --------- //
class Vect3
{
private:
	double x, y, z;

public:
	// Constructors
	Vect3() : x(0), y(0), z(0) {}
	Vect3(double given_x, double given_y, double given_z) : x(given_x), y(given_y), z(given_z) {}

	// Axes
	static const Vect3 XAxis;
	static const Vect3 YAxis;
	static const Vect3 ZAxis;

	// Operators
	Vect3 operator+(const Vect3 &right)
	{
		double new_x = this->x + right.x;
		double new_y = this->y + right.y;
		double new_z = this->z + right.z;
		return Vect3{new_x, new_y, new_z};
	}
	Vect3 operator*(const double &factor)
	{
		double new_x = factor * this->x;
		double new_y = factor * this->y;
		double new_z = factor * this->z;
		return Vect3{new_x, new_y, new_z};
	}

	// Get
	double get_x() { return x; }
	double get_y() { return y; }
	double get_z() { return z; }
	double get_magnitude() { return sqrt(x * x + y * y + z * z); }

	// Set
	void set_x(double new_x) { this->x = new_x; }
	void set_y(double new_y) { this->y = new_y; }
	void set_z(double new_z) { this->z = new_z; }
	void normalize()
	{
		double mag = this->get_magnitude();
		if (mag != 1.0)
		{
			this->x /= mag;
			this->y /= mag;
			this->y /= mag;
		}
	}

	// Utility
	Vect3 get_inverted()
	{
		return Vect3{this->get_x() * -1.0, this->get_y() * -1, this->get_z() * -1};
	}
	double get_distance(Vect3 other)
	{
		double x1 = this->get_x();
		double y1 = this->get_y();
		double z1 = this->get_z();
		double x2 = other.get_x();
		double y2 = other.get_y();
		double z2 = other.get_z();

		double dist = sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0) + pow((z2 - z1), 2.0));
		return dist;
	}
	Vect3 get_midpoint(Vect3 other)
	{
		double x1 = this->get_x();
		double y1 = this->get_y();
		double z1 = this->get_z();
		double x2 = other.get_x();
		double y2 = other.get_y();
		double z2 = other.get_z();

		return Vect3{(x2 + x1) / 2, (y2 + y1) / 2, (z2 + z1) / 2};
	}
};
const Vect3 Vect3::XAxis{1.0, 0.0, 0.0};
const Vect3 Vect3::YAxis{0.0, 1.0, 0.0};
const Vect3 Vect3::ZAxis{0.0, 0.0, 1.0};

class Matrix3by3
{
private:
	double a00, a01, a02,
		a10, a11, a12,
		a20, a21, a22;

public:
	Matrix3by3() : a00(0), a01(0), a02(0),
				   a10(0), a11(0), a12(0),
				   a20(0), a21(0), a22(0) {}

	Matrix3by3(double in_a00, double in_a01, double in_a02,
			   double in_a10, double in_a11, double in_a12,
			   double in_a20, double in_a21, double in_a22) : a00(in_a00), a01(in_a01), a02(in_a02),
															  a10(in_a10), a11(in_a11), a12(in_a12),
															  a20(in_a20), a21(in_a21), a22(in_a22) {}

	// Predefined matrices
	static const Matrix3by3 IdentityMatrix;
	static Matrix3by3 RotationMatrix(double angle, Vect3 axis);

	// Get
	Vect3 row_0() { return Vect3{this->a00, this->a01, this->a02}; }
	Vect3 row_1() { return Vect3{this->a10, this->a11, this->a12}; }
	Vect3 row_2() { return Vect3{this->a20, this->a21, this->a22}; }
};
const Matrix3by3 Matrix3by3::IdentityMatrix{1.0, 0.0, 0.0,
											0.0, 1.0, 0.0,
											0.0, 0.0, 1.0};
Matrix3by3 Matrix3by3::RotationMatrix(double angle, Vect3 axis)
{
	double cosO = rad_cos(angle);
	double sinO = rad_sin(angle);

	axis.normalize();
	double ux = axis.get_x();
	double uy = axis.get_y();
	double uz = axis.get_z();

	return Matrix3by3{cosO + ux * ux * (1 - cosO), ux * uy * (1 - cosO) - uz * sinO, ux * uz * (1 - cosO) + uy * sinO,
					  uy * ux * (1 - cosO) + uz * sinO, cosO + uy * uy * (1 - cosO), uy * uz * (1 - cosO) - ux * sinO,
					  uz * ux * (1 - cosO) - uy * sinO, uz * uy * (1 - cosO) + ux * sinO, cosO + uz * uz * (1 - cosO)};
}

// --------- 2D SPACE --------- //
class Vect2
{
private:
	double x, y;

public:
	// Constructors
	Vect2() : x(0), y(0) {}
	Vect2(double given_x, double given_y) : x(given_x), y(given_y) {}

	// Operators
	Vect2 operator+(const Vect2 &right)
	{
		double new_x = this->x + right.x;
		double new_y = this->y + right.y;
		return Vect2{new_x, new_y};
	}
	Vect2 operator*(const double &factor)
	{
		double new_x = factor * this->x;
		double new_y = factor * this->y;
		return Vect2{new_x, new_y};
	}

	// Get
	double get_x() { return x; }
	double get_y() { return y; }

	// Set
	void set_x(double new_x) { this->x = new_x; }
	void set_y(double new_y) { this->y = new_y; }

	// Transformations
	void rotate(double angle)
	{
		double new_x = this->x * rad_cos(angle) - this->y * rad_sin(angle);
		double new_y = this->x * rad_sin(angle) + this->y * rad_cos(angle);

		this->x = new_x;
		this->y = new_y;
	}
	void rotate_around(Vect2 pivot, double angle)
	{
		double new_x = rad_cos(angle) * (this->x - pivot.get_x()) - rad_sin(angle) * (this->y - pivot.get_y()) + pivot.get_x();
		double new_y = rad_sin(angle) * (this->x - pivot.get_x()) + rad_cos(angle) * (this->y - pivot.get_y()) + pivot.get_y();

		this->x = new_x;
		this->y = new_y;
	}

	// Utility
	Vect2 invert()
	{
		return Vect2{this->get_x() * -1.0, this->get_y() * -1};
	}
	double get_distance(Vect2 other)
	{
		double x1 = this->get_x();
		double y1 = this->get_y();
		double x2 = other.get_x();
		double y2 = other.get_y();

		double dist = sqrt(pow((x2 - x1), 2.0) + pow((y2 - y1), 2.0));
		return dist;
	}
};

// --------- OPERATIONS --------- //
double dot_prod(Vect3 v1, Vect3 v2)
{
	return v1.get_x() * v2.get_x() + v1.get_y() * v2.get_y() + v1.get_z() * v2.get_z();
}
Vect3 mult_matrix_by_vector3(Matrix3by3 m, Vect3 v)
{
	double new_x = dot_prod(m.row_0(), v);
	double new_y = dot_prod(m.row_1(), v);
	double new_z = dot_prod(m.row_2(), v);

	return Vect3{new_x, new_y, new_z};
}