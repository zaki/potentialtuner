bool Viewer::CheckD3DCaps ()
{
	if ((g_pD3D = Direct3DCreate8 (D3D_SDK_VERSION)) == NULL) 
	{
		ERRORMB(ID_FAILED_DEVICE);
	}
	
	return (true);
}

bool Viewer::CreateViewer ()
{
	D3DDISPLAYMODE d3ddm;
	D3DPRESENT_PARAMETERS d3dpp; 

	MSG msg = {NULL, 0, 0l, 0l, 0x00000000, {0, 0}};

    WNDCLASSEX wc = 
	{	
		sizeof (WNDCLASSEX), 
		CS_CLASSDC, 
		MsgProc, 
		0L, 0L, 
		GetModuleHandle (NULL), 
		NULL, NULL, NULL, NULL,
		"PT", 
		NULL
	};
    
	ActiveSensor = NULL;

	RegisterClassEx (&wc);

    // Create the application's window.
    HWND hWnd = CreateWindow ("PT",
							"PT",
							WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
							100, 100,
							ID_WINDOW_WIDTH, ID_WINDOW_HEIGHT,
							GetDesktopWindow (),
							LoadMenu (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDR_MAINMENU)),
							wc.hInstance,
							NULL);


	if (!CheckD3DCaps ()) ERRORMB(ID_WRONG_DXVERSION);
	
	if ((g_pD3D = Direct3DCreate8 (D3D_SDK_VERSION)) == NULL) 
				ERRORMB(ID_FAILED_DEVICE);

    if (FAILED(g_pD3D -> GetAdapterDisplayMode (D3DADAPTER_DEFAULT, &d3ddm))) 
				ERRORMB(ID_FAILED_DISPLAYMODE);
    
    ZeroMemory (&d3dpp, sizeof (d3dpp));
    d3dpp.Windowed = ID_WINDOWED_MODE;
    d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
    d3dpp.BackBufferFormat = d3ddm.Format;

	if (FAILED(g_pD3D -> CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice)))
	{
		MB(ID_FAILED_HAL);

		if (FAILED(g_pD3D -> CreateDevice (D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd,
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING,
                                      &d3dpp, &g_pd3dDevice))) ERRORMB(ID_FAILED_DEVICE);
	}
	
	g_pd3dDevice -> SetRenderState (D3DRS_LIGHTING, FALSE);
	g_pd3dDevice -> SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);

	ShowWindow (hWnd, SW_SHOWDEFAULT);
	UpdateWindow (hWnd);

	RenderInfo.WindowHandle = hWnd;

	if (pf) 
	{
		pf -> ResetCurrency (); 
		ActiveSensor = pf -> GetNextSensor (); 
		ActiveSensorNo = 1;
	}
		
	font = new CD3DFont (_T(Preferences.Font), Preferences.FontSize, 0L);
	if (font && g_pd3dDevice) font -> InitDeviceObjects (g_pd3dDevice);
	font -> RestoreDeviceObjects ();

	phifilter = new FilterComponent ();
	thetafilter = new FilterComponent ();
	radialfilter = new FilterComponent ();

	phifilter -> CreateGAUSS (0.5);
	thetafilter -> CreateGAUSS (0.5);
	radialfilter -> CreateGAUSS (0.5);

	while (msg.message != WM_QUIT)
	{
		if( PeekMessage( &msg, NULL, 0U, 0U, PM_REMOVE ) )
        {
			TranslateMessage (&msg);
			DispatchMessage (&msg);
		}
        else 
		{
			CheckCommands ();
			if (RenderInfo.ScreenActive) Render ();
		}

	}
	return (true);
}

bool Viewer::CheckCommands ()
{
	// Check idle time
	if (
			RenderInfo.MoveRayPoint != 0.0f ||
			RenderInfo.MovePhiPoint != 0.0f ||
			RenderInfo.MoveThetaPoint != 0.0f ||
			RenderInfo.MoveRadialPoint != 0.0f ||
			RenderInfo.RMBDown ||
			CommandInfo.AddSensor ||
			CommandInfo.DeleteRay ||
			CommandInfo.Exit ||
			CommandInfo.LoadPotentialField ||
			CommandInfo.NewPotentialField ||
			CommandInfo.NewType ||
			CommandInfo.NextSensor ||
			CommandInfo.Numeric ||
			CommandInfo.PrevSensor ||
			CommandInfo.ResetRay ||
			CommandInfo.SavePotentialField ||
			CommandInfo.SavePotentialFieldAs ||
			CommandInfo.SaveRayFunction ||
			CommandInfo.SelectRay ||
			idleTime == 0
		) 
	{
		DisplayHelper = false;
		idleTime = timeGetTime ();
	}
	
	else 
		if ((timeGetTime () - idleTime) > ID_HELPER_TIMEOUT) DisplayHelper = true;

	// MOVEMENT CONTROL
	if (RenderInfo.MoveRayPoint != 0.0f)
	{
		float movement;
		float *Samples = ActiveSensor -> GetSamples ();

		if (Samples[RenderInfo.ActiveNode] + RenderInfo.MoveRayPoint >= -10.0f && Samples[RenderInfo.ActiveNode] + RenderInfo.MoveRayPoint <= 10.0f)
		{
			Sensor *sensor;
			movement = RenderInfo.MoveRayPoint;
			float *orientation = ActiveSensor -> GetOrientation ();
			float radius = ActiveSensor -> GetLength () / ActiveSensor -> GetDivisions () * RenderInfo.ActiveNode;

			pf -> ResetCurrency ();
		
			while (sensor = (pf -> GetNextSensor ()))
			{
				float *orient;
				float *samples = sensor -> GetSamples ();
				float onestep;
				int divs = sensor -> GetDivisions ();
			
				orient = sensor -> GetOrientation ();
				onestep = sensor -> GetLength () / sensor -> GetDivisions ();
				for (int i = 0; i <= divs; i++)
				{
					double phif, thetaf, radf;
					float candidate;

					phif = (orient[0] - orientation[0]) / 360.0;
					thetaf = (orient[1] - orientation[1]) / 180.0;
					radf = onestep * i - radius;
					candidate = samples[i] + (float) (movement * phifilter -> CalculateValue (phif) * thetafilter -> CalculateValue (thetaf) * radialfilter -> CalculateValue (radf));

					if (candidate > 10.0f) candidate = 10.0f;
					if (candidate < -10.0f) candidate = -10.0f;

					samples[i] = candidate;
				}
			}
		}
		
		RenderInfo.MoveRayPoint = 0.0f;
	}

	if (RenderInfo.MovePhiPoint != 0.0f)
	{
		double actual = phifilter -> GetParam ();

		if (actual + RenderInfo.MovePhiPoint >= 1.0f) RenderInfo.MovePhiPoint = 0.0f;
		if (actual + RenderInfo.MovePhiPoint <= 0.0f) RenderInfo.MovePhiPoint = 0.0f;
		
		phifilter -> ChangeParametre (0, actual + RenderInfo.MovePhiPoint);

		RenderInfo.MovePhiPoint = 0.0f;
	}

	if (RenderInfo.MoveThetaPoint != 0.0f)
	{
		double actual = thetafilter -> GetParam ();

		if (actual + RenderInfo.MoveThetaPoint >= 1.0f) RenderInfo.MoveThetaPoint = 0.0f;
		if (actual + RenderInfo.MoveThetaPoint <= 0.0f) RenderInfo.MoveThetaPoint = 0.0f;
		
		thetafilter -> ChangeParametre (0, actual + RenderInfo.MoveThetaPoint);

		RenderInfo.MoveThetaPoint = 0.0f;
	}
	
	if (RenderInfo.MoveRadialPoint != 0.0f)
	{
		double actual = radialfilter -> GetParam ();

		if (actual + RenderInfo.MoveRadialPoint >= 1.0f) RenderInfo.MoveRadialPoint = 0.0f;
		if (actual + RenderInfo.MoveRadialPoint <= 0.0f) RenderInfo.MoveRadialPoint = 0.0f;
		
		radialfilter -> ChangeParametre (0, actual + RenderInfo.MoveRadialPoint);

		RenderInfo.MoveRadialPoint = 0.0f;
	}

	// MAIN MENU:
	if (CommandInfo.NewPotentialField)
	{
		CommandInfo.NewPotentialField = false;
		CommandInfo.ReallyNewPotentialField = false;
		int result = DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_NEWFIELD), 
					HWND_DESKTOP, (DLGPROC)NewFieldDlgProc);
		if (result != 9) return (false);
		if (CommandInfo.ReallyNewPotentialField)
		{
			if (CommandInfo.newField.fromFile)
				pf -> NewPotentialField (CommandInfo.newField.phi, CommandInfo.newField.theta, CommandInfo.newField.radial, 1.0f, CommandInfo.newField.filename); 
			else 
				pf -> NewPotentialField (CommandInfo.newField.phi, CommandInfo.newField.theta, CommandInfo.newField.radial, 1.0f); 

			pf -> ResetCurrency ();

			ActiveSensor = pf -> GetNextSensor ();
		}	
		CommandInfo.ReallyNewPotentialField = false;
		CommandInfo.newField.fromFile = false;
		CommandInfo.newField.phi = -1;
		CommandInfo.newField.theta = -1;
		CommandInfo.newField.radial = -1;

		strcpy (pfname, ID_DEFAULT_PF_NAME);
	}
	
	if (CommandInfo.LoadPotentialField || CommandInfo.SavePotentialFieldAs)
	{
		BOOL OpenSave;

		if (CommandInfo.LoadPotentialField)
		{
			CommandInfo.LoadPotentialField = false;
		
			OpenSave = TRUE;
		}
		else 
		{
			CommandInfo.SavePotentialFieldAs = false;
		
			OpenSave = FALSE;

			pf -> GetInfo (&(CommandInfo.AdminInfo.alpha),
						   &(CommandInfo.AdminInfo.h),
						   &(CommandInfo.AdminInfo.a),
						   &(CommandInfo.AdminInfo.b),
						   &(CommandInfo.AdminInfo.halpha));
		}

		CString fileName;

		if (!Load_Save_Dialog (&fileName, OpenSave ? true : false, true)) return (false);

		LPCTSTR lpCtStr = (LPCTSTR) fileName;
		
		if (OpenSave) 
		{
			if (!(pf -> LoadPotentialField ((char *) lpCtStr))) MB(ID_CANNOT_LOAD_PF)
			else 
			{
				pf -> ResetCurrency ();
				ActiveSensor = pf -> GetNextSensor ();
			}
		}

		else 
		{
			DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_ADMINISTER),
						HWND_DESKTOP, (DLGPROC)AdministerDlgProc);
			pf -> Administer (CommandInfo.AdminInfo.alpha,
				 			  CommandInfo.AdminInfo.h,
							  CommandInfo.AdminInfo.a,
							  CommandInfo.AdminInfo.b,
							  CommandInfo.AdminInfo.halpha,
							  1.0f);

			if (!(pf -> SavePotentialField ((char *) lpCtStr))) MB(ID_CANNOT_SAVE_PF)
		}

		strcpy (pfname, (char *) lpCtStr);
	}
	
	if (CommandInfo.SavePotentialField)
	{
		CommandInfo.SavePotentialField = false;
		if (!strcmp (pfname, ID_DEFAULT_PF_NAME)) CommandInfo.SavePotentialFieldAs = true;
		else if (!(pf -> SavePotentialField (pfname))) 
		{
			MB(ID_CANNOT_SAVE_PF)
			CommandInfo.SavePotentialFieldAs = true;
		}
		else MB(ID_PF_SAVED)
	}

	if (CommandInfo.AddSensor)
	{
		CommandInfo.AddSensor = false;
		CommandInfo.AddSensorFromFile = false;
		CommandInfo.ReallyAddSensor = false;
		
		DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_ADDSENSOR), 
					RenderInfo.WindowHandle, (DLGPROC)AddSensorDlgProc);
		
		if (CommandInfo.ReallyAddSensor)
		{
			Sensor *Addendum = new Sensor ();
		
			CommandInfo.ReallyAddSensor = false;
			
			Addendum -> ChangeOrientation (CommandInfo.orient);
			Addendum -> ChangeResolution (CommandInfo.divs);

			if (CommandInfo.AddSensorFromFile)
			{
				CommandInfo.AddSensorFromFile = false;
				Addendum -> LoadFromFile (CommandInfo.SensorFileName);
			}

			else 
			{
				float *samples = (float *) malloc ((CommandInfo.divs + 1) * sizeof (float));
				if (!samples) MALLOC_ERROR(false);

				for (int i = 0; i <= CommandInfo.divs; i++) samples[i] = 0.0f;
				Addendum -> ChangeSamples (samples);

				if (samples) free (samples);
			}

			if (Addendum) pf -> AddSensor (Addendum);
		}
	}

	if (CommandInfo.SelectRay)
	{
		CommandInfo.SelectRay = false;
		int ret = DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_SEL_RAY), 
					HWND_DESKTOP, (DLGPROC)SelectRayDlgProc);

		if (ret == IDCANCEL) return (false);
		
		pf -> ResetCurrency ();
		Sensor *s;
		bool found = false;
		int j = 0;
		while (!found)
		{
			s = pf -> GetNextSensor ();
			if (!s) return (false);

			if (s -> Match (CommandInfo.orient))
			{
				found = true;
				ActiveSensor = s;
			}
			else 
			{
				float *orient = s -> GetOrientation ();
				if ((orient[1] > CommandInfo.orient[1]) || (orient[1] == CommandInfo.orient[1] && orient[0] > CommandInfo.orient[0])) 
				{
					found = true;
					ActiveSensor = s;
				}
			}
			j++;
		}
		
		ActiveSensorNo = j;
		RenderInfo.ScreenActive = true;
	}

	if (CommandInfo.NextSensor)
	{
		CommandInfo.NextSensor = false;

		if (!pf) return (false);
		
		pf -> ResetCurrency ();

		for (int i = 0; i < ActiveSensorNo; i++) ActiveSensor = pf -> GetNextSensor ();
		
		if (!(ActiveSensor = pf -> GetNextSensor ()))
		{
			pf -> ResetCurrency ();

			ActiveSensor = pf -> GetNextSensor ();

			ActiveSensorNo = 1;
		}

		else ActiveSensorNo++;
	}

	if (CommandInfo.PrevSensor)
	{
		CommandInfo.PrevSensor = false;

		if (!pf) return (false);
		
		pf -> ResetCurrency ();

		if (ActiveSensorNo == 1) 
		{
			int sc = 0;
			pf -> ResetCurrency ();

			while (pf -> GetNextSensor ()) sc++;

			ActiveSensorNo = sc;
		}
		else ActiveSensorNo--;

		pf -> ResetCurrency ();

		for (int i = 0; i < ActiveSensorNo; i++) ActiveSensor = pf -> GetNextSensor ();
	}

	// RAY MENU
	if (CommandInfo.LoadRayFunction || CommandInfo.SaveRayFunction)
	{
		BOOL OpenSave = CommandInfo.LoadRayFunction;
		
		if (OpenSave) CommandInfo.LoadRayFunction = false;
		else CommandInfo.SaveRayFunction = false;

		CString fileName;

		if (!(Load_Save_Dialog (&fileName, (OpenSave) ? true : false, false))) return (false);

		LPCTSTR lpCtStr = (LPCTSTR) fileName;
		
		if (OpenSave) 
		{
			if (!(ActiveSensor -> LoadFromFile ((char *) lpCtStr))) MB(ID_CANNOT_LOAD_RAY)
		}

		else 
		{
			if (!(ActiveSensor -> SaveToFile ((char *) lpCtStr))) MB(ID_CANNOT_SAVE_RAY)
		}
	}
	
	if (CommandInfo.ResetRay)
	{
		CommandInfo.ResetRay = false;
		if (MessageBoxEx (HWND_DESKTOP, ID_RESET_QUESTION,
						  "Really reset?", MB_YESNO | MB_ICONWARNING, 0) == IDYES)
		{
			ActiveSensor -> Reset ();
		}
	}

	if (CommandInfo.ChangeRes)
	{
		CommandInfo.ChangeResNum = ActiveSensor -> GetDivisions ();
		CommandInfo.ChangeRes = false;
		DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_CHANGERES_DLG), 
					RenderInfo.WindowHandle, (DLGPROC)ChangeResDlgProc);

		ActiveSensor -> ChangeResolution (CommandInfo.ChangeResNum);
		RenderInfo.ActiveNode = 0;
		RenderInfo.ScreenActive = true;
	}

	if (CommandInfo.Numeric)
	{
		CommandInfo.Numeric = false;

		switch (CommandInfo.filterno)
		{
			case 0: CommandInfo.NewParam = phifilter -> GetParam () * 360.0;
					break;
	
			case 1: CommandInfo.NewParam = thetafilter -> GetParam () * 180.0;
					break;

			case 2: CommandInfo.NewParam = radialfilter -> GetParam ();
					break;

			case -1: {
						float *samples = ActiveSensor -> GetSamples ();
						if (RenderInfo.ActiveNode < 0) RenderInfo.ActiveNode = ActiveSensor -> GetDivisions ();
						if (RenderInfo.ActiveNode > ActiveSensor -> GetDivisions ()) RenderInfo.ActiveNode = 0;
						CommandInfo.NewParam = (double) samples[RenderInfo.ActiveNode];
					 }
						break;
		}
		
		DialogBox (RenderInfo.WindowInstance, MAKEINTRESOURCE(IDD_NUMERIC),
					HWND_DESKTOP, (DLGPROC)NumericDlgProc);

		switch (CommandInfo.filterno)
		{
			case 0: if (CommandInfo.NewParam > 0.0 && CommandInfo.NewParam <= 360.0)
						phifilter -> ChangeParametre (0, CommandInfo.NewParam / 360.0);
					break;
	
			case 1: if (CommandInfo.NewParam > 0.0 && CommandInfo.NewParam <= 180.0)
						thetafilter -> ChangeParametre (0, CommandInfo.NewParam / 180.0);
					break;

			case 2: if (CommandInfo.NewParam > 0.0 && CommandInfo.NewParam <= 1.0)
						radialfilter -> ChangeParametre (0, CommandInfo.NewParam);
					break;

			case -1: if (CommandInfo.NewParam >= -10.0 && CommandInfo.NewParam <= 10.0)
					 {
						float *samples = ActiveSensor -> GetSamples ();
						RenderInfo.MoveRayPoint = (float) CommandInfo.NewParam - samples[RenderInfo.ActiveNode];
					 }
					break;
		}
	}
	
	if (CommandInfo.DeleteRay)
	{
		CommandInfo.DeleteRay = false;

		if (MessageBoxEx (HWND_DESKTOP, ID_DELETE_QUESTION, "Really Delete?", MB_YESNO | MB_ICONWARNING | MB_TOPMOST, 0) == IDYES)
		{
			if (ActiveSensor) pf -> DeleteSensor (&ActiveSensorNo);

			pf -> ResetCurrency ();
		
			for (int i = 1; i < ActiveSensorNo; i++) pf -> GetNextSensor ();

			if (!(ActiveSensor = pf -> GetNextSensor ()))
			{
				pf -> ResetCurrency ();
				ActiveSensor = pf -> GetNextSensor ();
				ActiveSensorNo = 1;
			}
		}
	}

	// FILTER MENU
	if (CommandInfo.NewType)
	{
		CommandInfo.NewType = false;

		enum COMPONENTIDS CIDS;

		switch (CommandInfo.newtype)
		{
			case 0: CIDS = GAUSS;
					break;
	
			case 1: CIDS = SQUARE;
					break;

			case 2: CIDS = TRIANGLE;
					break;
		}

		switch (CommandInfo.filterno)
		{
			case 0: phifilter -> ChangeType (CIDS);
					break;
	
			case 1: thetafilter -> ChangeType (CIDS);
					break;

			case 2: radialfilter -> ChangeType (CIDS);
					break;
		}
	}

	// PREFERENCES
	if (CommandInfo.ChangeFont)
	{
		CommandInfo.ChangeFont = false;

		font = new CD3DFont (_T(Preferences.Font), Preferences.FontSize, 0L);
		font -> InitDeviceObjects (g_pd3dDevice);
		font -> RestoreDeviceObjects ();
	}

	return (true);
}

bool Viewer::ReleaseViewer ()
{
    if (g_pd3dDevice)	g_pd3dDevice -> Release ();
    if (g_pD3D)			g_pD3D -> Release ();
	if (g_pVB)			g_pVB -> Release ();
	if (font)			font -> InvalidateDeviceObjects ();

	return (true);
}

bool Viewer::SetupMatrices ()
{
	D3DXMATRIX matWorld, tmpWorld, matView;
	D3DXMatrixRotationX (&tmpWorld,	 RenderInfo.Rotation[1]);
	D3DXMatrixRotationY (&matWorld, -RenderInfo.Rotation[0]);
	D3DXMatrixMultiply (&matWorld, &matWorld, &tmpWorld);
	D3DXMatrixTranslation (&tmpWorld, 1.0f, 1.0f, 0.0f);
	D3DXMatrixMultiply (&matWorld, &matWorld, &tmpWorld);

	g_pd3dDevice -> SetTransform (D3DTS_WORLD, &matWorld);

    D3DXMatrixLookAtLH (&matView, &D3DXVECTOR3(0.0f, 3.0f,  7.0f), 
                                  &D3DXVECTOR3(0.0f, 0.0f, 0.0f), 
                                  &D3DXVECTOR3(0.0f, 1.0f, 0.0f));
    g_pd3dDevice -> SetTransform (D3DTS_VIEW, &matView);

    D3DXMATRIX matProj;
    D3DXMatrixPerspectiveFovLH (&matProj, D3DX_PI/4, 1.0f, 1.0f, 100.0f);
    g_pd3dDevice -> SetTransform (D3DTS_PROJECTION, &matProj);

	return (true);
}

bool Viewer::DisplayMenu ()
{
	DWORD color[8];
	int LOW = 0, HIGH = 0;
	int i;

	for (i = 0; i < 8; i++) color[i] = 0xffffffff;

	i = Get_Area_Code (RenderInfo.currentRightPosition.x, RenderInfo.currentRightPosition.y, ID_MENU_CODE) - ID_MENU_NW;
	if (i >= 0 && i < 8) color[i] = 0xffffff00;

	switch (Get_Area_Code (RenderInfo.rightclickPosition.x, RenderInfo.rightclickPosition.y))
	{
		case ID_CLICK_MAIN:
			LOW = 0; 
			HIGH = ID_MAIN_MENU_ITEMS;
			break;
		case ID_CLICK_RAY:
			LOW = ID_MAIN_MENU_ITEMS; 
			HIGH = ID_RAY_MENU_ITEMS;
			break;
		case ID_CLICK_PHIFILTER:
		case ID_CLICK_THETAFILTER:
		case ID_CLICK_RADIALFILTER:
			LOW = ID_RAY_MENU_ITEMS; 
			HIGH = ID_FILTER_MENU_ITEMS;
			break;
		default:	
			return (false);
	}

	font -> DrawText (RenderInfo.rightclickPosition.x + Menues[LOW].x, 
					  RenderInfo.rightclickPosition.y + Menues[LOW].y, 
					  0xffffffff, _T(Menues[LOW].MenuName));
	
	for (i = LOW + 1; i < HIGH; i++)
		font -> DrawText (RenderInfo.rightclickPosition.x + Menues[i].x, 
						 RenderInfo.rightclickPosition.y + Menues[i].y, 
						 color[i - LOW - 1], _T(Menues[i].MenuName));
		
	return (true);
}

bool Viewer::DisplayFilters ()
{
	D3DXMATRIX projection, world;
	float LENGTH = 2.0f;
	float HEIGHT = 1.0f;
	char str[40];
		
	CUSTOMVERTEX axes[18] = 
	{
		// Phi filter axes
		{-1.0f, 1.5f, 0.0f, 0xffffffff},
		{-2.7f, 1.5f, 0.0f, 0xffffffff},

		{-1.85f, 2.5f, 0.0f, 0xffffffff},
		{-1.85f, 1.5f, 0.0f, 0xffffffff},

		// Theta filter axes
		{-1.15f, -0.7f, 0.0f, 0xffffffff},
		{-3.0f, -0.7f, 0.0f, 0xffffffff},

		{-2.075f, -0.7f, 0.0f, 0xffffffff},
		{-2.075f, 0.5f, 0.0f, 0xffffffff},

		// Radial filter axes
		{-1.3f, -3.5f, 0.0f, 0xffffffff},
		{-3.4f, -3.5f, 0.0f, 0xffffffff},

		{-2.35f, -3.5f, 0.0f, 0xffffffff},
		{-2.35f, -2.0f, 0.0f, 0xffffffff},

		// Phi slider
		{-1.85f, 1.25f, 0.0f, 0xffffffff},
		{-2.7f, 1.25f, 0.0f, 0xffffffff},

		// Theta slider
		{-2.075f, -1.0f, 0.0f, 0xffffffff},
		{-3.0f, -1.0f, 0.0f, 0xffffffff},

		// Radial slider
		{-2.35f, -3.85f, 0.0f, 0xffffffff},
		{-3.4f, -3.85f, 0.0f, 0xffffffff},

	};

	CUSTOMVERTEX sliders[3] = 
	{
		{-1.85f - (float) phifilter -> GetParam () / 1.1764f, 1.25f, 0.0f, 0xffff0000},
		{-2.075f - (float) thetafilter -> GetParam () / 1.0810f, -1.0f, 0.0f, 0xffff0000},
		{-2.35f - (float) radialfilter -> GetParam () / 0.9524f, -3.85f, 0.0f, 0xffff0000},
	};

	D3DXMatrixIdentity (&world);
	g_pd3dDevice -> SetTransform (D3DTS_WORLD, &world);
	
	sprintf (str, "Param:  %1.3lfdeg", phifilter -> GetParam () * 360);
	font -> DrawText (Positions[ID_PHI_PARAM].x, Positions[ID_PHI_PARAM].y, 0xffffffff, _T(str));

	sprintf (str, "Param:  %1.3lfdeg", thetafilter -> GetParam () * 180);
	font -> DrawText (Positions[ID_THETA_PARAM].x, Positions[ID_THETA_PARAM].y, 0xffffffff, _T(str));

	sprintf (str, "Param:  %1.3lf", radialfilter -> GetParam ());
	font -> DrawText (Positions[ID_RADIAL_PARAM].x, Positions[ID_RADIAL_PARAM].y, 0xffffffff, _T(str));


	LPDIRECT3DVERTEXBUFFER8 axesVB, slidersVB;
	
	if (FAILED(g_pd3dDevice -> CreateVertexBuffer (18 * sizeof (CUSTOMVERTEX),
												0, D3DFVF_CUSTOMVERTEX,
												D3DPOOL_DEFAULT, &axesVB)))
		ERRORMB(ID_CANNOT_ALLOCATE_VB);

	if (FAILED(g_pd3dDevice -> CreateVertexBuffer (3 * sizeof (CUSTOMVERTEX), 0, D3DFVF_CUSTOMVERTEX, 
													D3DPOOL_DEFAULT, &slidersVB)))
		ERRORMB(ID_CANNOT_ALLOCATE_VB);
	VOID *axesVertices, *slidersVertices;
	if (FAILED(axesVB -> Lock (0, sizeof (axes), (BYTE **) &axesVertices, 0)))
		ERRORMB(ID_CANNOT_LOCK_VB)
	if (FAILED(slidersVB -> Lock (0, sizeof (sliders), (BYTE **) &slidersVertices, 0)))
		ERRORMB(ID_CANNOT_LOCK_VB)

	memcpy (axesVertices, axes, sizeof (axes));
	memcpy (slidersVertices, sliders, sizeof (sliders));

	axesVB -> Unlock ();
	slidersVB -> Unlock ();

	g_pd3dDevice -> SetStreamSource (0, axesVB, sizeof (CUSTOMVERTEX));
	g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

	g_pd3dDevice -> DrawPrimitive (D3DPT_LINELIST, 0, 9);

	if (axesVB) axesVB -> Release ();

	g_pd3dDevice -> SetStreamSource (0, slidersVB, sizeof (CUSTOMVERTEX));
	g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

	float PointSize = 4.0f;
	g_pd3dDevice -> SetRenderState (D3DRS_POINTSIZE, *((DWORD*)&PointSize));
	
	g_pd3dDevice -> DrawPrimitive (D3DPT_POINTLIST, 0, 3);

	if (slidersVB) slidersVB -> Release ();

	CUSTOMVERTEX *filter = (CUSTOMVERTEX *) malloc (ID_FILTER_RESOLUTION * sizeof (CUSTOMVERTEX));
	if (!filter) MALLOC_ERROR(false);

	int k;

	for (k = 0; k < 3; k++)
	{
		for (int j = 0; j < ID_FILTER_RESOLUTION; j++)
		{
			int s = j - ID_FILTER_RESOLUTION / 2;
			switch (k)
			{
				case 0: filter[j].x = -0.8f * LENGTH / ID_FILTER_RESOLUTION * j - 1.05f;
						filter[j].y = (float) phifilter -> CalculateValue (LENGTH / ID_FILTER_RESOLUTION * s) + 1.5f;
					    break;
				
				case 1: filter[j].x = 1.09f * -0.8f * LENGTH / ID_FILTER_RESOLUTION * j - 1.20f;
						filter[j].y = ((float) thetafilter -> CalculateValue (LENGTH / ID_FILTER_RESOLUTION * s)) * 1.2f - 0.7f;
					    break;
				
				case 2: filter[j].x = 1.24f * -0.8f * LENGTH / ID_FILTER_RESOLUTION * j - 1.35f;
						filter[j].y = ((float) radialfilter -> CalculateValue (LENGTH / ID_FILTER_RESOLUTION * s)) * 1.5f - 3.5f;
					    break;
			}
			filter[j].z = 0.0f;

			filter[j].color = 0xffff0000;
		}
		
		LPDIRECT3DVERTEXBUFFER8 filterVB;

		if (FAILED(g_pd3dDevice -> CreateVertexBuffer (ID_FILTER_RESOLUTION * sizeof (CUSTOMVERTEX), 
														0, D3DFVF_CUSTOMVERTEX, 
														D3DPOOL_DEFAULT, &filterVB)))
			ERRORMB(ID_CANNOT_ALLOCATE_VB);
		VOID *filterVertices;

		if (FAILED(filterVB -> Lock (0, ID_FILTER_RESOLUTION * sizeof (CUSTOMVERTEX), (BYTE **) &filterVertices, 0)))
			ERRORMB(ID_CANNOT_LOCK_VB);

		memcpy (filterVertices, filter, ID_FILTER_RESOLUTION * sizeof (CUSTOMVERTEX));

		filterVB -> Unlock ();

		g_pd3dDevice -> SetStreamSource (0, filterVB, sizeof (CUSTOMVERTEX));
		g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

		g_pd3dDevice -> DrawPrimitive (D3DPT_LINESTRIP, 0, ID_FILTER_RESOLUTION - 1);

		if (filterVB) filterVB -> Release ();
	}

	if (filter) free (filter);

	return (true);

}

bool Viewer::DisplayRay ()
{
	D3DXMATRIX projection, world;
	float LENGTH = 4.0f;
	float HEIGHT = 1.0f;

	D3DXMatrixIdentity (&world);
	g_pd3dDevice -> SetTransform (D3DTS_WORLD, &world);

	if (ActiveSensor)
	{
		float peek = 0.0f, mini = 0.0f;
		int divs = ActiveSensor -> GetDivisions ();
		float *samples = ActiveSensor -> GetSamples ();

		if (RenderInfo.ActiveNode < 0) RenderInfo.ActiveNode = divs;
		if (RenderInfo.ActiveNode > divs) RenderInfo.ActiveNode = 0;

		peek = 10.0f;
		
		CUSTOMVERTEX axes[4] = 
		{
			{3.0f, -2.7f, 0.0f, 0xffffffff},
			{-0.5f, -2.7f, 0.0f, 0xffffffff},

			{3.0f, -3.7f, 0.0f, 0xffffffff},
			{3.0f, -1.7f, 0.0f, 0xffffffff},
		};
			
		LPDIRECT3DVERTEXBUFFER8 axesVB;
	
		if (FAILED(g_pd3dDevice -> CreateVertexBuffer (4 * sizeof (CUSTOMVERTEX),
													0, D3DFVF_CUSTOMVERTEX,
													D3DPOOL_DEFAULT, &axesVB)))
			ERRORMB(ID_CANNOT_ALLOCATE_VB);

		VOID* axesVertices;
		if (FAILED(axesVB -> Lock (0, sizeof (axes), (BYTE **) &axesVertices, 0)))
			ERRORMB(ID_CANNOT_LOCK_VB)

		memcpy (axesVertices, axes, sizeof (axes));

		axesVB -> Unlock ();

		g_pd3dDevice -> SetStreamSource (0, axesVB, sizeof (CUSTOMVERTEX));
		g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

		g_pd3dDevice -> DrawPrimitive (D3DPT_LINELIST, 0, 2);

		if (axesVB) axesVB -> Release ();

		CUSTOMVERTEX *ray = (CUSTOMVERTEX *) malloc ((divs + 1) * sizeof (CUSTOMVERTEX));
		if (!ray) MALLOC_ERROR(false);
		
		for (int j = 0; j <= divs; j++)
		{
			ray[j].x = -0.8f * LENGTH / divs * j + 3.0f;
			ray[j].y = HEIGHT / peek * samples[j] - 2.7f;
			ray[j].z = 0.0f;

			ray[j].color = ToColor (samples[j]);
		}
		
		DWORD t = timeGetTime ();

		if ((t % 1000 > 500)) ray[RenderInfo.ActiveNode].color = D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00);
		
		char str[100];
		float *orient = ActiveSensor -> GetOrientation ();
		sprintf (str, "Value: %f", samples[RenderInfo.ActiveNode]);

		font -> DrawText (10, 530, 0xffff0000, _T(str));
		
		sprintf (str, "Orientation: %3.2f, %2.2f", orient[0], orient[1]);
		font -> DrawText (10, 350, 0xffff0000, _T(str));
		
		LPDIRECT3DVERTEXBUFFER8 rayVB;

		if (FAILED(g_pd3dDevice -> CreateVertexBuffer ((divs + 1) * sizeof (CUSTOMVERTEX), 
														0, D3DFVF_CUSTOMVERTEX, 
														D3DPOOL_DEFAULT, &rayVB)))
			ERRORMB(ID_CANNOT_ALLOCATE_VB);
		VOID *rayVertices;

		if (FAILED(rayVB -> Lock (0, (divs + 1) * sizeof (CUSTOMVERTEX), (BYTE **) &rayVertices, 0)))
			ERRORMB(ID_CANNOT_LOCK_VB);

		memcpy (rayVertices, ray, (divs + 1) * sizeof (CUSTOMVERTEX));

		rayVB -> Unlock ();

		g_pd3dDevice -> SetStreamSource (0, rayVB, sizeof (CUSTOMVERTEX));
		g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

		g_pd3dDevice -> DrawPrimitive (D3DPT_LINESTRIP, 0, divs);

		float PointSize = 3.0f;
		g_pd3dDevice -> SetRenderState (D3DRS_POINTSIZE, *((DWORD*)&PointSize));

		g_pd3dDevice -> DrawPrimitive (D3DPT_POINTLIST, 0, divs + 1);

		if (rayVB) rayVB -> Release ();

		if (ray) free (ray);
	}
	return (true);

}

bool Viewer::Render ()
{
	if (!g_pd3dDevice) return (false);

    // Clear the backbuffer to a black color
	g_pd3dDevice -> Clear (0L, NULL, D3DCLEAR_TARGET, Preferences.BackgroundColor, 1.0f, 0L);
    
    // Begin the scene
    g_pd3dDevice -> BeginScene ();

	g_pd3dDevice -> SetRenderState (D3DRS_CULLMODE, D3DCULL_NONE);

	font -> DrawText (Positions[ID_MAIN_TEXT].x, Positions[ID_MAIN_TEXT].y, 0xffff0000, _T(ID_MAIN_VIEW));
	font -> DrawText (Positions[ID_RAY_TEXT].x, Positions[ID_RAY_TEXT].y, 0xffff0000, _T(ID_RAY_VIEW));
	font -> DrawText (Positions[ID_PHI_TEXT].x, Positions[ID_PHI_TEXT].y, 0xffff0000, _T(ID_PHI_VIEW));
	font -> DrawText (Positions[ID_THETA_TEXT].x, Positions[ID_THETA_TEXT].y, 0xffff0000, _T(ID_THETA_VIEW));
	font -> DrawText (Positions[ID_RAD_TEXT].x, Positions[ID_RAD_TEXT].y, 0xffff0000, _T(ID_RAD_VIEW));
	
	if (DisplayHelper) font -> DrawText (Positions[ID_HELPER_TEXT].x, Positions[ID_HELPER_TEXT].y, 0xffffff00, ID_HELPER);

	char fname[400];
	char fname2[400];
	int i, j = 0;
	strcpy (fname, pfname);

	for (i = 0; i < (signed int) strlen (fname); i++)
	{
		fname[j] = fname[i];
		if (fname[i] == '\\' || fname[i] == '/') j = 0;
		else j++;
	}
	for (i = j; i < 400; i++) fname[i] = '\000';

	sprintf (fname2, "[%s]", fname);

	font -> DrawText (Positions[ID_FILE_TEXT].x, Positions[ID_FILE_TEXT].y, 0xffff0000, _T(fname2));

	DisplayRay ();
	DisplayFilters ();

	SetupMatrices ();
	
	if (RenderInfo.RMBDown)
	{
		DisplayMenu ();
	}

	Sensor *sensor;
	if (pf)
	{
		pf -> ResetCurrency ();
		while (sensor = pf -> GetNextSensor ())
		{
			float *samples, *orientation, length;
			int divs;
			bool active = false;

			if (ActiveSensor) 
			{
				float *orient;
				orient = ActiveSensor -> GetOrientation ();
				if (sensor -> Match (orient)) active = true;
			}
			
			divs = sensor -> GetDivisions ();
			length = sensor -> GetLength ();
			orientation = sensor -> GetOrientation ();
			samples = sensor -> GetSamples ();
			
			CUSTOMVERTEX *vertices = (CUSTOMVERTEX *) malloc ((divs + 2) * sizeof (CUSTOMVERTEX));
			if (!vertices) MALLOC_ERROR(false);
			LPDIRECT3DVERTEXBUFFER8 sensorVB;

			vertices[0].x = vertices[0].y = vertices[0].z = 0.0f;
			vertices[0].color = 0xffffffff;

			for (int i = 0; i <= divs; i++)
			{
				float desc[3];
				
				Spherical2Descartean (desc, orientation, length / divs * i);
				
				vertices[i].x = 2 * desc[0] * 0.8f;
				vertices[i].y = 2 * desc[1] * 0.8f;
				vertices[i].z = 2 * desc[2] * 0.8f;

				DWORD t = timeGetTime ();

				if ((active && (t % 1000 < 500)) || (!active)) vertices[i].color = ToColor (samples[i]);
				else vertices[i].color = D3DCOLOR_ARGB(0xff, 0x00, 0x00, 0x00);
			}

			if (FAILED(g_pd3dDevice -> CreateVertexBuffer ((divs + 1)* sizeof (CUSTOMVERTEX),
															0, D3DFVF_CUSTOMVERTEX,
															D3DPOOL_DEFAULT, &sensorVB)))
						ERRORMB(ID_CANNOT_ALLOCATE_VB);

			VOID* sensorVertices;
			if (FAILED(sensorVB -> Lock (0, (divs + 1) * sizeof (CUSTOMVERTEX), (BYTE **) &sensorVertices, 0)))
				ERRORMB(ID_CANNOT_LOCK_VB)

			memcpy (sensorVertices, vertices, (divs + 1) * sizeof (CUSTOMVERTEX));

			sensorVB -> Unlock ();

			g_pd3dDevice -> SetStreamSource (0, sensorVB, sizeof (CUSTOMVERTEX));
			g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

			g_pd3dDevice -> DrawPrimitive (D3DPT_LINESTRIP, 0, divs);

			if (SHOULD_DISPLAY_POINTS)
			{
				float PointSize = 3.0f;
				g_pd3dDevice -> SetRenderState (D3DRS_POINTSIZE, *((DWORD*)&PointSize));

				g_pd3dDevice -> DrawPrimitive (D3DPT_POINTLIST, 0, divs + 1);
			}

			if (sensorVB) sensorVB -> Release ();
			free (vertices);
		}
		
		float origin[3] = {0.0f, 0.0f, 0.0f};
		Spherical2Descartean (origin, origin, 1.5f);
		CUSTOMVERTEX indicator[6] = 
		{
			{origin[0] - 0.1f,	origin[1],			origin[2],			0x10ffff00},
			{origin[0],			origin[1],			origin[2] + 0.1f,	0x10ffff00},
			{origin[0] + 0.1f,	origin[1],			origin[2],			0x10ffff00},
			
			{origin[0],			origin[1],			origin[2],			0x10ffff00},
			{origin[0],			origin[1],			origin[2] + 0.1f,	0x10ffff00},
			{origin[0],			origin[1] + 0.1f,	origin[2],			0x10ffff00},
		};
		
		LPDIRECT3DVERTEXBUFFER8 indicatorVB;
			
		if (FAILED(g_pd3dDevice -> CreateVertexBuffer (6 * sizeof (CUSTOMVERTEX),
													0, D3DFVF_CUSTOMVERTEX,
													D3DPOOL_DEFAULT, &indicatorVB)))
			ERRORMB(ID_CANNOT_ALLOCATE_VB);

		VOID* indicatorVertices;
		if (FAILED(indicatorVB -> Lock (0, sizeof (indicator), (BYTE **) &indicatorVertices, 0)))
			ERRORMB(ID_CANNOT_LOCK_VB)

		memcpy (indicatorVertices, indicator, sizeof (indicator));

		indicatorVB -> Unlock ();

		g_pd3dDevice -> SetStreamSource (0, indicatorVB, sizeof (CUSTOMVERTEX));
		g_pd3dDevice -> SetVertexShader (D3DFVF_CUSTOMVERTEX);

		g_pd3dDevice -> DrawPrimitive (D3DPT_TRIANGLELIST, 0, 2);

		if (indicatorVB) indicatorVB -> Release ();		
	}
	// End the scene
    g_pd3dDevice -> EndScene ();
    
    // Present the backbuffer contents to the display
    g_pd3dDevice -> Present (NULL, NULL, NULL, NULL);
	
	return (true);
}