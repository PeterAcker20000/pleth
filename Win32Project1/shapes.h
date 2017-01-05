#pragma once

#include "resource.h"
#include <vector>
#include <windows.h>
#define PI 3.1415926535897932 


class d2D
{
public:
	d2D();
	d2D(const d2D& other);
	void operator=(const d2D &p) { x = p.x; y = p.y; }

	double x, y;
};

class Vertex
{
public:
	Vertex();
	Vertex(const Vertex& other);
	void SetIndex(int i) { index = i; }
	void SetPoint(d2D p) { dp = p; }

	int index;
	d2D dp;
};

class EdgeShape
{
public:
	double virtual GetLength();
	void virtual Draw(HDC hdc);
	double virtual ComputeCuttingCost() { return 0; }
	void SetEnd0(int i) { end0 = i; }
	void SetEnd1(int i) { end1 = i; }
	void SetIndex(int i) { index = i; }

	int end0, end1;
	int type;
	int index;

};
class Line : public EdgeShape
{
public:
	Line();
	Line(const Line& other);
	void Draw(HDC hdc);
	double ComputeCuttingCost();
};
class CircularArc : public EdgeShape
{
public:
	CircularArc();
	CircularArc(const CircularArc& other);
	void Draw(HDC hdc);
	double ComputeCuttingCost();
	double GetLength();
	

	void SetCenter(d2D c) { center = c; }
	void SetClockFrom(int cf) { clockFrom = cf; }
	d2D center;
	int clockFrom;
};

class Shape
{
public:
	~Shape();
	double ComputeCuttingCost();
	double wind();
	void ComputeBounds(double &xmin, double &xmax, double &ymin, double &ymax);
	void DrawEdges(HDC hdc);
	void ComputeScale(HWND hwnd, d2D &min, d2D &max);
	void CostFunction(double area);
	
	
	std::vector<EdgeShape*> edges;
	std::vector<Vertex> verts;
	int edgeCount;
	int vertCount;
};

double d2DDist(const d2D &p0, const d2D &p1);
bool FindXYFromIndex(int index, d2D &p);

