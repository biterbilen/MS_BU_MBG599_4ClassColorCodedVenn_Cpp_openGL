#include <GL/glut.h>
#include <GL/osmesa.h>
#include <cstdlib>
#include <iostream>
#include <string>
#include <map>
#include <cassert>
#include <cmath>
#include "texture.h"

using namespace std;

#define VERBOSE	0
#define ORI 	"original"
#define PRO 	"processed"
#define E	"E"
#define C	"C"
#define M	"M"
#define N	"N"
#define NORM_FACTOR 0.2
//#define FONT	GLUT_BITMAP_TIMES_ROMAN_10 //HELVETICA_10
#define FONT	GLUT_BITMAP_HELVETICA_12
#define WIDTH	512
#define HEIGHT	512

/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/
char 	*organism;
char 	*path;
int 	ids_texture[26];
float 	logNorm = 1.01;
double 	radius = 350.0;
//double 	radius = 50.0;

/* defined camera */
static GLint look = 1;
static GLfloat eye[3]   = {-500.0, -500.0, 0.0};
static GLfloat focus[3] = {0.0, 0.0, 500.0};
static GLfloat up[3]    = { 0.0, 0.0, -1.0};

// projection 
GLfloat Left = -5500; 
GLfloat Right = 5500;
GLfloat Buttom = 4000; 
GLfloat Top = -7000;
GLfloat Near = 6500; 
GLfloat Far = -3500;

/* defined colors */
GLfloat red[3]   = {0.8, 0.1, 0.0};
GLfloat green[3] = {0.0, 0.8, 0.2};
GLfloat blue[3]  = {0.2, 0.2, 1.0};
GLfloat yellow[3]= {0.8, 0.8, 0.0};
GLfloat pink[3]  = {1.0, 0.8, 0.8};
GLfloat purple[3]= {0.5, 0.2, 0.2};
GLfloat black[3] = {0.0, 0.0, 0.0};

static GLint x, y, z;
static GLint list1, list2, list3, list4;

class Coord
{
	float x;
	float y;
	float z;

	public:
	Coord(float xi=0, float yi=0):x(xi),y(yi),z(0){};
	Coord(Coord &coord) { *this = coord; };
	Coord & operator=(Coord &c); 
	void set(float xi, float yi){ x = xi; y = yi;};
	float getx() {return x;};
	float gety() {return y;};
	float getz() {return z;};
	friend ostream & operator<<(ostream &o, Coord &c); 
};

class Diagram 
{
	private:
	map<int,string> 	orderMap;
	map<string,int> 	freqMap;
	map<int,vector<int> > 	intersectMap;
	map<string,int>		diagramMap;
	void extractName(string line, string &name);
	void formId(int index, vector<string> &ids);
	void order(int index);  //simdilik silinebilir implement edilmedi

	public:
	char			labelMap[26];
	Coord			transMap[26];
	string			idMap[26];
	Color 			colorMap[26];	
	int 			stripeMap[26];
	Diagram();
	int print();
	int setFreq(char *fileName);
	int getFreq(string locClass);
	int getFreq(int locClass);
	char getLabel(int locClass);
	string getId(int i) { return idMap[i];}
	void setOrder(string s1, string s2, string s3, string s4);
	void setColor(int s1, int s2, int s3, int s4);
	string getOrder(int order);
	
} di;

GLint resolution = 10;
void myinit(void);
void render_image();
static void write_ppm(const char *, const GLubyte *, int, int);
void labelSet(string, int, int, int);

static void write_ppm(const char *filename, const GLubyte *buffer, int width, int height)
{
   const int binary = 1;
   FILE *f = fopen( filename, "w" );
   if (f) {
      cout << f << endl;
      int i, x, y;
      const GLubyte *ptr = buffer;
      if (binary) {
         fprintf(f,"P6\n");
         fprintf(f,"# ppm-file created by osdemo.c\n");
         fprintf(f,"%i %i\n", width,height);
         fprintf(f,"255\n");
         fclose(f);
         f = fopen( filename, "ab" );  /* reopen in binary append mode */
         for (y=height-1; y>=0; y--) {
            for (x=0; x<width; x++) {
               i = (y*width + x) * 4;
               fputc(ptr[i], f);   /* write red */
               fputc(ptr[i+1], f); /* write green */
               fputc(ptr[i+2], f); /* write blue */
            }
         }
      }
      else {
         /*ASCII*/
         int counter = 0;
         fprintf(f,"P3\n");
         fprintf(f,"# ascii ppm file created by osdemo.c\n");
         fprintf(f,"%i %i\n", width, height);
         fprintf(f,"255\n");
         for (y=height-1; y>=0; y--) {
            for (x=0; x<width; x++) {
               i = (y*width + x) * 4;
               fprintf(f, " %3d %3d %3d", ptr[i], ptr[i+1], ptr[i+2]);
               counter++;
               if (counter % 5 == 0)
                  fprintf(f, "\n");
            }
         }
      }
      fclose(f);
   }
   else {
	cout << "Error writing .ppm file" << endl;
   }
}

static void drawRect(GLint * rect, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2,
        GLfloat x3, GLfloat y3, GLfloat x4, GLfloat y4, GLfloat *color)
{
        glLineWidth(1);
        *rect = glGenLists(1);
        glNewList(*rect, GL_COMPILE);
                glBegin(GL_LINE_LOOP);
                glColor3fv(color);
                        glVertex3f(x1,y1,0);
                        glVertex3f(x2,y2,0);
                        glVertex3f(x3,y3,0);
                        glVertex3f(x4,y4,0);
                glEnd();
        glEndList();
}

static void drawLine(GLint * line, GLfloat start_x, GLfloat start_y, GLfloat end_x, GLfloat end_y,
        GLfloat end_z, GLfloat *color)
{
        *line = glGenLists(1);
        glNewList(*line, GL_COMPILE);
                glBegin(GL_LINES);
                	glColor3fv(color);
                        glVertex3f(start_x,start_y,0);
                        glVertex3f(end_x,end_y,end_z);
                glEnd();
        glEndList();
}

void myinit(void)
{
	int i;

        glClearColor( 1.0, 1.0, 1.0, 1.0 ); // This clear the background color to white 
    	//glShadeModel(GL_SMOOTH); // Type of shading for the polygons
        
	glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
    	glEnable(GL_DEPTH_TEST); // We enable the depth test (also called z buffer)
    	glEnable(GL_TEXTURE_2D); // This Enable the Texture mapping
        
	//drawLine(&x, 0, 0, 3000.0, 0.0, 0.0, purple);
        //drawLine(&y, 0, 0, 0.0, 5000.0, 0.0, purple);
        //drawLine(&z, 0, 0, 0.0, 0.0, 5000.0, purple);
        glColor3fv (black); //TODO write the organism name in black at the top of the page
        labelSet("O", -900, -5800, 2400);
	drawRect(&list1,-4000, -4000, 4000, -4000, 4000, 0, -4000, 0,red);
	drawRect(&list2,-4000, -2000, 4000,-2000, 4000,2000,-4000,2000,green);
	drawRect(&list3,-4000, -4000, 0, -4000, 0, 4000, -4000, 4000,blue);
	drawRect(&list4,-2000, -4000, 2000,-4000, 2000,4000,-2000,4000,yellow);
    	for (i=0; i<26; i++)
    	{
        	// The Function loadBitmap() return the current texture ID
        	ids_texture[i]=loadBitmap(di.colorMap[i],di.stripeMap[i]);
    	}


}

float normalize(int freq) 
{
	float val;
	if (logNorm > 0.9) // normalized 
	{
		val = (float) freq * NORM_FACTOR;
	}
	else //log normalized
	{
		val = sqrt((float)freq) * NORM_FACTOR;
		//val = log2((float) freq); 
		//val = log2((float) freq) / NORM_FACTOR; 
		//val = log((float) freq) / NORM_FACTOR; 
		//val = log10((float) freq) / NORM_FACTOR; 
	}
	return val;	
}

void setXYZ(float &x, float &y, float &z, char type)
{
	switch(type)
	{
		case('T'):
			x += 400; 
			z += 500;  	
			break;
		case('B'):
			x += 400; 
			y += 050; 
			z = -300;  	
			break;
		case('R'):
			x += -650; 
			y += -150; 
			z /= 2;
			z += -500;  	
			break;
		case('L'):
			x += 1500; 
			y += 300; 
			z /= 2;
			z += 850;  	
			break;
		default:
			break;
	}
}

void labelColumn(int freq, float x, float y, char type)
{
	int rem;
	char *s = new char[9];
	sprintf(s, "%d",freq);
	float z = normalize(freq); 
	setXYZ(x,y,z,type);
	glRasterPos3f(x,y,z);
	for(;*s;s++)
      		glutBitmapCharacter(FONT,*s);
}

void labelSet(string name, int x, int y, int z)
{
	char *s = new char[13];
	if (name == "E3")
		sprintf(s,"ER Targeted");
	else if (name == "C3")
		sprintf(s,"Cytosolic");
	else if (name == "N3")
		sprintf(s,"Nuclear");
	else if (name == "M3")
		sprintf(s,"Mitochondrial");
	else if (name == "O")
		sprintf(s, organism);
	else
		sprintf(s, "error");
	int i;
	glRasterPos3i(x,y,z);
	for(;*s;s++)
      		glutBitmapCharacter(FONT,*s);
}

void renderColumn(int i)
{
	GLint slices, stacks;
	GLUquadric *quads;
	GLUquadric *tdisks;
	GLUquadric *bdisks;

	slices = stacks = resolution;

	glBindTexture(GL_TEXTURE_2D, ids_texture[i]);
	quads=gluNewQuadric();
	gluQuadricNormals(quads, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
	gluQuadricTexture(quads, GL_TRUE);		// Create Texture Coords ( NEW )
	bdisks=gluNewQuadric();
	gluQuadricNormals(bdisks, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
	gluQuadricTexture(bdisks, GL_TRUE);		// Create Texture Coords ( NEW )
	glPushMatrix();
	if (logNorm > 0.9) // normalized 
	{
		tdisks=gluNewQuadric();
		gluQuadricNormals(tdisks, GLU_SMOOTH);	// Create Smooth Normals ( NEW )
		gluQuadricTexture(tdisks, GL_TRUE);		// Create Texture Coords ( NEW )
	}
	glTranslatef(di.transMap[i].getx()*logNorm, di.transMap[i].gety()*logNorm, di.transMap[i].getz()*logNorm);
	if (logNorm > 0.9) // normalized 
	{
		gluCylinder( quads, radius*logNorm, radius*logNorm, normalize(di.getFreq(i)), slices, stacks ); 
	}
	else
	{	
		//glutSolidTeapot(4.0);
		glutSolidCone( radius*logNorm, normalize(di.getFreq(i)), slices, stacks ); 
	}
	gluDisk( bdisks, 0.0, radius*logNorm, slices, stacks ); 
	//cout << di.getId(i)<<":"<<di.getFreq(i)<<endl;
	if (logNorm > 0.9) // normalized 
	{
		glPushMatrix();
		glTranslatef(0.0,0.0, normalize(di.getFreq(i)));
		gluDisk( tdisks, 0.0, radius*logNorm, slices, stacks ); 
		glPopMatrix();
	}
	glPopMatrix();
	
}


void render_image() {
        int i;

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); /* clear window and z-buffer */

        glMatrixMode(GL_PROJECTION);

        glLoadIdentity();
        glOrtho(Left*logNorm, Right*logNorm, Buttom*logNorm, Top*logNorm, Near*logNorm, Far*logNorm);
        glMatrixMode(GL_MODELVIEW);
        glLoadIdentity();
        if (look)
                gluLookAt( eye[0]*logNorm, eye[1]*logNorm, eye[2]*logNorm,
                        focus[0]*logNorm, focus[1]*logNorm, focus[2]*logNorm,
                        up[0], up[1], up[2]);

        glCallList(x);
        glCallList(y);
        glCallList(z);

        int tx = -2500, ty = -7000, tz = 2400;
        int setIndex = 1;
        for(i=0; i<26; i++)
        {
                renderColumn(i);
                labelColumn(di.getFreq(i), di.transMap[i].getx()*logNorm, di.transMap[i].gety()*logNorm, di.getLabel(i));
                if (i == 7 || i == 20 | i == 21 || i == 25)
                {
                        labelSet(di.idMap[i], tx, ty, tz);
                        setIndex++;
                        tx += 400;
                        ty += 400;
                        if (i == 7 )
                                glCallList(list1);
                        else if (i == 20)
                                glCallList(list2);
                        else if (i == 21)
                                glCallList(list3);
                        else if (i == 25)
                                glCallList(list4);
                }
        }

	/* This is very important!!!
	* Make sure buffered commands are finished!!!
	*/
	glFinish();
}

int main(int argc, char** argv)
{
	void *buffer;

	//if file name is not specified; exit
	assert(argv[1]);
	organism = argv[1];

	//if path is not specified; exit
	assert(argv[2]);
	path = argv[2];

	if (VERBOSE) cout << "organism:\t" << argv[1] <<endl;

	//set the parameters
	char *filename = NULL;
	/* Allocate filename buffer */
	filename = (char *) malloc( 100 * sizeof(char) );
	if (!filename) {
		cout << "Alloc filename buffer failed!" << endl;
		return 0;
	}
	strcpy(filename, argv[2]);
	strcat(filename, "/input/\0");
	strcat(filename, argv[1]);

	di.setFreq(filename);
	di.setOrder(E,C,M,N);  //bu sira sabit kalacak P2SL prediction.txt dosyasindaki siranin aynisi
	di.setColor(YELLOW,BLUE,GREEN,RED);

	if (VERBOSE) di.print();

	/* Create an RGBA-mode context */
#if OSMESA_MAJOR_VERSION * 100 + OSMESA_MINOR_VERSION >= 305
	/* specify Z, stencil, accum sizes */
	OSMesaContext ctx = OSMesaCreateContextExt( OSMESA_RGBA, 16, 0, 0, NULL );
#else
	OSMesaContext ctx = OSMesaCreateContext( OSMESA_RGBA, NULL );
#endif
	if (!ctx) {
		cout << "OSMesaCreateContext failed!" << endl;
		return 0;
	}

	/* Allocate the image buffer */
	buffer = malloc( WIDTH * HEIGHT * 4 * sizeof(GLubyte) );
	if (!buffer) {
		cout << "Alloc image buffer failed!" << endl;
		return 0;
	}

	/* Bind the buffer to the context and make it current */
	if (!OSMesaMakeCurrent( ctx, buffer, GL_UNSIGNED_BYTE, WIDTH, HEIGHT )) {
		cout << "OSMesaMakeCurrent failed!" << endl;
		return 0;
	}
	{
		int z, s, a;
		glGetIntegerv(GL_DEPTH_BITS, &z);
		glGetIntegerv(GL_STENCIL_BITS, &s);
		glGetIntegerv(GL_ACCUM_RED_BITS, &a);
		printf("Depth=%d Stencil=%d Accum=%d\n", z, s, a);
	}

	myinit();

	render_image();

	strcpy(filename, argv[2]);
	strcat(filename, "/output/ppm/\0");
	strcat(filename, argv[1]);
	strcat(filename, ".ppm\0");
	write_ppm(filename, (GLubyte*)buffer, WIDTH, HEIGHT);

	cout << "all done" << endl;

	/* free the image buffer */
	free( buffer );

	/* free the filename buffer */
	free( filename );

	/* destroy the context */
	OSMesaDestroyContext( ctx );

	return 0;
}

Diagram::Diagram()
{
	transMap[0].set(-3500, -3000);

	transMap[1].set(-3000, -3500);

	transMap[2].set(-2500, -2500);
	
	transMap[3].set(-1000, -3000);
	
	transMap[4].set(500, -3000);

	transMap[5].set(1000, -3500);

	transMap[6].set(1500, -2500);

	transMap[7].set(3000, -3000);
	
	transMap[8].set(-3000, -1000);
	
	transMap[9].set(1000, -1000);
	
	transMap[10].set(2500, -1000);
	
	transMap[11].set(3000, -1500);
	
	transMap[12].set(3500, -500);
	
	transMap[13].set(-3500, 1000);
	
	transMap[14].set(-3000, 500);
	
	transMap[15].set(-2500, 1500);
	
	transMap[16].set(-1000, 1000);
	
	transMap[17].set(500, 1000);
	
	transMap[18].set(1000, 500);
	
	transMap[19].set(1500, 1500);
	
	transMap[20].set(3000, 1000);
	
	transMap[21].set(-3000, 3000);
	
	transMap[22].set(-1500, 3000);
	
	transMap[23].set(-1000, 2500);
	
	transMap[24].set(-500, 3500);
	
	transMap[25].set(1000, 3000);

	labelMap[0] = 'T';	
	labelMap[1] = 'T';	
	labelMap[2] = 'B';	
	labelMap[3] = 'T';	
	labelMap[4] = 'T';	
	labelMap[5] = 'T';	
	labelMap[6] = 'R';	
	labelMap[7] = 'T';	
	labelMap[8] = 'T';	
	labelMap[9] = 'T';	
	labelMap[10] = 'R';	
	labelMap[11] = 'L';	
	labelMap[12] = 'B';	
	labelMap[13] = 'T';	
	labelMap[14] = 'T';	
	labelMap[15] = 'R';	
	labelMap[16] = 'L';	
	labelMap[17] = 'T';	
	labelMap[18] = 'T';	
	labelMap[19] = 'B';	
	labelMap[20] = 'B';	
	labelMap[21] = 'T';	
	labelMap[22] = 'R';	
	labelMap[23] = 'L';	
	labelMap[24] = 'B';	
	labelMap[25] = 'B';	

	intersectMap[0].push_back(1);
	intersectMap[0].push_back(4);

	intersectMap[1].push_back(1);
	intersectMap[1].push_back(3);
	intersectMap[1].push_back(4);

	intersectMap[2].push_back(1);
	intersectMap[2].push_back(3);

	intersectMap[3].push_back(1);

	intersectMap[4].push_back(1);
	intersectMap[4].push_back(2);
	intersectMap[4].push_back(4);

	intersectMap[6].push_back(1);
	intersectMap[6].push_back(2);
	intersectMap[6].push_back(3);

	intersectMap[7].push_back(1);
	intersectMap[7].push_back(2);

	intersectMap[8].push_back(2);
	intersectMap[8].push_back(4);

	intersectMap[9].push_back(2);
	intersectMap[9].push_back(3);
	intersectMap[9].push_back(4);

	intersectMap[10].push_back(2);
	intersectMap[10].push_back(3);

	intersectMap[11].push_back(2);

	intersectMap[12].push_back(4);

	intersectMap[13].push_back(3);
	intersectMap[13].push_back(4);

	intersectMap[14].push_back(3);

}

string 
Diagram::getOrder(int order)
{
	return orderMap[order];	
}

void 
Diagram::setColor(int c1, int c2, int c3, int c4)
{
	int i,j;
	int len;
	int k;
	vector<int> cMap;
	cMap.push_back(0);  //one dummy variable since intersectMap starts from index 1
	cMap.push_back(c1);
	cMap.push_back(c2);
	cMap.push_back(c3);
	cMap.push_back(c4);

	k = 0;
	for(i=0; i<15; i++)
	{
		if (i == 5) continue;
		len = intersectMap[i].size();
		if (len == 1)
		{
			colorMap[k].set(len,cMap[intersectMap[i][0]]); 
			k++;
		}		
		else if (len == 2)
		{
			colorMap[k].set(len,cMap[intersectMap[i][0]],cMap[intersectMap[i][1]]); 
			k++;
			colorMap[k].set(len,cMap[intersectMap[i][1]],cMap[intersectMap[i][0]]); 
			k++;
			colorMap[k].set(len,cMap[intersectMap[i][0]],cMap[intersectMap[i][1]]); 
			k++;
		}		
		else if (len == 3)
		{
			colorMap[k].set(len,cMap[intersectMap[i][0]],cMap[intersectMap[i][1]],cMap[intersectMap[i][2]]); 
			k++;
		}		
		else 
		{
			cout << "No 4 set intersections exist !!" << endl;
			exit(1);
		}		
	}
}

void 
Diagram::setOrder(string s1, string s2, string s3, string s4)
{
	orderMap[1] = s1;
	orderMap[2] = s2;
	orderMap[3] = s3;
	orderMap[4] = s4;
	
	int i,j;
	int len;
	int k = 0;	
	for(i=0; i<15; i++)
	{
		if (i == 5) continue;
		vector<string> ids;
		formId(i,ids);
		len = ids.size();
		for(j=0; j<len; j++)
		{
			stripeMap[k] = 0;
			if(j == 2) stripeMap[k] = 1;
			diagramMap[ids[j]] = freqMap[ids[j]];	
			idMap[k++] = ids[j];
		}
	}
}

//this function may need to reorder the locs assignment here
void
Diagram::formId(int index,vector<string> &ids)
{
	int len = intersectMap[index].size();
	string id;
	if(len == 1)
	{
		id = getOrder(intersectMap[index][0]) + "3";  //E3
		ids.push_back(id);
	}
	else if(len == 2)
	{
		id = getOrder(intersectMap[index][0]) + "3";  //E3C2
		id += getOrder(intersectMap[index][1]) + "2";
		ids.push_back(id);
		id = getOrder(intersectMap[index][1]) + "3";  //C3E2
		id += (getOrder(intersectMap[index][0]) + "2");
		ids.push_back(id);
		id = getOrder(intersectMap[index][0]) + "2";  //E2C2
		id += getOrder(intersectMap[index][1]) + "2";
		ids.push_back(id);
	}
	else if(len == 3)
	{
		id = getOrder(intersectMap[index][0]) + "2";  //E2C2M2
		id += getOrder(intersectMap[index][1]) + "2";
		id += getOrder(intersectMap[index][2]) + "2";
		ids.push_back(id);
	}
	else
	{
		cout << "error in formId!! len:" << len<< endl;
		exit(1);
	}
}

int 
Diagram::print()
{
	int i;
	for (i = 0; i<26; i++)
	{
		cout << i << "\t";
		cout << freqMap[idMap[i]]<< "\t";
		//cout << sqrt((float)freqMap[idMap[i]]) << "\t";
		//cout << log2((float) freqMap[idMap[i]]) << "\t";
		//cout << log((float) freqMap[idMap[i]]) << "\t";
		//cout << log10((float) freqMap[idMap[i]]) << "\t";
		cout << idMap[i] << "\t" ;	
		cout << colorMap[i] << "\t";
		cout << stripeMap[i] << endl;
	}
	return 0;
}

char
Diagram::getLabel(int index)
{
	return labelMap[index];	
}

int 
Diagram::getFreq(string locClass)
{
	return freqMap[locClass];	
}

int 
Diagram::getFreq(int order)
{
	return freqMap[idMap[order]];
}

int 
Diagram::setFreq(char * filename)
{
	ifstream in(filename);

	if (!in) {
		cout << "Can't read input file " << filename << endl;
		exit(1);
	}

	string line;	
	int freq;
	//in >> freqMap[ORI];
        //getline(in, line, '\n');

	//in >> freqMap[PRO];
       	//getline(in, line, '\n');

	string name;
	while(!in.eof())
        {
		in >> freq;
        	getline(in, line, '\n');
		
		if( line == " total" ) break;
		extractName(line,name);
		freqMap[name]= freq;
		if (VERBOSE)
		{
			cout <<name<< "\t"<< freqMap[name] <<endl;
		}
        }
	in.close();
        return 0;

}

void 
Diagram::extractName(string line, string &name)
{
	string::size_type pos1 = line.find('-', 0);
	string::size_type pos2 = line.find('.', pos1);
	string::size_type length = pos2 - pos1 - 1;

	name = line.substr(pos1+1, length);	
}


/////////////////////////////////////////
ostream & operator<<(ostream &o, Coord &c) 
{
	o<<"x:"<<c.x<<" y:"<<c.y<<" z:"<<c.z<<endl;
	return o;
}

Coord & Coord::operator=(Coord &c) 
{ 
	x=c.x; 
	y=c.y; 
	z=c.z; 
	return *this; 
} 

