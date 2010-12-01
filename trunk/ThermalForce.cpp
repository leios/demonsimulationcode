/*===- ThermalForce.cpp - libSimulation -=======================================
*
*                                  DEMON
* 
* This file is distributed under the BSD Open Source License. See LICENSE.TXT  
* for details. 
* 
*===-----------------------------------------------------------------------===*/

#include "ThermalForce.h"
#include <cmath>
#include <ctime>
#include "VectorCompatibility.h"

ThermalForce::ThermalForce(Cloud *myCloud, double redFactor) 
: Force(myCloud), mt(time(NULL)), heatVal(redFactor) {}

void ThermalForce::force1(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n/2; currentParticle < numParticles; currentParticle += 2) 
	{
		force(currentParticle);
	}
}

void ThermalForce::force2(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n/2; currentParticle < numParticles; currentParticle += 2) 
	{
		force(currentParticle);
	}
}

void ThermalForce::force3(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n/2; currentParticle < numParticles; currentParticle += 2) 
	{
		force(currentParticle);
	}
}

void ThermalForce::force4(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n/2; currentParticle < numParticles; currentParticle += 2) 
	{
		force(currentParticle);
	}
}

inline void ThermalForce::force(const unsigned int currentParticle)
{	
	// MT random number in (0,1)
	const __m128d thermV = _mm_set1_pd(heatVal) + _mm_set_pd(mt(), mt());
	const double thetaL = mt()*M_2_PI;
	const double thetaH = mt()*M_2_PI;
	
	double *pFx = &cloud->forceX[currentParticle];
	double *pFy = &cloud->forceY[currentParticle];
	
	_mm_store_pd(pFx, _mm_load_pd(pFx) + thermV*_mm_set_pd(sin(thetaH), sin(thetaL))); // _mm_set_pd() is backwards
	_mm_store_pd(pFy, _mm_load_pd(pFy) + thermV*_mm_set_pd(cos(thetaH), cos(thetaL)));
}

void ThermalForce::writeForce(fitsfile *file, int *error)
{
	//move to primary HDU:
	if(!*error)
		//file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	//add flag indicating that the thermal force is used:
	if(!*error) 
	{
		long forceFlags = 0;
		fits_read_key_lng(file, const_cast<char *> ("FORCES"), &forceFlags, NULL, error);

		//add ThermalForce bit:
		forceFlags |= ThermalForceFlag;		//compound bitwise OR

		if(*error == 202 || *error == 204)	//keyword does not exist yet
			*error = 0;			//clear above error.

		//add or update keyword:
		if(!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, const_cast<char *> ("Force configuration."), error);
	}

	if(!*error)
	{
		//file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("heatingValue"), heatVal, 6, const_cast<char *> ("[N] (ThermalForce)"), error);
	}
}

void ThermalForce::readForce(fitsfile *file, int *error)
{
	//move to primary HDU:
	if(!*error)
		//file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	if(!*error)
	{
		//file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("heatingValue"), &heatVal, NULL, error);
	}
}