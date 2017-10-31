/*
 *
 * Tutorial 3: 3d engine - Texture mapping with OpenGL!
 *
 * File header: texture.h
 *
 */

/*
just run: make and you are done.
of course you may need to change the makefile
*/
#include <fstream>
#include <vector>

#define YELLOW	1
#define BLUE	2
#define GREEN	3
#define RED	4

using namespace std;

typedef struct                       /**** BMP file info structure ****/
{
    	int            biWidth;          /* Width of image */
    	int            biHeight;         /* Height of image */
    	char *data;
} BITMAPINFOHEADER;

struct Color
{
        int             count;
        vector<int>     data;
        Color(int c=0, int c1=0, int c2=0, int c3=0):count(c) {};
        Color & operator=(Color &c);
        void set(int c, int c1);
        void set(int c, int c1, int c2);
        void set(int c, int c1, int c2, int c3);
        friend ostream & operator<<(ostream &o, Color &c);
};


/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

/*** Counter to keep track of the last loaded texture ***/
extern int num_texture;


/**********************************************************
 *
 * FUNCTION loadBitmap(Color &)
 *
 * This function loads a bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/
extern int loadBitmap(Color & colors, int stripeType);
//extern int LoadBitmap(int colors);  //sil

