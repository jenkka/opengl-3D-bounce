#ifndef CYLINDER_H_
#define CYLINDER_H_

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Utils.h"
#include "Mat4.h"
#include "Transforms.h"
#include <stdio.h>
#include <math.h>

#define M_PI 3.14159265358979323846
#define RESET 0xFFFF
typedef float vec3[3];

struct strCylinder{
	float lenght;
	float bottomRadius;
	float topRadius;
	int slices;
	int stacks;
	vec3 bottomColor;
	vec3 topColor;
};

typedef struct strCylinder * Cylinder;

static GLuint vertexPositionLoc,  vertexNormalLoc, projectionMatrixLoc,  viewMatrixLoc;
static GLuint vertexColorLoc, vertexPositionLoc, vertexNormalLoc;
static GLuint cylinderVA, topFlapVA, bottomFlapVA, cylinderColorVA, cylinderBuffer[4], topFlapBuffer[4], bottomFlapBuffer[4];
static GLuint programId, modelMatrixLoc;
static Mat4   modelMatrix;
float wallLoc[];
int indexLenght;
int draw;


GLuint rhombusVA, rhombusBuffer[3];

void crossProduct(float vect_A[], float vect_B[], float cross_P[]);
Cylinder cylinderCreate(float l, float br, float tr, int sl, int st, vec3 bc, vec3 tc);
void cylinderBind(Cylinder c, GLuint vpl, GLuint vcl, GLuint vnl);
void cylinderDraw(Cylinder c);
void cylinderDrawLines(Cylinder c);

#endif /* CYLINDER_H_ */
