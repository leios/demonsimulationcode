/**
* @file  TimeVaryingThermalForce.h
* @brief Defines the data and methods of the TimeVaryingThermalForce class
*
* @license This file is distributed under the BSD Open Source License. 
*          See LICENSE.TXT for details. 
**/

#ifndef TIMEVARYINGTHERMALFORCE_H
#define TIMEVARYINGTHERMALFORCE_H

#include "ThermalForce.h"

class TimeVaryingThermalForce : public ThermalForce {
public:
	TimeVaryingThermalForce(Cloud * const C, const double scale, const double offset)
	: ThermalForce(C, offset), heatValScale(scale), heatValOffset(offset) {}
	~TimeVaryingThermalForce() {}

	void force1(const double currentTime); // rk substep 1
	void force2(const double currentTime); // rk substep 2
	void force3(const double currentTime); // rk substep 3
	void force4(const double currentTime); // rk substep 4
    
    void writeForce(fitsfile * const file, int * const error) const;
    void readForce(fitsfile * const file, int * const error);

private:
    double heatValScale;  //<! Scale of function that changes heatVal [N/s]
    double heatValOffset; //<! Offset of function that changes heatVal [N]

    const double calculateHeatVal(const double currentTime) const;
};

#endif // TIMEVARYINGTHERMALFORCE_H
