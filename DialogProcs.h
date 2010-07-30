INT_PTR CALLBACK ChangeResDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
			char Chres[40];
			sprintf (Chres, "%d", CommandInfo.ChangeResNum);
			SetDlgItemText (hDlg, IDC_RESOLUTION, Chres);
			break;

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:	
				{
					char str[10];
					GetDlgItemText (hDlg, IDC_RESOLUTION, str, 10);
					int chres = atoi (str);
					if (chres > 0) CommandInfo.ChangeResNum = chres;
					EndDialog (hDlg, 1);
				}
				break;
				case IDCANCEL:	
					EndDialog (hDlg, 0);
					break;
			}
			break;

		default: return (FALSE);
	}

	return (FALSE);
}

INT_PTR CALLBACK AddSensorDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
			char Chres[40];
			sprintf (Chres, "%d", CommandInfo.ChangeResNum);
			SetDlgItemText (hDlg, IDC_LENGTH, "1.0");
			SetDlgItemText (hDlg, IDC_PHI, "0.0");
			SetDlgItemText (hDlg, IDC_THETA, "0.0");
			SetDlgItemText (hDlg, IDC_RAY_FILE, "");
			SetDlgItemInt (hDlg, IDC_DIVS, 0, false);
			break;

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:	
				{
					char str[400];
					float temp;
					int tmp;
					GetDlgItemText (hDlg, IDC_PHI, str, 10);
					if (!sscanf (str, "%f", &temp)) break;
					CommandInfo.orient[0] = temp;
					GetDlgItemText (hDlg, IDC_THETA, str, 10);
					if (!sscanf (str, "%f", &temp)) break;
					CommandInfo.orient[1] = temp;
					GetDlgItemText (hDlg, IDC_DIVS, str, 10);
					if (!sscanf (str, "%d", &tmp)) break;
					CommandInfo.divs = tmp;
					GetDlgItemText (hDlg, IDC_RAY_FILE, str, 400);
					if (strlen (str) > 0) 
					{
						strcpy (CommandInfo.SensorFileName, str);
						CommandInfo.AddSensorFromFile = true;
					}
					CommandInfo.ReallyAddSensor = true;
					EndDialog (hDlg, 1);
				}
				break;
				case IDCANCEL:	
					EndDialog (hDlg, 0);
					break;

				case IDC_RESET: 
					SetDlgItemText (hDlg, IDC_LENGTH, "1.0");
					SetDlgItemText (hDlg, IDC_PHI, "0.0");
					SetDlgItemText (hDlg, IDC_THETA, "0.0");
					SetDlgItemInt (hDlg, IDC_DIVS, 0, false);
					SetDlgItemText (hDlg, IDC_RAY_FILE, "");
					EnableWindow (GetDlgItem(hDlg, IDC_DIVS), TRUE);
					break;
								
				case IDC_LOAD_RAY: 
				{
					CString fileName;
					if (!Load_Save_Dialog (&fileName, true, false)) break;
					LPCTSTR lpCtStr = (LPCTSTR) fileName;
					SetDlgItemText (hDlg, IDC_RAY_FILE, (char *)lpCtStr);
					FILE *rayfile = fopen ((char *)lpCtStr, "r");
					char str[40];
					int divs;
					fscanf (rayfile, "%s", str);
					fscanf (rayfile, "%d", &divs);
					fclose (rayfile);
					SetDlgItemInt (hDlg, IDC_DIVS, divs, false);
					EnableWindow (GetDlgItem(hDlg, IDC_DIVS), FALSE);
			   }
			   break;

				case IDC_CLEAR_RAY: 
					SetDlgItemText (hDlg, IDC_RAY_FILE, "");
					SetDlgItemText (hDlg, IDC_DIVS, "0");
					EnableWindow (GetDlgItem(hDlg, IDC_DIVS), TRUE);
					break;

			}
			break;

		default: return (FALSE);
	}

	return (FALSE);
}

INT_PTR CALLBACK AboutDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: break;

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:	
					EndDialog (hDlg, 1);
					break;
			}
			break;

		default: return (FALSE);
	}

	return (FALSE);
}

INT_PTR CALLBACK NumericDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
		{
			char str[40];
			sprintf (str, "%lf", CommandInfo.NewParam);
			SetDlgItemText (hDlg, IDC_PARAMETRE, str);
			break;
		}

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:	
				{
					char str[40];
					double param;
					GetDlgItemText (hDlg, IDC_PARAMETRE, str, 40);
					sscanf (str, "%lf", &param);
					CommandInfo.NewParam = param;
					EndDialog (hDlg, 1);
					break;
				}
				case IDCANCEL: 
					EndDialog (hDlg, 1);
			}
			break;

		default: return (FALSE);
	}
	return (FALSE);
}

INT_PTR CALLBACK NewFieldDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
		{
			SetDlgItemInt (hDlg, IDC_PHI, 1, false);
			SetDlgItemInt (hDlg, IDC_THETA, 1, false);
			SetDlgItemInt (hDlg, IDC_RADIAL, 1, false);
			SetDlgItemText (hDlg, IDC_RAY_FILE, "");
			CommandInfo.ReallyNewPotentialField = false;
			break;
		}

		case WM_COMMAND:
			switch (LOWORD(wParam))
			{
				case IDOK:	
				{
					char str[400];
					GetDlgItemText (hDlg, IDC_RAY_FILE, str, 400);
					if (strlen (str) == 0) CommandInfo.newField.fromFile = false;
					else 
					{
						CommandInfo.newField.fromFile = true;
						//CommandInfo.newField.filename = (char *)malloc (strlen (str) * sizeof (char));
						strcpy (CommandInfo.newField.filename, str);
					}
												
					GetDlgItemText (hDlg, IDC_PHI, str, 400);
					sscanf (str, "%d", &CommandInfo.newField.phi);
					GetDlgItemText (hDlg, IDC_THETA, str, 400);
					sscanf (str, "%d", &CommandInfo.newField.theta);
					GetDlgItemText (hDlg, IDC_RADIAL, str, 400);
					sscanf (str, "%d", &CommandInfo.newField.radial);
					CommandInfo.ReallyNewPotentialField = true;
					EndDialog (hDlg, 9);
					break;
				}
				case IDCANCEL:
					CommandInfo.ReallyNewPotentialField = true;
					EndDialog (hDlg, 1);
				case IDC_RESET: 
				{
					SetDlgItemInt (hDlg, IDC_PHI, 1, false);
					SetDlgItemInt (hDlg, IDC_THETA, 1, false);
					SetDlgItemInt (hDlg, IDC_RADIAL, 1, false);
					SetDlgItemText (hDlg, IDC_RAY_FILE, "");
					EnableWindow (GetDlgItem(hDlg, IDC_RADIAL), FALSE);
					break;
				}
				case IDC_CLEAR_RAY: 
					SetDlgItemText (hDlg, IDC_RAY_FILE, "");
					SetDlgItemInt (hDlg, IDC_RADIAL, 1, false);
					EnableWindow (GetDlgItem(hDlg, IDC_RADIAL), TRUE);
					break;													
				case IDC_LOAD_RAY: 
				{
					CString fileName;
					char str[40];
					int divs;
					if (!Load_Save_Dialog (&fileName, true, false)) break;
					LPCTSTR lpCtStr = (LPCTSTR) fileName;
					SetDlgItemText (hDlg, IDC_RAY_FILE, (char *)lpCtStr);
					FILE *rayfile = fopen ((char *)lpCtStr, "r");
					fscanf (rayfile, "%s", str);
					fscanf (rayfile, "%d", &divs);
					fclose (rayfile);
					SetDlgItemInt (hDlg, IDC_RADIAL, divs, false);
					EnableWindow (GetDlgItem(hDlg, IDC_RADIAL), FALSE);
				}
				break;
			}
		default: return (FALSE);
	}
	return (FALSE);
}

INT_PTR CALLBACK PreferencesDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
		{
			char str[40];
			DWORD color = Preferences.BackgroundColor;
			sprintf (str, "%x", ((color&0xff)<<16) + (color&0xff00) + ((color&0xff0000)>>16));
			SetDlgItemText (hDlg, IDC_BGCOLOR, str);
			SetDlgItemText (hDlg, IDC_FONT_NAME, Preferences.Font);
			SetDlgItemInt (hDlg, IDC_FONT_SIZE, Preferences.FontSize, false);
			break;
		}

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:	
				{
					char str[400];
					int i;
					GetDlgItemText (hDlg, IDC_BGCOLOR, str, 40);
					DWORD color;
					if (sscanf (str, "%x", &color)) 
					Preferences.BackgroundColor = 0xff000000 + color;
					GetDlgItemText (hDlg, IDC_FONT_SIZE, str, 40);
					if (!sscanf (str, "%d", &i)) i = 12;
					GetDlgItemText (hDlg, IDC_FONT_NAME, str, 40);
					if (strlen (str) != 0)
					{
						strcpy (Preferences.Font, str);
						Preferences.FontSize = i;
					}
					CommandInfo.ChangeFont = true;
					EndDialog (hDlg, 9);
					break;
				}
				case IDCANCEL:	
					EndDialog (hDlg, 0);
				case IDC_RESET: 
				{
					char str[40];
					DWORD color = 0;
					sprintf (str, "%x", ((color&0xff)<<16) + (color&0xff00) + ((color&0xff0000)>>16));
					SetDlgItemText (hDlg, IDC_BGCOLOR, str);
					SetDlgItemText (hDlg, IDC_FONT_NAME, "Impact");
					SetDlgItemText (hDlg, IDC_FONT_SIZE, "12");
					break;
				}
				case IDC_CHANGE_COLOR:	
				{
					CColorDialog *ccd = new CColorDialog ();
					int result = ccd -> DoModal ();
					if (result != IDOK) return (FALSE);
					DWORD color = ccd -> GetColor ();
					color = ((color&0xff)<<16) + (color&0xff00) + ((color&0xff0000)>>16);
					char str[40];
					sprintf (str, "%x", color);
					SetDlgItemText (hDlg, IDC_BGCOLOR, str);
				}
				break;

				case IDC_CHANGE_FONT:	
				{
					CFontDialog *ccd = new CFontDialog ();
					int result = ccd -> DoModal ();
					if (result != IDOK) return (FALSE);
					CString face = ccd -> GetFaceName ();
					LPCTSTR LpCtString = (LPCTSTR) face;
					SetDlgItemText (hDlg, IDC_FONT_NAME, (char *) LpCtString);
					SetDlgItemInt (hDlg, IDC_FONT_SIZE, ccd -> GetSize () / 10, false);
				}
				break;
			}
		default: return (FALSE);
	}
	return (FALSE);
}

INT_PTR CALLBACK SelectRayDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
			SetDlgItemText (hDlg, IDC_PHI, "0");
			SetDlgItemText (hDlg, IDC_THETA, "0");
			break;

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					char str[40];
					float param;
					GetDlgItemText (hDlg, IDC_PHI, str, 40);
					sscanf (str, "%f", &param);
					CommandInfo.orient[0] = param;
					GetDlgItemText (hDlg, IDC_THETA, str, 40);
					sscanf (str, "%f", &param);
					CommandInfo.orient[1] = param;
					EndDialog (hDlg, IDOK);
				}
				break;
				
				case IDCANCEL: 
					EndDialog (hDlg, IDCANCEL);
					break;

				case IDC_RESET:
					SetDlgItemText (hDlg, IDC_PHI, "0");
					SetDlgItemText (hDlg, IDC_THETA, "0");
					break;
			}
			break;

		default: return (FALSE);
	}
	return (FALSE);
}

INT_PTR CALLBACK AdministerDlgProc (HWND hDlg, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
		case WM_INITDIALOG: 
		{
			char str[40];
			sprintf (str, "%f", CommandInfo.AdminInfo.alpha);
			SetDlgItemText (hDlg, IDC_ALPHA, str);
			sprintf (str, "%f", CommandInfo.AdminInfo.h);
			SetDlgItemText (hDlg, IDC_H, str);
			sprintf (str, "%f", CommandInfo.AdminInfo.a);
			SetDlgItemText (hDlg, IDC_A, str);
			sprintf (str, "%f", CommandInfo.AdminInfo.b);
			SetDlgItemText (hDlg, IDC_B, str);
			sprintf (str, "%f", CommandInfo.AdminInfo.halpha);
			SetDlgItemText (hDlg, IDC_HALPHA, str);
		}
			break;

		case WM_COMMAND:	
			switch (LOWORD(wParam))
			{
				case IDOK:
				{
					char str[40];
					GetDlgItemText (hDlg, IDC_ALPHA, str, 40);
					sscanf (str, "%f", &(CommandInfo.AdminInfo.alpha));
					GetDlgItemText (hDlg, IDC_H, str, 40);
					sscanf (str, "%f", &(CommandInfo.AdminInfo.h));
					GetDlgItemText (hDlg, IDC_A, str, 40);
					sscanf (str, "%f", &(CommandInfo.AdminInfo.a));
					GetDlgItemText (hDlg, IDC_B, str, 40);
					sscanf (str, "%f", &(CommandInfo.AdminInfo.b));
					GetDlgItemText (hDlg, IDC_HALPHA, str, 40);
					sscanf (str, "%f", &(CommandInfo.AdminInfo.halpha));
					EndDialog (hDlg, IDOK);
				}
				break;
				
				case IDCANCEL: 
					EndDialog (hDlg, IDCANCEL);
					break;

				case IDC_RESET:
				{
					char str[40];
					sprintf (str, "%f", CommandInfo.AdminInfo.alpha);
					SetDlgItemText (hDlg, IDC_ALPHA, str);
					sprintf (str, "%f", CommandInfo.AdminInfo.h);
					SetDlgItemText (hDlg, IDC_H, str);
					sprintf (str, "%f", CommandInfo.AdminInfo.a);
					SetDlgItemText (hDlg, IDC_A, str);
					sprintf (str, "%f", CommandInfo.AdminInfo.b);
					SetDlgItemText (hDlg, IDC_B, str);
					sprintf (str, "%f", CommandInfo.AdminInfo.halpha);
					SetDlgItemText (hDlg, IDC_HALPHA, str);
				}
				break;
			}
			break;

		default: return (FALSE);
	}
	return (FALSE);
}
