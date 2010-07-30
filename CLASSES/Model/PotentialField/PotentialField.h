// ************************************************************
// FILE: PotentialField
// Author: Zoltan Dezso (in cooperation with Denisz Polgar)
//
// ************************************************************

class PotentialField : public intPotentialField
{
private:
	SensorArray *Sensors;
	float alpha;
	float h;
	float a;
	float b;
	float halpha;
	float MaxDist;

public:
	PotentialField () {Sensors = new SensorArray (); MaxDist = 1.0f;}

	~PotentialField () {if (Sensors) delete Sensors;}

	bool NewPotentialField  (int, int, int, float);
	bool NewPotentialField  (int, int, int, float, char *);
	bool LoadPotentialField (char *);
	bool SavePotentialField (char *);

	bool AddSensor (Sensor *nS);
	bool DeleteSensor (int *);

	float CalculateAt (float *);

	bool Administer (float, float, float, float, float, float);
	bool GetInfo (float *, float *, float *, float *, float *);
	
	bool Show ();

	bool ResetArrayCurrency () {Sensors -> ResetCurrency (); return (true);}
	bool ResetCurrency () {Sensors -> ResetCurrency (); return (true);}

	Sensor *GetNextSensor () 
	{
		return (Sensors -> GNS ());
	}
};

#include "Implementation/Model/PotentialField/PotentialFieldFUNC.h"