// *****************************************************************************
// File: FunctionCLASS.h
// Author: Zoltan Dezso
// -----------------------
// This file contains the implementation for the class Function. A function consists
// of components. A maximum number of MAXNUMBEROFCOMPONENTS components can be
// registered in a function. The function acts as a container for components, and
// implements the following formula:
// coefficient1 * component1 + coefficient2 * component2 + ... +
//              + coefficientMAXNUMBEROFCOMPONENTS * componentMAXNUMBEROFCOMPONENTS
// *****************************************************************************

#include <math.h>
#include <stdio.h>

#define MAXNUMBEROFCOMPONENTS 3

class Function
{
private:
	Component Components[MAXNUMBEROFCOMPONENTS];
	double Scalars[MAXNUMBEROFCOMPONENTS];
	int FirstEmptyPlace;

public:
	// Constructors
	Function () {FirstEmptyPlace = 0; for (int i = 0; i < MAXNUMBEROFCOMPONENTS; i++) Scalars[i] = 0.0;}
	~Function ();

	// Modification functions
	int Reset () {FirstEmptyPlace = 0;}
	int ChangeComponent  (int, Component);
	int ChangeComponents (Component[MAXNUMBEROFCOMPONENTS]);
	int ChangeScalar  (int, double);
	int ChangeScalars (double[MAXNUMBEROFPARAMETRES]);

	// List-like modification functions
	int AddComponent (Component, double);
	int RemoveComponent (int);

	// Calculation functions
	double CalculateValue (double);

	// IO Functions
	int ShowMe () {for (int i = 0; i < MAXNUMBEROFCOMPONENTS; i++) {printf ("Component: %d  Scalar: %lf\n", i, Scalars[i]);Components[i].ShowMe ();}}
	int SaveToFile (FILE *);
};

// ChangeComponent: change the component at index 'order' to NewComponent
// One must specify an index, at which there IS a previously added component
int Function::ChangeComponent (int order, Component NewComponent)
{
	if (order >= 0 && order < FirstEmptyPlace) Components[order] = NewComponent;
	return (1);
}

// ChangeComponents: Change all the available components at once.
// USE OF THIS FUNCTION IS NOT RECOMMENDED (see Component::ChangeParametres)
int Function::ChangeComponents (Component NewComponents[MAXNUMBEROFPARAMETRES])
{
	for (int i = 0; i < MAXNUMBEROFCOMPONENTS; i++)
		Components[i] = NewComponents[i];
	return (1);
}

// ChangeScalar: Change the coefficient of component at index 'order' to NewScalar
int Function::ChangeScalar (int order, double NewScalar)
{
	if (order >= 0 && order < FirstEmptyPlace)
		Scalars[order] = NewScalar;
	return (1);
}

// ChangeScalars: Change all the scalars at once. USE NOT RECOMMENDED!
int Function::ChangeScalars (double NewScalars[MAXNUMBEROFCOMPONENTS])
{
	for (int i = 0; i < MAXNUMBEROFCOMPONENTS; i++)
		Scalars[i] = NewScalars[i];
	return (1);
}

// AddComponent: Register a new component, if it is possible - ie. there is
// empty space for the new component. Failing to do so means a -2 as return value
int Function::AddComponent (Component NewComponent, double NewScalar)
{
	if (FirstEmptyPlace < (MAXNUMBEROFCOMPONENTS))
	{
		Components[FirstEmptyPlace] = NewComponent;
		Scalars[FirstEmptyPlace] = NewScalar;
		FirstEmptyPlace++;
		return (FirstEmptyPlace);
	}
	else return (-2);
}

// RemoveComponent: Removes the component at index 'order', and shifts all the
// components above it one slot down. The index must refer to an existing component
// else the function returns -2
int Function::RemoveComponent (int order)
{
	if (order < FirstEmptyPlace)
	{
		// Shift
		for (int i = order; i < FirstEmptyPlace - 1; i++)
		{
			Components[i] = Components[i+1];
			Scalars[i] = Scalars[i+1];
		}
		FirstEmptyPlace--;
		return (FirstEmptyPlace);
	}
	return (-2);
}

// CalculateValue: calculate the implemented function at x
double Function::CalculateValue (double x)
{
	double results = 0;

	for (int i = 0; i < FirstEmptyPlace; i++)
		results += Scalars[i]*(Components[i].CalculateValue (x));
	return (results);
}

// SaveToFile: Write the information of the function into the file outputfile
// as text data. The file of course must be opened and valid. For a reference on
// the standard format of the generated file, please read 'FormatFUN.txt' which
// should be included in the package You got this file with
int Function::SaveToFile (FILE *outputfile)
{
	// Print header first
	fprintf (outputfile, "#define %d\n", FirstEmptyPlace);
	for (int i = 0; i < FirstEmptyPlace; i++)
	{
		// And also print component informations
		fprintf (outputfile, "#fun%d ", i+1);
		Components[i].WriteToFile (outputfile);
	}

	// Enter build section
	fprintf (outputfile, "#build\n");
	for (int j = 0; j < FirstEmptyPlace; j++)
	{
		// Print out implemented function
		fprintf (outputfile, "%lf*#fun%d", Scalars[j], j+1);
		if (j < FirstEmptyPlace - 1) fprintf (outputfile, "+");
	}
	fprintf (outputfile, "\n");

	return (1);
}
