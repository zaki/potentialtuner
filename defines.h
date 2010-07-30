int		ID_WINDOW_WIDTH		=	800;
int		ID_WINDOW_HEIGHT	=	600;
bool	ID_WINDOWED_MODE	=	true;
bool	SHOULD_DISPLAY_POINTS	= false;

int ID_FILTER_RESOLUTION		= 50;

struct POSITIONS
{
	float x;
	float y;
};

struct MENUES
{
	float x;
	float y;
	char *MenuName;
};

#define ID_MAIN_MENU_ITEMS		9
#define ID_RAY_MENU_ITEMS		18
#define ID_FILTER_MENU_ITEMS	25

#define	ID_AREA_CODE			1001
#define ID_MENU_CODE			1002

#define ID_HELPER_TIMEOUT		30000

#define ID_NORMAL_CURSOR_STRING	"rghtclck.cur"
#define ID_MOVE_CURSOR_2_STRING	"movewe.cur"

#define ID_ACTIVE_TITLE			"Active"
#define ID_INACTIVE_TITLE		"Inactive"

#define ID_WRONG_DXVERSION		"You don't have the appropriate\nversion of DirectX\nDirectX 8.0 or later is required"
#define ID_FAILED_DEVICE		"Failed to create Direct3D Device"
#define ID_FAILED_DISPLAYMODE	"Failed to get adapter display mode"
#define ID_FAILED_HAL			"Failed to create Hardware accelerated D3D device\nSwitching to the slower software device"

#define ID_DEFAULT_PF_NAME		"untitled.pf"
#define ID_CANNOT_LOAD_PF		"An error occured during loading the field"
#define ID_CANNOT_SAVE_PF		"An error occured during saving the field"
#define ID_PF_SAVED				"Potential field saved successfully"

#define ID_CANNOT_LOAD_RAY		"An error occured during loading the function"
#define ID_CANNOT_SAVE_RAY		"An error occured during saving the function"

#define ID_CANNOT_ALLOCATE_VB	"Unable to allocate vertex buffer"
#define ID_CANNOT_LOCK_VB		"Cannot obtain lock to vertex buffer"

#define ID_RESET_QUESTION		"Do you really want to reset the ray?\nAll sample will be lost!"
#define ID_DELETE_QUESTION		"Really delete the ray?\nAll information will be lost!"

#define ID_ARE_YOU_SURE			MessageBoxEx (HWND_DESKTOP, "Are you sure?", "Really quit?", MB_YESNO | MB_ICONQUESTION, 0)
#define ERRORMB(Str)			{MessageBoxEx (HWND_DESKTOP, (Str), "Error Occured", MB_OK, 0); exit (-1);}
#define MB(Str)					{MessageBoxEx(HWND_DESKTOP, (Str), "Message", MB_OK, 0);}

#define MALLOC_ERROR(ret)		{MessageBoxEx(HWND_DESKTOP, "Insufficient memory\nThe operation cannot proceed", "Warning!", MB_OK | MB_ICONERROR, 0); return ((ret));}

#define ID_MAIN_VIEW			"Main view"
#define ID_RAY_VIEW				"Ray view"
#define ID_PHI_VIEW				"Phi filter"
#define ID_THETA_VIEW			"Theta filter"
#define ID_RAD_VIEW				"Radial filter"
#define ID_HELPER				"Press F1 for Help"

struct POSITIONS Positions[] =
{
	{510.0f,  155.0f},
	{510.0f,  345.0f},
	{510.0f,  520.0f},

	{10.0f,	  10.0f},
	{10.0f,  380.0f},
	{510.0f,  10.0f},
	{510.0f, 190.0f},
	{510.0f, 380.0f},
	{100.0f,  10.0f}, 

	{350.0f,  250.0f},
};

enum POSITIONIDS 
{
	ID_PHI_PARAM, ID_THETA_PARAM, ID_RADIAL_PARAM, 
	ID_MAIN_TEXT, ID_RAY_TEXT, ID_PHI_TEXT, ID_THETA_TEXT, ID_RAD_TEXT,
	ID_FILE_TEXT,

	ID_HELPER_TEXT,
};

enum RECTIDS
{
	// AREAS
	ID_CLICK_MAIN, ID_CLICK_RAY, 
	ID_CLICK_PHIFILTER, ID_CLICK_THETAFILTER, ID_CLICK_RADIALFILTER,

	// MENU AREAS
	ID_MENU_NW, ID_MENU_N, ID_MENU_NE,
	ID_MENU_W,
	ID_MENU_SW, ID_MENU_S, ID_MENU_SE,
	ID_MENU_E,
};

enum CURSORIDS {ID_NORMAL_CURSOR, ID_MOVE_CURSOR_1, ID_MOVE_CURSOR_2, ID_MENU_CURSOR};

struct MENUES Menues[] = 
{
	{-32.4f,	-6.0f,	"Main menu"},
	{-86.4f,	-48.0f,	"Load\nField"},
	{-14.4f,	-48.0f,	"Save\nField"},
	{ 46.8f,	-48.0f,	"Save as"},
	{-104.4f,	-6.0f,	"New Field"},
	{-100.8f,	+18.0f,	"Previous\nSensor"},
	{-18.6f,	+18.0f,	"    Add\nSensor"},
	{ 50.4f,	+18.0f,	"      Next\nSensor"},
	{46.8f,		-6.0f,	"Select Ray"},

	{-28.8f,	-6.0f,	"Ray menu"},
	{-86.4f,	-48.0f,	"Load\nRay"},
	{-14.4f,	-48.0f,	"Save\nRay"},
	{ 54.0f,	-48.0f,	"Reset\nRay"},
	{-100.2f,	-6.0f,	"Numeric"},
	{-100.8f,	+18.0f,	"Previous\nPoint"},
	{-33.0f,	+18.0f,	"   Change\nResolution"},
	{ 54.0f,	+18.0f,	"Next\nPoint"},
	{46.8f,		-6.0f,	"Delete Ray"},
	
	{-37.4f,	-6.0f,	"Filter menu"},
	{-90.8f,	-36.0f,	"Gauss"},
	{-21.6f,	-36.0f,	"Square"},
	{ 43.2f,	-36.0f,	"Triangle"},
	{ 0.0f,		 0.0f,	""},
	{ 0.0f,		 0.0f,	""},
	{-23.6f,	+24.0f,	"Numeric"},
};
