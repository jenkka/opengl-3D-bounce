#include "Cylinder.h"

void crossProduct(float vect_A[], float vect_B[], float cross_P[])
{
    cross_P[0] = vect_A[1] * vect_B[2] - vect_A[2] * vect_B[1];
    cross_P[1] = vect_A[2] * vect_B[0] - vect_A[0] * vect_B[2];
    cross_P[2] = vect_A[0] * vect_B[1] - vect_A[1] * vect_B[0];
}

Cylinder cylinderCreate(float l, float br, float tr, int sl, int st, vec3 bc, vec3 tc)
{
	printf("Creating cylinder...\n");

	Cylinder obj = malloc(sizeof(struct strCylinder));

	obj->lenght = l;
	obj->bottomRadius = br;
	obj->topRadius = tr;
	obj->slices = sl;
	obj->stacks = st;
	obj->bottomColor[0] = bc[0];
	obj->bottomColor[1] = bc[1];
	obj->bottomColor[2] = bc[2];
	obj->topColor[0] = tc[0];
	obj->topColor[1] = tc[1];
	obj->topColor[2] = tc[2];

	//printf("%.2f, %.2f, %.2f, %d, %d", obj->lenght, obj->bottomRadius, obj->topRadius, obj->slices, obj->stacks);

	return obj;
}

void cylinderBind(Cylinder c, GLuint vpl, GLuint vcl, GLuint vnl)
{
	printf("Binding cylinder...\n");

	float positions[(c->slices * c->stacks + c->slices) * 3];
	float tmpColors[(c->stacks + 1) * 3];
	float colors[sizeof(positions) / sizeof(float)];
	float tmpNormals[c->slices * 3];
	float normals[sizeof(positions) / sizeof(float)];
	GLuint indexes[(c->slices * 2 * c->stacks) + (c->stacks -1) + (c->stacks * 2)];

	indexLenght = sizeof(indexes) / sizeof(GLuint);
	float zPos = (c->lenght / 2.0);
	float radiusInc = -(c->topRadius - c->bottomRadius) / c->stacks;
	float radiusAngleDiff = 2 * M_PI / c->slices;
	float lenghtStep = c->lenght / (float)c->stacks;

	int offset;
	int locationCounter = 0;
	for(int i = 0; i < (c->stacks + 1); i++)
	{
		for(int j = 0; j < (c->slices * 3); j+=3)
		{
			offset = i * (c->slices * 3);
			positions[offset + j] 		= (c->topRadius + ((float)i * radiusInc)) * sin((float)j * radiusAngleDiff / 3);
			positions[offset + j + 1] 	= zPos - (lenghtStep * i);
			positions[offset + j + 2]	= (c->topRadius + ((float)i * radiusInc)) * cos((float)j * radiusAngleDiff / 3);
		}
	}

	// WALLS LOCATIONS
	float wallLoc[(c->stacks + 1) * 2 * 2];
	for(int i = 0; i < sizeof(wallLoc) / sizeof(float); i += 4)
	{
		wallLoc[i] 		= c->topRadius + radiusInc * i / 4;
		wallLoc[i + 1] 	= -(c->lenght / 2) + (i * lenghtStep / 4);
		wallLoc[i + 2] 	= -(c->topRadius + radiusInc * i / 4);
		wallLoc[i + 3] 	= -(c->lenght / 2) + (i * lenghtStep / 4);
	}

/*	printf("\nWALLS\n");
	for(int i = 0; i < (sizeof(wallLoc) / sizeof(float)); i+=2)
	{
		printf("%d:		%.2f, %.2f\n", (i/2 + 1), wallLoc[i], wallLoc[i+1]);
	}*/


	// TMP COLORS
	float randomR;
	float randomG;
	float randomB;
	float rInc = (c->bottomColor[0] - c->topColor[0]) / (c->stacks);
	float gInc = (c->bottomColor[1] - c->topColor[1]) / (c->stacks);
	float bInc = (c->bottomColor[2] - c->topColor[2]) / (c->stacks);
	for(int i = 0; i < sizeof(tmpColors) / sizeof(float); i+=3)
	{
		tmpColors[i] 		= c->topColor[0] + (rInc * i / 3);
		tmpColors[i + 1] 	= c->topColor[1] + (gInc * i / 3);
		tmpColors[i + 2] 	= c->topColor[2] + (bInc * i / 3);
	}

/*	printf("\nTEMP COLORS:\n");
	for(int i = 0; i < (sizeof(tmpColors) / sizeof(float)); i+=3)
	{
		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), tmpColors[i], tmpColors[i+1], tmpColors[i+2]);
	}*/

	// TMP COLRS WITH RANDOM
	srand(time(NULL));
	for(int i = 0; i < sizeof(tmpColors) / sizeof(float); i+=3)
	{
		randomR = (float)rand()/(float)(RAND_MAX/1.0);
		randomG = (float)rand()/(float)(RAND_MAX/1.0);
		randomB = (float)rand()/(float)(RAND_MAX/1.0);
		tmpColors[i] 		= tmpColors[i] * 0.8 +  randomR * 0.2;
		tmpColors[i + 1] 	= tmpColors[i + 1] * 0.8 +  randomG * 0.2;
		tmpColors[i + 2] 	= tmpColors[i + 2] * 0.8 +  randomB * 0.2;
	}

/*	printf("\nTEMP COLORS:\n");
	for(int i = 0; i < (sizeof(tmpColors) / sizeof(float)); i+=3)
	{
		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), tmpColors[i], tmpColors[i+1], tmpColors[i+2]);
	}*/

	// TMP COLORS TO ALL POSITIONS
	for(int i = 0; i < c-> stacks + 1; i++)
	{
		for(int j = 0; j < (c->slices * 3); j+=3)
		{
			offset = i * (c->slices * 3);
			colors[offset + j] 		= tmpColors[i * 3];
			colors[offset + j + 1] 	= tmpColors[i * 3 + 1];
			colors[offset + j + 2] 	= tmpColors[i * 3 + 2];
		}
	}

	// CROSS PRODUCT FOR NORMALS
	float vThis[3];
	float vNext[3];
	float vBelow[3];
	float vF[3];
	float vF2[3];
	for(int i = 0; i < sizeof(tmpNormals) / sizeof(float); i+=3)
	{
		vThis[0] = positions[i];
		vThis[1] = positions[i + 1];
		vThis[2] = positions[i + 2];
		vBelow[0]= positions[i + c->slices * 3];
		vBelow[1]= positions[i + 1 + c->slices * 3];
		vBelow[2]= positions[i + 2 + c->slices * 3];

		if(i == 3 * (c->slices - 1))
		{
			vNext[0] = positions[0];
			vNext[1] = positions[1];
			vNext[2] = positions[2];
		}
		else
		{
			vNext[0] = positions[i + 3];
			vNext[1] = positions[i + 4];
			vNext[2] = positions[i + 5];
		}

		crossProduct(vNext, vThis, vF);
		crossProduct(vF, vBelow, vF2);
		//printf("%d:		%d, %d, %d\n", (i/3 + 1), i + 1 - (c->slices), (i + 2) - (c->slices), (i + 3) - (c->slices));
//		printf("THIS");
//		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), vThis[0], vThis[1], vThis[2]);
//		printf("NEXT");
//		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), vNext[0], vNext[1], vNext[2]);
//		printf("BELOW");
//		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), vBelow[0], vBelow[1], vBelow[2]);

		//crossProduct(vBelow, vF, vF2);

		tmpNormals[i] 		= vF2[0];
		tmpNormals[i + 1] 	= vF2[1];
		tmpNormals[i + 2] 	= vF2[2];
	}

/*	printf("\nTEMP NORMALS:\n");
	for(int i = 0; i < (sizeof(tmpNormals) / sizeof(float)); i+=3)
	{
		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), tmpNormals[i], tmpNormals[i+1], tmpNormals[i+2]);
	}*/

	// FILLING NORMALS WITH TMPNORMALS
	for(int i = 0; i < c-> slices; i++)
	{
		for(int j = 0; j < (c->stacks + 1) * 3; j+=3)
		{
			offset = j * (c->slices -1);
			normals[offset + j + i * 3] 		= tmpNormals[i * 3];
			normals[offset + j + 1 + i * 3] 	= tmpNormals[i * 3 + 1];
			normals[offset + j + 2 + i * 3] 	= tmpNormals[i * 3 + 2];
		}
	}

/*	printf("\nNORMALS:\n");
	for(int i = 0; i < (sizeof(normals) / sizeof(float)); i+=3)
	{
		printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), normals[i], normals[i+1], normals[i+2]);
	}*/

	int indexOffset;
	float close1 = 0;
	float close2 = 0;
	int counter = 0;
	for(int i = 0; i < c->stacks; i++)
	{
		for(int j = 0; j < (c->slices * 2); j = j + 2)
		{
			indexOffset = i * (c->slices * 2);
			if(j == 0)
			{
				close1 = (j / 2) + indexOffset / 2;
				close2 = (j / 2) + indexOffset / 2 + c->slices;
			}

			indexes[indexOffset + j + counter] = (j / 2) + indexOffset / 2;
			indexes[indexOffset + j + 1 + counter] = (j / 2) + indexOffset / 2 + c->slices;
		}
		// CLOSE CYLINDER
		indexes[((i + 1) * (c->slices * 2)) + counter++] = close1;
		indexes[((i + 1) * (c->slices * 2)) + counter++] = close2;

		// RESET
		indexes[((i + 1) * (c->slices * 2)) + counter++] = RESET;
	}

/*printf("\nTEMP COLORS:\n");
for(int i = 0; i < (sizeof(tmpColors) / sizeof(float)); i+=3)
{
	printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), tmpColors[i], tmpColors[i+1], tmpColors[i+2]);
}
*/
/*printf("\nFINAL COLORS:\n");
for(int i = 0; i < (sizeof(colors) / sizeof(float)); i+=3)
{
	printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), colors[i], colors[i+1], colors[i+2]);
}*/

/*printf("\nPOSITIONS:\n");
for(int i = 0; i < sizeof(positions) / sizeof(float); i = i + 3)
{
	printf("%d:		%.2f, %.2f, %.2f\n", (i/3 + 1), positions[i], positions[i+1], positions[i+2]);
}*/

	GLuint flap1Indexes[c->slices];
	for(int i = 0; i < c->slices; i++)
	{
		flap1Indexes[i] = i;
	}

	GLuint flap2Indexes[c->slices];
	int flapCounter = (sizeof(positions) / sizeof(float) / 3) - 1;
	for(int i = 0; i < c->slices; i++)
	{
		flap2Indexes[i] = flapCounter--;
	}

	//CYLINDER BODY
	glUseProgram(programId);
	glGenVertexArrays(1, &cylinderVA);
	glBindVertexArray(cylinderVA);
	glGenBuffers(4, cylinderBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vpl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vpl);

	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vcl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vcl);

	glBindBuffer(GL_ARRAY_BUFFER, cylinderBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vnl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vnl);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderBuffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexes), indexes, GL_STATIC_DRAW);

	//TOP FLAP
	glGenVertexArrays(1, &topFlapVA);
	glBindVertexArray(topFlapVA);
	glGenBuffers(4, topFlapBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, topFlapBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vpl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vpl);

	glBindBuffer(GL_ARRAY_BUFFER, topFlapBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vcl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vcl);

	glBindBuffer(GL_ARRAY_BUFFER, topFlapBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vnl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vnl);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFlapBuffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flap1Indexes), flap1Indexes, GL_STATIC_DRAW);

	//BOTTOM FLAP
	glGenVertexArrays(1, &bottomFlapVA);
	glBindVertexArray(bottomFlapVA);
	glGenBuffers(4, bottomFlapBuffer);

	glBindBuffer(GL_ARRAY_BUFFER, bottomFlapBuffer[0]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(positions), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(vpl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vpl);

	glBindBuffer(GL_ARRAY_BUFFER, bottomFlapBuffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(colors), colors, GL_STATIC_DRAW);
	glVertexAttribPointer(vcl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vcl);

	glBindBuffer(GL_ARRAY_BUFFER, bottomFlapBuffer[2]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(normals), normals, GL_STATIC_DRAW);
	glVertexAttribPointer(vnl, 3, GL_FLOAT, 0, 0, 0);
	glEnableVertexAttribArray(vnl);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomFlapBuffer[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(flap2Indexes), flap2Indexes, GL_STATIC_DRAW);

	//Turn on next draw
	draw = 1;
}

void cylinderDraw(Cylinder c)
{
	if(draw == 1)
	{
		printf("Drawing cylinder...\n");
		draw = 0;
	}

	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	//BODY
	glBindVertexArray(cylinderVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderBuffer[3]);
	glDrawElements(GL_TRIANGLE_STRIP, indexLenght, GL_UNSIGNED_INT, 0);

	//TOP FLAP
	glBindVertexArray(topFlapVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFlapBuffer[3]);
	glDrawElements(GL_POLYGON, c->slices, GL_UNSIGNED_INT, 0);

	//BOTTOM FLAP
	glBindVertexArray(bottomFlapVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomFlapBuffer[3]);
	glDrawElements(GL_POLYGON, c->slices, GL_UNSIGNED_INT, 0);
}

void cylinderDrawLines(Cylinder c)
{
	if(draw == 1)
	{
		printf("Drawing cylinder...\n");
		draw = 0;
	}

	glUniformMatrix4fv(modelMatrixLoc, 1, true, modelMatrix.values);
	//BODY
	glBindVertexArray(cylinderVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderBuffer[3]);
	glDrawElements(GL_LINE_STRIP, indexLenght, GL_UNSIGNED_INT, 0);

	//TOP FLAP
	glBindVertexArray(topFlapVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, topFlapBuffer[3]);
	glDrawElements(GL_LINE_LOOP, c->slices, GL_UNSIGNED_INT, 0);

	//BOTTOM FLAP
	glBindVertexArray(bottomFlapVA);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bottomFlapBuffer[3]);
	glDrawElements(GL_LINE_LOOP, c->slices, GL_UNSIGNED_INT, 0);
}
