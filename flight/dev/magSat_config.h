/* file: magSat_config.h
 * author: Jacob Cook
 * created on: 2/20/2014
 *
 * This file contains configuration information shared between the 
 * linear and non linear dynamic function files. It is mostly for
 * defining system constants, such as the Inertia tensor and orbital
 * parameters.
 */

#ifndef __MAGSAT_CONFIG_H
#define __MAGSAT_CONFIG_H

#include <math.h>

/* Orbit constants */
#define MU_EARTH 	(3.986004415e5) /* Gravitational Constant [km^3/s^2] */
#define R_EARTH		(6.378136e3)	/* Earth's mean radius */
#define R_ORBIT		(6.728136e3)	/* radius of orbit [m] (350 km) */
#define R_ORBIT3	(R_ORBIT*R_ORBIT*R_ORBIT) /* r_orb^3 */

/* Julian date of the start time of the orbit 
 * May 1, 2015 00:00:00 
 * Use matlab function JulianDate.m for J0 calculation
 *
 * NOTE: propagate JD as JD = J0 + ((sec/60+min)/60 + h)/2
 */
/*#define J0 (2.4571435e6) */
#define J0 (2.4551975e6)
#define JD2010 (2.4551975e6) 		/* Julian Date on 1 Jan 2010 00:00:00 */
#define JD2010_J0 (J0 - JD2010) /* Days past JD2010 */

/* IGRF magnetic field model order */
#define USE_IGRF
#define IGRF_ORDER 8
#define IGRF_DEGREE IGRF_ORDER

/* Orbit inclination (pi/2) */
#define INC				(M_PI_2)

/* Orbital Period and time scalar 
 * 
 * 	T_ORB = 2*pi*sqrt(r_orbit^3/mu_earth)
 * 				= 5492.3 seconds
 *
 *		r_orbit  = 6728.1363 [km] (350 km)
 *		mu_earht = 398600.4415 [km^3/s^2]
 * 
 *	Time saclar alpha:
 *		ALPHA = T_ORB/(2*pi)
 */
#define ALPHA (874.1245) 
#define ALPHA2 (ALPHA*ALPHA)
#define OM	(1.0) /* Magnitude of orbit angular rate */

/*
 * Jx = 0.0033
 * Jy & Jz = 0.0101
 * OM = ALPHA*2*pi/T = 1
 */
#define JX		(0.0033)
#define JY		(0.0101)
#define JZ		(JY)
#define IJX		(1/JX)
#define IJY		(1/JY)
#define IJZ		(1/JZ)

/* Max dipole */
#define M0		(0.08)

/* scaling parameters */
#define USE_NORMALIZED_B (0)
#define BETA 	(1e-4)
#define GAMMA (1e-9)

#endif
