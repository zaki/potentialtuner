// **************************************************************
// FILE: SensorArray.h
// Author: Zoltan Dezso
// This file contains the declaration of the class SensorArray, 
// that is responsible for providing a storage area for the 
// sensors in the Potential Field. Also the declaration of the 
// structure that is the basis for the chained list that actually
// stores the sensors in an ordered list is included here.
// **************************************************************
struct SensorArrayElement
{
	Sensor sensor;				// The actual storage element
	SensorArrayElement *next;	// Pointer to the next element in the list or NULL if no next
};

class SensorArray
{
private:
	int sensorcount;				// Used for getting FAST information on the number of sensor
	SensorArrayElement *Sensors;	// The beginning of the sensor list
	SensorArrayElement *current;	// A currency pointer, that represents the next sensor to be returned
									// when GetNextSensor is called (see later)

public:
	// Constructors
	SensorArray () 
	{
		// The constructor creates an empty list
		sensorcount = 0;
		Sensors = NULL;
	}

	~SensorArray ()
	{
		// Should there be any sensors in the list, we should free all the space they use
		if (Sensors)
		{
			SensorArrayElement *curSensor = Sensors, *freedSensor;
			
			// Note, that GetNextSensor is not preferred here
			while (curSensor)
			{
				freedSensor = curSensor;
				curSensor = curSensor -> next;

				free (freedSensor);
			}
		}
	}

	// FUNCTION: ResetCurrency ()
	// This function resets the currency pointer, so that it points to the first if any element in the 
	// sensor list. By calling this function, one can assure, that the next call to GetNextSensor
	// returns the first if any element in the sensor list.
	void ResetCurrency () {current = Sensors;}

	// FUNCTION: Clear ()
	// This function deletes all the sensors in the sensor list.
	void Clear () 
	{
		if (Sensors)
		{
			SensorArrayElement *curSensor = Sensors, *freedSensor;
			
			while (curSensor)
			{
				freedSensor = curSensor;
				curSensor = curSensor -> next;

				free (freedSensor);
			}
		}

		sensorcount = 0;

		Sensors = NULL;
	}

	// FUNCTION: AddSensor (Sensor *)
	// This function inserts the sensor given by input argument, if it exists. First the function
	// makes sure, that the sensor to be added is not already in the list, then it searches for the
	// appropriate place for the sensor in the list and finally inserts it.
	// Returns false if the sensor does not exist (ie is NULL) or is already in the list
	bool AddSensor (Sensor *newSensor);

	// FUNCTION: GetNextSensor ()
	// Returns the element pointed by the currency pointer, if it is a valid element. 
	// Returns NULL otherwise
	// Please note, that this function returns a pointer to a SENSORARRAYELEMENT structure
	SensorArrayElement *GetNextSensor () 
	{
		SensorArrayElement *ret = NULL;

		if (current) 
		{
			ret = current;
			current = current -> next;
		}

		// If the currency pointer has come around, also reset it to the first element in the list if any.
		else 
			ResetCurrency ();

		return (ret);
	}

	// FUNCTION: Show ()
	// !!!!!!!!!!!!!!! LEGACY FUNCTION DO NOT USE !!!!!!!!!!!!!!!!!!!!!!
	bool Show ()
	{
		char *Str = (char *)malloc (40);
		int i = 0;

		SensorArrayElement *sae;

		ResetCurrency ();

		while (sae = GetNextSensor ())
		{
			sprintf (Str, "Sensor #%d", i);

			MB(Str);
			sae -> sensor.Show ();

			i++;
		}
		return (true);
	}

	bool Delete (int *);

	// FUNCTION: GetSensorCount ()
	// Returns the number of sensors in the sensor list.
	int GetSensorCount () {return (sensorcount);}

	// FUNCTION: GNS()
	// This function is a helper, that extracts the actual sensor pointer from the 
	// SensorArrayElement structure returned by GetNextSensor, or returns NULL, if there is
	// no next element in the list
	// LEGACY FUNCTION, do not use
	Sensor *GNS ()
	{
		SensorArrayElement *sae = GetNextSensor ();
		if (!sae) return (NULL);
		return (&(sae -> sensor));
	}
};

#include "Implementation/Model/SensorArray/SensorArrayFUNC.h"
