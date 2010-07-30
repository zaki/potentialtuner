// ************************************************************
// FILE: Sensor.h
// Author: Zoltan Dezso (previous version by Denisz Polgar)
// This file contains the declaration of the class Sensor, which
// is intended to become the second generation of the distance
// sensor class used in previous releases. It makes a difference
// in the representation of the sensor, as it uses a spherical
// coordinate system. Please make sure also to check the imple-
// mentation file as it contains important comments on the code.
// ************************************************************

class Sensor : public intSensor
{	
private:
	float Direction[2];		// Using spherical coordinate system, describe the sensor's direction
	int Divisions;			// The number of samples along the sensor ray (there always is a sample at 0 radius)
	float Length;			// The maximum distance the sensor can see
	float *Samples;			// The holder for the actual samples


public:
	Sensor ()
	{
		// Default constructor, fills everything with zeroes
		Direction[0] = Direction[1] = 0.0f;
		Length = 1.0f;
		Divisions = 1;
		Samples = (float *) malloc (2 * sizeof (float));
		Samples[0] = Samples[1] = 0.0f;
	}

	~Sensor () 
	{
		free (Samples);
	}

	float	CalculateAt			(float);
	bool	ChangeResolution	(int);
	bool	ChangeOrientation	(float [2]);
	bool	ChangeLength		(float);
	bool	ChangeSamples		(float *);
	bool	ChangeSample		(int, float);

	bool	Change				(float[2], int, float, float *);
	bool	Reset				();

	bool	Match				(float[2]);

	float  *GetOrientation		() {return (Direction);}
	float   GetLength			() {return (Length);}
	int		GetDivisions		() {return (Divisions);}
	float  *GetSamples			() {return (Samples);}

	bool	SaveToFile			(char *);
	bool	LoadFromFile		(char *);

	bool	Show				() 
	{
		char *Str0 = (char *)malloc (40);
		char *Str = (char *)malloc (1000);

		sprintf (Str, "phi:%5.2f theta:%5.2f len:%5.2f div:%5d\n", Direction[0], Direction[1], Length, Divisions);

		for (int i = 0; i < Divisions; i++)
		{
			sprintf (Str0, ",%5.2f", Samples[i]);
			Str = strcat (Str, Str0);
		}

		MB(Str);
		return (true);
	}
};

#include "Implementation/Model/Sensor/SensorFUNC.h"