#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"
#include <math.h>
#include "barnesHut.h"

void instantiateBodies(stellarBody* stellarBodies, int numOfBodies)
{
	srand(time(NULL));

	for (int i = 0; i < numOfBodies; i++)
	{
		stellarBodies[i].posx = ((rand() % 1000) * (rand() % 1000)) / 1000000.0f;
		if ((rand() % 2 + 1) % 2 == 0) stellarBodies[i].posx *= -1.0f;

		stellarBodies[i].posy = ((rand() % 1000) * (rand() % 1000)) / 1000000.0f;
		if ((rand() % 2 + 1) % 2 == 0) stellarBodies[i].posy *= -1.0f;

		stellarBodies[i].mass = 1;
	}
}

void planetarySimulationBruteForce(stellarBody* stellarBodies, int n, float G, float steps, float lowerBound)
{
	double distancex;
	double forcex;
	double distancey;
	double forcey;

	double force;
	double angle;

	for (int i = 0; i < n; i++)
	{
		for (int j = 0; j < i; j++)
		{
			distancex = (stellarBodies[i].posx * 1000.0f) - (stellarBodies[j].posx * 1000.0f);
			distancey = (stellarBodies[i].posy * 1000.0f) - (stellarBodies[j].posy * 1000.0f);

			if (((distancex * distancex) + (distancey * distancey)) > lowerBound)
			{
				force = G / ((distancex * distancex) + (distancey * distancey));

				angle = atan2(distancey, distancex);

				forcex = cos(angle) * force;
				forcey = sin(angle) * force;

				stellarBodies[i].accx -= forcex;
				stellarBodies[j].accx += forcex;

				stellarBodies[i].accy -= forcey;
				stellarBodies[j].accy += forcey;
			}
			else
			{
				force = G / (lowerBound * lowerBound);

				angle = atan2(distancey, distancex);

				forcex = cos(angle) * force;
				forcey = sin(angle) * force;

				stellarBodies[i].accx -= forcex;
				stellarBodies[j].accx += forcex;

				stellarBodies[i].accy -= forcey;
				stellarBodies[j].accy += forcey;
			}
		}
	}

	printf(".\n");

	for (int i = 0; i < n; i++)
	{
		stellarBodies[i].velx += (stellarBodies[i].accx/1000.0f) * (1.0f / steps);
		stellarBodies[i].vely += (stellarBodies[i].accy/1000.0f) * (1.0f / steps);

		stellarBodies[i].posx += stellarBodies[i].velx * (1.0f / steps);
		stellarBodies[i].posy += stellarBodies[i].vely * (1.0f / steps);

		printf("forcex: %.20lf\n", stellarBodies[i].accx);
		printf("forcey: %.20lf\n", stellarBodies[i].accy);
		printf("vx: %.20lf\n", stellarBodies[i].velx);
		printf("vy: %.20lf\n", stellarBodies[i].vely);
		printf(".\n");

		stellarBodies[i].accx = 0.0f;
		stellarBodies[i].accy = 0.0f;
	}
}

void planetarySimulationBarnesHut(stellarBody* stellarBodies, int n, float G, float theta, float steps, float lowerBound)
{

	treenode* root = populateQuadtree(stellarBodies, n);

	for (int i = 0; i < n; i++)
	{

		force* frc = traverseTreeC(&stellarBodies[i], root, theta);

		stellarBodies[i].accx = frc->x;
		stellarBodies[i].accy = frc->y;

		free(frc);

	}

	printf(".\n");

	for (int i = 0; i < n; i++)
	{
		stellarBodies[i].velx += (stellarBodies[i].accx / 1000.0f) * (1.0f / steps);
		stellarBodies[i].vely += (stellarBodies[i].accy / 1000.0f) * (1.0f / steps);

		stellarBodies[i].posx += stellarBodies[i].velx * (1.0f / steps);
		stellarBodies[i].posy += stellarBodies[i].vely * (1.0f / steps);

		printf("forcex: %.20lf\n", stellarBodies[i].accx);
		printf("forcey: %.20lf\n", stellarBodies[i].accy);
		printf("vx: %.20lf\n", stellarBodies[i].velx);
		printf("vy: %.20lf\n", stellarBodies[i].vely);
		printf(".\n");

		stellarBodies[i].accx = 0.0f;
		stellarBodies[i].accy = 0.0f;
	}
}