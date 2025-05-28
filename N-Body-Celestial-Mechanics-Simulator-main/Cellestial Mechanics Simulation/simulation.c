#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "simulation.h"
#include <math.h>
#include "frames.h"


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

void planetarySimulation(stellarBody* stellarBodies, int n, float G, float dt, float lowerBound, float theta, int iter)
{

	quad* rootQuad = (quad*)malloc(sizeof(quad));

	rootQuad->len = 1000;
	rootQuad->llx = 0;
	rootQuad->lly = 1000;

	quadTree* root = newTree(*rootQuad);

	for (int i = 0; i < n; i++)
	{
		insert(root, (stellarBodies + i));
	}

	for (int i = 0; i < n; i++)
	{
		updateForce(root, (stellarBodies + i), G, theta, lowerBound);
	}

	for (int i = 0; i < n; i++)
	{
		stellarBodies[i].velx += (stellarBodies[i].accx/1000.0f) * dt;
		stellarBodies[i].vely += (stellarBodies[i].accy/1000.0f) * dt;

		stellarBodies[i].posx += stellarBodies[i].velx * dt;
		stellarBodies[i].posy += stellarBodies[i].vely * dt;

		stellarBodies[i].accx = 0.0f;
		stellarBodies[i].accy = 0.0f;
	}

	freeTree(root);
	free(rootQuad);

	frame* currFrm;
	newFrame(&currFrm, 1000, 1000);

	drawBlankFrame(currFrm);

	for (int i = 0; i < n; i++) {

		int x = (int)round(stellarBodies[i].posx * 1000) + 500;
		int y = (int)round(stellarBodies[i].posy * 1000) + 500;

		if (x > 999) x = 999;
		if (y > 999) y = 999;
		if (x < 0) x = 0;
		if (y < 0) y = 0;

		setPixel(currFrm, y, x, 255, 255, 255);
	}

	char fileName[11];

	snprintf(fileName, sizeof(fileName), "frame%05d.png", iter);
	frm2png(currFrm, fileName);
	destroyFrame(currFrm, fileName);
}

int in(stellarBody b, quad q) {

	return contains(q, b.posx, b.posy);
}

void updateCofG(stellarBody* a, stellarBody* b) {

	int nmass = a->mass + b->mass;
	int nposx = ((a->posx * a->mass) + (b->posx * b->mass)) / nmass;
	int nposy = ((a->posy * a->mass) + (b->posy * b->mass)) / nmass;

	a->mass = nmass;
	a->posx = nposx;
	a->posy = nposy;
}

int contains(quad in, float x, float y) {

	if (((in.llx < x) && (in.llx + in.len > x)) && ((in.lly > y) && (in.lly - in.len < y)))
	{
		return 1;
	}

	return 0;
}

quad NW(quad in) {

	quad out;

	out.len = in.len / 2;
	out.llx = in.llx;
	out.lly = in.lly - (in.lly / 2);

	return out;
}

quad NE(quad in) {

	quad out;

	out.len = in.len / 2;
	out.llx = in.llx + (in.llx / 2);
	out.lly = in.lly - (in.lly / 2);

	return out;
}

quad SE(quad in) {

	quad out;

	out.len = in.len / 2;
	out.llx = in.llx + (in.llx / 2);
	out.lly = in.lly;

	return out;
}

quad SW(quad in) {

	quad out;

	out.len = in.len / 2;
	out.llx = in.llx;
	out.lly = in.lly;

	return out;
}

quadTree* newTree(quad q) {

	quadTree* out = (quadTree*)malloc(sizeof(quadTree));

	out->body = NULL;
	out->quadrant = q;

	out->NW = NULL;
	out->NE = NULL;
	out->SE = NULL;
	out->SW = NULL;

	return out;
}

void insert(quadTree* tree, stellarBody* body) {

	if (tree->body == NULL) {

		stellarBody* copy = (stellarBody*)malloc(sizeof(stellarBody));

		*copy = *body;
		tree->body = copy;
	}
	else if (tree->NW != NULL) {

		updateCofG(tree->body, body);

		if (in(*body, NW(tree->quadrant)) == 1) {

			insert(tree->NW, body);
		}
		else if (in(*body, NE(tree->quadrant)) == 1) {

			insert(tree->NE, body);
		}
		else if (in(*body, SE(tree->quadrant)) == 1) {

			insert(tree->SE, body);
		}
		else if (in(*body, SW(tree->quadrant)) == 1) {

			insert(tree->SW, body);
		}
	}
	else {

		tree->NW = newTree(NW(tree->quadrant));
		tree->NE = newTree(NE(tree->quadrant));
		tree->SE = newTree(SE(tree->quadrant));
		tree->SW = newTree(SW(tree->quadrant));

		if (in(*body, NW(tree->quadrant)) == 1) {

			insert(tree->NW, tree->body);
		}
		else if (in(*(tree->body), NE(tree->quadrant)) == 1) {

			insert(tree->NE, tree->body);
		}
		else if (in(*(tree->body), SE(tree->quadrant)) == 1) {

			insert(tree->SE, tree->body);
		}
		else if (in(*(tree->body), SW(tree->quadrant)) == 1) {

			insert(tree->SW, tree->body);
		}

		// above is for old body, below for new body

		if (in(*body, NW(tree->quadrant)) == 1) {

			insert(tree->NW, body);
		}
		else if (in(*body, NE(tree->quadrant)) == 1) {

			insert(tree->NE, body);
		}
		else if (in(*body, SE(tree->quadrant)) == 1) {

			insert(tree->SE, body);
		}
		else if (in(*body, SW(tree->quadrant)) == 1) {

			insert(tree->SW, body);
		}

		updateCofG(tree->body, body);
	}
}

void updateForce(quadTree* tree, stellarBody* body, double G, float theta, float lb) {

	float distancex = tree->body->posx - body->posx;
	float distancey = tree->body->posy - body->posy;

	float distance = ((distancex * distancex) + (distancey * distancey)) > lb ? (distancex * distancex) + (distancey * distancey) : lb;

	float force = G / distance;

	float angle = atan2(distancey, distancex);

	if ((tree->NW != NULL) && (tree->body != body)) {
		
		body->accx += force * cos(angle);
		body->accy += force * sin(angle);
	}
	else if ((tree->quadrant.len / sqrt(distance)) < theta) {

		body->accx += force * cos(angle);
		body->accy += force * sin(angle);
	}
	else {

		if (tree->NW->body != NULL) updateForce(tree->NW, body, G, theta, lb);
		if (tree->NE->body != NULL) updateForce(tree->NE, body, G, theta, lb);
		if (tree->SE->body != NULL) updateForce(tree->SE, body, G, theta, lb);
		if (tree->SW->body != NULL) updateForce(tree->SW, body, G, theta, lb);
	}
}

void freeTree(quadTree* tree) {

	if (tree == NULL) {

		return;
	}

	else {

		freeTree(tree->NW);
		freeTree(tree->NE);
		freeTree(tree->SE);
		freeTree(tree->SW);

		free(tree->body);
		free(tree);
	}
}