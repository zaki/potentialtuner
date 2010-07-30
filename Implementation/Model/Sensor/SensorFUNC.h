// **************************************************************
// FUNCTION: CalculateAt
// This function takes a float input argument, which must fall
// between zero and the length of the sensor, and approximates 
// the value of the potential function on the sensor ray.
// Should the input argument be outside the valid limits, the
// function returns zero. The return value is type float.
// **************************************************************
float Sensor::CalculateAt	(float point)
{
	int leftindex;				// Needed for approximation (this is the index of the left neighbor)
	float onestep, Phi, ret;	// NOTE: Phi is the value of the member function for approximation

	// Check constraints
	if (point > Length || point < 0) return (0.0f);

	// Calculate necessary information for the approximator
	leftindex = (int) floor (Divisions / Length * point);
	onestep = Length / Divisions;

	// Phi is calculates by a function called CalculatePhi. For more information on the method
	// of calculating phi, please see the declaration of that function.
	Phi = CalculatePhi (point, onestep*leftindex, onestep*(leftindex+1 < Divisions ? leftindex+1 : -1), PHIMETHOD);

	// Value = SUMi (PHIi)*FUNCTION(i) We also know, that PHI0 = 1 - PHI1
	ret = Phi * Samples[leftindex] + (1 - Phi) * (leftindex + 1 < Divisions ? Samples[leftindex + 1] : 0);

	return (ret);
}

// **************************************************************
// FUNCTION: ChangeResolution
// This function changes the number of divisons (sample points)
// on the sensor. Please note, that it also calculates the
// APPROXIMATED values at the new sample points. Please also note,
// that no resolution below 1 is acceptable, and that the actual
// number of samples will be one more, as there is a point at zero.
// If the input argument is negative or zero, or an error occured,
// false is returned. True is returned otherwise.
// **************************************************************
bool Sensor::ChangeResolution	(int newResolution)
{
	if (newResolution <= 0) return (false);

	float newstep = Length / newResolution;
	float *NewSamples = (float *) malloc ((newResolution + 1) * sizeof (float));

	if (!NewSamples) MALLOC_ERROR (false);

	for (int i = 0; i <= newResolution; i++)
		NewSamples[i] = CalculateAt (newstep*i);

	free (Samples);

	Samples = NewSamples;

	Divisions = newResolution;

	return (true);
}


bool Sensor::ChangeOrientation	(float newOrientation[2])
{
	if (newOrientation[0] < 0.0f || newOrientation[0] >= 360.0f) return (false);
	if (newOrientation[1] < -90.0f || newOrientation[1] > 90.0f) return (false);
	
	Direction[0] = newOrientation[0];
	Direction[1] = newOrientation[1];

	return (true);
}

bool Sensor::ChangeLength		(float newLength)
{
	Length = newLength;
	return (true);
}

bool Sensor::ChangeSamples		(float *newSamples)
{
	if (!newSamples) return (false);

	for (int i = 0; i <= Divisions; i++)
		Samples[i] = newSamples[i];

	return (true);
}

bool Sensor::ChangeSample		(int n, float newSample)
{
	if (n < 0 || n > Divisions) return (false);

	Samples[n] = newSample;
	return (true);
}

bool Sensor::Change				(float newOrientation[2], int newDivisions, float newLength, float *newSamples)
{
	ChangeOrientation (newOrientation);
	ChangeResolution (newDivisions);
	ChangeLength (newLength);
	ChangeSamples (newSamples);
	return (true);
}

bool Sensor::Match (float reqOrientation[2])
{
	if (Direction[0] == reqOrientation[0] && Direction[1] == reqOrientation[1]) return (true);

	return (false);
}

bool Sensor::SaveToFile (char *filename)
{
	FILE *output = fopen (filename, "w");

	fprintf (output, "[Divisions]\n");
	fprintf (output, "%d\n", Divisions);
	fprintf (output, "[Samples]\n");
	for (int i = 0; i <= Divisions; i++)
	{
		fprintf (output, "%f\n", Samples[i]);
	}

	fclose (output);

	return (true);
}

bool Sensor::LoadFromFile (char *filename)
{
	FILE *input = fopen (filename, "r");
	char str[100];
	int divs;
	float *samples = NULL;

	fscanf (input, "%s", str);
	if (strcmp (str, "[Divisions]")) return (false);

	fscanf (input, "%d", &divs);
	samples = (float *)malloc ((divs + 1) * sizeof (float));

	if (!samples) ERRORMB("Cannot allocate dynamic memory");

	fscanf (input, "%s", str);
	if (strcmp (str, "[Samples]")) return (false);

	
	for (int i = 0;i <= divs; i++)
		fscanf (input, "%f", &samples[i]);

	fclose (input);

	ChangeResolution (divs);
	ChangeSamples (samples);
	if (samples) free (samples);

	return (true);
}

bool Sensor::Reset ()
{
	for (int i = 0; i <= Divisions; i++)
		Samples[i] = 0.0f;
	return (true);
}