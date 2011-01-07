/*===- RotationalForce.cpp - libSimulation -====================================
*
*                                  DEMON
*
* This file is distributed under the BSD Open Source License. See LICENSE.TXT 
* for details. 
*
*===-----------------------------------------------------------------------===*/

#include "RotationalForce.h"
#include <cmath>

using namespace std;

RotationalForce::RotationalForce(Cloud * const myCloud, const double rmin, const double rmax, const double rotConst)
: Force(myCloud), innerRad(rmin), outerRad(rmax), rotationalConst(rotConst) {}

//1D:
void RotationalForce::force1_1D(const double currentTime)
{
	cout << "Error: RotationalForce not specified for 1D.\n";
	exit(1);
}

void RotationalForce::force2_1D(const double currentTime)
{
	force1_1D(currentTime);
}

void RotationalForce::force3_1D(const double currentTime)
{
	force1_1D(currentTime);
}

void RotationalForce::force4_1D(const double currentTime)
{
	force1_1D(currentTime);
}

//2D:
void RotationalForce::force1_2D(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n; currentParticle < numParticles; currentParticle += 2)
		force(currentParticle, cloud->getx1_pd(currentParticle), cloud->gety1_pd(currentParticle));
}

void RotationalForce::force2_2D(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n; currentParticle < numParticles; currentParticle += 2)
		force(currentParticle, cloud->getx2_pd(currentParticle), cloud->gety2_pd(currentParticle));
}

void RotationalForce::force3_2D(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n; currentParticle < numParticles; currentParticle += 2)
		force(currentParticle, cloud->getx3_pd(currentParticle), cloud->gety3_pd(currentParticle));
}

void RotationalForce::force4_2D(const double currentTime)
{
	for (unsigned int currentParticle = 0, numParticles = cloud->n; currentParticle < numParticles; currentParticle += 2)
		force(currentParticle, cloud->getx4_pd(currentParticle), cloud->gety4_pd(currentParticle));
}

//3D:
void RotationalForce::force1_3D(const double currentTime)
{
	force1_2D(currentTime); //rotation independent of z-dimension
}

void RotationalForce::force2_3D(const double currentTime)
{
	force2_2D(currentTime);
}

void RotationalForce::force3_3D(const double currentTime)
{
	force3_2D(currentTime);
}

void RotationalForce::force4_3D(const double currentTime)
{
	force4_2D(currentTime);
}

//RotationalForce given by 2D case. Since privite force function is not inherited
// from Force.h, no need to overload generic force function.
inline void RotationalForce::force(const unsigned int currentParticle, const __m128d currentPositionX, const __m128d currentPositionY)
{
	const __m128d dustRadV = _mm_sqrt_pd(currentPositionX*currentPositionX + currentPositionY*currentPositionY);

	// dustRad > innerRad && dustRadV < outerRad
	const __m128d compV = _mm_and_pd(_mm_cmpgt_pd(dustRadV, _mm_set1_pd(innerRad)), _mm_cmplt_pd(dustRadV, _mm_set1_pd(outerRad)));
	double compL, compH;
	_mm_storel_pd(&compL, compV);
	_mm_storeh_pd(&compH, compV);

	const bool nanL = isnan(compL);
	const bool nanH = isnan(compH);
	if (!nanL && !nanH) //neither in, early return
		return;

	__m128d cRotConst = _mm_set_pd(nanH ? rotationalConst : 0.0, // _mm_set_pd() is backwards.
		nanL ? rotationalConst : 0.0);

	double * const pFx = cloud->forceX + currentParticle;
	double * const pFy = cloud->forceY + currentParticle;

	//force in theta direction:
	// Fx = -c*x/r, Fy = c*y/r
	_mm_store_pd(pFx, _mm_load_pd(pFx) - cRotConst*currentPositionY/dustRadV);
	_mm_store_pd(pFy, _mm_load_pd(pFy) + cRotConst*currentPositionX/dustRadV);
}

void RotationalForce::writeForce(fitsfile * const file, int * const error, const int dimension) const
{
	//move to primary HDU:
	if(!*error)
		//file, # indicating primary HDU, HDU type, error
		fits_movabs_hdu(file, 1, IMAGE_HDU, error);

	//add flag indicating that the drag force is used:
	if(!*error) 
	{
		long forceFlags = 0;
		fits_read_key_lng(file, const_cast<char *> ("FORCES"), &forceFlags, NULL, error);

		//add RotationalForce bit:
		forceFlags |= RotationalForceFlag; //compound bitwise OR

		if(*error == KEY_NO_EXIST || *error == VALUE_UNDEFINED)
			*error = 0;                //clear above error.

		//add or update keyword:
		if(!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, const_cast<char *> ("Force configuration."), error);
	}

	if(!*error)
	{
		//file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("rotationalConst"), rotationalConst, 6, const_cast<char *> ("[N] (RotationalForce)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("innerRadius"), innerRad, 6, const_cast<char *> ("[m] (RotationalForce)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("outerRadius"), outerRad, 6, const_cast<char *> ("[m] (RotationalForce)"), error);
	}
}

void RotationalForce::readForce(fitsfile * const file, int * const error, const int dimension)
{
	//move to primary HDU:
	if(!*error)
		//file, # indicating primary HDU, HDU type, error
		fits_movabs_hdu(file, 1, IMAGE_HDU, error);

	if(!*error)
	{
		//file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("rotationalConst"), &rotationalConst, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("innerRadius"), &innerRad, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("outerRadius"), &outerRad, NULL, error);
	}
}