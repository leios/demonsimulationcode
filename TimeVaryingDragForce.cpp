/**
* @file  ThermalForce.cpp
* @class TimeVaryingDragForce TimeVaryingDragForce.h
*
* @brief Computes a drag force that changes with defined gamma function
*
* @license This file is distributed under the BSD Open Source License. 
*          See LICENSE.TXT for details. 
**/

#include "TimeVaryingDragForce.h"

void TimeVaryingDragForce::force1(const double currentTime) {
	dragConst = calculateGamma(currentTime);
	DragForce::force1(currentTime);
}

void TimeVaryingDragForce::force2(const double currentTime) {
	dragConst = calculateGamma(currentTime);
	DragForce::force2(currentTime);
}

void TimeVaryingDragForce::force3(const double currentTime) {
	dragConst = calculateGamma(currentTime);
	DragForce::force3(currentTime);
}

void TimeVaryingDragForce::force4(const double currentTime) {
	dragConst = calculateGamma(currentTime);
	DragForce::force4(currentTime);
}

/**
* @brief Calculates the gamma function that determines how the drag force changes with form g(t) = -m*t + b
*
* @param[in] currentTime Current time of the simulation
**/
inline const double TimeVaryingDragForce::calculateGamma(const double currentTime) const {
	return -(scaleConst*currentTime + offsetConst);
}

void TimeVaryingDragForce::writeForce(fitsfile * const file, int * const error) const {
	DragForce::writeForce(file, error);
	
	// add flag indicating that the time varying drag force is used:
	if (!*error) {
		long forceFlags = 0;
		fits_read_key_lng(file, const_cast<char *> ("FORCES"), &forceFlags, NULL, error);

		// add TimeVaryingDragForce bit:
		forceFlags |= DragForceFlag;
		forceFlags |= TimeVaryingDragForceFlag;

		// add or update keyword:
		if (!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, 
                            const_cast<char *> ("Force configureation."), error);
	}
		
	if (!*error) {
		// file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("TVDragScaleConst"), scaleConst, 
                           6, const_cast<char *> ("[Hz/s] (TimeVaryingDragForce)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("TVDragOffsetConst"), offsetConst, 
                           6, const_cast<char *> ("[Hz] (TimeVaryingDragForce)"), error);
	}
}

void TimeVaryingDragForce::readForce(fitsfile * const file, int * const error) {
	DragForce::readForce(file, error);
	
	if (!*error) {
		// file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("TVDragScaleConst"), &scaleConst, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("TVDragOffsetConst"), &offsetConst, NULL, error);
	}
}
