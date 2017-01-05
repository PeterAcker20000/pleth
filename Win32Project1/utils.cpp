#include "stdafx.h"
#include "utils.h"
#include "shapes.h"
extern Shape* g_part;
bool FindXYFromIndex(int index, d2D &p)
{
	bool found = false;
	int i = 0;
	while (!found && i < g_part->vertCount)
	{
		if (index == g_part->verts[i].index)
		{
			found = true;
			p = g_part->verts[i].dp;
		}
		i++;
	}
	if (!found)
		MessageBox(0, NULL, _T("Invalid Index"),
			MB_OK | MB_OKCANCEL);

	return found;
}

double d2DDist(const d2D &p0, const d2D &p1)
{
	return sqrt((p1.x - p0.x)*(p1.x - p0.x) + (p1.y - p0.y)*(p1.y - p0.y));
}
double turn(const d2D &p0, const d2D &p1, const d2D &p2, const d2D &p3)
{
	double ang, ang1, ang2;

	d2D del1, del2;
	del1.x = p1.x - p0.x;
	del1.y = p1.y - p0.y;
	del2.x = p3.x - p2.x;
	del2.y = p3.y - p2.y;

	ang1 = atan2(del1.y, del1.x);
	ang2 = atan2(del2.y, del2.x);
	ang = ang2 - ang1;
	ang = ang / PI * 180.0;
	if (ang > 180)
		ang = ang - 360;
	else if (ang < -180)
		ang = ang + 360;
	return ang;
}
double ArcLengthJ(const d2D &c, const d2D & p0, const d2D & p1)
{
	double ang, ang1, ang2;
	d2D q0, q1;
	q0.x = p0.x - c.x;
	q0.y = p0.y - c.y;
	q1.x = p1.x - c.x;
	q1.y = p1.y - c.y;


	ang1 = atan2(q0.y, q0.x);
	ang2 = atan2(q1.y, q1.x);
	ang = ang2 - ang1;
	if (ang < 0)
		ang = -1.0*ang;
	double radius = sqrt(q0.x*q0.x + q0.y*q0.y);
	return ang * radius;

}
std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}