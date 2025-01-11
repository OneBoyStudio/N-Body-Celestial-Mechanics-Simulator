#ifndef BARNESHUT_H
#define BARNESHUT_H
#include "simulation.h"

typedef struct
{

	float mass; //total mass of all bodies in this node; if this is an outer node, then it is the mass of the body
	float cmx; //center of mass of this node; if this is an outer node, then it is the center of mass of the body
	float cmy; //center of mass of this node; if this is an outer node, then it is the center of mass of the body

	float uleftx; //the upper left bound of the node
	float ulefty; //the upper left bound of the node

	float brightx; //the lower right bound of the node
	float brighty; //the lower right bound of the node

	struct treenode* nw; //Northwest
	struct treenode* ne; //Northeast
	struct treenode* se; //Southeast
	struct treenode* sw; //Southwest

	struct stellarBody* body;

} treenode;

typedef struct
{

	float x;
	float y;
	float mass;

} force;

treenode* createQuadtreeNode(float m, float cmx, float cmy);

treenode* populateQuadtree(stellarBody* stellarbodies, int numOfBodies);

void populateNode(treenode* root);

void traverseTreeP(stellarBody* body, treenode* root);

force* traverseTreeC(stellarBody* body, treenode* root, float theta);

void freeTree(treenode* roots);

#endif