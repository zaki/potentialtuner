class Application : public intApplication
{
private:
	PotentialField *potentialfield;
	Viewer *viewer;	
	//OnScreenMenu *onscreenmenu;

public:
	bool Initialize ();
	bool Run ();
	bool Exit ();
};

#include "Implementation/Control/Application/ApplicationFUNC.h"