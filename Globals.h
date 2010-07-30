#include <afx.h>
#include <afxdlgs.h>

#include <d3dx8.h>
#include <d3dx8core.h>
#include "Auxiliary/d3dfont.h"

#include <malloc.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <commctrl.h>
#include <mmsystem.h>

const float PI = 3.141692f;

enum PHIMETHODS {LINEAR};
#define PHIMETHOD LINEAR

struct CUSTOMVERTEX
{
    FLOAT x, y, z;		// The transformed position for the vertex.
    DWORD color;        // The vertex color.
};

#define D3DFVF_CUSTOMVERTEX (D3DFVF_XYZ | D3DFVF_DIFFUSE)

RECT Rects[] =
{
	// REGIONS
	{0,		0,		500,	350},
	{0,		350,	500,	600},

	{500,	0,		800,	200},
	{500,	200,	800,	400},
	{500,	400,	800,	600},

	// MENU REGIONS
	{-103,	-45,	-35,	-10},	// NW
	{-35,	-45,	 40,	-10},	// N
	{ 40,	-45,	103,	-10},	// NE
	{-103,	-10,	-35,	 20},	// W
	{-103,	 20,	-35,	 55},	// SW
	{-35,	 20,	 40,	 55},	// S
	{ 40,	 20,	103,	 55},	// SE
	{ 40,	-10,	103,	 20},	// E
};

struct RENDERINFO
{
	bool ScreenActive;				// Shows if the main window has the focus
	bool OnScreenMenuActive;		// The rightclick menues should be displayed too
	bool LMBDown;					// Left mouse button down
	bool RMBDown;					// Right mouse button down
	bool MBDDown;					// Middle mouse button down

	int ActiveNode;					// Indicator of the selected ray sample

	// Click positions stored
	POINT realleftclickPosition;
	POINT leftclickPosition;
	POINT rightclickPosition;
	POINT currentRightPosition;
	POINT middleclickPosition;

	// Movement information
	float Rotation[2];
	float MoveRayPoint;
	float MovePhiPoint, MoveThetaPoint, MoveRadialPoint;

	// In case we needed these
	HWND WindowHandle;
	HINSTANCE WindowInstance;
} RenderInfo;

// Create new field information
struct NewFieldInfo
{
	bool fromFile;
	int phi, theta, radial;
	char filename[400];
};

struct ADMININFO
{
	float alpha, h, a, b, halpha;
};

struct COMMANDINFO
{
	// Field menu
	// NEW FIELD
	bool NewPotentialField;
	bool ReallyNewPotentialField;
	struct NewFieldInfo newField;

	bool LoadPotentialField;
	bool SavePotentialField;
	bool SavePotentialFieldAs;
	bool SelectRay;

	struct ADMININFO AdminInfo;

	bool NextSensor;
	bool PrevSensor;
	bool AddSensor;

	bool LoadRayFunction;
	bool SaveRayFunction;
	bool ResetRay;
	bool ChangeRes;
	int ChangeResNum;
	bool DeleteRay;

	// Add sensor information
	bool ReallyAddSensor;
	float orient[2];
	float length;
	int divs;
	bool AddSensorFromFile;
	char SensorFileName[400];

	// Filter management
	bool Numeric;
	double NewParam;
	int filterno;
	bool RefreshFilter;
	bool NewType;
	int newtype;
	
	// Preferences
	bool ChangeFont;

	bool Exit;
} CommandInfo;

struct PREFERENCES
{
	DWORD BackgroundColor;
	char Font[100];
	int FontSize;
};

struct PREFERENCES Preferences = {0xff000000, "Impact", 12};

DWORD idleTime = 0x0;
bool DisplayHelper;

float CalculatePhi (float point, float before, float after, enum PHIMETHODS method)
{
	float ret, length;

	length = after - before;

	switch (method)
	{
		case LINEAR: ret = 1 - (point - before) / length;
					 break;
	}

	return (ret);
}

bool Spherical2Descartean (float *out, float phitheta[2], float r)
{
	if (!out) return (false);

	float pt[2] = {phitheta[0] * PI / 180.0f, phitheta[1] * PI / 180.0f};
	
	out[0] = (float) (r * cos ((double) pt[1]) * sin ((double) pt[0]));
	out[2] = (float) (r * cos ((double) pt[1]) * cos ((double) pt[0]));
	out[1] = (float) (r * sin ((double) pt[1]));
	
	return (true);
}

DWORD ToColor (float sample)
{
	int R, G, B;

	if (sample < 0.0f) 
	{
		sample = -1 * sample;
		B = 0xff; G = R = (int) ((10.0f - sample) * 25.5f);
	}
	else 
	{
		R = 0xff; 
		G = B = (int) ((10.0f - sample) * 25.5f);
	}

	if (R > 255 || R < 0) R = 255;
	if (G > 255 || G < 0) G = 255;
	if (B > 255 || B < 0) B = 255;

	return (D3DCOLOR_ARGB(0xff, R, G, B));
}

void Are_You_Sure ()
{
	if (MessageBoxEx (HWND_DESKTOP, "Are you sure?", "Really quit?", MB_YESNO | MB_ICONQUESTION, 0) == 6)
	{
		CommandInfo.Exit = true;
		PostQuitMessage (0);
	} 
}

bool SelectCursor (int no)
{
	switch (no)
	{
		case ID_NORMAL_CURSOR:	SetCursor (LoadCursorFromFile(ID_NORMAL_CURSOR_STRING));
								break;
		case ID_MOVE_CURSOR_1:	SetCursor (LoadCursor(NULL, MAKEINTRESOURCE(IDC_SIZEALL)));
								break;
		case ID_MOVE_CURSOR_2:	SetCursor (LoadCursorFromFile(ID_MOVE_CURSOR_2_STRING));
								break;
		case ID_MENU_CURSOR:	SetCursor (LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW)));
								break;
	}
	ShowCursor (TRUE);

	return (true);
}

int Get_Area_Code (int ix, int iy, int Type = ID_AREA_CODE)
{
	LONG x = (LONG) ix, y = (LONG) iy;
	int LOW, HIGH;

	switch (Type)
	{
		case ID_AREA_CODE:
			LOW = 0;
			HIGH = 5;
			break;
		case ID_MENU_CODE:
			LOW = ID_MENU_NW;
			HIGH = ID_MENU_E + 1;
			break;
	}

	int ret;
	for (int i = LOW; i < HIGH; i++)
		if (y < Rects[i].bottom && y > Rects[i].top && x > Rects[i].left && x < Rects[i].right) ret = i;

	return (ret);
}

CString defPotExt = _T("pf");
CString defRayExt = _T("sf");

bool Load_Save_Dialog (CString *fileName, bool opensave, bool potential)
{
	if (!fileName) return (false);

	CFileDialog dlgFile (opensave);
	CString title;
	CString strFilter;
	CString allFilter;
	
	strFilter += (potential) ? "Potential fields (*.pf)" : "Sensor functions (*.sf)";
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += (potential) ? _T("*.pf") : _T("*.sf");
	strFilter += (TCHAR)'\0';  
	strFilter += "Text files (*.txt)";
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.txt");
	strFilter += (TCHAR)'\0';   // last string

	dlgFile.m_ofn.nMaxCustFilter += 2;

	VERIFY(allFilter.LoadString (AFX_IDS_ALLFILTER));
	strFilter += allFilter;
	strFilter += (TCHAR)'\0';   // next string please
	strFilter += _T("*.*");
	strFilter += (TCHAR)'\0';   // last string
	dlgFile.m_ofn.nMaxCustFilter++;
		
	if (opensave) VERIFY(title.LoadString (AFX_IDS_OPENFILE));
	else VERIFY(title.LoadString (AFX_IDS_SAVEFILE));

	dlgFile.m_ofn.lpstrFilter = strFilter;
	dlgFile.m_ofn.lpstrTitle = title;
	dlgFile.m_ofn.lpstrFile = (*fileName).GetBuffer(_MAX_PATH);

	dlgFile.m_ofn.lpstrDefExt = (potential) ? defPotExt : defRayExt;
	INT_PTR nResult = dlgFile.DoModal();
		
	if (nResult != IDOK) return (false);

	return (true);
}

bool Progress (int max, int current)
{
	char PercentString[50];
	float PercentComplete = 100.0f / max * current;
	sprintf (PercentString, "Completed: %3.2f", PercentComplete);
	SetWindowText (RenderInfo.WindowHandle, PercentString);
	return (true);
}