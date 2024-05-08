#pragma once
/*
* Author: Jaime Rivera
* Date : 2020.04.20
* Copyright : Copyright 2020 Jaime Rivera | www.jaimervq.com
* Brief: Basic implementation of a rgba color
*/


#include <algorithm>
#include <string>


// --------- BASIC COLOR --------- //
class BasicColor
{
private:
	double r, g, b, a;

public:
	// Constructors
	BasicColor() : r(0.0), g(0.0), b(0.0), a(0.0) {}
	BasicColor(double input_r, double input_g, double input_b) : r(input_r), g(input_g), b(input_b), a(1.0) {}
	BasicColor(double input_r, double input_g, double input_b, double input_a) : r(input_r), g(input_g), b(input_b), a(input_a) {}

	// Predefined colors
	static const BasicColor White;    static const BasicColor Black; 
	static const BasicColor Red;      static const BasicColor Cyan; 
	static const BasicColor Green;    static const BasicColor Magenta; 
	static const BasicColor Blue;     static const BasicColor Yellow;

	// Operators
	BasicColor operator + (const BasicColor &right)
	{
		double new_r = this->r + right.r;
		double new_g = this->g + right.g;
		double new_b = this->b + right.b;
		double new_a = this->a + right.a;
		return BasicColor{ new_r, new_g, new_b, new_a };
	}
	BasicColor operator - (const BasicColor &right)
	{
		double new_r = this->r - right.r;
		double new_g = this->g - right.g;
		double new_b = this->b - right.b;
		double new_a = this->a - right.a;
		return BasicColor{ new_r, new_g, new_b, new_a };
	}
	BasicColor operator * (const BasicColor &right)
	{
		double new_r, new_g, new_b, new_a;
		if (this->r < 0.0 && right.r < 0.0) new_r = this->r; else new_r = this->r * right.r;
		if (this->g < 0.0 && right.g < 0.0) new_g = this->g; else new_g = this->g * right.g;
		if (this->b < 0.0 && right.b < 0.0) new_b = this->b; else new_b = this->b * right.b;
		if (this->a < 0.0 && right.a < 0.0) new_a = this->a; else new_a = this->a * right.a;
		return BasicColor{ new_r, new_g, new_b, new_a };
	}

	// Blend modes and IDs
	BasicColor plus(const BasicColor &right)
	{
		return *this + right;
	}
	BasicColor minus(const BasicColor &right)
	{
		return *this - right;
	}
	BasicColor multiply(const BasicColor &right)
	{
		return *this * right;
	}
	BasicColor over(const BasicColor &right)
	{
		double new_r = this->r + (right.r * (1.0 - this->a) );
		double new_g = this->g + (right.g * (1.0 - this->a) );
		double new_b = this->b + (right.b * (1.0 - this->a) );
		double new_a = this->a + (right.a * (1.0 - this->a) ); // Todo revisit this
		return BasicColor{ new_r, new_g, new_b, new_a };
	}
	BasicColor unpremultiplied_over(const BasicColor &right)
	{
		double new_r = (this->r * this->a) + (right.r * (1.0 - this->a));
		double new_g = (this->g * this->a) + (right.g * (1.0 - this->a));
		double new_b = (this->b * this->a) + (right.b * (1.0 - this->a));
		double new_a = std::max(this->a, right.a); // Todo revisit this
		return BasicColor{ new_r, new_g, new_b, new_a };
	}

	static const int plus_ID{ 0 };
	static const int minus_ID{ 1 };
	static const int multiply_ID{ 2 };
	static const int over_ID{ 3 };
	static const int unpremultiplied_over_ID{ 4 };

	// Get
	double get_r() { return r; }   double get_g() { return g; }   double get_b() { return b; }   double get_a() { return a; }

	// Get (clipped)
	double r01() { if (r < 0.0) return 0.0; else if (r > 1.0) return 1.0; return r; }
	double g01() { if (g < 0.0) return 0.0; else if (g > 1.0) return 1.0; return g; }
	double b01() { if (b < 0.0) return 0.0; else if (b > 1.0) return 1.0; return b; }
	double a01() { if (a < 0.0) return 0.0; else if (a > 1.0) return 1.0; return a; }

	int r255() { if (r < 0.0) return 0; else if (r > 1.0) return 255; return (int)(r * 255); }
	int g255() { if (g < 0.0) return 0; else if (g > 1.0) return 255; return (int)(g * 255); }
	int b255() { if (b < 0.0) return 0; else if (b > 1.0) return 255; return (int)(b * 255); }
	int a255() { if (a < 0.0) return 0; else if (a > 1.0) return 255; return (int)(a * 255); }
};
const BasicColor BasicColor::White { 1.0, 1.0, 1.0, 1.0 };     const BasicColor BasicColor::Black   { 0.0, 0.0, 0.0, 1.0 }; 
const BasicColor BasicColor::Red   { 1.0, 0.0, 0.0, 1.0 };     const BasicColor BasicColor::Cyan    { 0.0, 1.0, 1.0, 1.0 }; 
const BasicColor BasicColor::Green { 0.0, 1.0, 0.0, 1.0 };     const BasicColor BasicColor::Magenta { 1.0, 0.0, 1.0, 1.0 }; 
const BasicColor BasicColor::Blue  { 0.0, 0.0, 1.0, 1.0 };     const BasicColor BasicColor::Yellow  { 1.0, 1.0, 0.0, 1.0 };


// --------- GENERAL BLEND --------- //
BasicColor blend_two_colors(BasicColor A, int blend_type_ID, BasicColor B)
{
	switch (blend_type_ID)
	{
	case BasicColor::plus_ID:
		return A.plus(B);
		break;
	case BasicColor::minus_ID:
		return A.minus(B);
		break;
	case BasicColor::multiply_ID:
		return A.multiply(B);
		break;
	case BasicColor::over_ID:
		return A.over(B);
		break;
	case BasicColor::unpremultiplied_over_ID:
		return A.unpremultiplied_over(B);
		break;
	default:
		return A.plus(B);
		break;
	}
}