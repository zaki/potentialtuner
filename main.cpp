#include "resource.h"
#include "defines.h"
#include "Globals.h"

#include "DialogProcs.h"

LRESULT WINAPI MsgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
 
bool Render ();

#include "Classes/Model/Component/ComponentClass.h"

#include "Interfaces/Model/Sensor/intSensor.h"
#include "Classes/Model/Sensor/Sensor.h"

#include "Classes/Model/SensorArray/SensorArray.h"

#include "Interfaces/Model/PotentialField/intPotentialField.h"
#include "Classes/Model/PotentialField/PotentialField.h"

#include "Interfaces/View/Viewer/intViewer.h"
#include "Classes/View/Viewer/Viewer.h"

#include "Interfaces/Control/Application/intApplication.h"
#include "Classes/Control/Application/Application.h"

Viewer *viewer;
Sensor *Addendum = NULL;

POINT lastknownpos = {0, 0};



LRESULT WINAPI MsgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
    {
		case WM_CLOSE:	
			RenderInfo.ScreenActive = false;
			Are_You_Sure ();
			RenderInfo.ScreenActive = true;
			break;

		case WM_DESTROY:	
			if (CommandInfo.Exit) PostQuitMessage (0);
			return (0);

		case WM_CREATE:		
			RenderInfo.ScreenActive = true;
			SelectCursor (ID_NORMAL_CURSOR);
			return (0);
							
		case WM_SETFOCUS:	
			RenderInfo.ScreenActive = true;
			SetWindowText (hWnd, ID_ACTIVE_TITLE);
			return (0);

		case WM_KILLFOCUS:	
			RenderInfo.ScreenActive = false;
			SetWindowText (hWnd, ID_INACTIVE_TITLE);
			return (0);
							
		case WM_LBUTTONDOWN:
			RenderInfo.realleftclickPosition.x = LOWORD(lParam);
			RenderInfo.realleftclickPosition.y = HIWORD(lParam);
			GetCursorPos (&(RenderInfo.leftclickPosition));
			if (Get_Area_Code (RenderInfo.realleftclickPosition.x, RenderInfo.realleftclickPosition.y) == ID_CLICK_MAIN)
				SelectCursor (ID_MOVE_CURSOR_1);
			else SelectCursor (ID_MOVE_CURSOR_2);
			RenderInfo.LMBDown = true;
			return (0);

		case WM_LBUTTONUP:	
			RenderInfo.LMBDown = false;
			SelectCursor (ID_NORMAL_CURSOR);
			return (0);

		case WM_MOUSEMOVE:		
			if (RenderInfo.LMBDown)
			{
				POINT move;
				GetCursorPos (&move);
	
				switch (Get_Area_Code (RenderInfo.realleftclickPosition.x, RenderInfo.realleftclickPosition.y))
				{
					case 0:	
						RenderInfo.Rotation[0] += (float) (PI / 180.0f * (float) ((move.x - RenderInfo.leftclickPosition.x)));
						RenderInfo.Rotation[1] += (float) (PI / 180.0f * (float) ((move.y - RenderInfo.leftclickPosition.y)));
						RenderInfo.leftclickPosition = move;
						break;
	
					case 1: 
						RenderInfo.MoveRayPoint = +1 * (float) (move.x - RenderInfo.leftclickPosition.x) / 100.0f - (float) (move.y - RenderInfo.leftclickPosition.y) / 1000.0f;
						RenderInfo.leftclickPosition = move;
						break;
						
					case 2:	
						RenderInfo.MovePhiPoint = (float) (move.x - RenderInfo.leftclickPosition.x) / 100.0f - (float) (move.y - RenderInfo.leftclickPosition.y) / 1000.0f;
						RenderInfo.leftclickPosition = move;
						break;
						
					case 3:	
						RenderInfo.MoveThetaPoint = (float) (move.x - RenderInfo.leftclickPosition.x) / 100.0f - (float) (move.y - RenderInfo.leftclickPosition.y) / 1000.0f;
						RenderInfo.leftclickPosition = move;
						break;
						
					case 4:	
						RenderInfo.MoveRadialPoint = (float) (move.x - RenderInfo.leftclickPosition.x) / 100.0f - (float) (move.y - RenderInfo.leftclickPosition.y) / 1000.0f;
						RenderInfo.leftclickPosition = move;
						break;
				}
				DisplayHelper = false;
				idleTime = timeGetTime ();
			}
			
			if (RenderInfo.RMBDown)
			{
				POINT move;
				GetCursorPos (&move);
	
				RenderInfo.currentRightPosition.x = LOWORD(lParam) - RenderInfo.rightclickPosition.x;
				RenderInfo.currentRightPosition.y = HIWORD(lParam) - RenderInfo.rightclickPosition.y;
			}
			return (0);

		case WM_RBUTTONDOWN:
			RenderInfo.RMBDown = true;
			GetCursorPos (&lastknownpos);
			RenderInfo.rightclickPosition.x = LOWORD(lParam);
			RenderInfo.rightclickPosition.y = HIWORD(lParam);
			RenderInfo.currentRightPosition.x = 0;
			RenderInfo.currentRightPosition.y = 0;
			SelectCursor (ID_MENU_CURSOR);
			return (0);

		case WM_RBUTTONUP:
			RenderInfo.RMBDown = false;
			switch (Get_Area_Code (RenderInfo.rightclickPosition.x, RenderInfo.rightclickPosition.y))
			{
				case ID_CLICK_MAIN:
					switch (Get_Area_Code (RenderInfo.currentRightPosition.x, RenderInfo.currentRightPosition.y, ID_MENU_CODE))
					{
						case ID_MENU_NW:	
							CommandInfo.LoadPotentialField = true;
							break;
						case ID_MENU_N:
							CommandInfo.SavePotentialField = true;
							break;
						case ID_MENU_NE:
							CommandInfo.SavePotentialFieldAs = true;
							break;
						case ID_MENU_W:
							CommandInfo.NewPotentialField = true;
							break;
						case ID_MENU_SW:
							CommandInfo.PrevSensor = true;
							break;
						case ID_MENU_S:
							CommandInfo.AddSensor = true;
							break;
						case ID_MENU_SE:
							CommandInfo.NextSensor = true;
							break;
						case ID_MENU_E:
							CommandInfo.SelectRay = true;
							break;
					}
					break;

					case ID_CLICK_RAY:
					switch (Get_Area_Code (RenderInfo.currentRightPosition.x, RenderInfo.currentRightPosition.y, ID_MENU_CODE))
					{
						case ID_MENU_NW:	
							CommandInfo.LoadRayFunction = true;
							break;
						case ID_MENU_N:
							CommandInfo.SaveRayFunction = true;
							break;
						case ID_MENU_NE:
							CommandInfo.ResetRay = true;
							break;
						case ID_MENU_W:
							CommandInfo.filterno = -1;
							CommandInfo.Numeric = true;
							break;
						case ID_MENU_SW:
							RenderInfo.ActiveNode--;
							break;
						case ID_MENU_S:
							CommandInfo.ChangeRes = true;
							break;
						case ID_MENU_SE:
							RenderInfo.ActiveNode++;
							break;
						case ID_MENU_E:
							CommandInfo.DeleteRay = true;
							break;
					}
					break;

				case ID_CLICK_PHIFILTER:
				case ID_CLICK_THETAFILTER:
				case ID_CLICK_RADIALFILTER:
					CommandInfo.filterno = Get_Area_Code (RenderInfo.rightclickPosition.x, RenderInfo.rightclickPosition.y) - ID_CLICK_PHIFILTER;
					switch (Get_Area_Code (RenderInfo.currentRightPosition.x, RenderInfo.currentRightPosition.y, ID_MENU_CODE))
					{
						case ID_MENU_NW:
						case ID_MENU_N:
						case ID_MENU_NE:
							CommandInfo.NewType = true;
							CommandInfo.newtype = Get_Area_Code (RenderInfo.currentRightPosition.x, 
																 RenderInfo.currentRightPosition.y, 
																 ID_MENU_CODE) - ID_MENU_NW;
							break;
						case ID_MENU_W:
							break;
						case ID_MENU_SW:
							break;
						case ID_MENU_S:
							CommandInfo.Numeric = true;
							break;
						case ID_MENU_SE:
							break;
					}
					break;
			}
			SelectCursor (ID_NORMAL_CURSOR);
			return (0);

		
		case WM_MBUTTONDOWN:
			RenderInfo.MBDDown = true;
			return (0);

		case WM_MBUTTONUP:	
			RenderInfo.MBDDown = false;
			return (0);

		case WM_KEYDOWN:
			switch (LOWORD(wParam))
			{
				case VK_ESCAPE: 
					Are_You_Sure ();
					break;
				case VK_RIGHT:
					CommandInfo.NextSensor = true;
					break;
				case VK_LEFT:
					CommandInfo.PrevSensor = true;
					break;
				case VK_PRIOR:
					RenderInfo.ActiveNode--;
					break;
				case VK_NEXT:
					RenderInfo.ActiveNode++;
					break;
				case VK_SPACE:
					SHOULD_DISPLAY_POINTS = !SHOULD_DISPLAY_POINTS;
					break;
				case VK_F1:
					WinHelp (HWND_DESKTOP, "dxTuner", HELP_CONTENTS, 0l);
					break;
			}
			RenderInfo.ScreenActive = true;
			return (0);

		case WM_COMMAND:
			//RenderInfo.ScreenActive = false;
			switch (LOWORD(wParam))
			{
				case IDM_NEW_FIELD:		
					CommandInfo.NewPotentialField = true;
					break;
				case ID_FILE_LOADFIELD: 
					CommandInfo.LoadPotentialField = true;
					break;
				case ID_FILE_SAVEFIELD: 
					CommandInfo.SavePotentialField = true;
					break;
				case ID_FILE_SAVEFIELDAS:
					CommandInfo.SavePotentialFieldAs = true;
					break;
				case IDM_ADD_SENSOR:
					CommandInfo.AddSensor = true; 
					break;
				case ID_FILE_EXIT:		
					Are_You_Sure ();
					break;
				case IDM_PHI_GAUSS:
					CommandInfo.filterno = 0; 
					CommandInfo.NewType = true;
					CommandInfo.newtype = 0;
					break;
				case IDM_PHI_SQUARE:
					CommandInfo.filterno = 0;
					CommandInfo.NewType = true;
					CommandInfo.newtype = 1;
					break;
				case IDM_PHI_TRIANGLE:	
					CommandInfo.filterno = 0;
					CommandInfo.NewType = true;
					CommandInfo.newtype = 2;
					break;
				case IDM_PHI_NUMERIC:
					CommandInfo.filterno = 0;
					CommandInfo.Numeric = true;
					break;

				case IDM_THETA_GAUSS:
					CommandInfo.filterno = 1; 
					CommandInfo.NewType = true;
					CommandInfo.newtype = 0;
					break;
				case IDM_THETA_SQUARE:	
					CommandInfo.filterno = 1;
					CommandInfo.NewType = true;
					CommandInfo.newtype = 1;
					break;
				case IDM_THETA_TRIANGLE:
					CommandInfo.filterno = 1;
					CommandInfo.NewType = true; 
					CommandInfo.newtype = 2;
					break;
				case IDM_THETA_NUMERIC:
					CommandInfo.filterno = 1;
					CommandInfo.Numeric = true;
					break;
				case IDM_RAD_GAUSS:	
					CommandInfo.filterno = 2;
					CommandInfo.NewType = true;
					CommandInfo.newtype = 0;
					break;
				case IDM_RAD_SQUARE:	
					CommandInfo.filterno = 2;
					CommandInfo.NewType = true; 
					CommandInfo.newtype = 1;
					break;
				case IDM_RAD_TRIANGLE:	
					CommandInfo.filterno = 2;
					CommandInfo.NewType = true;
					CommandInfo.newtype = 2;
					break;
				case IDM_RAD_NUMERIC:
					CommandInfo.filterno = 2;
					CommandInfo.Numeric = true;
					break;
				case IDM_LOAD_RAY:
					CommandInfo.LoadRayFunction = true;
					break;
				case IDM_SAVE_RAY:
					CommandInfo.SaveRayFunction = true;
					break;
				case IDM_RAY_NUMERIC:
					CommandInfo.filterno = -1; 
					CommandInfo.Numeric = true;
					break;
				case IDM_RESET_RAY:
					CommandInfo.ResetRay = true;
					break;
				case IDM_CHANGE_RES:	
					CommandInfo.ChangeRes = true;
					break;
				case IDM_SEL_RAY:	
					CommandInfo.SelectRay = true;
					break;
				case IDM_DELETE_RAY:
					CommandInfo.DeleteRay = true;
					break;
				case ID_ABOUT:
					DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_ABOUT), 
							   HWND_DESKTOP, (DLGPROC)AboutDlgProc);
					break;
				case IDM_PREFS:
					DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_PREFERENCES), 
					 		   HWND_DESKTOP, (DLGPROC)PreferencesDlgProc);	
					break;
			}
			RenderInfo.ScreenActive = true;
	}// END SWITCH

	return DefWindowProc (hWnd, msg, wParam, lParam);
}

int CALLBACK WinMain (HINSTANCE h, HINSTANCE h1, LPSTR lp, int nc)
{
	CommandInfo.Exit = false;
	RenderInfo.WindowInstance = h;

	viewer = new Viewer ();

	return (0);
}

