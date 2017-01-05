#include "stdafx.h"
#include "OpenFileDialog.h"
#include "CostCalc.h"
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "shapes.h"


#include <vector>

using namespace std;

double g_scale, g_offsetx, g_offsety;
Shape* g_part; 

void DrawScene(HWND hwnd, HDC hdc)
{
	if (!g_part)
		return;

	g_part->DrawEdges(hdc);

}
bool ParenBalance(string& line, int &paren, int parenSave, bool &going)
{
	if (strstr(line.c_str(), "{"))	paren++;
	if (strstr(line.c_str(), "}"))	paren--;
	if (paren == parenSave)
	{
		going = false;
		return true;
	}
	return false;
}

int ExtractIndex(string line)
{
	size_t off0, off1;
	string number;
	int index;
	off0 = line.find('"');
	off1 = line.find('"', off0 + 1);
	number = line.substr(off0 + 1, off1 - off0 - 1);
	index = atoi(number.c_str());
	return index;
}
int ExtractIndex1(string line)
{
	size_t off0, off1;
	int index;
	string number;
	off0 = line.find_last_of('\t');
	off1 = line.find(',', off0 + 1);
	number = line.substr(off0 + 1, off1 - off0 - 1);
	index = atoi(number.c_str());
	return index;
}
int ExtractIndex2(string line)
{
	size_t off0, off1;
	string number;
	size_t len = line.length();
	off0 = line.find(':');
	off1 = line.find('"', off0 + 1);
	number = line.substr(off0 + 1, len);
	int index = atoi(number.c_str());
	return index;
}

double ExtractFloat(string line)
{
	size_t off0, off1;
	string number;
	off0 = line.find(':');
	off1 = line.find(',', off0 + 1);
	number = line.substr(off0 + 1, off1 - off0 - 1);
	double d = atof(number.c_str());
	return d;
}


int readFile(LPWSTR fName)
{
	ifstream myfile(fName);
	string line;
	bool findingEdges = false;
	bool findingVerts = false;
	bool going = true;
	int paren = 0;
	int parenSave = 0;
	g_part->vertCount = 0;
	g_part->edgeCount = 0;
	d2D p;

	if (myfile.is_open())
	{
		while (going)
		{
			getline(myfile, line);
			if (ParenBalance(line, paren, 0, going)) continue;

			if (strstr(line.c_str(), "Vertices"))
			{
				parenSave = paren - 1;
				findingVerts = true;
				while (findingVerts)
				{
					Vertex vert;
					getline(myfile, line);
					if (ParenBalance(line, paren, parenSave, findingVerts))	continue;
					int index = ExtractIndex(line);
					vert.SetIndex(index);
					getline(myfile, line);
					if (ParenBalance(line, paren, parenSave, findingVerts))	continue;
					getline(myfile, line);
					p.x = ExtractFloat(line);
					getline(myfile, line);
					p.y = ExtractFloat(line);
					vert.SetPoint(p);
					g_part->verts.push_back(vert);
					g_part->vertCount++;
					getline(myfile, line);
					if (ParenBalance(line, paren, parenSave, findingVerts))	continue;
					getline(myfile, line);
					if (ParenBalance(line, paren, parenSave, findingVerts))	continue;
				}
			}
			else if (strstr(line.c_str(), "Edges"))
			{
				parenSave = paren - 1;
				findingEdges = true;
				while (findingEdges)
				{
					getline(myfile, line);
					if (ParenBalance(line, paren, parenSave, findingEdges))	continue;
					int indexMain = ExtractIndex(line);
					getline(myfile, line);
					if (strstr(line.c_str(), "LineSegment"))
					{
						Line *l = new Line();
						l->SetIndex(indexMain);
						getline(myfile, line);
						getline(myfile, line);
						l->SetEnd0(ExtractIndex1(line));
						getline(myfile, line);
						l->SetEnd1(ExtractIndex1(line));
						g_part->edgeCount++;
						g_part->edges.push_back(l);
						getline(myfile, line);
						getline(myfile, line);
						if (ParenBalance(line, paren, parenSave, findingEdges))	continue;
					}
					else if (strstr(line.c_str(), "Arc"))
					{
						CircularArc *a = new CircularArc();
						getline(myfile, line);
						getline(myfile, line);

						a->SetIndex(indexMain);
						int index = ExtractIndex1(line);
						a->SetEnd0(index);
						getline(myfile, line);
						index = ExtractIndex1(line);
						a->SetEnd1(index);
						getline(myfile, line);
						getline(myfile, line);
						if (ParenBalance(line, paren, parenSave, findingEdges))	continue;
						getline(myfile, line);
						p.x = ExtractFloat(line);
						getline(myfile, line);
						p.y = ExtractFloat(line);
						a->SetCenter(p);
						getline(myfile, line);
						if (ParenBalance(line, paren, parenSave, findingEdges))	continue;
						getline(myfile, line);
						index = ExtractIndex2(line);
						a->SetClockFrom(index);
						g_part->edges.push_back(a);
						g_part->edgeCount++;
						getline(myfile, line);
						if (ParenBalance(line, paren, parenSave, findingEdges))	continue;
					}
				}
			}
		}
		myfile.close();
	}
	else cout << "Unable to open file";

	return 0;
}


void OpenJSONFile()
{
	if (g_part)
		delete g_part;
	g_part = new Shape();

	OpenFileDialog* openFileDialog1 = new OpenFileDialog();
	openFileDialog1->FilterIndex = 1;
	openFileDialog1->Flags |= OFN_SHOWHELP;
	openFileDialog1->InitialDir = _T("C:\\JSON\\");
	openFileDialog1->Title = _T("Open JSON File");


	if (openFileDialog1->ShowDialog())
	{
		int a = 0;

		readFile(openFileDialog1->FileName);

	}
}



void InitShape()
{
	g_part = NULL;
}
