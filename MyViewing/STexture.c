#include <GL/glut.h>
#include <GL/glu.h>
#include <stdio.h>
#include <math.h>

#define	WIRE 0		
#define	SHADE 1		
#define	TEXTURE 2		

#define	POINT 0		
#define	LINEAR 1		

#define	NOMIP 1		
#define	MIPNEAREST 2		
#define	MIPLINEAR 3		

#define	CLAMP 0		
#define	REPEAT 1		

#define	DECAL 1		
#define	REPLACE 2		
#define	MODULATE 3		
#define	BLEND 4		

GLfloat light_diffuse[] = { 0.0, 1.0, 0.0, 1.0 };  /* White diffuse light */
GLfloat light_position[] = { 0.0, 0.0, 200.0, 1.0 };  /* Infinite light location */
GLfloat	light_specular[] = { 1.0, 1.0, 1.0, 1.0 }; /* Specular light */
GLfloat	light_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  /* Ambient light */

GLfloat light1_diffuse[] = { 1.0, 0.0, 0.0, 1.0 };  /* White diffuse light */
GLfloat light1_position[] = { 0.0, 0.0, 200.0, 1.0 };  /* Infinite light location */
GLfloat	light1_specular[] = { 1.0, 1.0, 1.0, 1.0 }; /* Specular light */
GLfloat	light1_ambient[] = { 0.3f, 0.3f, 0.3f, 1.0f };  /* Ambient light */

GLfloat mKa[4] = { 0.11f, 0.06f, 0.11f, 1.0f }; /* Object : Ambient */
GLfloat mKd[4] = { 0.0f, 0.0f, 1.0f, 1.0f }; /* Object : Diffuse */
GLfloat mKs[4] = { 0.33f, 0.33f, 0.52f, 1.0f }; /* Object : Specular */
GLfloat mKe[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; /* Object : Emission */
GLfloat shininess[1] = { 50 }; /* Object : Shininess */


typedef struct {
	float x;
	float y;
	float z;
} Point;

typedef struct {
	unsigned long ip[3];
} Face;

int pnum;
int fnum;
Point *mpoint;
Face *mface;

GLfloat angle1 = 0;   /* in degrees */
GLfloat angle2 = 0;   /* in degrees */
GLfloat light_angle1 = 0;   /* in degrees */
GLfloat light_angle2 = 0;   /* in degrees */

GLfloat xloc = 0, yloc = 0, zloc = 0;
int moving;
int trans;
int light_moving;
int beginx, beginy;
float scalefactor = 1.0;
int scaling = 0;         
int status=0;           // WIRE or SHADE
int cull=0;             // CULLING toggle 
char *fname="cube.dat";
//char *fname = "teapot.dat";
int w,h;
int IsFull = 0;

int filter=0;
int mode=1;
int tscale=1;
int modulate=1;
int mip=1;

void DrawWire(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	glCallList(1);
	glutSwapBuffers();
}

void DrawShade(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);
	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glutSwapBuffers();
}

void DrawTexture(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_COLOR_MATERIAL);

	if (cull) glEnable(GL_CULL_FACE);
	else glDisable(GL_CULL_FACE);

	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	glCallList(1);
	glutSwapBuffers();
}

Point cnormal(Point a, Point b, Point c)
{
    Point p, q, r;
    double val;
    p.x = a.x-b.x; p.y = a.y-b.y; p.z = a.z-b.z;
    q.x = c.x-b.x; q.y = c.y-b.y; q.z = c.z-b.z;

    r.x = p.y*q.z - p.z*q.y;
    r.y = p.z*q.x - p.x*q.z;
    r.z = p.x*q.y - p.y*q.x;

    val = sqrt(r.x*r.x + r.y*r.y + r.z*r.z);
    r.x = r.x / val;
    r.y = r.y / val;
    r.z = r.z / val;
    return r;
}

void SetTextureMode()
{
	if (mode == CLAMP) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}

	if (mip == NOMIP)  {
		if (filter == POINT) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else if (mip == MIPNEAREST) {
		if (filter == POINT) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}
	else {
		if (filter == POINT) {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		}
		else {
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		}
	}

	if (modulate == DECAL) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
	}
	else if (modulate == REPLACE) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
	}
	else if (modulate == MODULATE) {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else {
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_BLEND);
	}
}

void MakeGL_Model(void)
{
	int i;
	Point norm;
	unsigned char * image = NULL;
	int iwidth, iheight, idepth;

	glPushMatrix();
	glRotatef(light_angle1, 0.0, 1.0, 0.0);
	glRotatef(light_angle2, 1.0, 0.0, 0.0);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);
	glPopMatrix();


	glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
	glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);


	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

//	SetTextureMode();

	glShadeModel(GL_SMOOTH);


	if (glIsList(1)) glDeleteLists(1, 1);
	glNewList(1, GL_COMPILE);
	glPushMatrix();

	glTranslatef(xloc, yloc, zloc);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 1.0, 0.0, 0.0);
	glScalef(scalefactor, scalefactor, scalefactor);

	image = read_bmp("check3.bmp", &iwidth, &iheight, &idepth);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    //gluBuild2DMipmaps(GL_TEXTURE_2D, 3, iwidth, iheight, GL_RGB, GL_UNSIGNED_BYTE, image);

	for (i = 0; i < fnum; i++) {
		norm = cnormal(mpoint[mface[i].ip[2]], mpoint[mface[i].ip[1]],
			mpoint[mface[i].ip[0]]);
		glBegin(GL_TRIANGLES);
		glNormal3f(norm.x, norm.y, norm.z);
		if (i % 2 == 0) {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
			glTexCoord2f(1.0*tscale, 1.0*tscale);
			glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
			glTexCoord2f(1.0*tscale, 0.0);
			glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
		}
		else {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(mpoint[mface[i].ip[0]].x, mpoint[mface[i].ip[0]].y, mpoint[mface[i].ip[0]].z);
			glTexCoord2f(1.0*tscale, 0.0);
			glVertex3f(mpoint[mface[i].ip[1]].x, mpoint[mface[i].ip[1]].y, mpoint[mface[i].ip[1]].z);
			glTexCoord2f(1.0*tscale, 1.0*tscale);
			glVertex3f(mpoint[mface[i].ip[2]].x, mpoint[mface[i].ip[2]].y, mpoint[mface[i].ip[2]].z);
		}
		glEnd();
	}
	glPopMatrix();
	glEndList();
}

void MakeGL_SimpleTextureModel(void)
{
	unsigned char * image = NULL;
	int iwidth, iheight, idepth;

	glPushMatrix();
	glRotatef(light_angle1, 0.0, 1.0, 0.0);
	glRotatef(light_angle2, 1.0, 0.0, 0.0);

	glMaterialfv(GL_FRONT, GL_AMBIENT, mKa);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, mKd);
	glMaterialfv(GL_FRONT, GL_SPECULAR, mKs);
	glMaterialfv(GL_FRONT, GL_EMISSION, mKe);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);

	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
//	glEnable(GL_LIGHT0);
//	glEnable(GL_LIGHTING);
	glPopMatrix();

	SetTextureMode();

	glShadeModel(GL_SMOOTH);

	if (glIsList(1)) glDeleteLists(1,1);			
	glNewList(1, GL_COMPILE);
	glPushMatrix();
	glTranslatef(xloc, yloc, zloc);
	glRotatef(angle1, 0.0, 1.0, 0.0);
	glRotatef(angle2, 1.0, 0.0, 0.0);
    glScalef(scalefactor, scalefactor, scalefactor);

	image =read_bmp("check3.bmp", &iwidth, &iheight, &idepth);
//	glTexImage2D(GL_TEXTURE_2D, 0, 3, iwidth, iheight,
//	 0, GL_RGB, GL_UNSIGNED_BYTE, image);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, iwidth, iheight, GL_RGB, 
		GL_UNSIGNED_BYTE, image);

	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_TRIANGLES);
		glNormal3d(0,0,-1);
		glTexCoord2d(0,0);
   		glVertex3d(-100,-100,0);  
		glTexCoord2d(1*tscale,0);
		glVertex3d(100,-100,0);
		glTexCoord2d(0,1*tscale);
		glVertex3d(-100,100,0);
	glEnd();

	glColor3f(0.0f, 1.0f, 0.0f);
	glBegin(GL_TRIANGLES);
		glNormal3d(0,0,-1);
		glTexCoord2d(1*tscale,0);
 		glVertex3d(100,-100,0);  
		glTexCoord2d(1*tscale,1*tscale);
		glVertex3d(100,100,0);
		glTexCoord2d(0,1*tscale);
		glVertex3d(-100,100,0);
	glEnd();

	glPopMatrix();
	glEndList();

}


void GLSetupRC(void) 
{
  /* Enable a single OpenGL light. */
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse); //난반사
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular); //정반사
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient); //주변광
  glLightfv(GL_LIGHT0, GL_POSITION, light_position); //조명의 위치
  glEnable(GL_LIGHT0);
  //glDisable(GL_LIGHT0);


  //glLightfv(GL_LIGHT1, GL_DIFFUSE, light1_diffuse);
  //glLightfv(GL_LIGHT1, GL_SPECULAR, light1_specular);
  //glLightfv(GL_LIGHT1, GL_AMBIENT, light1_ambient);
  //glLightfv(GL_LIGHT1, GL_POSITION, light1_position);
  //glEnable(GL_LIGHT1);
  //glDisable(GL_LIGHT1);


  glEnable(GL_LIGHTING);


  /* Use depth buffering for hidden surface elimination. */
  glEnable(GL_DEPTH_TEST);

  
 /* Setup the view */
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  
  // 1. 물체 투영 방법 변경 
  gluPerspective(  40.0, // field of view in degree 
  1.0, // aspect ratio 
  1.0, // Z near 
  2000.0); // Z far 

  // 1. 시각좌표계 변경 
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
  gluLookAt(0.0, 0.0, 500.0,  // eye is at (0,0,500) 
    0.0, 0.0, 0.0,      // center is at (0,0,0) 
    0.0, 1.0, 0.0);      // up is in positive Y direction 
}

void ReadModel()
{
	FILE *f1;
	char s[81];

	int i;

	if ((f1 = fopen(fname, "rt")) == NULL) {
		printf("No file\n");
		exit(0);
	}
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%d", &pnum);     printf("%d\n", pnum);

	mpoint = (Point*)malloc(sizeof(Point)*pnum);

	for (i = 0; i<pnum; i++){
		fscanf(f1, "%f", &mpoint[i].x);
		fscanf(f1, "%f", &mpoint[i].y);
		fscanf(f1, "%f", &mpoint[i].z);
		printf("%f %f %f\n", mpoint[i].x, mpoint[i].y, mpoint[i].z);
	}

	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%s", s);     printf("%s", s);
	fscanf(f1, "%d", &fnum);     printf("%d\n", fnum);

	mface = (Face*)malloc(sizeof(Face)*fnum);
	for (i = 0; i<fnum; i++){
		fscanf(f1, "%d", &mface[i].ip[0]);
		fscanf(f1, "%d", &mface[i].ip[1]);
		fscanf(f1, "%d", &mface[i].ip[2]);
		printf("%d %d %d\n", mface[i].ip[0], mface[i].ip[1], mface[i].ip[2]);
	}
	fclose(f1);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	MakeGL_Model();
//	MakeGL_SimpleTextureModel();
	if (status==WIRE) DrawWire();
	else if (status==SHADE) DrawShade();
	else DrawTexture();
}



void mouse(int button, int state, int x, int y)
{
  if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
	  if(glutGetModifiers()==GLUT_ACTIVE_CTRL) {
		// 물체 이동 옵션 ON
		trans=1;  
	  }
	  else {
		// 물체 회전 옵션 ON
		moving = 1;
	  }
	  // 마우스의 초기위치값
 	  beginx = x;
	  beginy = y;

  }
  if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
    // 물체 이동 및 회전 옵션 OFF 
	moving = 0;
	trans = 0;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_DOWN) {
    // 물체 크기변환 옵션 ON
	scaling = 1;
    beginx = x;
    beginy = y;
  }

  if (button == GLUT_MIDDLE_BUTTON && state == GLUT_UP) {
    // 물체 크기변환 옵션 OFF
    scaling = 0;
  }

  if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
    // 조명 회전 옵션 ON
	scaling=0;
    light_moving = 1;
    beginx = x;
    beginx = y;
  }
  if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP) {
    // 조명 회전 옵션 OFF
    light_moving = 0;
  }
}

void motion(int x, int y)
{
  if (scaling) {
    // scalefactor 를 이용한 물체의 크기변환 
	// beginx (이전 마우스의 x 위치) 와 x (현재 마우스의 x 위치)를 이용
    scalefactor = scalefactor * (1.0+ (beginx - x)*0.0001);
    glutPostRedisplay();
  }
  if (trans) {
     //1. xloc(물체의 이동)을 beginx (이전 마우스의 x 위치) 와 x (현재 마우스의 x 위치)를 이용하여 변경
	xloc = xloc + (x-beginx);
	 //2. 현재 마우스의 위치(x)를 biginx에 저장
	beginx = x;

	 //1. yloc(물체의 이동)을 beginy (이전 마우스의 y 위치) 와 y (현재 마우스의 y 위치)를 이용하여 변경
	yloc = yloc + (beginy - y);
	 //2. 현재 마우스의 위치(y)를 biginy에 저장
	beginy = y;
    glutPostRedisplay();
  }
  if (moving) {
	// 1. 물체를 Y축으로 angle1 만큼 회전
    angle1 = angle1 + (x - beginx);
	// 2. 현재 마우스의 위치(x)를 biginx에 저장
	beginx = x;

	// 1. 물체를 X축으로 angle2 만큼 회전
	angle2 = angle2 + (y - beginy);
	// 2. 현재 마우스의 위치(y)를 biginy에 저장
	beginy = y;
    glutPostRedisplay();
  }
  if (light_moving) {
	// 1. 조명을 Y축으로 light_angle1 만큼 회전
	  light_angle1 = light_angle1 + (x - beginx);
	// 2. 현재 마우스의 위치(x)를 biginx에 저장
	  beginx= x;
	// 1. 조명을 X축으로 light_angle2 만큼 회전
	  light_angle2 = light_angle2 + (y - beginy);
	// 2. 현재 마우스의 위치(y)를 biginy에 저장
	  beginy = y;
    glutPostRedisplay();
  }
}

void special(int key, int x, int y)
{

	switch (key)
	{
		case GLUT_KEY_LEFT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				// 1. xloc을 사용하여 물체를 왼쪽으로 이동
			    xloc = xloc - 10;
				printf("CTRL + Left directional key \n");
				glutPostRedisplay();
			}
			else if (glutGetModifiers()==GLUT_ACTIVE_SHIFT) 
			{
				// 1. scalefactor를 사용하여 물체를 확대
			    scalefactor = scalefactor*1.1;
				printf("Shift + Left directional key \n");
				glutPostRedisplay();
			}
			else 
			{
				// 1. angle1을 사용하여 물체를 Y축으로 왼쪽 회전
				angle1 = angle1 - 10;
				printf("Left directional key. \n\n");
				glutPostRedisplay();
			}
			break;


		case GLUT_KEY_RIGHT :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				// 1. xloc을 사용하여 물체를 오른쪽으로 이동
				xloc = xloc + 10;
				printf("CTRL + Right directional key \n");
				glutPostRedisplay();
			}
			else if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				// 1. xloc을 사용하여 물체를 오른쪽으로 이동
			    xloc = xloc + 10;
				printf("CTRL + Right directional key \n");
				glutPostRedisplay();
			}
			else if (glutGetModifiers()==GLUT_ACTIVE_SHIFT) 
			{
				// 1. scalefactor를 사용하여 물체를 축소
				scalefactor = scalefactor/1.1;
				printf("Shift + Right directional key \n");
				glutPostRedisplay();
			}
			else 
			{
				// 1. angle1을 사용하여 물체를 Y축으로 오른쪽 회전
				angle1 = angle1 + 10;
				printf("Right directional key. \n\n");
				glutPostRedisplay();
			}
			break;

		case GLUT_KEY_UP :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				// 1. yloc을 사용하여 물체를 윗쪽으로 이동
				yloc = yloc + 10;
				printf("CTRL + Up directional key \n");
				glutPostRedisplay();
			}
			else if (glutGetModifiers()==GLUT_ACTIVE_SHIFT) 
			{
				// 1. scalefactor를 사용하여 물체를 확대
				scalefactor = scalefactor*1.1;
				printf("Shift + Up directional key \n");
				glutPostRedisplay();
			}
			else 
			{
				// 1. angle2을 사용하여 물체를 X축으로 회전
				angle2 = angle2 - 10;
				printf("Up directional key. \n\n");
				glutPostRedisplay();
			}
			break;
			
		case GLUT_KEY_DOWN :
			if (glutGetModifiers()==GLUT_ACTIVE_CTRL) 
			{
				// 1. yloc을 사용하여 물체를 아랫쪽으로 이동
				yloc = yloc - 10;
				printf("CTRL + Down directional key \n");
				glutPostRedisplay();
			}
			else if (glutGetModifiers()==GLUT_ACTIVE_SHIFT) 
			{
				// 1. scalefactor를 사용하여 물체를 축소
				scalefactor = scalefactor/1.1;
				printf("Shift + Down directional key \n");
				glutPostRedisplay();
			}
			else 
			{
				// 1. angle2을 사용하여 물체를 X축으로 회전
				angle2 = angle2 + 10;
				printf("Down directional key. \n\n");
				glutPostRedisplay();
			}
		default :
			printf("Function key. \n");
			break;
	}


	display();

}

void keyboard(unsigned char key, int x, int y)
{
//	printf("key %d\n",key);
	switch (key)
	{
		case 'w':
			printf("Wireframe \n");
			status=WIRE;
			glutPostRedisplay();
			break;
		case 's':
			printf("Shading \n");
			status=SHADE;
		    glutPostRedisplay();
			break;
		case 't':
			printf("Texture Mapping \n");
			status=TEXTURE;
		    glutPostRedisplay();
			break;
		case 'c':
			if (cull) {
				cull=0;
				printf("Culling Off\n");
			}
			else {
				cull=1;
				printf("Culling On\n");
			}
		    glutPostRedisplay();
			break;
		case 'f':
			if (!IsFull) {
				glutFullScreen();
				printf("FullScreen\n");
				IsFull = 1;
			}
			else {
				glutPositionWindow(100, 100);
				glutReshapeWindow(400, 400);
				printf("Window\n");
				IsFull = 0;
			}
			break;
		case 'p':
			if (filter) {
				filter=POINT;
				printf("Point Samping\n");
			}
			else {
				filter=LINEAR;
				printf("Linear Samping\n");
			}
		    glutPostRedisplay();
			break;
		case 'r':
			if (mode) {
				mode=CLAMP;
				printf("Texture Clamp\n");
			}
			else {
				mode=REPEAT;
				printf("Texture Repeat\n");
			}
		    glutPostRedisplay();
			break;
		case '1':
			fname = "cube.dat";
			ReadModel();
			glutPostRedisplay();
			break;
		case '2':
			fname = "sphere.dat";
			ReadModel();
			glutPostRedisplay();
			break;
		case '3':
			fname = "teapot.dat";
			ReadModel();
			glutPostRedisplay();
			break;
		case '4':
			fname = "mymodel.dat";
			ReadModel();
			glutPostRedisplay();

		case '7':
			modulate=DECAL;
			printf("Texture Mapping: Decal\n");
		    glutPostRedisplay();
			break;
		case '8':
			modulate=REPLACE;
			printf("Texture Mapping: Replace\n");
		    glutPostRedisplay();
			break;
		case '9':
			modulate=MODULATE;
			printf("Texture Mapping: Modulate\n");
		    glutPostRedisplay();
			break;
		case '0':
			modulate=BLEND;
			printf("Texture Mapping: Blend\n");
		    glutPostRedisplay();
			break;
		case ',':
			mip=NOMIP;
			printf("No Mipmapping\n");
		    glutPostRedisplay();
			break;
		case '.':
			mip=MIPNEAREST;
			printf("Mipmapping: Nearest\n");
		    glutPostRedisplay();
			break;
		case '/':
			mip=MIPLINEAR;
			printf("Mipmapping: Linear\n");
		    glutPostRedisplay();
			break;

		case '+':
			tscale++;
			printf("Texture Scale Up\n");
		    glutPostRedisplay();
			break;
		case '-':
			if (tscale<1) tscale=1;
			else tscale--;
			printf("Texture Scale Down\n");
		    glutPostRedisplay();
			break;
		case 'h':
			printf("Help : Mouse/Key Button Usage\n");
			printf("Left/Right/Up/Down Button : Objet Rotation\n");
			printf("Ctrl + Left/Right/Up/Down Button : Object Translation\n");
			printf("Middle Button : Object Scaling\n");
			printf("w ==> Wireframe \n");
			printf("s ==> Shading \n");
			printf("t ==> Texture Mapping \n");
			printf("c ==> Culling On/Off\n");
			printf("f ==> Full Screen On/Off\n");
			printf("+ ==> Texture Scale Up\n");
			printf("- ==> Texture Scale Down\n");
			printf("p ==> Point/Linear Samping\n");
			printf("r ==> Texture Clamp/Repeat\n");
			printf("1 ==> Read Model: Cube\n");
			printf("2 ==> Read Model: Sphere\n");
			printf("3 ==> Read Model: Teapot\n");
			printf("7 ==> Texture Mapping: Decal\n");
			printf("8 ==> Texture Mapping: Replace\n");
			printf("9 ==> Texture Mapping: Modulate\n");
			printf("0 ==> Texture Mapping: Blend\n");
			printf(", ==> No Mipmapping\n");
			printf(". ==> Mipmapping: Nearest\n");
			printf("/ ==> Mipmapping: Linear\n");
		    glutPostRedisplay();
			break;
		case 27:
			exit(0);
			break;
	}
}

void Myhelp ()
{
		printf("Help : Mouse/Key Button Usage\n");
		printf("Left Button : Objet Rotation\n");
		printf("Ctrl + Left Button : Object Translation\n");
		printf("Middle Button : Object Scaling\n");
		printf("Right Button : Light Rotation\n");
		printf("Directional Key : Objet Rotation\n");
		printf("Ctrl + Directional Key : Object Translation\n");
		printf("Shift + Directional Key : Object Scaling\n");
		printf("w ==> Wireframe \n");
		printf("s ==> Shading \n");
		printf("t ==> Texture Mapping \n");
		printf("c ==> Culling On/Off\n");
		printf("f ==> Full Screen On/Off\n");
		printf("+ ==> Texture Scale Up\n");
		printf("- ==> Texture Scale Down\n");
		printf("p ==> Point/Linear Samping\n");
		printf("r ==> Texture Clamp/Repeat\n");
		printf("1 ==> Read Model: Cube\n");
		printf("2 ==> Read Model: Sphere\n");
		printf("3 ==> Read Model: Teapot\n");
		printf("4 ==> Read Model: mymodel\n");
		printf("7 ==> Texture Mapping: Decal\n");
		printf("8 ==> Texture Mapping: Replace\n");
		printf("9 ==> Texture Mapping: Modulate\n");
		printf("0 ==> Texture Mapping: Blend\n");
		printf(", ==> No Mipmapping\n");
		printf(". ==> Mipmapping: Nearest\n");
		printf("/ ==> Mipmapping: Linear\n");
}


int main(int argc, char **argv)
{
	glutInit(&argc, argv);

	w=400,h=400;
	glutInitWindowSize(400,400);
	glutInitWindowPosition(100,100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutCreateWindow("Simple Modeling");
	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutSpecialFunc(special);
	ReadModel();
	GLSetupRC();
	Myhelp();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}

