#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include <math.h>
/*#include <glew.h>
#include <glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
*/
#include <gl\glew.h>
#include <gl\glut.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shader_utils.h"
#include "tdraw.h"
#include "readObj.h"
#include "readBMP.h"

#define M_PI 3.14159265358979323846264338327

GLuint program;
GLuint program1;
GLint attribute_coord3d;
GLint attribute_texcoord;
GLint attribute_coord3;
GLint attribute_normal;
GLint uniform_matrix;
GLint uniform_matrix2;
GLint uniform_proj_matrix;  // pointer to uniform variable - total matrix
GLint uniform_texture;
GLint uniform_red;
GLint uniform_green;
GLint uniform_blue;

//GLint attribute_normal;
Mesh myobj;
Mesh robj;
Mesh dobj;

bool horse = true;
bool texture = true;
bool rabbit = true;
bool dragon = true;

static GLfloat objVerts[112642*3]; //horse
static GLfloat normVerts[112642*3];
static GLuint objElements[225280*3];

static GLfloat rabbitVerts[70658*3]; //rabbit
static GLfloat rabbitNorms[70658*3];
static GLuint rabbitElements[141312*3];

static GLfloat dragVerts[50000*3]; //dragon
static GLfloat dragNorms[50000*3];
static GLuint dragElements[100000*3];

glm::mat4 totalMatrix, totalMatrixR, totalMatrixD;
glm::mat4 turtleMatrix, turtleMatrixR, turtleMatrixD;
glm::mat4 projMatrix; // Current total transform
glm::vec4 cameraPosition = glm::vec4(0.0, 0.0, -8.0, 1.0);

// move cube into box centered at (0,0,-8)
glm::mat4 view  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			     glm::vec4(0.0, 1.0, 0.0, 0.0),
			     glm::vec4(0.0, 0.0, 1.0, 0.0),
			     glm::vec4(0.0, 0.0, -8.0, 1.0));

// projection
// n = near plane = -3
// f = far plane = 21*(-3) = -63
glm::mat4 proj = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			   glm::vec4(0.0, 1.0, 0.0, 0.0),
			   glm::vec4(0.0, 0.0, -22.0/(20*3), -0.33),
			   glm::vec4(0.0, 0.0, -2.0*21/20, 0.0));

Image texImage ,texImage2, texImage3;
GLuint textureId, textureId2, textureId3;

glm::mat4 proj1 = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			   glm::vec4(0.0, 1.0, 0.0, 0.0),
			   glm::vec4(0.0, 0.0, -0.66, -0.33),
			   glm::vec4(0.0, 0.0, -3.0, 0.0));

// move object into box centered at (0,0,-7)
glm::mat4 view1  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			     glm::vec4(0.0, 1.0, 0.0, 0.0),
			     glm::vec4(0.0, 0.0, 1.0, 0.0),
			     glm::vec4(0.0, 0.0, -7.0, 1.0));

int size = 4; // number of triangles to draw in scene

GLfloat vertices[] = {
  -4, -4, 0.0, -2.0, -2.0, //wall : 0-3
  4, -4, 0.0, 2.0, -2.0, 
  4, 4, 0.0, 2.0, 2.0, 
  -4, 4, 0.0, -2.0, 2.0, 

    -20, -4, -25, -10.0, -12.5, //ground : 4-7
    20, -4, -25, 10.0, -12.5,
    20, -4, 25, 10.0, 12.5, 
    -20, -4, 25, -10.0, 12.5,

	11.0, -4.5, -26.5, -2.0, -2.0, //brickwall: 8-11 <----------------------------------------------
	22.0, -4.5, -26.5, 2.0, -2.0,
	22.0, 0.0, -26.5, 2.0, 2.0,
	11.0, 0.0, -26.5, -2.0, 2.0,

	0.0, -4.5, -26.5, -2.0, -2.0, //brickwall: 12-15 <----------------------------------------------
	11.0, -4.5, -26.5, 2.0, -2.0,
	11.0, 0.0, -26.5, 2.0, 2.0,
	0.0, 0.0, -26.5, -2.0, 2.0,

	-11.0, -4.5, -26.5, -2.0, -2.0, //brickwall:16-19 <----------------------------------------------
	0.0, -4.5, -26.5, 2.0, -2.0,
	0.0, 0.0, -26.5, 2.0, 2.0,
	-11.0, 0.0, -26.5, -2.0, 2.0,

	-22.0, -4.5, -26.5, -2.0, -2.0, //brickwall:20-23 <----------------------------------------------
	-11.0, -4.5, -26.5, 2.0, -2.0,
	-11.0, 0.0, -26.5, 2.0, 2.0,
	-22.0, 0.0, -26.5, -2.0, 2.0

    -20, -4, -25, -10.0, -12.5, //water: 24-27
    -40, -4, -25, 10.0, -12.5,
    -40, -4, 25, 10.0, 12.5, 
    -20, -4, 25, -10.0, 12.5,

	20, -4, -25, -10.0, -12.5, //Water: 28-31
    40, -4, -25, 10.0, -12.5,
    40, -4, 25, 10.0, 12.5, 
    20, -4, 25, -10.0, 12.5,

	11.0, -4.5, -26.5, -2.0, -2.0, //brickwall2: 32-35 <----------------------------------------------
	22.0, -4.5, -26.5, 2.0, -2.0,
	22.0, 0.0, -26.5, 2.0, 2.0,
	11.0, 0.0, -26.5, -2.0, 2.0,

	0.0, -4.5, -26.5, -2.0, -2.0, //brickwall2: 36-39 <----------------------------------------------
	11.0, -4.5, -26.5, 2.0, -2.0,
	11.0, 0.0, -26.5, 2.0, 2.0,
	0.0, 0.0, -26.5, -2.0, 2.0,

	-11.0, -4.5, -26.5, -2.0, -2.0, //brickwall2: 40-43 <----------------------------------------------
	0.0, -4.5, -26.5, 2.0, -2.0,
	0.0, 0.0, -26.5, 2.0, 2.0,
	-11.0, 0.0, -26.5, -2.0, 2.0,

	-22.0, -4.5, -26.5, -2.0, -2.0, //brickwall2: 44-47 <----------------------------------------------
	-11.0, -4.5, -26.5, 2.0, -2.0,
	-11.0, 0.0, -26.5, 2.0, 2.0,
	-22.0, 0.0, -26.5, -2.0, 2.0
};


  GLubyte elements[] = {
//    0, 1, 2,
//    2, 3, 0,
    4, 5, 6,
    6, 7, 4
  };

  GLubyte elements2[] = {
	  8, 9, 10,
	  10, 11, 8,
	  12, 13, 14,
	  14, 15, 12,
	  16, 17, 18,
	  18, 19, 16,
	  20, 21, 22,
	  22, 23, 20
  };

  GLubyte elements3[] = {
	  24, 25, 26,
	  26, 27, 24,
//	  28, 29, 30,
//	  30, 31, 28
  };

  GLubyte elements4[] = {
	  32, 33, 34,
	  34, 35, 32,
	  36, 37, 38,
	  38, 39, 36,
	  40, 41, 42,
	  42, 43, 40,
	  44, 45, 46,
	  46, 47, 44
  };

// global matricies
// specify by columns

// print out matrix by rows
void printMat(glm::mat4  mat){
  int i,j;
  for (j=0; j<4; j++){
    for (i=0; i<4; i++){
    printf("%f ",mat[i][j]);
  }
  printf("\n");
 }
}

void moveCamera(float move) {
  cameraPosition[2] += move;
  view[3] = cameraPosition;
}

void rotateCamera(float angle) {
	glm::mat4 motion = glm::mat4(glm::vec4(cos(angle), 0.0, sin(angle), 0.0),
			   glm::vec4(0.0, 1.0, 0.0, 0.0),
			   glm::vec4(-sin(angle), 0.0, cos(angle), 0.0),
			   glm::vec4(0.0, 0.0, 0.0, 1.0));

	view = view * motion;
}

// Set up the shader programs, compile and link them, get pointers to 
// where the shader variables are in GPU memory. 
int init_resources()
{
  // Projection matrix
  projMatrix = proj * view;
glm::mat4 trans, transR, transD;
glm::mat4 scale, scaleR, scaleD;


  // Read in texture image
  int flag = ImageLoad("grassSHRINK.bmp", &texImage);
  if (flag != 1) {
    printf("Trouble reading image grassShrink.bmp\n");
  }

  flag = ImageLoad("brick.bmp", &texImage2); //sky <--------------------------
  if (flag != 1) {
	  printf("Trouble reading image brick.bmp\n");
  }

  flag = ImageLoad("water.bmp", &texImage3); // Water <------------------------
  if (flag != 1) {
	  printf("Trouble reading image water.bmp\n");
  }

	int i;
	float maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

	load_obj("horse.obj", &myobj);
  	if (horse == true) {
		for (i = 0; i < 112642; i++) {
			glm::vec4 temp = myobj.vertices[i];
			glm::vec3 temp2 = myobj.normals[i];

			objVerts[i*3]= temp[0];
			objVerts[i*3+1] = temp[1]; 
			objVerts[i*3+2] = temp[2];
			normVerts[i*3] = temp2[0];
			normVerts[i*3+1] = temp2[1];
			normVerts[i*3+2] = temp2[2];

			if (temp[0] > maxX)
				maxX = temp[0];
			if (temp[0] < minX)
				minX = temp[0];
			if (temp[1] > maxY)
				maxY = temp[1];
			if (temp[1] < minY)
				minY = temp[1];
			if (temp[2] > maxZ)
				maxZ = temp[2];
			if (temp[2] < minZ)
				minZ = temp[2];
		
		}

		for (i=0; i < 225280*3; i++)
			objElements[i] = myobj.elements[i];

		  trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-(minX+maxX)/2.0 - 2.0, -(minY+maxY)/2.0 - 1.0, -(minZ+maxZ)/2.0, 1.0));

		  scale = glm::mat4(glm::vec4(4.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 4.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 4.0, 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		  glm::mat4 rot_y = glm::mat4(glm::vec4(cos(M_PI), 0.0, sin(M_PI), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI), 0.0, cos(M_PI), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		 glm::mat4 rot_x = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(-M_PI), sin(-M_PI), 0.0),
			       glm::vec4(0.0, -sin(-M_PI), cos(-M_PI), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

		 glm::mat4 rot_x2 = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(M_PI/10), sin(M_PI/10), 0.0),
			       glm::vec4(0.0, -sin(M_PI/10), cos(M_PI/10), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

  		  turtleMatrix = rot_x * rot_y;
		  turtleMatrix = turtleMatrix * rot_x2;
		  turtleMatrix = trans * scale * turtleMatrix;
		  totalMatrix = projMatrix * view1 * turtleMatrix;

	}

	load_obj("rabbit.obj", &robj);
  	if (rabbit == true) {
		maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

		for (i = 0; i < 70658; i++) {
			glm::vec4 temp = robj.vertices[i];
			glm::vec3 temp2 = robj.normals[i];

			rabbitVerts[i*3]= temp[0];
			rabbitVerts[i*3+1] = temp[1]; 
			rabbitVerts[i*3+2] = temp[2];
			rabbitNorms[i*3] = temp2[0];
			rabbitNorms[i*3+1] = temp2[1];
			rabbitNorms[i*3+2] = temp2[2];

			if (temp[0] > maxX)
				maxX = temp[0];
			if (temp[0] < minX)
				minX = temp[0];
			if (temp[1] > maxY)
				maxY = temp[1];
			if (temp[1] < minY)
				minY = temp[1];
			if (temp[2] > maxZ)
				maxZ = temp[2];
			if (temp[2] < minZ)
				minZ = temp[2];
		
		}

		for (i=0; i < 141312*3; i++)
			rabbitElements[i] = robj.elements[i];

		  transR = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-(minX+maxX)/2.0 + 8.2, -(minY+maxY)/2.0 - 3.0, -(minZ+maxZ)/2.0, 1.0));

		  scaleR = glm::mat4(glm::vec4(2.0, 0.0, 0.0, 0.0), //0.2
					glm::vec4(0.0, 2.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 2.0, 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		  glm::mat4 rot_y = glm::mat4(glm::vec4(cos(M_PI), 0.0, sin(M_PI), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI), 0.0, cos(M_PI), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		 glm::mat4 rot_x = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(-5*M_PI/4), sin(-5*M_PI/4), 0.0),
			       glm::vec4(0.0, -sin(-5*M_PI/4), cos(-5*M_PI/4), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

		 glm::mat4 rot_x2 = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(M_PI/10), sin(M_PI/10), 0.0),
			       glm::vec4(0.0, -sin(M_PI/10), cos(M_PI/10), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

  		  turtleMatrixR = rot_x * rot_y;
		  turtleMatrixR = turtleMatrixR * rot_x2;
		  turtleMatrixR = transR * scaleR * turtleMatrixR;
		  totalMatrixR = projMatrix * view1 * turtleMatrixR;

	}

	load_obj("dragon.obj", &dobj);
  	if (dragon == true) {
		maxX = 0, maxY = 0, maxZ = 0, minX = 0, minY = 0, minZ = 0;

		for (i = 0; i < 50000; i++) {
			glm::vec4 temp = dobj.vertices[i];
			glm::vec3 temp2 = dobj.normals[i];

			dragVerts[i*3]= temp[0];
			dragVerts[i*3+1] = temp[1]; 
			dragVerts[i*3+2] = temp[2];
			dragNorms[i*3] = temp2[0];
			dragNorms[i*3+1] = temp2[1];
			dragNorms[i*3+2] = temp2[2];

			if (temp[0] > maxX)
				maxX = temp[0];
			if (temp[0] < minX)
				minX = temp[0];
			if (temp[1] > maxY)
				maxY = temp[1];
			if (temp[1] < minY)
				minY = temp[1];
			if (temp[2] > maxZ)
				maxZ = temp[2];
			if (temp[2] < minZ)
				minZ = temp[2];
		
		}

		for (i=0; i < 100000*3; i++)
			dragElements[i] = dobj.elements[i];

		  transD = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-(minX+maxX)/2.0 - 6.0, -(minY+maxY)/2.0 + 7.0, -(minZ+maxZ)/2.0, 1.0));

		  scaleD = glm::mat4(glm::vec4(3.0, 0.0, 0.0, 0.0), //0.2
					glm::vec4(0.0, 3.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 3.0, 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		  glm::mat4 rot_y = glm::mat4(glm::vec4(cos(M_PI/2), 0.0, sin(M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), 0.0, cos(M_PI/2), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		 glm::mat4 rot_x = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(M_PI/15), sin(M_PI/15), 0.0),
			       glm::vec4(0.0, -sin(M_PI/15), cos(M_PI/15), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

		 glm::mat4 rot_x2 = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
			       glm::vec4(0.0, cos(13.5*M_PI/7), sin(13.5*M_PI/7), 0.0),
			       glm::vec4(0.0, -sin(13.5*M_PI/7), cos(13.5*M_PI/7), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

  		  turtleMatrixD = rot_x * rot_y;
		  turtleMatrixD = turtleMatrixD * rot_x2;
		  turtleMatrixD = transD * scaleD * turtleMatrixD;
		  totalMatrixD = projMatrix * view1 * turtleMatrixD;

	}

	if (texture == true){
		glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
		glGenTextures(1,&textureId); // Make a texture object
		glBindTexture(GL_TEXTURE_2D, textureId); // Use this object as the 
		// current 2D texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the image into GPU texture memory
		glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
	       128, // size in s coord
	       128, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage.data // pointer to the data
	       ); 

	// Error flag is initially false */
		GLint link_ok = GL_FALSE;
	  // Indices for vertex and fragment shaders
		GLuint vs, fs;
		vs = create_shader("maze.v.glsl", GL_VERTEX_SHADER);
			if (vs == 0) return 0;
		fs = create_shader("maze.f.glsl", GL_FRAGMENT_SHADER);
			if (fs == 0) return 0;
	  // The GPU program contains both the vertex and shader programs. 
	program = glCreateProgram();
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	// Link them together.
	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &link_ok);
	if (!link_ok) {
		fprintf(stderr, "glLinkProgram: failed\n");
		return 0;
	}
  
	attribute_coord3d = glGetAttribLocation(program, "coord3d");
	if (attribute_coord3d == -1) {
		fprintf(stderr, "Could not bind attribute coord3d\n");
		return 0;
	}
	// Ask for the index of shader variable texcoord
	attribute_texcoord = glGetAttribLocation(program, "texcoord");
	if (attribute_texcoord == -1) {
		fprintf(stderr, "Could not bind attribute texcoord\n");
		return 0;
	  }
  
	uniform_texture = glGetUniformLocation(program, "u_texture");
	if (uniform_texture == -1) {
		fprintf(stderr, "Could not bind uniform u_texture \n");
		return 0;
	}
  
	// Get linker's index to the uniform variable
	uniform_proj_matrix = glGetUniformLocation(program, "m_proj");
	if (uniform_proj_matrix == -1) {
		fprintf(stderr, "Could not bind uniform variable m_proj \n");
		return 0;
	}

  }
	if (horse == true || rabbit == true){
		GLint link_ok = GL_FALSE;
		GLuint vs1, fs1;
		vs1 = create_shader("glassball.v.glsl", GL_VERTEX_SHADER);
		if (vs1 == 0) return 0;
		fs1 = create_shader("glassball.f.glsl", GL_FRAGMENT_SHADER);
		if (fs1 == 0) return 0;

		program1 = glCreateProgram();
		glAttachShader(program1, vs1);
		glAttachShader(program1, fs1);
		glLinkProgram(program1);
		glGetProgramiv(program1, GL_LINK_STATUS, &link_ok);
		if (!link_ok) {
			fprintf(stderr, "glLinkProgram: failed\n");
			return 0;
		}
  
		attribute_coord3 = glGetAttribLocation(program1, "coord3d");
		if (attribute_coord3 == -1) {
			fprintf(stderr, "Could not bind attribute coord3d\n");
			return 0;
		}

		attribute_normal = glGetAttribLocation(program1, "n_coord");
		if (attribute_normal == -1) {
			fprintf(stderr, "Could not bind attribute n_coord\n");
			return 0;
		} 

		 uniform_matrix = glGetUniformLocation(program1, "m_transform");
		if (uniform_matrix == -1) {
			fprintf(stderr, "Could not bind uniform variable m_transform \n");
			return 0;
		}
  
		uniform_matrix2 = glGetUniformLocation(program1, "n_transform");
		if (uniform_matrix2 == -1) {
			fprintf(stderr, "Could not bind uniform variable n_tranform \n");
			return 0;
		}
		
		uniform_red = glGetUniformLocation(program1, "red");
		if (uniform_red == -1) {
			fprintf(stderr, "Cound not bind uniform variable red \n");
			return 0;
		}

		uniform_green = glGetUniformLocation(program1, "green");
		if (uniform_green == -1) {
			fprintf(stderr, "Cound not bind uniform variable green \n");
			return 0;
		}

		uniform_blue = glGetUniformLocation(program1, "blue");
		if (uniform_blue == -1) {
			fprintf(stderr, "Cound not bind uniform variable blue \n");
			return 0;
		}
	}
  // If all went well....
  return 1;
}

// Draw the floor and wall
void drawScene(void) {

	if (texture == true){
		// Send the program to the GPU
		glUseProgram(program);
		// Projection matrix
		projMatrix = proj * view;

		//first texture
		glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0 <-------------------------
		glGenTextures(1,&textureId); // Make a texture object
		glBindTexture(GL_TEXTURE_2D, textureId); // Use this object as the 
		// current 2D texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the image into GPU texture memory
		glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
	       128, // size in s coord
	       128, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage.data // pointer to the data
	       ); 

		// Now hook up input data to program.
	  // Two attributes for the vertex, position and texture coordinate
	  // Let OpenGL know we'll use both of them. 
		glEnableVertexAttribArray(attribute_coord3d);
		glEnableVertexAttribArray(attribute_texcoord);
		// Only attribute for the vertex is position. 
		    // Describe the position attribute and where the data is in the array
		 glVertexAttribPointer(
			attribute_coord3d, // attribute ID
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			5*sizeof(float),  // stride between one position and the next
			vertices  // pointer to first position in the C array
		);
		glVertexAttribPointer(
			attribute_texcoord, // attribute
			2,                  // number of elements per vertex, (s,t)
			GL_FLOAT,           // the type of each element
			GL_FALSE,           // take our values as-is
			5*sizeof(float),    // stride to next texture element
			vertices+3              // offset of first element
		);

		// Send GPU projection matrix
		glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));

		// Tell GPU to use Texture Unit 0
		glUniform1i(uniform_texture, 0);  

		// draw the wall....
		glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_BYTE, elements);


		//Next texture - Wall 
		glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
		glGenTextures(1,&textureId2); // Make a texture object
		glBindTexture(GL_TEXTURE_2D, textureId2); // Use this object as the 
		// current 2D texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the image into GPU texture memory
		glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
	       128, // size in s coord
	       128, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage2.data // pointer to the data
	       ); 

		glDrawElements(GL_TRIANGLES, 8*3, GL_UNSIGNED_BYTE, elements2); 

		glm::mat4 rot_y = glm::mat4(glm::vec4(cos(M_PI/2), 0.0, sin(M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), 0.0, cos(M_PI/2), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		glm::mat4 scale = glm::mat4(glm::vec4(1.3, 0.0, 0.0, 0.0), //0.2
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		glm::mat4 view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(-8.0, 0.0, 0.0, 1.0));

		projMatrix = projMatrix * view2;

		glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * rot_y * scale));
		glDrawElements(GL_TRIANGLES, 8*3, GL_UNSIGNED_BYTE, elements2); // <------------------- walls

		projMatrix = proj * view;
		
		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(8.0, 0.0, 0.0, 1.0));
		
		projMatrix = projMatrix * view2; 

		rot_y = glm::mat4(glm::vec4(cos(-M_PI/2), 0.0, sin(-M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(-M_PI/2), 0.0, cos(-M_PI/2), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));

		glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * rot_y * scale));
		glDrawElements(GL_TRIANGLES, 8*3, GL_UNSIGNED_BYTE, elements2);

		/*
		// Next Texture - WATER <---------------------------------------------------------
		glActiveTexture(GL_TEXTURE0); // Load texture into GPU texture unit 0
		glGenTextures(1,&textureId3); // Make a texture object
		glBindTexture(GL_TEXTURE_2D, textureId3); // Use this object as the 
		// current 2D texture
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Load the image into GPU texture memory
		glTexImage2D(GL_TEXTURE_2D, // the current 2D texture
	       0, // Mipmap level
	       GL_RGB, //internal format for texture
	       128, // size in s coord
	       128, // size in t coord
	       0, // should always be 0
	       GL_RGB, // incoming data format; should match internal
	       GL_UNSIGNED_BYTE, // type of incoming data
	       texImage3.data // pointer to the data
	       ); 

		 glm::mat4 rot = glm::mat4(glm::vec4(cos(M_PI/2), 0.0, sin(M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), 0.0, cos(M_PI/2), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		glm::mat4 rot2 = glm::mat4(glm::vec4(cos(M_PI/2), sin(M_PI/2), 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), cos(M_PI/2), 0.0, 0.0),
			       glm::vec4(0.0, 0.0, 1.0, 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		glm::mat4 trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-10.0, 0.0, 0.0, 1.0));
		
		glUniformMatrix4fv(uniform_proj_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * rot * rot2 * trans));
//		glDrawElements(GL_TRIANGLES, 4*3, GL_UNSIGNED_BYTE, elements3); */

		glDisableVertexAttribArray(attribute_coord3d);
		glDisableVertexAttribArray(attribute_texcoord);
	}


	if (horse == true){
		projMatrix = proj * view;
		glUseProgram(program1);
		glEnableVertexAttribArray(attribute_coord3);
		// Add attribute normal for the vertex.
		glEnableVertexAttribArray(attribute_normal);
		glVertexAttribPointer(
			attribute_coord3, // attribute ID
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			objVerts  // pointer to first position in the C array
		);
 
		 glVertexAttribPointer(
			attribute_normal, // attribute ID
			3,                 // number of elements per vertex, here (nx,ny,nz)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			normVerts  // pointer to first normal in the C array
		); 

		glUniform1f(uniform_red, 3.5*0.15);
		glUniform1f(uniform_green, 2.0*0.15);
		glUniform1f(uniform_blue, 1.5*0.15);

		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * turtleMatrix));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrix));
		glDrawElements(GL_TRIANGLES, 225280*3, GL_UNSIGNED_INT, objElements);

		glm::mat4 trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), //2nd horse
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(0.0, 0.0, -5.0, 1.0));

		glm::mat4 rotate = glm::mat4(glm::vec4(cos(M_PI/2), 0.0, sin(M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), 0.0, cos(M_PI/2), 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		glm::mat4 view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, 20.0, 1.0));

		projMatrix = projMatrix * view2; 
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * rotate * trans * turtleMatrix));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(rotate * turtleMatrix));
		glDrawElements(GL_TRIANGLES, 225280*3, GL_UNSIGNED_INT, objElements);

		trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0), //3rd horse
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(0.0, 0.0, -5.0, 1.0));

		rotate = glm::mat4(glm::vec4(cos(-M_PI/2), 0.0, sin(-M_PI/2), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(-M_PI/2), 0.0, cos(-M_PI/2), 0.0),
					glm::vec4(0.0, 0.0, 0.0, 1.0));

		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, -13.0, 1.0));

		projMatrix = projMatrix * view2; 
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * rotate * trans * turtleMatrix));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(rotate * turtleMatrix));
		glDrawElements(GL_TRIANGLES, 225280*3, GL_UNSIGNED_INT, objElements);

		glDisableVertexAttribArray(attribute_coord3);
		glDisableVertexAttribArray(attribute_normal);

	}

	if (rabbit == true){
		projMatrix = proj * view;
		glUseProgram(program1);
		glEnableVertexAttribArray(attribute_coord3);
		// Add attribute normal for the vertex.
		glEnableVertexAttribArray(attribute_normal);
		glVertexAttribPointer(
			attribute_coord3, // attribute ID
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			rabbitVerts  // pointer to first position in the C array
		);
 
		 glVertexAttribPointer(
			attribute_normal, // attribute ID
			3,                 // number of elements per vertex, here (nx,ny,nz)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			rabbitNorms  // pointer to first normal in the C array
		); 

		glm::mat4 view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, -20.0, 1.0));

		projMatrix = projMatrix * view2;
		glUniform1f(uniform_red, 1.0*0.7);
		glUniform1f(uniform_green, 1.0*0.7);
		glUniform1f(uniform_blue, 1.0*0.7);
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * turtleMatrixR));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixR));
		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, rabbitElements);

/*		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(1.0, 1.0, 2.0, 1.0));

		projMatrix = proj * view2;

		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, objElements); //draws moon */

		projMatrix = proj * view; //2nd rabbit
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));

		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, 17.0, 1.0));
		
		glm::mat4 trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-10.0, 0.0, 0.0, 1.0));
		
		projMatrix = projMatrix * view2;

		glUniform1f(uniform_red, 1.0*0.7);
		glUniform1f(uniform_green, 1.0*0.7);
		glUniform1f(uniform_blue, 1.0*0.7);
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * trans * turtleMatrixR));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixR));
		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, rabbitElements);

		projMatrix = proj * view; //3rd rabbit
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));

		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, -10.0, 1.0));
		
		trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(-5.0, 0.0, 0.0, 1.0));

		glm::mat4 rotate = glm::mat4(glm::vec4(cos(M_PI/2), 0.0, sin(M_PI/2), 0.0), //next to horse
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/2), 0.0, cos(M_PI/2), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		
		projMatrix = projMatrix * view2;

		glUniform1f(uniform_red, 1.0*0.7);
		glUniform1f(uniform_green, 1.0*0.7);
		glUniform1f(uniform_blue, 1.0*0.7);
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * trans * rotate * turtleMatrixR));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixR));
		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, rabbitElements);

		projMatrix = proj * view; //4th rabbit
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));

		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, -15.0, 1.0));
		
		trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(7.0, 0.0, 0.0, 1.0));

		rotate = glm::mat4(glm::vec4(cos(M_PI/6), 0.0, sin(M_PI/6), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(M_PI/6), 0.0, cos(M_PI/6), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		
		projMatrix = projMatrix * view2;

		glUniform1f(uniform_red, 1.0*0.7);
		glUniform1f(uniform_green, 1.0*0.7);
		glUniform1f(uniform_blue, 1.0*0.7);
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * trans * rotate * turtleMatrixR));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixR));
		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, rabbitElements);

		projMatrix = proj * view; //5th rabbit - on top of horse
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix));

		view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, 2.5, 1.0));
		
		trans = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
					glm::vec4(0.0, 1.0, 0.0, 0.0),
					glm::vec4(0.0, 0.0, 1.0, 0.0),
					glm::vec4(5.5, 3.4, 0.0, 1.0));

		rotate = glm::mat4(glm::vec4(cos(-5*M_PI/6), 0.0, sin(-5*M_PI/6), 0.0),
			       glm::vec4(0.0, 1.0, 0.0, 0.0),
			       glm::vec4(-sin(-5*M_PI/6), 0.0, cos(-5*M_PI/6), 0.0),
			       glm::vec4(0.0, 0.0, 0.0, 1.0));
		
		projMatrix = projMatrix * view2;

		glUniform1f(uniform_red, 1.0*0.7);
		glUniform1f(uniform_green, 1.0*0.7);
		glUniform1f(uniform_blue, 1.0*0.7);
		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * trans * rotate * turtleMatrixR));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixR));
		glDrawElements(GL_TRIANGLES, 141312*3, GL_UNSIGNED_INT, rabbitElements);

		glDisableVertexAttribArray(attribute_coord3);
		glDisableVertexAttribArray(attribute_normal);

	}


	if (dragon == true){
		projMatrix = proj * view;
		glm::mat4 view2  = glm::mat4(glm::vec4(1.0, 0.0, 0.0, 0.0),
							glm::vec4(0.0, 1.0, 0.0, 0.0),
							glm::vec4(0.0, 0.0, 1.0, 0.0),
							glm::vec4(0.0, 0.0, -20.0, 1.0));

		projMatrix = projMatrix * view2;

		glUseProgram(program1);
		glEnableVertexAttribArray(attribute_coord3);
		// Add attribute normal for the vertex.
		glEnableVertexAttribArray(attribute_normal);
		glVertexAttribPointer(
			attribute_coord3, // attribute ID
			3,                 // number of elements per vertex, here (x,y,z)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			dragVerts  // pointer to first position in the C array
		);
 
		 glVertexAttribPointer(
			attribute_normal, // attribute ID
			3,                 // number of elements per vertex, here (nx,ny,nz)
			GL_FLOAT,          // the type of each element
			GL_FALSE,          // take our values as-is, don't normalize
			3*sizeof(float),  // stride between one position and the next
			dragNorms  // pointer to first normal in the C array
		); 

		glUniform1f(uniform_red, 1.5*0.20);
		glUniform1f(uniform_green, 5.0*0.20);
		glUniform1f(uniform_blue, 2.5*0.20);

		glUniformMatrix4fv(uniform_matrix, 1, GL_FALSE, glm::value_ptr(projMatrix * turtleMatrixD));
		glUniformMatrix4fv(uniform_matrix2, 1, GL_FALSE, glm::value_ptr(turtleMatrixD));
		glDrawElements(GL_TRIANGLES, 100000*3, GL_UNSIGNED_INT, dragElements);
		glDisableVertexAttribArray(attribute_coord3);
		glDisableVertexAttribArray(attribute_normal);

	}
}


void free_resources()
{
	if (texture == true){
		glDeleteProgram(program);
		glDeleteTextures(1,&textureId);
	}
	if (horse == true || rabbit == true)
		glDeleteProgram(program1);
  
}
