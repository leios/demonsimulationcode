/*===- Cloud.h - libSimulation -================================================
*
*                                  DEMON
*
* This file is distributed under the BSD Open Source License. See LICENSE.TXT 
* for details.
*
*===-----------------------------------------------------------------------===*/

#ifndef CLOUD_H
#define CLOUD_H

#include "fitsio.h"

class Cloud
{	
public:
	Cloud(unsigned int numPar, double sizeOfCloud);	//overloaded constructor
	~Cloud();

//public variables:
	unsigned int n;			//number of elements (particles)
	double cloudSize;
	double *k1, *k2, *k3, *k4;	//velocityX (Runge-Kutta) tidbits
	double *l1, *l2, *l3, *l4;	//positionsX (Runge-Kutta) tidbits
	double *m1, *m2, *m3, *m4;	//velocityY (Runge-Kutta) tidbits
	double *n1, *n2, *n3, *n4;	//positionsY (Runge-Kutta) tidbits
	double *x, *y, *Vx, *Vy;	//current positions and velocities=
	double *charge, *mass;
	double *forceX, *forceY;

//public functions:
	//Input: int index
	//Preconditions: 0 <= index < number of particles
	//Postconditions: x,y positions of particle #index randomly set
	void setPosition(const unsigned int index);
	
	//Input: int index, initialPosX, intialPosY
	//Preconditions: 0 <= index < number of particles
	//Postconditions: x,y positions of particle #index set to initialPosX,initialPosY
	void setPosition(const unsigned int index, const double initialPosX, const double initialPosY);

	//Input: int index
	//Preconditions: 0 <= index < number of particles
	//Postconditions: velocity vector of particle #index randomly set
	void setVelocity(const unsigned int index);

	//Input: int index
	//Preconditions: 0 <= index < number of particles
	//Postconditions: charge of particle #index randomly set, range 5900 to 6100 *1.6E-19
	void setCharge(const unsigned int index);

	//Input: int index
	//Preconditions: 0 <= index < number of particles
	//Postconditions: mass of particle #index set according to radius, density
	void setMass(const unsigned int index);

	//Input: fitsfile *file, int *error
	//Preconditions: fitsfile exists, error = 0
	//Postconditions: initial cloud data, including mass & charge, output to file
	void writeCloudSetup(fitsfile *file, int *error);
	
	//Input: fitsfile *file, int *error, double currentTime
	//Preconditions: fitsfile exists, error = 0, currentTime > 0, writeCloudSetup has previously been called
	//Postconditions: positions and velocities for current time step output to file
	void writeTimeStep(fitsfile *file, int *error, double currentTime);

//static functions:
	//Input: int numParticles, double cloudSize
	//Preconditions: both inputs positive
	//Postconditions: randomly initialized cloud with radius = cloudSize
	static Cloud *initializeNew(const unsigned int numParticles, const double cloudSize);

	//Input: int numParticles, double cloudSize
	//Preconditions: both inputs positive
	//Postconditions: cloud initialized on spatial grid with side length = 2*cloudSize
	static Cloud *initializeGrid(const unsigned int numParticles, const double cloudSize);

	//Input: fitsFile *file, int *error
	//Preconditions: fitsfile exists, error = 0
	//Postconditions: cloud initialized with last time step of fitsfile,
	//	forces and other simulation information extracted as well
	static Cloud *initializeFromFile(fitsfile *file, int *error, double *currentTime);
};

#endif /* CLOUD_H */