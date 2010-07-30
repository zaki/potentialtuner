bool SensorArray::AddSensor (Sensor *newSensor)
{
	SensorArrayElement *current;
	float *orientation;
	float *neworientation;
	bool found = false;
	
	current = Sensors;
	neworientation = newSensor -> GetOrientation ();

	if (newSensor)
	{
		SensorArrayElement *sae;
		ResetCurrency ();
		while (sae = GetNextSensor ())
		{
			orientation = sae -> sensor.GetOrientation ();
			if ((orientation[0] == neworientation[0] && orientation[1] == neworientation[1]) || 
				(orientation[1] ==  90.0f && neworientation[1] ==  90.0f) ||
				(orientation[1] == -90.0f && neworientation[1] == -90.0f)) return (false);
		}

		while (!found)
		{
			// If the array is empty
			if (!Sensors)
			{
				SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

				newSAE -> sensor = (*newSensor);
				newSAE -> next = NULL;

				Sensors = newSAE;

				sensorcount++;

				return (true);
			}
		
			// If we should insert before the first
			else if (current == Sensors)
			{
				orientation = (current -> sensor).GetOrientation ();

				if ((orientation[1] > neworientation[1]) || (orientation[1] == neworientation[1] && orientation[0] > neworientation[0]))
				{
					SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

					newSAE -> sensor = (*newSensor);
					newSAE -> next = Sensors;

					Sensors = newSAE;

					sensorcount++;

					return (true);
				}

				else if ((orientation[1] == neworientation[1] && orientation[0] == neworientation[0])) 
				{
					return (false);
				}
				
				else 
				{
					// There is only one element in the array
					if (!current -> next)
					{
						SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

						newSAE -> sensor = (*newSensor);
						newSAE -> next = NULL;

						Sensors -> next = newSAE;

						sensorcount++;

						return (true);
					}

					else
					{
						orientation = (current -> next -> sensor).GetOrientation ();

						if ((orientation[1] > neworientation[1]) || (orientation[1] == neworientation[1] && orientation[0] > neworientation[0]))
						{
							SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

							newSAE -> sensor = (*newSensor);
							newSAE -> next = current -> next;

							current -> next = newSAE;

							sensorcount++;

							return (true);
						}

						else if ((orientation[1] == neworientation[1] && orientation[0] == neworientation[0])) 
						{
							return (false);
						}

						else current = current -> next;
					}
				}
			}

			// We should insert to the end, ie no more elements in the array
			else if (!(current -> next))
			{
				SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));

				newSAE -> sensor = (*newSensor);
				newSAE -> next = NULL;

				current -> next = newSAE;

				sensorcount++;
	
				return (true);
			}

			// Just an ordinary insertion somewhere in the middle
			else
			{
				orientation = current -> next -> sensor.GetOrientation ();
			
				if ((orientation[1] > neworientation[1]) || (orientation[1] == neworientation[1] && orientation[0] > neworientation[0]))
				{
					SensorArrayElement *newSAE = (SensorArrayElement *) malloc (sizeof (SensorArrayElement));
	
					newSAE -> sensor = (*newSensor);
					newSAE -> next = current -> next;
	
					current -> next = newSAE;	

					sensorcount++;

					return (true);
				}

				else if ((orientation[1] == neworientation[1] && orientation[0] == neworientation[0]))
				{
					return (false);
				}	

				else current = current -> next;
			}
		}
	}
	return (true);
}

bool SensorArray::Delete (int *sensorno)
{
	SensorArrayElement *sae_1, *sae0, *saep1;
	if (*sensorno < 1 || *sensorno > GetSensorCount ()) return (false);
	ResetCurrency ();

	if (*sensorno == 1)
	{
		if (!(sae0 = GetNextSensor ())) return (false);
		
		saep1 = GetNextSensor ();
		
		Sensors = saep1;

		delete (sae0);

		sensorcount--;

		return (true);
	}

	for (int i = 0; i < *sensorno - 1; i++) sae_1 = GetNextSensor ();

	if (!sae_1) return (false);

	sae0 = GetNextSensor ();

	if (!sae0) return (false);

	saep1 = GetNextSensor ();

	sae_1 -> next = saep1;

	if (!saep1) *sensorno = 1;

	delete (sae0);

	sensorno--;

	return (true);
}