#include "vboplane.h"
#include "cg.h"

#include <cstdio>
#include <cmath>

VBOPlane::VBOPlane(float xsize, float zsize, int xdivs, int zdivs)
{

	faces = xdivs * zdivs;
	float * v = new float[3 * (xdivs + 1) * (zdivs + 1)];
	float * n = new float[3 * (xdivs + 1) * (zdivs + 1)];
	float * tex = new float[2 * (xdivs + 1) * (zdivs + 1)];
	unsigned int * el = new unsigned int[6 * xdivs * zdivs];

	float x2 = xsize / 2.0f;
	float z2 = zsize / 2.0f;
	float iFactor = (float)zsize / zdivs;
	float jFactor = (float)xsize / xdivs;
	float texi = 1.0f / zdivs;
	float texj = 1.0f / xdivs;
	float x, z;
	int vidx = 0, tidx = 0;
	for (int i = 0; i <= zdivs; i++) {
		z = iFactor * i - z2;
		for (int j = 0; j <= xdivs; j++) {
			x = jFactor * j - x2;
			v[vidx] = x; //vertex position
			v[vidx + 1] = 0.0f;
			v[vidx + 2] = z;
			n[vidx] = 0.0f; //normal (0,1,0)
			n[vidx + 1] = 1.0f;
			n[vidx + 2] = 0.0f;
			vidx += 3;
			tex[tidx] = j * texi; //모든 벌텍스마다 값이 있게 함.
			tex[tidx + 1] = i * texj;
			tidx += 2;
		}
	}

	unsigned int rowStart, nextRowStart;
	int idx = 0;
	for (int i = 0; i < zdivs; i++) {
		rowStart = i * (xdivs + 1);
		nextRowStart = (i + 1) * (xdivs + 1);
		for (int j = 0; j < xdivs; j++) { //벌텍스 4개 가지고 삼각형 6개 만들어서 분해한것.
			el[idx] = rowStart + j;
			el[idx + 1] = nextRowStart + j;
			el[idx + 2] = nextRowStart + j + 1;
			el[idx + 3] = rowStart + j;
			el[idx + 4] = nextRowStart + j + 1;
			el[idx + 5] = rowStart + j + 1;
			idx += 6;
		}
	}

	//gpu 바운딩
	unsigned int handle[4];
	glGenBuffers(4, handle);

	glGenVertexArrays(1, &vaoHandle);
	glBindVertexArray(vaoHandle);

	glBindBuffer(GL_ARRAY_BUFFER, handle[0]); 
	glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), v, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0))); //0에다가 v주고
	glEnableVertexAttribArray(0);  // Vertex position

	glBindBuffer(GL_ARRAY_BUFFER, handle[1]); 
	glBufferData(GL_ARRAY_BUFFER, 3 * (xdivs + 1) * (zdivs + 1) * sizeof(float), n, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0))); //1에다가 n주고
	glEnableVertexAttribArray(1);  // Vertex normal

	glBindBuffer(GL_ARRAY_BUFFER, handle[2]);
	glBufferData(GL_ARRAY_BUFFER, 2 * (xdivs + 1) * (zdivs + 1) * sizeof(float), tex, GL_STATIC_DRAW);
	glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, ((GLubyte *)NULL + (0))); //2에다가 tex주고
	glEnableVertexAttribArray(2);  // Texture coords

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, handle[3]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * xdivs * zdivs * sizeof(unsigned int), el, GL_STATIC_DRAW);

	glBindVertexArray(0);

	delete[] v;
	delete[] n;
	delete[] tex;
	delete[] el;
}

void VBOPlane::render() const {
	glBindVertexArray(vaoHandle);
	glDrawElements(GL_TRIANGLES, 6 * faces, GL_UNSIGNED_INT, ((GLubyte *)NULL + (0)));
}
