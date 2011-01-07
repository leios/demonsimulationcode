/*===- Runge_Kutta.h - libSimulation -==========================================
*
*                                  DEMON
*
* This file is distributed under the BSD Open Source License. See LICENSE.TXT 
* for details.
*
*===-----------------------------------------------------------------------===*/

#ifndef RUNGA_KUTTA_H
#define RUNGA_KUTTA_H

#include "Force.h"

class Runge_Kutta
{
public:
	Runge_Kutta(Cloud * const myCloud, Force ** const forces, const double timeStep, const unsigned int forcesSize, double startTime); //overloaded constructor

//public variables:
	Cloud * const cloud;          //pointer to cloud object
	Force ** const theForce;      //pointer to Force object
	const unsigned int numForces;
	const double init_dt;         //store initial time step
	const double red_dt;          //store reduced time step
	double currentTime;

//public functions:
	//Input: double endTime
	//Preconditions: endTime > 0
	//Postconditions: Runge-Kutta algorithm updates position, velocity, time
	void moveParticles1D(const double endTime);
	void moveParticles2D(const double endTime);
	void moveParticles3D(const double endTime);

private:
//private functions:
	void force1_1D(const double currentTime) const; //rk substep 1
	void force2_1D(const double currentTime) const; //rk substep 2
	void force3_1D(const double currentTime) const; //rk substep 3
	void force4_1D(const double currentTime) const; //rk substep 4

	void force1_2D(const double currentTime) const; 
	void force2_2D(const double currentTime) const; 
	void force3_2D(const double currentTime) const; 
	void force4_2D(const double currentTime) const; 

	void force1_3D(const double currentTime) const; 
	void force2_3D(const double currentTime) const; 
	void force3_3D(const double currentTime) const; 
	void force4_3D(const double currentTime) const; 

	const double modifyTimeStep1D() const;
	const double modifyTimeStep2D() const;
	const double modifyTimeStep3D() const;
};

#endif /* RUNGE_KUTTA_H */
