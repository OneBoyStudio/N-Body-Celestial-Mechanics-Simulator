#include <stdio.h>
#include <stdlib.h>
#include "barnesHut.h"
#include "simulation.h"
#include <math.h>

treenode* createQuadtreeNode(float m, float cmx, float cmy, float uleftx, float ulefty, float brightx, float brighty)
{

	treenode* out = malloc(sizeof(treenode));

	if (out != NULL)
	{
		out->nw = NULL;
		out->ne = NULL;
		out->se = NULL;
		out->sw = NULL;

		out->mass = m;
		out->cmx = cmx;
		out->cmy = cmy;

		out->uleftx = uleftx;
		out->ulefty = ulefty;
		out->brightx = brightx;
		out->brighty = brighty;

		out->body = NULL;

	}

	return out;
}

force* createForceObject(float x, float y)
{

	force* out = malloc(sizeof(force));

	if (out != NULL)
	{

		out->x = x;
		out->y = y;

	}

	return out;

}

treenode* populateQuadtree(stellarBody* stellarbodies, int numOfBodies)
{

	treenode* root = createQuadtreeNode(0, 0, 0, 0, 0, 2000.0f, 2000.0f); //its fine to keep the root node mass at 0

	root->nw = createQuadtreeNode(0, 0, 0, root->uleftx, root->ulefty, root->brightx / 2.0f, root->brighty / 2.0f);
	root->ne = createQuadtreeNode(0, 0, 0, root->brightx / 2.0f, root->ulefty, root->brightx, root->brighty / 2.0f);
	root->se = createQuadtreeNode(0, 0, 0, root->brightx / 2.0f, root->brighty / 2.0f, root->brightx, root->brighty);
	root->sw = createQuadtreeNode(0, 0, 0, root->uleftx, root->brighty / 2.0f, root->brightx / 2.0f, root->brighty);

	for (int i = 0; i < numOfBodies; i++)
	{

		traverseTreeP(&stellarbodies[i], root);

	}

	return root;
}

void populateNode(stellarBody* body, treenode* parent)
{

	if (parent->nw == NULL)
	{

		parent->mass = body->mass;
		parent->cmx = body->posx;
		parent->cmy = body->posy;
		parent->body = body;

	}
	else
	{

		parent->mass += body->mass;
		parent->cmx = ((parent->cmx * parent->mass) + (body->mass * body->posx)) / parent->mass;
		parent->cmy = ((parent->cmy * parent->mass) + (body->mass * body->posy)) / parent->mass;

	}

}

void traverseTreeP(stellarBody* body, treenode* root)
{

	if ((root->nw == NULL) && root->mass == 0.0f)
	{

		populateNode(body, root);

	}

	if ((root->nw == NULL) && root->mass != 0.0f)
	{

		root->nw = createQuadtreeNode(0, 0, 0, root->uleftx, root->ulefty, root->brightx / 2.0f, root->brighty / 2.0f);
		root->ne = createQuadtreeNode(0, 0, 0, root->brightx / 2.0f, root->ulefty, root->brightx, root->brighty / 2.0f);
		root->se = createQuadtreeNode(0, 0, 0, root->brightx / 2.0f, root->brighty / 2.0f, root->brightx, root->brighty);
		root->sw = createQuadtreeNode(0, 0, 0, root->uleftx, root->brighty / 2.0f, root->brightx / 2.0f, root->brighty);

		stellarBody* nodeA = malloc(sizeof(stellarBody));

		if (nodeA != NULL)
		{
			nodeA->posx = root->cmx;
			nodeA->posy = root->cmy;
			nodeA->mass = root->mass;

			root->cmx = 0.0f;
			root->cmy = 0.0f;
			root->mass = 0.0f;

			traverseTreeP(nodeA, root);
			traverseTreeP(body, root);

			free(nodeA);
		}

	}

	if (((body->posx + 1000.0f) >= root->uleftx) && ((body->posx + 1000.0f) <= root->brightx / 2.0f))
	{

		if (((body->posy + 1000.0f) >= root->ulefty) && ((body->posy + 1000.0f) <= root->brighty / 2.0f)) //NW
		{

			populateNode(body, root);
			traverseTreeP(body, root->nw);

		}

		else if (((body->posy + 1000.0f) >= root->brighty / 2.0f) && ((body->posy + 1000.0f) <= root->brighty)) //SW
		{

			populateNode(body, root);
			traverseTreeP(body, root->sw);

		}

	}

	else if (((body->posx + 1000.0f) >= root->brightx / 2.0f) && ((body->posx + 1000.0f) <= root->brightx))
	{
		
		if (((body->posy + 1000.0f) >= root->ulefty) && ((body->posy + 1000.0f) <= root->brighty / 2.0f)) //NE
		{

			populateNode(body, root);
			traverseTreeP(body, root->ne);

		}

		else if (((body->posy + 1000.0f) >= root->brighty / 2.0f) && ((body->posy + 1000.0f) <= root->brighty)) //SE
		{

			populateNode(body, root);
			traverseTreeP(body, root->se);

		}

	}

}

force* traverseTreeC(stellarBody* body, treenode* root, float theta, float G)
{

	force* out = createForceObject(0.0f, 0.0f);

	float d_x = root->cmx - body->posx;
	float d_y = root->cmy - body->posy;
	float d = sqrt((d_x * d_x) + (d_y * d_y));

	if (root->body != body)
	{

		if (root->nw == NULL)
		{

			if (root->body != NULL)
			{

				float frc = (G * body->mass * root->mass) / (((d_x * 1000) * (d_x * 1000)) + ((d_y * 1000) * (d_y * 1000)));
				float angle = atan2(d_y, d_x);

				out->x = cos(angle) * frc;
				out->y = sin(angle) * frc; //work based on this for everything else

				return out;

			}

			else
			{

				out->x = 0.0f;
				out->y = 0.0f;
				return out;

			}

		}
		
		else if (((root->brightx - root->uleftx) / d) < theta)
		{

			float frc = (G * body->mass * root->mass) / (((d_x * 1000) * (d_x * 1000)) + ((d_y * 1000) * (d_y * 1000)));
			float angle = atan2(d_y, d_x);

			out->x = cos(angle) * frc;
			out->y = sin(angle) * frc;

			return out;

		}

		else
		{

			force* trav1 = traverseTreeC(body, root->nw, theta, G);
			force* trav2 = traverseTreeC(body, root->ne, theta, G);
			force* trav3 = traverseTreeC(body, root->se, theta, G);
			force* trav4 = traverseTreeC(body, root->sw, theta, G);

			out->x = trav1->x + trav2->x + trav3->x + trav4->x;
			out->y = trav1->y + trav2->y + trav3->y + trav4->y;

			return out;

		}

	}

	else
	{

		out->x = 0.0f;
		out->y = 0.0f;
		return out;

	}

}

void freeTree(treenode* roots)
{

	if (roots->nw == NULL)
	{

		free(roots);

	}

	else
	{

		treenode* nodes[] = roots;
		freeTree();

	}

}