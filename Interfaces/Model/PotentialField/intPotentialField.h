// ************************************************************
// FILE: intPotentialField
// Author: Zoltan Dezso
// This file contains the declaration of a tunable Potential Field
// interface.
// ************************************************************

interface intPotentialField
{
public:
	// FUNCTION: bool NewPotentialField (int phires, int thetares, int divisions, float length)
	// This function creates a new default potential field, which is restricted to have a fixed resolution of sensors
	// along both axes. It also requires that *all* sensors have the same length and radial resolution.
	// This method serves only as to retain a compatibility and ease of use towards the user, and
	// please note, that new potential field creation is deprecated this way. One should load a previously
	// created template file instead.
	// All the resolution values are integers. They should not be negative, but a 0 value in either of them should
	// create no sensors, or only one, pointing at 0, 0 (degrees phi and theta) 
	// Length is a non-negative float value. A zero value indicates, that no, or one sensor should be created
	bool NewPotentialField  (int, int, int, float);
	
	// FUNCTION: bool LoadPotentialField (char *)
	// This funtion loads a fixed format potential field from the disk. The name of the file is passed
	// as input argument, as zero-terminated char string. The string should not be NULL or consisting only
	// of a single '\000' character. 
	// The Function returns true if the loading was successful and the appropriate modifications to the
	// potential field are done and false otherwise.
	bool LoadPotentialField (char *);

	// FUNCTION: bool SavePotentialField (char *)
	// This function saves a fixed format potential field file to disk. The name of the file is given as input 
	// argument the very same way as in LoadPotentialField ();
	// The function returns true if the save was successful, false otherwise
	bool SavePotentialField (char *);

	// FUNCTION: bool AddSensor (Sensor *)
	// This funtion creates a new sensor provided a sensor with the same orientation does not exist alredy.
	// The sensor is given as an input argument, which is the pointer to the sensor. The pointer should
	// not be NULL. The function returns true if the sensor was added to the potential field, false  otherwise.
	bool AddSensor (Sensor *);

	// FUNCTION: float CalculateAt (float *point)
	// This function calculates the value of the field in a given point. The point in question is given
	// as a pointer to an array of floats containing at least three elements, which are explained as follows:
	// point[0] is angle phi, point[1] is angle theta, while point[2] is the radius along this ray. All angles
	// are given as degrees, phi being between [0.0f, 360.0f), theta being between [-90.0f, 90.0f]
	// If the radius is outside the potential field, a zero value should be returned
	float CalculateAt (float *);

	// FUNCTION: bool ResetCurrency (void)
	// This function returns the currency pointer of the class to the first sensor, so that the next call
	// of the below GetNextSensor returns the first sensor in the potential field.
	// NB.: First does not necessarily mean ordering, it only means, that the sensors are accessible
	// in a serial way, one at a time, each one only once in a run.
	// The function returns true if the reset action was successful, false otherwise
	bool ResetCurrency ();

	// Sensor *GetNextSensor (void)
	// This function returns the next unprocessed sensor (whatever processing means), that is the next existing
	// sensor in the field that was not returned since the last time the currency was reset (either by force,
	// or by having all the sensors returned).
	// The function returns NULL, if there are no more sensors to be returned. It preferably resets currency
	// after this, but this feature is not a must.
	Sensor *GetNextSensor ();
};
