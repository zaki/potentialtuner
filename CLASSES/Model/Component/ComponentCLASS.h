// *****************************************************************************
// File: ComponentCLASS.h
// Author: Zoltan Dezso
// -----------------------
// This file contains the implementation for the class Component. A Component is
// basically a function, which has values between 0 and 1. Presently there are
// the following types of components available:
//     - GAUSS: standard normal distribution, with m median and sigma variation
//     - TRIANGLE: y=x function, translated and restricted to interval: [start, end]
//     - SQUARE: y=1 function, restricted to interval: [start, end] (aka: impulse)
//     - SINE: sine function, with f frequency and phi starting phase
//     - DIRAC: y=1 if x = position, please note, this is a discrete Dirac-impulse
//              and is not a continous Dirac-delta function
// The component uses parameters, which parametres are maximized at MAXNUMBEROFPARAMETRES
// Inside, all the Parametre slots are filled, however to the outside, only the
// relevant ones are passed
// *****************************************************************************

#include <math.h>
#include <stdio.h>

// Use this to control the maximum number of parametres that can be used for a
// component
#define MAXNUMBEROFPARAMETRES 3

// Define possible component IDs and their corresponding names
enum COMPONENTIDS            {GAUSS, TRIANGLE, SQUARE, SINE, DIRAC};
char *COMPONENTTYPES[]=      {"GAUSS", "TRIANGLE", "SQUARE", "SINE", "DIRAC"};

class FilterComponent
{
      private:
              COMPONENTIDS Type;
              double Parametres[MAXNUMBEROFPARAMETRES];

      public:
              // Constructors
              FilterComponent () {Type = SQUARE; Parametres[0] = 0.0; Parametres[1] = 0.0; Parametres[2] = 0.0;};
              ~FilterComponent () {;}

              //Auxiliary constructors:
              int CreateGAUSS (double);
              int CreateTRIANGLE (double);
              int CreateSQUARE (double);
              int CreateSINE (double, double);
              int CreateDIRAC ();

              // Modification functions
              int ChangeType (COMPONENTIDS);
              int ChangeParametres (double[MAXNUMBEROFPARAMETRES]);
              int ChangeParametre (int, double);
              int Change (COMPONENTIDS, double[MAXNUMBEROFPARAMETRES]);

			  // Value return
			  double GetParam () {return (Parametres[0]);}

              // Value calculation
              double CalculateValue (double);

              // IO functions
              int ShowMe () {printf ("Type: %d    Parametres: %lf, %lf, %lf\n", Type, Parametres[0], Parametres[1], Parametres[2]);}
              int WriteToFile (FILE *);
};

// ChangeType: changes the type of the component to NewID
// Please note, that there cannot be an incompatibility issue due to the difference
// in parametre number, because - as mentioned above - all the possible parametres
// are always filled in with zeros as needed
int FilterComponent::ChangeType (COMPONENTIDS NewID)
{
               Type = NewID;
               return (0);
}

// ChangeParametres: It is possible to change all the parametres at once, just
// pass an appropriate array of doubles into this function
int FilterComponent::ChangeParametres (double NewParametres[MAXNUMBEROFPARAMETRES])
{
               for (int i = 0; i < MAXNUMBEROFPARAMETRES; i++)
                          Parametres[i] = NewParametres[i];
               return (0);
}

// ChangeParametre: Changes a single parametre at position Number to NewParametre
// The function uses verification to make sure the index does exist
int FilterComponent::ChangeParametre (int Number, double NewParametre)
{
               if (Number < MAXNUMBEROFPARAMETRES && Number > -1)
                          Parametres[Number] = NewParametre;
               return (0);
}

// Change: To change all in one. Please use the appropriate CreateXXXXX functions
// instead. The Change fuction is for legacy support only.
int FilterComponent::Change (COMPONENTIDS NewType, double NewParametres[MAXNUMBEROFPARAMETRES])
{
               Type = NewType;
               for (int i = 0; i < MAXNUMBEROFPARAMETRES; i++)
                          Parametres[i] = NewParametres[i];
               return (0);
}

// CalculateValue: To obtain the value of the component at a given position
double FilterComponent::CalculateValue (double x)
{
	x = (x < 0.0) ? -1.0 * x : x;
	switch (Type)
	{
		case GAUSS:    return (exp (-1 * x * x / (2 * Parametres[0] * Parametres[0])));
		case TRIANGLE: return (( x <= Parametres[0]) ? -1 * x / Parametres[0] + 1.0 : 0.0);
		case SQUARE:   return ((x <= Parametres[0]) ? 1.0 : 0.0);
		case SINE:     return sin (x * Parametres[0] + Parametres[1]);
		case DIRAC:    return (x == Parametres[0]);
	}
	return (0.0);
}

// WriteToFile: Put the properties of the component into the file
// WARNING: NEVER USE THIS AS A STANDALONE!!!! This is only intended to be called
// by Function::SaveToFile ()
int FilterComponent::WriteToFile (FILE *outputfile)
{
    fprintf (outputfile, "%s ", COMPONENTTYPES[Type]);

    // Print only the relevant parametres
    switch (Type)
	{
        case GAUSS:  fprintf (outputfile, "(%lf,%lf)\n", Parametres[0], Parametres[1]);
                    break;

        case TRIANGLE:fprintf (outputfile, "(%lf,%lf)\n", Parametres[0], Parametres[1]);
                    break;

        case SQUARE:fprintf (outputfile, "(%lf,%lf)\n", Parametres[0], Parametres[1]);
                    break;

        case SINE:  fprintf (outputfile, "(%lf,%lf)\n", Parametres[0], Parametres[1]);
                    break;

		case DIRAC: fprintf (outputfile, "(%lf)\n", Parametres[0]);
					break;
	}
	return (1l);
}

// The following functions are special constructors, for creating the specific
// type of components easily. Each type of component is passed the appropriate
// parametres, please refer to the main section for more help
int FilterComponent::CreateGAUSS (double sigma)
{
               Type = GAUSS;
               Parametres[0] = sigma;
               for (int i = 1; i < MAXNUMBEROFPARAMETRES; i++) Parametres[i] = 0.0;

               return (0);
}

int FilterComponent::CreateTRIANGLE (double halfwidth)
{
               Type = TRIANGLE;
               Parametres[0] = halfwidth;
               for (int i = 1; i < MAXNUMBEROFPARAMETRES; i++) Parametres[i] = 0.0;

               return (0);
}

int FilterComponent::CreateSQUARE (double halfwidth)
{
               Type = SQUARE;
               Parametres[0] = halfwidth;
               for (int i = 1; i < MAXNUMBEROFPARAMETRES; i++) Parametres[i] = 0.0;

               return (0);
}

int FilterComponent::CreateSINE (double f, double phi)
{
               Type = SINE;
               Parametres[0] = f;
               Parametres[1] = phi;
               for (int i = 2; i < MAXNUMBEROFPARAMETRES; i++) Parametres[i] = 0.0;

               return (0);
}

int FilterComponent::CreateDIRAC ()
{
               Type = DIRAC;
               Parametres[0] = 0.0;
               for (int i = 1; i < MAXNUMBEROFPARAMETRES; i++) Parametres[i] = 0.0;

               return (0);
}

