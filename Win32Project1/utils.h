#pragma once
#include "shapes.h"
bool FindXYFromIndex(int index, d2D &p);
double d2DDist(const d2D &p0, const d2D &p1);
double turn(const d2D &p0, const d2D &p1, const d2D &p2, const d2D &p3);
double ArcLengthJ(const d2D &c, const d2D & p0, const d2D & p1);
std::wstring s2ws(const std::string& s);
