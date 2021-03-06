/**
* @file  RectConfinementForce.cpp
* @class RectConfinementForce RectConfinementForce.h
*
* @brief Computes a rectangular confinement force to keep particles in
*
* @license This file is distributed under the BSD Open Source License. 
*          See LICENSE.TXT for details. 
**/

#include "RectConfinementForce.h"

void RectConfinementForce::force1(const double currentTime) {
    (void)currentTime;
	BEGIN_PARALLEL_FOR(currentParticle, numParticles, cloud->n, DOUBLE_STRIDE, static)
		force(currentParticle, cloud->getx1_pd(currentParticle), cloud->gety1_pd(currentParticle));
    END_PARALLEL_FOR
}

void RectConfinementForce::force2(const double currentTime) {
    (void)currentTime;
	BEGIN_PARALLEL_FOR(currentParticle, numParticles, cloud->n, DOUBLE_STRIDE, static) 
		force(currentParticle, cloud->getx2_pd(currentParticle), cloud->gety2_pd(currentParticle));
    END_PARALLEL_FOR
}

void RectConfinementForce::force3(const double currentTime) {
    (void)currentTime;
	BEGIN_PARALLEL_FOR(currentParticle, numParticles, cloud->n, DOUBLE_STRIDE, static)
		force(currentParticle, cloud->getx3_pd(currentParticle), cloud->gety3_pd(currentParticle));
    END_PARALLEL_FOR
}

void RectConfinementForce::force4(const double currentTime) {
    (void)currentTime;
	BEGIN_PARALLEL_FOR(currentParticle, numParticles, cloud->n, DOUBLE_STRIDE, static) 
		force(currentParticle, cloud->getx4_pd(currentParticle), cloud->gety4_pd(currentParticle));
    END_PARALLEL_FOR
}


/**
* @brief Computes a confinement force in the x and y directions
*		 with forms F_x = c_x*q*x, F_y = c_y*q*y
*
* @param[in] currentParticle  Particle whose force is being computed
* @param[in] currentPositionX The x-position of the current particle
* @param[in] currentPositionY The y-position of the current particle
**/
inline void RectConfinementForce::force(const cloud_index currentParticle, 
                                        const doubleV currentPositionX, 
                                        const doubleV currentPositionY) {
	const doubleV charge = load_pd(cloud->charge + currentParticle); 
	
	plusEqual_pd(cloud->forceX + currentParticle, 
                 mul_pd(mul_pd(charge, confineX), currentPositionX));
	plusEqual_pd(cloud->forceY + currentParticle, 
                 mul_pd(mul_pd(charge, confineY), currentPositionY));
}

void RectConfinementForce::writeForce(fitsfile * const file, int * const error) const {
	// move to primary HDU:
	if (!*error)
		// file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	// add flag indicating that the rectangular confinement force is used:
	if (!*error) {
		long forceFlags = 0;
		fits_read_key_lng(file, const_cast<char *> ("FORCES"), &forceFlags, NULL, error);

		// add RectConfinementForce bit:
		forceFlags |= RectConfinementForceFlag;

		if (*error == KEY_NO_EXIST || *error == VALUE_UNDEFINED)
			*error = 0; // clear above error.

		// add or update keyword:
		if (!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, 
                            const_cast<char *> ("Force configuration."), error);
	}

	if (!*error) {
		// file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("confineConstX"), confineX, 
                           6, const_cast<char *> ("[V/m^2] (RectConfinementForce)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("confineConstY"), confineY, 
                           6, const_cast<char *> ("[V/m^2] (RectConfinementForce)"), error);
	}
}

void RectConfinementForce::readForce(fitsfile * const file, int * const error) {
	// move to primary HDU:
	if (!*error)
		// file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);

	if (!*error) {
		// file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("confineConstX"), &confineX, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("confineConstY"), &confineY, NULL, error);
	}
}
