/*
 *
 * Tutorial 3: 3d engine - Texture mapping with OpenGL!
 *
 * Include File: texture.cpp
 *
 */

/*
just run: make and you are done.
of course you may need to change the makefile
*/

#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glut.h>
#include <GL/glu.h>
#include "texture.h"        

using namespace std;

/**********************************************************
 *
 * VARIABLES DECLARATION
 *
 *********************************************************/

/*** Counter to keep track of the last loaded texture ***/
int num_texture=-1;


/**********************************************************
 *
 * FUNCTION setPixelColor(int, char *)
 *
 * This function sets pixel color acoording to input :)
 *
 *********************************************************/
void setPixelColor(int color, char *pixel)
{
	if (color == YELLOW)
	{
		pixel[0] = ( GLubyte ) 240;
		pixel[1] = ( GLubyte ) 240;
		//pixel[2] = ( GLubyte ) 010;
		pixel[2] = ( GLubyte ) 001;
	}
	else if (color == RED)
	{
		pixel[0] = ( GLubyte ) 255;
		//pixel[1] = ( GLubyte ) 110;
		//pixel[2] = ( GLubyte ) 110;
		pixel[1] = ( GLubyte ) 001;
		pixel[2] = ( GLubyte ) 001;
	}
	else if (color == GREEN)
	{
		//pixel[0] = ( GLubyte ) 110;
		//pixel[1] = ( GLubyte ) 255;
		//pixel[2] = ( GLubyte ) 110;
		pixel[0] = ( GLubyte ) 001;
		pixel[1] = ( GLubyte ) 255;
		pixel[2] = ( GLubyte ) 001;
	}
	else if (color == BLUE)
	{
		//pixel[0] = ( GLubyte ) 150;
		//pixel[1] = ( GLubyte ) 150;
		//pixel[2] = ( GLubyte ) 255;
		pixel[0] = ( GLubyte ) 001;
		pixel[1] = ( GLubyte ) 001;
		pixel[2] = ( GLubyte ) 255;
	}
	else
	{
		pixel[0] = ( GLubyte ) 0;
		pixel[1] = ( GLubyte ) 0;
		pixel[2] = ( GLubyte ) 0;
		printf("color:%d Undefined color. Exiting...\n",color);
		//exit(1);
	}
}

int chooseColor(Color colors, int index, int stripeType)
{
	int color;
	int rem;
	int mod = 3;

	if (stripeType == 1)	mod = 2;
	
	if(colors.count == 1)
	{
		color = colors.data[0];
	}
	else if(colors.count == 2)
	{
		rem = index % mod;
		if (mod == 3)
		{
			if (rem == 0 || rem == 1)
				color = colors.data[0];
			else if (rem == 2)
				color = colors.data[1];
		}
		else
		{
			if (rem == 0 )
				color = colors.data[0];
			else if (rem == 1)
				color = colors.data[1];
			else
				cout << rem << " is bizarre" <<endl;
		}
	}
	else if(colors.count == 3)
	{
		rem = index % mod;
		if (rem == 0)
			color = colors.data[0];
		else if (rem == 1)
			color = colors.data[1];
		else if (rem == 2)
			color = colors.data[2];
	}
	else 
	{
		color = 0;
	}
	return color;
}

/**********************************************************
 *
 * FUNCTION loadBitmap(Color &, int)
 *
 * This function loads a bitmap file and return the OpenGL reference ID to use that texture
 *
 *********************************************************/
int loadBitmap(Color &colors, int stripeType)
{
    char temp;
    int i,j,c;
    int color;

    BITMAPINFOHEADER infoheader;

    num_texture++; // The counter of the current texture is increased

    if (stripeType == 1)
    {	
    	//infoheader.biWidth = infoheader.biHeight = 8;
    }
    else
    {
    	//infoheader.biWidth = infoheader.biHeight = 8;
    }
    infoheader.biWidth = infoheader.biHeight = 3;
    infoheader.data = (char *) malloc(infoheader.biWidth * infoheader.biHeight * 3);
    c = 0;
    for ( i = 0; i < infoheader.biWidth; i++ ) {
        for ( j = 0 ; j < infoheader.biHeight; j++ ) {
	      color = chooseColor(colors, i, stripeType);
	      setPixelColor(color, &(infoheader.data[c]));	
	      c+=3;
        }
    }

    glBindTexture(GL_TEXTURE_2D, num_texture); // Bind the ID texture specified by the 2nd parameter

    // The next commands sets the texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT); // If the u,v coordinates overflow the range 0,1 the image is repeated
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); // The magnification function ("linear" produces better results)
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); //The minifying function
    //glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); //The minifying function
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_SPHERE_MAP); //The minifying function

    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE); // We don't combine the color with the original surface color, use only the texture map.

    // Finally we define the 2d texture
    glTexImage2D(GL_TEXTURE_2D, 0, 3, infoheader.biWidth, infoheader.biHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);

    // And create 2d mipmaps for the minifying function
    gluBuild2DMipmaps(GL_TEXTURE_2D, 3, infoheader.biWidth, infoheader.biHeight, GL_RGB, GL_UNSIGNED_BYTE, infoheader.data);
    
    free(infoheader.data); // Free the memory we used to load the texture

    return (num_texture); // Returns the current texture OpenGL ID
}


/******************************************
 *
 * struct Color Function Definitions
 *
 *****************************************/
ostream & operator<<(ostream &o, Color &c)
{
        int i;
        o<<"count:"<<c.count<<" colors:";
        for(i=0; i < c.count; i++)
        {
		if (c.data[i] == RED) 
                	o<<"red"<<" ";
		else if (c.data[i] == YELLOW) 
                	o<<"yellow"<<" ";
		else if (c.data[i] == GREEN) 
                	o<<"green"<<" ";
		else if (c.data[i] == BLUE) 
                	o<<"blue"<<" ";
        }
        return o;
}

void Color::set(int c, int c1)
{
        count = c;
        data.push_back(c1);
}

void Color::set(int c, int c1, int c2)
{
        count = c;
        data.push_back(c1);
        data.push_back(c2);
}

void Color::set(int c, int c1, int c2, int c3)
{
        count = c;
        data.push_back(c1);
        data.push_back(c2);
        data.push_back(c3);
}

Color & Color::operator=(Color &c)
{
        int i;
        count = c.count;
        for(i=0; i < count; i++)
        {
                data[i] = c.data[i];
        }
        return *this;
}

