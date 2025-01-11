#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct
{
	float posx;
	float posy;

	double velx;
	double vely;

	double accx;
	double accy;

	double mass;
} stellarBody;

void instantiateBodies(stellarBody*, int);

void planetarySimulationBruteForce(stellarBody*, int, float, float, float);

void planetarySimulationBarnesHut(stellarBody*, int, float, float, float, float);

#endif
