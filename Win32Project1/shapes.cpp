#include "stdafx.h"
#include "shapes.h"
#include "utils.h"

extern Shape* g_part;
extern double g_scale, g_offsetx, g_offsety;
static bool g_ClockwiseTopology;

d2D::d2D() {}
d2D::d2D(const d2D& other)
{
	x = other.x;
	y = other.y;
}

void EdgeShape::Draw(HDC hdc)
{
}

double EdgeShape::GetLength()
{
	d2D p0, p1;
	FindXYFromIndex(end0, p0);
	FindXYFromIndex(end1, p1);
	return d2DDist(p0, p1);
}

Line::Line()
{
	type = 1;
}

Line::Line(const Line& other)
{
	end0 = other.end0;
	end1 = other.end1;
}

double Line::ComputeCuttingCost()
{
	double time;
	d2D p0, p1;
	FindXYFromIndex(end0, p0);
	FindXYFromIndex(end1, p1);
	time = GetLength() / 0.5;
	return time;
}

void Line::Draw(HDC hdc)
{
	d2D p0, p1;
	FindXYFromIndex(end0, p0);
	FindXYFromIndex(end1, p1);
	POINT pnt0, pnt1;
	pnt0.x = (int)floor(g_scale * p0.x + g_offsetx);
	pnt0.y = (int)floor(g_scale * p0.y + g_offsety);
	pnt1.x = (int)floor(g_scale * p1.x + g_offsetx);
	pnt1.y = (int)floor(g_scale * p1.y + g_offsety);

	MoveToEx(hdc, pnt0.x, pnt0.y, NULL);
	LineTo(hdc, pnt1.x, pnt1.y);
}

CircularArc::CircularArc()
{
	type = 2;
}

CircularArc::CircularArc(const CircularArc& other)
{
	end0 = other.end0;
	end1 = other.end1;
	center.x = other.center.x;
	center.y = other.center.y;
	clockFrom = other.clockFrom;
}

void CircularArc::Draw(HDC hdc)
{
	d2D p0, p1;
	if (clockFrom == end0)
	{
		FindXYFromIndex(end0, p0);
		FindXYFromIndex(end1, p1);
	}
	else if (clockFrom == end1)
	{
		FindXYFromIndex(end1, p0);
		FindXYFromIndex(end0, p1);
	}

	d2D p, pLast;
	double rad = d2DDist(center, p0);
	double spanAng, ang1, ang2;

	d2D del1, del2;
	del1.x = p0.x - center.x;
	del1.y = p0.y - center.y;
	del2.x = p1.x - center.x;
	del2.y = p1.y - center.y;

	ang1 = atan2(del1.y, del1.x);
	ang2 = atan2(del2.y, del2.x);
	spanAng = ang2 - ang1;

	double startAng = ang1;
	double incr = spanAng / 18.0;
	if (incr > 0)
		incr *= -1;

	pLast.x = cos(startAng)*rad + center.x;
	pLast.y = sin(startAng)*rad + center.y;

	for (int j = 0; j < 19; j++)
	{
		double ang = startAng + j * incr;
		p.x = cos(ang)*rad + center.x;
		p.y = sin(ang)*rad + center.y;
		POINT pnt0, pnt1;
		pnt0.x = (int)floor(g_scale * pLast.x + g_offsetx);
		pnt0.y = (int)floor(g_scale * pLast.y + g_offsety);
		pnt1.x = (int)floor(g_scale * p.x + g_offsetx);
		pnt1.y = (int)floor(g_scale * p.y + g_offsety);

		MoveToEx(hdc, pnt0.x, pnt0.y, NULL);
		LineTo(hdc, pnt1.x, pnt1.y);

		pLast = p;
	}
}

double CircularArc::ComputeCuttingCost()
{
	double time;
	d2D p0, p1;
	FindXYFromIndex(end0, p0);
	double radius = d2DDist(p0, center);
	double al = GetLength();
	double speed = exp(-1 / radius) * 0.5;
	time = al / speed;
	return time;
}

double CircularArc::GetLength()
{
	d2D p0, p1;
	FindXYFromIndex(end0, p0);
	FindXYFromIndex(end1, p1);
	return ArcLengthJ(center, p0, p1);
}

Shape::~Shape()
{
	for (int i = 0; i < edgeCount; i++)
		delete edges[i];

	edges.clear();
	verts.clear();
}

double Shape::ComputeCuttingCost()
{
	double time = 0.0;
	for (int i = 0; i < edgeCount; i++)
		time = time + edges[i]->ComputeCuttingCost();
	return time;
}

double Shape::wind()
{
	int j;
	double accum = 0;
	d2D p0, p1, p2, p3;
	for (int i = 0; i < edgeCount; i++)
	{
		j = i + 1;
		if (j >= edgeCount)
			j = 0;

		FindXYFromIndex(edges[i]->end0, p0);
		FindXYFromIndex(edges[i]->end1, p1);
		FindXYFromIndex(edges[j]->end0, p2);
		if ((p1.x != p2.x) || (p1.y != p2.y))
			MessageBox(0, NULL, _T("Invalid ClockwiseFrom Index"),
				MB_OK | MB_OKCANCEL);
		FindXYFromIndex(edges[j]->end1, p3);
		accum = accum + turn(p0, p1, p2, p3);
	}
	return accum;
}

void Shape::ComputeBounds(double &xmin, double &xmax, double &ymin, double &ymax)
{
	double windDir = wind();
	if (windDir < 0)
		g_ClockwiseTopology = true;
	else
		g_ClockwiseTopology = false;

	xmin = 1E20;
	xmax = -1E20;
	ymin = 1E20;
	ymax = -1E20;
	d2D p0, p1, c;
	for (int i = 0; i < edgeCount; i++)
	{
		if (edges[i]->type == 1)
		{
			FindXYFromIndex(edges[i]->end0, p0);
			FindXYFromIndex(edges[i]->end1, p1);
			if (p0.x < xmin)
				xmin = p0.x;
			if (p0.x > xmax)
				xmax = p0.x;
			if (p0.y < ymin)
				ymin = p0.y;
			if (p0.y > ymax)
				ymax = p0.y;

			if (p1.x < xmin)
				xmin = p1.x;
			if (p1.x > xmax)
				xmax = p1.x;
			if (p1.y < ymin)
				ymin = p1.y;
			if (p1.y > ymax)
				ymax = p1.y;

		}
		else if (edges[i]->type == 2)
		{
			CircularArc* arc = (CircularArc*)edges[i];
			int clockFrom = arc->clockFrom;
			if (g_ClockwiseTopology)
			{
				if (clockFrom != arc->end0)
					continue;
			}
			else
			{
				if (clockFrom != arc->end1)
					continue;

			}
			c = arc->center;

			FindXYFromIndex(arc->end0, p0);
			double radius = d2DDist(p0, c);


			if (c.x + radius < xmin)
				xmin = c.x + radius;
			if (c.x + radius > xmax)
				xmax = c.x + radius;
			if (c.y + radius < ymin)
				ymin = c.y + radius;
			if (c.y + radius > ymax)
				ymax = c.y + radius;

			if (c.x - radius < xmin)
				xmin = c.x - radius;
			if (c.x - radius > xmax)
				xmax = c.x - radius;
			if (c.y - radius < ymin)
				ymin = c.y - radius;
			if (c.y - radius > ymax)
				ymax = c.y - radius;
		}
	}

}

void Shape::DrawEdges(HDC hdc)
{
	for (int i = 0; i < edgeCount; i++)
		edges[i]->Draw(hdc);
}

void Shape::ComputeScale(HWND hwnd, d2D &min, d2D &max)
{
	if (!g_part)
		return;
	RECT rect;
	g_part->ComputeBounds(min.x, max.x, min.y, max.y);
	max.x = max.x + 0.1;
	max.y = max.y + 0.1;

	GetClientRect(hwnd, &rect);
	double w = (max.x - min.x)*1.1;
	double h = (max.y - min.y)*1.1;
	int rw = rect.right - rect.left;
	int rh = rect.bottom - rect.top;
	if (w == 0) w = 1;
	if (h == 0) h = 1;
	double xScale = rw / w;
	double yScale = rh / h;

	if (xScale > yScale)
		g_scale = yScale;
	else
		g_scale = xScale;

	double drawingCenterx = g_scale*(max.x + min.x) / 2.0;
	double drawingCentery = g_scale*(max.y + min.y) / 2.0;
	int rectCenterx = (rect.right + rect.left) / 2;
	int rectCentery = (rect.bottom - rect.top) / 2;


	g_offsetx = rectCenterx - drawingCenterx + 5;
	g_offsety = rectCentery - drawingCentery;
}

void Shape::CostFunction(double area)
{
	double time = ComputeCuttingCost();
	double materialCost = 0.75 * area;
	double cuttingCost = time * 0.07;
	double cost = materialCost + cuttingCost;
	cost = cost*100.0;
	int icost = (int)floor(round(cost) + 0.5);
	char ccost[100];
	sprintf_s(ccost, "%d", icost);
	size_t len = strlen(ccost);
	char dollars[100];
	char cents[100];
	char outStr[100];
	int i = 0;
	int j = 0;
	while (i < len - 2)
		dollars[j++] = ccost[i++];
	dollars[j] = NULL;
	j = 0;
	while (ccost[i] != NULL)
		cents[j++] = ccost[i++];
	cents[j] = NULL;
	strstr("b", "p");
	sprintf_s(outStr, "Cost= $%s.%s", dollars, cents);
	std::wstring stemp = s2ws(outStr);
	LPCWSTR result = stemp.c_str();

	MessageBox(0, result, _T("COST = Material Cost + Cutting Cost"),
		MB_OK | MB_OKCANCEL);
}


Vertex::Vertex() {}

Vertex::Vertex(const Vertex& other)
{
	index = other.index;
	dp.x = other.dp.x;
	dp.y = other.dp.y;
}





