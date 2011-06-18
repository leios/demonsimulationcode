/*===- ThermalForceLocalized.cpp - libSimulation -==============================
*
*                                  DEMON
* 
* This file is distributed under the BSD Open Source License. See LICENSE.TXT  
* for details. 
* 
*===-----------------------------------------------------------------------===*/

#include "ThermalForceLocalized.h"
#include <ctime>
#include <cmath>

ThermalForceLocalized::ThermalForceLocalized(Cloud * const myCloud, const double thermRed1, const double thermRed2, const double specifiedRadius) 
: Force(myCloud), mt(time(NULL)), heatingRadius(specifiedRadius), heatVal1(thermRed1), heatVal2(thermRed2), 
evenRandCache(new RandCache[myCloud->n/2]), oddRandCache(new RandCache[myCloud->n/2]), 
evenRandGroup(dispatch_group_create()), oddRandGroup(dispatch_group_create()),
randQueue(dispatch_queue_create("com.DEMON.ThermalForceLocalized", NULL)) 
{
	dispatch_group_async(oddRandGroup, randQueue, ^{
		for (cloud_index i = 0, e = cloud->n/2; i < e; i++)
			oddRandCache[i] = RandCache(_mm_set_pd(mt(), mt()), mt(), mt());
	});
}

ThermalForceLocalized::~ThermalForceLocalized()
{
	delete[] evenRandCache;
	delete[] oddRandCache;
	dispatch_release(evenRandGroup);
	dispatch_release(oddRandGroup);
	dispatch_release(randQueue);
}

void ThermalForceLocalized::force1(const double currentTime)
{
	dispatch_group_async(evenRandGroup, randQueue, ^{
		for (cloud_index i = 0, e = cloud->n/2; i < e; i++)
			evenRandCache[i] = RandCache(_mm_set_pd(mt(), mt()), mt(), mt());
	});
	
	dispatch_group_wait(oddRandGroup, DISPATCH_TIME_FOREVER);
	dispatch_apply(cloud->n/2, queue, ^(cloud_index currentHalfParticle) {
		const cloud_index currentParticle = currentHalfParticle*2; 
		force(currentParticle, cloud->getx1_pd(currentParticle), cloud->gety1_pd(currentParticle), oddRandCache[currentHalfParticle]);
	});
}

void ThermalForceLocalized::force2(const double currentTime)
{
	dispatch_group_async(oddRandGroup, randQueue, ^{
		for (cloud_index i = 0, e = cloud->n/2; i < e; i++)
			oddRandCache[i] = RandCache(_mm_set_pd(mt(), mt()), mt(), mt());
	});
	
	dispatch_group_wait(evenRandGroup, DISPATCH_TIME_FOREVER);
	dispatch_apply(cloud->n/2, queue, ^(cloud_index currentHalfParticle) {
		const cloud_index currentParticle = currentHalfParticle*2;  
		force(currentParticle, cloud->getx2_pd(currentParticle), cloud->gety2_pd(currentParticle), evenRandCache[currentHalfParticle]);
	});
}

void ThermalForceLocalized::force3(const double currentTime)
{
	dispatch_group_async(evenRandGroup, randQueue, ^{
		for (cloud_index i = 0, e = cloud->n/2; i < e; i++)
			evenRandCache[i] = RandCache(_mm_set_pd(mt(), mt()), mt(), mt());
	});
	
	dispatch_group_wait(oddRandGroup, DISPATCH_TIME_FOREVER);
	dispatch_apply(cloud->n/2, queue, ^(cloud_index currentHalfParticle) {
		const cloud_index currentParticle = currentHalfParticle*2;  
		force(currentParticle, cloud->getx3_pd(currentParticle), cloud->gety3_pd(currentParticle), oddRandCache[currentHalfParticle]);
	});
}

void ThermalForceLocalized::force4(const double currentTime)
{
	dispatch_group_async(oddRandGroup, randQueue, ^{
		for (cloud_index i = 0, e = cloud->n/2; i < e; i++)
			oddRandCache[i] = RandCache(_mm_set_pd(mt(), mt()), mt(), mt());
	});
	
	dispatch_group_wait(evenRandGroup, DISPATCH_TIME_FOREVER);
	dispatch_apply(cloud->n/2, queue, ^(cloud_index currentHalfParticle) {
		const cloud_index currentParticle = currentHalfParticle*2;  
		force(currentParticle, cloud->getx4_pd(currentParticle), cloud->gety4_pd(currentParticle), evenRandCache[currentHalfParticle]);
	});
}

inline void ThermalForceLocalized::force(const cloud_index currentParticle, const __m128d displacementX, const __m128d displacementY, const RandCache &rc)
{
	const __m128d radiusV = _mm_sqrt_pd(displacementX*displacementX + displacementY*displacementY);
	const double thetaL = rc.l*2.0*M_PI;
	const double thetaH = rc.h*2.0*M_PI;
	
	double rL, rH;
	_mm_storel_pd(&rL, radiusV);
	_mm_storeh_pd(&rH, radiusV);
	
	const __m128d thermV = _mm_set_pd((rH < heatingRadius) ? heatVal1 : heatVal2, // _mm_set_pd() is backwards
									  (rL < heatingRadius) ? heatVal1 : heatVal2)*rc.r;
	
	double * const pFx = cloud->forceX + currentParticle;
	double * const pFy = cloud->forceY + currentParticle;
	
	_mm_store_pd(pFx, _mm_load_pd(pFx) + thermV*_mm_set_pd(sin(thetaH), sin(thetaL))); // _mm_set_pd() is backwards
	_mm_store_pd(pFy, _mm_load_pd(pFy) + thermV*_mm_set_pd(cos(thetaH), cos(thetaL)));
}

void ThermalForceLocalized::writeForce(fitsfile * const file, int * const error) const
{
	// move to primary HDU:
	if (!*error)
		// file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	// add flag indicating that the localized thermal force is used:
	if (!*error) 
	{
		long forceFlags = 0;
		fits_read_key_lng(file, const_cast<char *> ("FORCES"), &forceFlags, NULL, error);

		// add ThermalForce bit:
		forceFlags |= ThermalForceLocalizedFlag; // compound bitwise OR

		if (*error == KEY_NO_EXIST || *error == VALUE_UNDEFINED)
			*error = 0; // clear above error.

		// add or update keyword:
		if (!*error) 
			fits_update_key(file, TLONG, const_cast<char *> ("FORCES"), &forceFlags, const_cast<char *> ("Force configuration."), error);
	}

	if (!*error)
	{
		// file, key name, value, precision (scientific format), comment
		fits_write_key_dbl(file, const_cast<char *> ("heatingValue1"), heatVal1, 6, const_cast<char *> ("[N] (ThermalForceLocalized)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("heatingValue2"), heatVal2, 6, const_cast<char *> ("[N] (ThermalForceLocalized)"), error);
		fits_write_key_dbl(file, const_cast<char *> ("heatingRadius"), heatingRadius, 6, const_cast<char *> ("[m] (ThermalForceLocalized)"), error);
	}
}

void ThermalForceLocalized::readForce(fitsfile * const file, int * const error)
{
	// move to primary HDU:
	if (!*error)
		// file, # indicating primary HDU, HDU type, error
 		fits_movabs_hdu(file, 1, IMAGE_HDU, error);
	
	if (!*error)
	{
		// file, key name, value, don't read comment, error
		fits_read_key_dbl(file, const_cast<char *> ("heatingValue1"), &heatVal1, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("heatingValue2"), &heatVal2, NULL, error);
		fits_read_key_dbl(file, const_cast<char *> ("heatingRadius"), &heatingRadius, NULL, error);
	}
}