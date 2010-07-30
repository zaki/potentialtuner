class Viewer// : public intViewer
{
private:
	LPDIRECT3D8             g_pD3D;			// base d3d object
	LPDIRECT3DDEVICE8       g_pd3dDevice;	// d3d device
	LPDIRECT3DVERTEXBUFFER8 g_pVB;			// vertex buffer for primitives
	CD3DFont				*font;			// A font, that can render itself into the screen
	int						WindowSize[2];	// Window size (width, height)

	PotentialField			*pf;
	Sensor					*ActiveSensor;
	int						ActiveSensorNo;
	char					pfname[400];
	FilterComponent			*phifilter, *thetafilter, *radialfilter;

public:
	Viewer () 
	{
		pf = new PotentialField (); 
		pf -> NewPotentialField (5, 4, 10, 1.0); 
		//pfname = (char *)malloc (strlen (ID_DEFAULT_PF_NAME) * sizeof (char));
		strcpy (pfname, ID_DEFAULT_PF_NAME);
		WindowSize[0] = ID_WINDOW_WIDTH;
		WindowSize[1] = ID_WINDOW_HEIGHT;
		CreateViewer ();
	}
	~Viewer () {ReleaseViewer ();}

	bool CreateViewer ();
	bool ReleaseViewer ();

	bool Render ();

	bool RegisterPField (PotentialField *_pf) {if (_pf) {pf = _pf; return (true);} return (false);}

private:
	bool SetupMatrices ();
	bool CheckCommands ();
	bool DisplayMenu ();
	bool DisplayRay ();
	bool DisplayFilters ();

	bool CheckD3DCaps ();
};

#include "Implementation/View/Viewer/ViewerFUNC.h"