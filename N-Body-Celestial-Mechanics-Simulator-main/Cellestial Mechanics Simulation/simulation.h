#ifndef SIMULATION_H
#define SIMULATION_H

typedef struct quadTree quadTree;

typedef struct
{
	float llx;
	float lly;
	float len;

} quad;

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

struct quadTree
{
	stellarBody* body;
	quad quadrant;

	quadTree* NW;
	quadTree* NE;
	quadTree* SE;
	quadTree* SW;

};

int contains(quad in, float x, float y);
quad NW(quad in);
quad NE(quad in);
quad SE(quad in);
quad SW(quad in);

quadTree* newTree(quad q);
void insert(quadTree* tree, stellarBody* body);
void updateForce(quadTree* tree, stellarBody* body, double G, float theta, float lowerbound);

void instantiateBodies(stellarBody*, int);
void planetarySimulation(stellarBody*, int, float, float, float, float, int);
int in(stellarBody b, quad q);
void updateCofG(stellarBody* a, stellarBody* b);

void freeTree(quadTree* tree);

#endif