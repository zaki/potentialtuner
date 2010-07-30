bool PotentialField::NewPotentialField (int phires, int thetares, int divisions, float length)
{
	Sensor *newSensor;
	float onestepphi, onesteptheta;
	float orientation[2];
	float *samples = NULL;

	if (phires == 0 || divisions == 0) return (false);
	
	if (Sensors) delete Sensors;
	Sensors = new SensorArray ();

	samples = (float *)malloc ((divisions + 1)* sizeof (float));

	for (int i = 0; i <= divisions; i++) samples[i] = 0.0f;
	
	onestepphi = 360.0f / phires;
	onesteptheta = 180.0f / thetares;

	if (phires == 1) onestepphi = 0.0f;
	if (thetares == 1) onesteptheta = 0.0f;

	if (thetares == 0)
	{
		for (int i = 0; i <= phires; i++)
		{
			orientation[0] = i * onestepphi;
			orientation[1] = 0.0f;
			newSensor = new Sensor ();
			newSensor -> Change (orientation, divisions, length, samples);
			Sensors -> AddSensor (newSensor);

			Progress (phires + 1, i);
		}
	}
	
	else 
	{
		for (int j = 0; j <= thetares; j++)
			for (int i = 0; i <= phires; i++)
			{
				orientation[0] = i * onestepphi;
				orientation[1] = (thetares != 1) ? j * onesteptheta - 90.0f : 0.0f;
				newSensor = new Sensor ();
				newSensor -> Change (orientation, divisions, length, samples);
				Sensors -> AddSensor (newSensor);

				Progress ((phires + 1)*(thetares + 1), (phires + 1)*j + i);
			}
	}

	if (samples) free (samples);
	return (true);
}

bool PotentialField::NewPotentialField (int phires, int thetares, int divisions, float length, char *filename)
{
	Sensor *newSensor;
	float onestepphi, onesteptheta;
	float orientation[2];
	float *samples;

	if (phires == 0 || divisions == 0) return (false);

	if (Sensors) delete Sensors;
	Sensors = new SensorArray ();
	samples = (float *)malloc ((divisions + 1)* sizeof (float));

	for (int i = 0; i <= divisions; i++) samples[i] = 0.0f;
	
	onestepphi = 360.0f / phires;
	onesteptheta = 180.0f / thetares;

	if (thetares == 0)
	{
		for (int i = 0; i <= phires; i++)
		{
			orientation[0] = i * onestepphi;
			orientation[1] = 0.0f;
			newSensor = new Sensor ();
			newSensor -> Change (orientation, divisions, length, samples);
			newSensor -> LoadFromFile (filename);
			Sensors -> AddSensor (newSensor);

			Progress ((phires + 1)*(thetares + 1), i);
		}
	}
	
	else 
	{
		for (int j = 1; j < thetares; j++)
			for (int i = 0; i < phires; i++)
			{
				orientation[0] = i * onestepphi;
				orientation[1] = j * onesteptheta - 90.0f;
				newSensor = new Sensor ();
				newSensor -> Change (orientation, divisions, length, samples);
				newSensor -> LoadFromFile (filename);
				Sensors -> AddSensor (newSensor);

				Progress ((phires + 1)*(thetares + 1), (phires + 1)*j + i);
			}
	}
	if (samples) free (samples);
	return (true);
}

bool PotentialField::LoadPotentialField (char *filename)
{
	float params, length;
	float orientation[2];
	int SensorNum;
	int Sdivisions;
	Sensor *Addendum;
	char *Str = (char *) malloc (100);
	FILE *loadfile = fopen (filename, "r");
	
	if (!loadfile) return (false);

	Sensors -> Clear ();

	fscanf (loadfile, "%s", Str);
	if (strcmp (Str, "[Parametres]")) return (false);
	memset (Str, '\000', 100);
	
	fscanf (loadfile, "%s", Str);
	if (strcmp (Str, "<Learning>")) return (false);
	memset (Str, '\000', 100);

	fscanf (loadfile, "%f", &alpha);
	fscanf (loadfile, "%f", &h);
	fscanf (loadfile, "%f", &a);
	fscanf (loadfile, "%f", &b);
	fscanf (loadfile, "%f", &halpha);
	
	fscanf (loadfile, "%s", Str);
	if (strcmp (Str, "<SensorCount>")) return (false);
	memset (Str, '\000', 100);
	fscanf (loadfile, "%d", &SensorNum);

	fscanf (loadfile, "%s", Str);
	if (strcmp (Str, "[Sensors]")) return (false);
	memset (Str, '\000', 100);

	for (int i = 0; i < SensorNum; i++)
	{
		fscanf (loadfile, "%s", Str);
		if (strcmp (Str, "<Coordinate>")) return (false);
		memset (Str, '\000', 100);

		fscanf (loadfile, "%f", &orientation[0]);
		fscanf (loadfile, "%f", &orientation[1]);

		fscanf (loadfile, "%s", Str);
		if (strcmp (Str, "<Administration>")) return (false);
		memset (Str, '\000', 100);

		fscanf (loadfile, "%f", &length);
		fscanf (loadfile, "%d", &Sdivisions);
		
		fscanf (loadfile, "%s", Str);
		if (strcmp (Str, "<Samples>")) return (false);
		memset (Str, '\000', 100);
		
		Addendum = new Sensor ();
		Addendum -> ChangeOrientation (orientation);
		Addendum -> ChangeResolution (Sdivisions);
		Addendum -> ChangeLength (length);

		for (int j = 0; j <= Sdivisions; j++)
		{
			fscanf (loadfile, "%f", &params);
			Addendum -> ChangeSample (j, params);
		}

		Sensors -> AddSensor (Addendum);
	
		Progress (SensorNum, i);
	}

	SetWindowText (RenderInfo.WindowHandle, ID_ACTIVE_TITLE);
	fclose (loadfile);
	return (true);
}


bool PotentialField::SavePotentialField (char *filename)
{
	float *orientation;
	float *samples;
	FILE *savefile = fopen (filename, "w");
	
	if (!savefile || !Sensors) return (false);

	fprintf (savefile, "[Parametres]\n");
	fprintf (savefile, "<Learning>\n");
	
	fprintf (savefile, "%f\n", alpha);
	fprintf (savefile, "%f\n", h);
	fprintf (savefile, "%f\n", a);
	fprintf (savefile, "%f\n", b);
	fprintf (savefile, "%f\n", halpha);

	fprintf (savefile, "<SensorCount>\n");
	fprintf (savefile, "%d\n", Sensors -> GetSensorCount ());
	fprintf (savefile, "[Sensors]\n");

	SensorArrayElement *sae = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

	Sensors -> ResetCurrency ();
	int i = 0;

	while (sae = Sensors -> GetNextSensor ())
	{
		fprintf (savefile, "<Coordinate>\n");

		orientation = sae -> sensor.GetOrientation ();

		fprintf (savefile, "%f\n", orientation[0]);
		fprintf (savefile, "%f\n", orientation[1]);
		
		fprintf (savefile, "<Administration>\n");
		fprintf (savefile, "%f\n", sae -> sensor.GetLength ());
		fprintf (savefile, "%d\n", sae -> sensor.GetDivisions ());
		
		fprintf (savefile, "<Samples>\n");		
		samples = sae -> sensor.GetSamples ();

		for (int j = 0; j <= sae -> sensor.GetDivisions (); j++)
			fprintf (savefile, "%f\n", samples[j]);

		Progress (Sensors -> GetSensorCount (), i);
		i++;
	}

	fclose (savefile);
	SetWindowText (RenderInfo.WindowHandle, ID_ACTIVE_TITLE);
	free (sae);

	return (true);
}

bool PotentialField::Administer (float _alpha, float _h, float _a, float _b, float _halpha, float _MaxDist)
{
	alpha = _alpha;
	h = _h;
	a = _a;
	b = _b;
	halpha = _halpha;
	MaxDist = _MaxDist;

	return (true);
}

bool PotentialField::GetInfo (float *_alpha, float *_h, float *_a, float *_b, float *_halpha)
{
	*_alpha = alpha;
	*_h = h;
	*_a = a;
	*_b = b;
	*_halpha = halpha;

	return (true);
}

bool PotentialField::AddSensor (Sensor *newSensor)
{
	// APPEND THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	Sensors -> AddSensor (newSensor);

	return (true);
}

bool PotentialField::DeleteSensor (int *sensorno)
{
	Sensors -> Delete (sensorno);
	
	return (true);
}

float PotentialField::CalculateAt (float point[3])
{
	SensorArrayElement *sae = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));
	
/*	if (!Sensors) return (0.0f);
	
	Sensors -> ResetCurrency ();

	// Let's see if we unintently hit a ray
	while (sae = Sensors -> GetNextSensor ())
		if (sae -> sensor.Match (point)) return (sae -> sensor.CalculateAt (point[3]));

	// Check to see if there are four rays so that a normal approximation be done
	if (Sensors -> GetSensorCount () < 4)
	{
		// No rays hit, with less than four rays, we are not too lucky now
		// We should try and approximate from the nearest ray
		SensorArrayElement *nearest;
		float *orient, distance[2];
		int i = 0;

		distance[0] = distance[1] = 10000.0f;

		Sensors -> ResetCurrency ();
		
		while (sae = Sensors -> GetNextSensor ())
		{
			orient = sae -> sensor.GetOrientation ();

			if 
		
		}
	
	}

	// The tough case now
	else 
	{

	}

	free (sae);
	
	// APPEND THIS!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */
	return (0.0f);


}

bool PotentialField::Show ()
{
	return (true);
}