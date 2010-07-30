// ************************************************************
// FILE: intSensor.h
// Author: Zoltan Dezso
// This file contains the declaration of the interface Sensor, which
// is intended to become the second generation of the distance
// sensor class used in previous releases. It makes a difference
// in the representation of the sensor, as it uses a spherical
// coordinate system. 
// ************************************************************


interface intSensor
{	
private:

public:
	Sensor ();

	// FUNCTION: float CalculateAt (float radius)
	// This function approximates the value of the sensor at the distance 'radius' from the sensor's
	// origin. It returns a float type value, containing the calculated value
	float	CalculateAt			(float);

	// FUNCTION: bool Match (float orientation[2])
	// This function returns true if and only if the orientation of the sensor matches (ie. is equal)
	// to the parameters given as float [2] type orientation (orientation[0] is phi while orientation[1] is theta)
	// NB.: phi means the rotation *along* the base circle as theta means rotation *around* the base axis, 
	// (ie. phi is horizontal, theta is vertical angle)
	// The range of phi if between 0.0f and 360.0f with the latter excluded from the range.
	// The range of theta is between -90.0f and 90.0f including both limits.
	bool	Match				(float[2]);

	// FUNCTION: float *GetOrientation (void)
	// This function returns the orientation of the sensor in a non-NULL pointer to a float array
	// containing at least two elements. The first two elements ([0] and [1]) are implemented as
	// phi and theta (see above) of the sensor
	float  *GetOrientation		();

	// FUNCTION: float GetLength (void)
	// This funtion returns the length of the sensor, ie. the distance between the sensor stem (origin) and
	// its maximal sensing distance. It returns a float value containing the length of the sensor.
	float   GetLength			();
	
	// FUNCTION: int GetDivisions ()
	// This function returns the number of radii, that contain a sample value. That is the radial resolution
	// of the sensor. The number cannot be negative, and it has to match the number of actual samples in the
	// sensor. By returning 0 the sensor can indicate either incapability or unwillingness of returning samples.
	int		GetDivisions		();
	
	// FUNCTION: float *GetSamples (void)
	// This function returns a float type array of samples, that contains as many number of float type values
	// as the sensor would return when called GetDivisions. The sensor can (but should not) indicate incapability
	// or unwillingness to give samples any time by returning NULL. However it is strongly advised, that the
	// sensor indicates this by returning 0 when GetDivisions is called (see above)
	float  *GetSamples			();
};
