// JUAN CARLOS GONZALEZ GUERRA
// '1' BUTTON = DARK MODE
// '2' BUTTON = BLACK LIGHT
// '3' BUTTON = RANDOM ROOM
// ARROW KEYS = MOVE AND ROTATE
// W, S 	  = LOOK UP AND DOWN

#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include "Transforms.h"
#include "Cylinder.h"

#define toRadians(deg) deg * M_PI / 180.0

typedef enum { IDLE, LEFT, RIGHT, UP, DOWN, FRONT, BACK } MOTION_TYPE;
typedef float vec3[3];

static Mat4   modelMatrix, projectionMatrix, viewMatrix;
static GLuint programId, vertexColorLoc, vertexPositionLoc,  vertexNormalLoc, modelMatrixLoc,  projectionMatrixLoc,  viewMatrixLoc;
static GLuint programId2, vertexPositionLoc2, modelColorLoc2,  modelMatrixLoc2, projectionMatrixLoc2, viewMatrixLoc2;
static GLuint lightPositionLoc, exponentLoc, diffuseLightLoc, ambientLightLoc, materialALoc, materialDLoc;
static GLuint materialSLoc, cameraPositionLoc;
static GLuint lightsBufferId;

GLuint roomVA;
GLuint cubeVA;
GLuint roomBuffers[3];

static GLboolean usePerspective = GL_TRUE;

static MOTION_TYPE motionType      = 0;

static float cameraSpeed     = 0.1;
static float cameraRotationSpeed = 1.5;
static float cameraX         = 0;
static float cameraZ         = 5;
static float cameraXAngle    = 0;
static float cameraYAngle 	 = 0;
static float radians;

static const int ROOM_WIDTH  = 30;
static const int ROOM_HEIGHT =  15;
static const int ROOM_DEPTH  = 30;

static float w1 = -ROOM_WIDTH  / 2;
static float w2 = ROOM_WIDTH  / 2;
static float h1 = -ROOM_HEIGHT / 2;
static float h2 = ROOM_HEIGHT / 2;
static float d1 = -ROOM_DEPTH  / 2;
static float d2 = ROOM_DEPTH  / 2;

static float w1Color[] = { 0.9, 0.9, 0.9 };
static float w2Color[] = { 0.9, 0.9, 0.9 };
static float h1Color[] = { 0.8, 0.8, 0.8 };
static float h2Color[] = { 1, 1, 1 };
static float d1Color[] = { 0.9, 0.9, 0.9 };
static float d2Color[] = { 0.9, 0.9, 0.9 };

static float ambientLight[]  	= {1.0, 1.0, 1.0};
static float materialA[]     	= {0.8, 0.8, 0.8};
static float diffuseLight[]  	= {0.8, 0.8, 0.8};
static float lightPosition[] 	= {0.0, 2.0, 0.0};
static float materialD[]     	= {0.8, 0.8, 0.8};
static float materialS[]		= {0.4, 0.4, 0.4};
static float exponent			= 20;

//                         		   Color    subcutoff,  Position  Exponent   Direction  Cos(cutoff)
static float lights[]   = { -0.8, -0.8, -0.8,  0.92,    0, 0, 0,     40,     0, -1, 0,     0.9 };

float length = 1, bottomRadius = 0.5, topRadius = 0.5;
int slices = 4, stacks = 1;
vec3 topColor 		= { 1, 0, 0 };
vec3 bottomColor 	= { 1, 0, 0 };
vec3 cylinderPos	= { 0.0, 0.0, 0.0 };
vec3 cylinderDir 	= { 0.0, 0.0, 0.0 };
float upper = 0.03;
float lower = -0.03;

Cylinder cylinder;

static void initShaders()
{
	//CREATING CYLINDER
	cylinder = cylinderCreate(length, bottomRadius, topRadius, slices, stacks, bottomColor, topColor);

	GLuint vShader = compileShader("shaders/phong.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	GLuint fShader = compileShader("shaders/phong.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;

	programId = glCreateProgram();
	glAttachShader(programId, vShader);
	glAttachShader(programId, fShader);
	glLinkProgram(programId);

	vertexPositionLoc   = glGetAttribLocation(programId, "vertexPosition");
	vertexNormalLoc     = glGetAttribLocation(programId, "vertexNormal");
	vertexColorLoc    	= glGetAttribLocation(programId, "vertexColor");

	modelMatrixLoc      = glGetUniformLocation(programId, "modelMatrix");
	viewMatrixLoc       = glGetUniformLocation(programId, "viewMatrix");
	projectionMatrixLoc = glGetUniformLocation(programId, "projMatrix");

	ambientLightLoc     = glGetUniformLocation(programId, "ambientLight");
	diffuseLightLoc     = glGetUniformLocation(programId, "diffuseLight");
	lightPositionLoc    = glGetUniformLocation(programId, "lightPosition");
	materialALoc        = glGetUniformLocation(programId, "materialA");
	materialDLoc        = glGetUniformLocation(programId, "materialD");
	materialSLoc        = glGetUniformLocation(programId, "materialS");
	exponentLoc 		= glGetUniformLocation(programId, "exponent");
	cameraPositionLoc   = glGetUniformLocation(programId, "cameraPosition");

	vShader = compileShader("shaders/position_mvp.vsh", GL_VERTEX_SHADER);
	if(!shaderCompiled(vShader)) return;
	fShader = compileShader("shaders/modelColor.fsh", GL_FRAGMENT_SHADER);
	if(!shaderCompiled(fShader)) return;
	programId2 = glCreateProgram();
	glAttachShader(programId2, vShader);
	glAttachShader(programId2, fShader);
	glLinkProgram(programId2);

	vertexPositionLoc2   = glGetAttribLocation(programId2, "vertexPosition");
	modelMatrixLoc2      = glGetUniformLocation(programId2, "modelMatrix");
	viewMatrixLoc2       = glGetUniformLocation(programId2, "viewMatrix");
	projectionMatrixLoc2 = glGetUniformLocation(programId2, "projectionMatrix");
	modelColorLoc2       = glGetUniformLocation(programId2, "modelColor");

	//BINDING CYLINDER
	cylinderBind(cylinder, vertexPositionLoc, vertexColorLoc, vertexNormalLoc);
}

static void initLights()
{
	glUseProgram(programId);
	glUniform3fv(ambientLightLoc,  1, ambientLight);
	glUniform3fv(materialALoc,     1, materialA);
	glUniform3fv(materialDLoc,     1, materialD);
	glUniform3fv(materialSLoc,     1, materialS);

	glUniform3fv(diffuseLightLoc,  1, diffuseLight);
	glUniform3fv(lightPositionLoc, 1, lightPosition);
	glUniform1f(exponentLoc, exponent);

	glGenBuffers(1, &lightsBufferId);
	glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW);

	GLuint uniformBlockIndex = glGetUniformBlockIndex(programId, "LightBlock");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
	glUniformBlockBinding(programId, uniformBlockIndex, 0);
}

static void initLightCubes() {
	float l1 = -0.2, l2 = 0.2;
	float positions[] = {l1, l1, l2, l2, l1, l2, l1, l2, l2, l2, l1, l2, l2, l2, l2, l1, l2, l2,  // Frente
						 l2, l1, l1, l1, l1, l1, l2, l2, l1, l1, l1, l1, l1, l2, l1, l2, l2, l1,  // Atras
						 l1, l1, l1, l1, l1, l2, l1, l2, l1, l1, l1, l2, l1, l2, l2, l1, l2, l1,  // Izquierda
						 l2, l2, l1, l2, l2, l2, l2, l1, l1, l2, l2, l2, l2, l1, l2, l2, l1, l1,  // Derecha
						 l1, l1, l1, l2, l1, l1, l1, l1, l2, l2, l1, l1, l2, l1, l2, l1, l1, l2,  // Abajo
						 l2, l2, l1, l1, l2, l1, l2, l2, l2, l1, l2, l1, l1, l2, l2, l2, l2, l2   // Arriba
	};

	glUseProgram(programId2);
	glGenVertexArrays(1, &cubeVA);
	glBindVertexArray(cubeVA);
	GLuint bufferId;
	glGenBuffers(1, &bufferId);
	glBindBuffer(GL_ARRAY_BUFFER, bufferId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc2, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc2);
}



static void initRoom() {
	w1 = -ROOM_WIDTH  / 2;
	w2 = ROOM_WIDTH  / 2;
	h1 = -ROOM_HEIGHT / 2;
	h2 = ROOM_HEIGHT / 2;
	d1 = -ROOM_DEPTH  / 2;
	d2 = ROOM_DEPTH  / 2;

	float positions[] =
	{
		w1, h2, d1, w1, h1, d1, w2, h1, d1,   w2, h1, d1, w2, h2, d1, w1, h2, d1,  // Frente
		w2, h2, d2, w2, h1, d2, w1, h1, d2,   w1, h1, d2, w1, h2, d2, w2, h2, d2,  // Atrás
		w1, h2, d2, w1, h1, d2, w1, h1, d1,   w1, h1, d1, w1, h2, d1, w1, h2, d2,  // Izquierda
		w2, h2, d1, w2, h1, d1, w2, h1, d2,   w2, h1, d2, w2, h2, d2, w2, h2, d1,  // Derecha
		w1, h1, d1, w1, h1, d2, w2, h1, d2,   w2, h1, d2, w2, h1, d1, w1, h1, d1,  // Abajo
		w1, h2, d2, w1, h2, d1, w2, h2, d1,   w2, h2, d1, w2, h2, d2, w1, h2, d2   // Arriba
	};

	float normals[] =
	{
		0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  // Frente
		0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  // Atrás
		1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  // Izquierda
	   -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0,  // Derecha
		0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  0,  1,  0,  // Abajo
		0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  0, -1,  0,  // Arriba
	};

	float colors[] =
	{
			w1Color[0], w1Color[1], w1Color[2], w1Color[0], w1Color[1], w1Color[2], w1Color[0], w1Color[1], w1Color[2], w1Color[0], w1Color[1], w1Color[2], w1Color[0], w1Color[1], w1Color[2], w1Color[0], w1Color[1], w1Color[2],
			w2Color[0], w2Color[1], w2Color[2], w2Color[0], w2Color[1], w2Color[2], w2Color[0], w2Color[1], w2Color[2], w2Color[0], w2Color[1], w2Color[2], w2Color[0], w2Color[1], w2Color[2], w2Color[0], w2Color[1], w2Color[2],
			d1Color[0], d1Color[1], d1Color[2], d1Color[0], d1Color[1], d1Color[2], d1Color[0], d1Color[1], d1Color[2], d1Color[0], d1Color[1], d1Color[2], d1Color[0], d1Color[1], d1Color[2], d1Color[0], d1Color[1], d1Color[2],
			d2Color[0], d2Color[1], d2Color[2], d2Color[0], d2Color[1], d2Color[2], d2Color[0], d2Color[1], d2Color[2], d2Color[0], d2Color[1], d2Color[2], d2Color[0], d2Color[1], d2Color[2], d2Color[0], d2Color[1], d2Color[2],
			h1Color[0], h1Color[1], h1Color[2], h1Color[0], h1Color[1], h1Color[2], h1Color[0], h1Color[1], h1Color[2], h1Color[0], h1Color[1], h1Color[2], h1Color[0], h1Color[1], h1Color[2], h1Color[0], h1Color[1], h1Color[2],
			h2Color[0], h2Color[1], h2Color[2], h2Color[0], h2Color[1], h2Color[2], h2Color[0], h2Color[1], h2Color[2], h2Color[0], h2Color[1], h2Color[2], h2Color[0], h2Color[1], h2Color[2], h2Color[0], h2Color[1], h2Color[2]
	};

	glUseProgram(programId);
	glGenVertexArrays(1, &roomVA);
	glBindVertexArray(roomVA);
	GLuint buffers[3];
	glGenBuffers(3, buffers);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexPositionLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexPositionLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexNormalLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexNormalLoc);

	glBindBuffer(GL_ARRAY_BUFFER, buffers[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vertexColorLoc, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vertexColorLoc);
}

static float randFrom(float min, float max)
{
    double range = (max - min);
    double div = RAND_MAX / range;

    return min + (rand() / div);
}

static void setStartingDirection()
{
	srand(time(NULL));
	float maxSpeed = 0.1;
	float num = randFrom(-maxSpeed, maxSpeed);
	do{ num = randFrom(-0.1, 0.1); } while(num > -0.05  && num < 0.05);
	cylinderDir[0] = num;
	do{ num = randFrom(-0.1, 0.1); } while(num > -0.05  && num < 0.05);
	cylinderDir[1] = num;
	do{ num = randFrom(-0.1, 0.1); } while(num > -0.05  && num < 0.05);
	cylinderDir[2] = num;
	printf("Starting Direction: %.2f, %.2f, %.2f\n", cylinderDir[0], cylinderDir[1], cylinderDir[2]);
}

static float getCylinderNextX()
{
	float newPos = cylinderPos[0] + cylinderDir[0];

	return newPos;
}

static float getCylinderNextY()
{
	float newPos = cylinderPos[1] + cylinderDir[1];

	return newPos;
}

static float getCylinderNextZ()
{
	float newPos = cylinderPos[2] + cylinderDir[2];

	return newPos;
}

static int cylinderWallHit()
{
	float inc = 0.3;
	if(getCylinderNextX() >= w2 - inc || getCylinderNextX() <= w1 + inc) { return 1; }
	if(getCylinderNextY() >= h2 - inc || getCylinderNextY() <= h1 + inc) { return 2; }
	if(getCylinderNextZ() >= d2 - inc || getCylinderNextZ() <= d1 + inc) { return 3; }
	return 0;
}

static void updateCylinderLoc()
{
	if(cylinderWallHit() == 1) { cylinderDir[0] = (-cylinderDir[0]); }
	if(cylinderWallHit() == 2) { cylinderDir[1] = (-cylinderDir[1]); }
	if(cylinderWallHit() == 3) { cylinderDir[2] = (-cylinderDir[2]); }
	cylinderPos[0] += cylinderDir[0];
	cylinderPos[1] += cylinderDir[1];
	cylinderPos[2] += cylinderDir[2];
}

static float getNextX(float x)
{
	radians = M_PI * cameraXAngle / 180;
	x -= cameraSpeed * sin(-radians);

	return x;
}

static float getNextZ(float z)
{
	radians = M_PI * cameraXAngle / 180;
	z -= cameraSpeed * cos(radians);

	return z;
}

static float getPastX(float x)
{
	radians = M_PI * cameraXAngle / 180;
	x += cameraSpeed * sin(-radians);

	return x;
}

static float getPastZ(float z)
{
	radians = M_PI * cameraXAngle / 180;
	z += cameraSpeed * cos(radians);

	return z;
}

static int canMove(MOTION_TYPE m)
{
	switch(m)
	{
		case FRONT:
			if(getNextX(cameraX) >= w2 - cameraSpeed * 13) { return 0; }
			if(getNextZ(cameraZ) >= d2 - cameraSpeed * 13) { return 0; }
			if(getNextX(cameraX) <= w1 + cameraSpeed * 13) { return 0; }
			if(getNextZ(cameraZ) <= d1 + cameraSpeed * 13) { return 0; }
			break;

		case BACK:
			if(getPastX(cameraX) >= w2 - cameraSpeed * 13) { return 0; }
			if(getPastZ(cameraZ) >= d2 - cameraSpeed * 13) { return 0; }
			if(getPastX(cameraX) <= w1 + cameraSpeed * 13) { return 0; }
			if(getPastZ(cameraZ) <= d1 + cameraSpeed * 13) { return 0; }
			break;
	}
	return 1;
}

static void reshapeFunc(int w, int h)
{
	glViewport(0, 0, w, h);
	float aspect = (float) w / h;
	if(usePerspective) {
		setPerspective(&projectionMatrix, 60, aspect, -1, -60);
	} else {
		float cubeLength = 10;
		if(aspect >= 1.0) setOrtho(&projectionMatrix, -cubeLength * aspect, cubeLength * aspect,
				                                -cubeLength, cubeLength, -cubeLength, cubeLength);
		else setOrtho(&projectionMatrix, -cubeLength,  cubeLength, -cubeLength / aspect, cubeLength / aspect,
										        -cubeLength, cubeLength);
	}
	glUniformMatrix4fv(projectionMatrixLoc, 1, GL_TRUE, projectionMatrix.values);
}

static void timerFunc(int id)
{
	glutTimerFunc(1, timerFunc, id);
	glutPostRedisplay();
}

static void specialKeyReleasedFunc(int key,int x, int y)
{
	motionType = IDLE;
}

static void keyReleasedFunc(unsigned char key,int x, int y)
{
	motionType = IDLE;
}

static void specialKeyPressedFunc(int key, int x, int y)
{
	switch(key)
	{
		case 100: motionType = LEFT;  break;
		case 102: motionType = RIGHT; break;
		case 101: motionType = FRONT; break;
		case 103: motionType = BACK;
	}
}

static void keyPressedFunc(unsigned char key, int x, int y)
{
	switch(key) {
		case 'w':
		case 'W':
			motionType = UP; break;
		case 's':
		case 'S':
			motionType = DOWN; break;
		case '1':
			for(int i = 0; i < sizeof(w1Color) / sizeof(float); i++)
			{
				w1Color[i] = 0;
				w2Color[i] = 0;
				h1Color[i] = 0;
				h2Color[i] = 0;
				d1Color[i] = 0;
				d2Color[i] = 0;
			}
			bottomColor[0] = 1;
			bottomColor[1] = 0;
			bottomColor[2] = 0;
			topColor[0] = 1;
			topColor[1] = 0;
			topColor[2] = 0;

			lights[0] = 1;
			lights[1] = 0;
			lights[2] = 0;
			lights[3] = 0;
			lights[11] = 0.6;

			initShaders();
			initLights();
			initRoom();
			initLightCubes();
			break;

		case '2':
			for(int i = 0; i < sizeof(w1Color) / sizeof(float); i++)
			{
				w1Color[i] = 1;
				w2Color[i] = 1;
				h1Color[i] = 1;
				h2Color[i] = 1;
				d1Color[i] = 1;
				d2Color[i] = 1;
			}
			bottomColor[0] = 0;
			bottomColor[1] = 0;
			bottomColor[2] = 0;
			topColor[0] = 0;
			topColor[1] = 0;
			topColor[2] = 0;

			lights[0] = -1;
			lights[1] = -1;
			lights[2] = -1;
			lights[3] = 0;
			lights[11] = 0.3;

			initShaders();
			initLights();
			initRoom();
			initLightCubes();
			break;

		case'3':
			w1Color[0] = randFrom(0,1);
			w1Color[1] = randFrom(0,1);
			w1Color[2] = randFrom(0,1);
			w2Color[0] = randFrom(0,1);
			w2Color[1] = randFrom(0,1);
			w2Color[2] = randFrom(0,1);
			h1Color[0] = randFrom(0,1);
			h1Color[1] = randFrom(0,1);
			h1Color[2] = randFrom(0,1);
			h2Color[0] = randFrom(0,1);
			h2Color[1] = randFrom(0,1);
			h2Color[2] = randFrom(0,1);
			d1Color[0] = randFrom(0,1);
			d1Color[1] = randFrom(0,1);
			d1Color[2] = randFrom(0,1);
			d2Color[0] = randFrom(0,1);
			d2Color[1] = randFrom(0,1);
			d2Color[2] = randFrom(0,1);


			bottomColor[0] = randFrom(0,1);
			bottomColor[1] = randFrom(0,1);
			bottomColor[2] = randFrom(0,1);
			topColor[0] = randFrom(0,1);;
			topColor[1] = randFrom(0,1);;
			topColor[2] = randFrom(0,1);;

			lights[0] = -0.8;
			lights[1] = -0.8;
			lights[2] = -0.8;
			lights[3] = 0.92;
			lights[11] = 0.9;

			initShaders();
			initLights();
			initRoom();
			initLightCubes();
			break;

		case 27 : exit(0);
	}
 }

static void displayFunc() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//	Actualizar posición de la cámara
	switch(motionType)
	{
  		case  LEFT  : cameraXAngle-=cameraRotationSpeed; break;
  		case  RIGHT : cameraXAngle+=cameraRotationSpeed; break;
		case  FRONT :
			if(canMove(FRONT) == 1)
			{
				radians = M_PI * cameraXAngle / 180;
				cameraX -= cameraSpeed * sin(-radians);
				cameraZ -= cameraSpeed * cos(radians);
			}
			break;

		case  BACK  :
			if(canMove(BACK) == 1)
			{
				radians = M_PI * cameraXAngle / 180;
				cameraX += cameraSpeed * sin(-radians);
				cameraZ += cameraSpeed * cos(radians);
			}
			break;

		case  UP    : if(cameraYAngle <= 90) { cameraYAngle+= cameraRotationSpeed; }; break;
		case  DOWN  : if(cameraYAngle >= -90) { cameraYAngle-= cameraRotationSpeed; }; break;
		case  IDLE  :  ;
	}


	//	Projection and view (programId)
	glUseProgram(programId);
	glUniformMatrix4fv(projectionMatrixLoc, 1, true, projectionMatrix.values);
	mIdentity(&viewMatrix);
	rotateX(&viewMatrix, cameraYAngle);
	rotateY(&viewMatrix, cameraXAngle);
	translate(&viewMatrix, -cameraX, 0, -cameraZ);
	glUniformMatrix4fv(viewMatrixLoc, 1, true, viewMatrix.values);
	glUniform3f(cameraPositionLoc, cameraX, 0, cameraZ);

	//Create cylinder
	translate(&modelMatrix, cylinderPos[0], cylinderPos[1], cylinderPos[2]);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	updateCylinderLoc();
	cylinderDraw(cylinder);
	mIdentity(&modelMatrix);

	// Room
	mIdentity(&modelMatrix);
	glBindVertexArray(roomVA);
	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	mIdentity(&modelMatrix);

	// Projection and view (programId2)
	glUseProgram(programId2);
	glBindVertexArray(cubeVA);
	glUniformMatrix4fv(projectionMatrixLoc2, 1, true, projectionMatrix.values);
	glUniformMatrix4fv(viewMatrixLoc2, 1, true, viewMatrix.values);

	// Shadow
	lights[4] = cylinderPos[0];
	lights[5] = cylinderPos[1];
	lights[6] = cylinderPos[2];
	glBindBuffer(GL_UNIFORM_BUFFER, lightsBufferId);
	glBufferData(GL_UNIFORM_BUFFER, sizeof(lights), lights, GL_DYNAMIC_DRAW);
	GLuint uniformBlockIndex = glGetUniformBlockIndex(programId, "LightBlock");
	glBindBufferBase(GL_UNIFORM_BUFFER, 0, lightsBufferId);
	glUniformBlockBinding(programId, uniformBlockIndex, 0);
	mIdentity(&modelMatrix);
	//printf("Current Position: %.2f, %.2f, %.2f\n", cylinderPos[0], cylinderPos[1], cylinderPos[2]);
	//	printf("Current Position: %.2f, %.2f, %.2f\n", cylinderPos[0], cylinderPos[1], cylinderPos[2]);
	//printf("Current Position: %.2f, %.2f\n", cameraX, cameraZ);
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	setbuf(stdout, NULL);
	setStartingDirection();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Bounce");
    glutDisplayFunc(displayFunc);
    glutReshapeFunc(reshapeFunc);
    glutTimerFunc(10, timerFunc, 1);
    glutKeyboardFunc(keyPressedFunc);
    glutKeyboardUpFunc(keyReleasedFunc);
    glutSpecialFunc(specialKeyPressedFunc);
    glutSpecialUpFunc(specialKeyReleasedFunc);
    glewInit();
    glEnable(GL_DEPTH_TEST);
    initShaders();
    initLights();
    initRoom();
    initLightCubes();
    glClearColor(0.1, 0.1, 0.1, 1.0);

    glutMainLoop();

	return 0;
}
