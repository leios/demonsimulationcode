/*===- TimeVaryingThermalForce.cpp - libSimulation -============================
*
*                                  DEMON
* 
* This file is distributed under the BSD Open Source License. See LICENSE.TXT  
* for details. 
* 
*===-----------------------------------------------------------------------===*/

#include "TimeVaryingThermalForce.h"

TimeVaryingThermalForce::TimeVaryingThermalForce(Cloud *myCloud, double scale, double offset) 
: ThermalForce(myCloud, offset), heatValScale(scale), heatValOffset(offset) {}

void TimeVaryingThermalForce::force1(const double currentTime)
{
	heatVal = heatValScale*currentTime + heatValOffset;
	ThermalForce::force1(currentTime);
}

void TimeVaryingThermalForce::force2(const double currentTime)
{
	heatVal = heatValScale*currentTime + heatValOffset;
	ThermalForce::force2(currentTime);
}

void TimeVaryingThermalForce::force3(const double currentTime)
{
	heatVal = heatValScale*currentTime + heatValOffset;
	ThermalForce::force3(currentTime);
}

void TimeVaryingThermalForce::force4(const double currentTime)
{
	heatVal = heatValScale*currentTime + heatValOffset;
	ThermalForce::force4(currentTime);
}

void TimeVaryingThermalForce::writeForce(fitsfile *file, int *error)
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
		forceFlags |= TimeVaryingThermalForceFlag;		//compound bitwise OR

		if(*error == 202 || *error == 204)	//keyword does not exist yet
			*error = 0;			//clear above error.

		//add or update keyword:
		if(!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, const_cast<char *> ("Force configuration."), error);
	}

	if(!*error)
	{
		//file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("heatingValueScale"), heatValScale, 6, const_cast<char *> ("[N/s] (TimeVaryingThermalForce)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("heatingValueOffset"), heatValOffset, 6, const_cast<char *> ("[N] (TimeVaryingThermalForce)"), error);
	}
}

void TimeVaryingThermalForce::readForce(fitsfile *file, int *error)
{
	//move to primary HDU:
	if(!*error)
		//file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	if(!*error)
	{
		//file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("heatingValueScale"), &heatValScale, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("heatingValueOffset"), &heatValOffset, NULL, error);
	}
}