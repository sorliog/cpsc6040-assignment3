//-------------------------------------------------------
//
//  StarterViewer.C
//
//  This viewer is a wrapper of the Glut calls needed to
//  display opengl data.  Options for zooming, 
//  labeling the window frame, etc are available for 
//  derived classes to use.
//
//
//  Copyright (c) 2003,2017,2023 Jerry Tessendorf
//
//--------------------------------------------------------


#include <GL/gl.h>   // OpenGL itself.
#include <GL/glu.h>  // GLU support library.
#include <GL/glut.h> // GLUT support library.
#include <OpenImageIO/imageio.h>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include "StarterViewer.h"
#include "ImgProc.h"
#include "Stencil.h"
#include "ImgOperations.h"
#include "ColorLUT.h"

#define DEFAULT_WRITE_FILENAME "test"
#define DEFAULT_NEW_IMAGE_SIZE 150
#define DEFAULT_NEW_IMAGE_NUM_CHANNELS 3

// Half width is 5 so that the total width is 11
#define DEFAULT_STENCIL_HALF_WIDTH 5

using namespace std;
using namespace OIIO;
using namespace img;

namespace starter{


// Helper functions
void StarterViewer::apply_gamma(const float amount){
    std::cout << "Applying " << amount << " gamma to image " << std::endl;
    ImgProc placeholder;
    img::gamma(image, placeholder, amount);
    image = placeholder;

}

void StarterViewer::apply_brightness(const float amount){
    std::cout << "Applying " << amount << " brightness to image " << std::endl;
    ImgProc placeholder;
    img::brightness(image, placeholder, amount);
    image = placeholder;

}



// Apply random linear bounded convolution to image
void StarterViewer::apply_random_linear_conv(){
	 cout << "Applying random linear convolution to image " << endl;
	 float total = 0;
	 time_t seed = time(0);
	 cout << "Using seed: " << seed << endl;
	 srand(seed);

	 Stencil stencil(DEFAULT_STENCIL_HALF_WIDTH);     
	 for(int j = -stencil.halfsize(); j <= stencil.halfsize(); ++j){
	     for(int i = -stencil.halfsize(); i <= stencil.halfsize(); ++i){
	         if(i != 0 && j != 0){  
	             float num = ((float)rand() / (float)RAND_MAX) - 0.5;
	             // cout << "Stencil vals " << i << " " << j << " " << num << endl;
	             stencil.set_value(i, j, num);
	             total += num;
	         }
	         
	                      
	     }
	 }
	 // cout << "Setting 0,0 to " << (1 - total) << " because total=" << total << endl; 
	 stencil.set_value(0, 0, 1 - total);
  
	
	 ImgProc placeholder;
	 bounded_linear_conv(stencil, image, placeholder);
	 image = placeholder;
}



// Apply emboss linear bounded convolution
void StarterViewer::apply_emboss_linear_conv(){
	 Stencil stencil(1);
	 
	 stencil.set_value(0,0, 1);
	 stencil.set_value(1,0, 1);
	 stencil.set_value(-1,0, -1);
	 stencil.set_value(0, 1, -1);
	 stencil.set_value(-1,1, -2);
	 stencil.set_value(1,1, 0);
	 stencil.set_value(0,-1, 1);
	 stencil.set_value(-1,-1, 0);
	 stencil.set_value(1,-1, 2);
	     
	 ImgProc placeholder;
	 bounded_linear_conv(stencil, image, placeholder);
	 image = placeholder;
}

// Draw the julia set
void StarterViewer::draw_julia_set(){
    std::cout << "Drawing julia set " << std::endl;
    Point center;
    center.setX(0.03811);
    center.setY(0.01329);
    float range = 1;
    int iterations = 250;
    int cycles = 11;
    
    image.clear(image.nx(), image.ny(), image.depth());    
    std::cout << "Generating JuliaSet center=(" << center.getX() << "," << center.getY() << ") range=" << range << ", iterations=" << iterations << ", cycles=" << cycles << " with " << colorTable->getName() << " pallete" << std::endl;

    apply_fractal_lut(center, colorTable, image, range, iterations, cycles);
    std::cout << "Finished Drawing Julia set " << std::endl;
}

// These are the GLUT Callbacks that are implemented in StarterViewer.
void cbDisplayFunc()
{
//    std::cout << "cbDisplayFunc()" << std::endl;
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );	
    StarterViewer::Instance() -> Display();
    glutSwapBuffers();
    glutPostRedisplay();
}

void cbIdleFunc()
{
    StarterViewer::Instance() -> Idle();
}


void cbKeyboardFunc( unsigned char key, int x, int y )
{
    StarterViewer::Instance() -> Keyboard( key, x, y );
}

void cbMotionFunc( int x, int y )
{
   StarterViewer::Instance() -> Motion( x, y );
   glutPostRedisplay();
}

void cbMouseFunc( int button, int state, int x, int y )
{
    StarterViewer::Instance() -> Mouse( button, state, x, y );
}

void cbReshapeFunc( int w, int h )
{
//    std::cout << "cbReshapeFunc(" << w << ", " << h << ")" << std::endl;
    StarterViewer::Instance() -> Reshape( w, h );
    glutPostRedisplay();
}


StarterViewer* StarterViewer::pStarterViewer = nullptr;
	
StarterViewer::StarterViewer() : 
   initialized    ( false ),
   width          ( image.is_loaded() ? image.nx() : 512 ), 
   height         ( image.is_loaded() ? image.ny() : 512 ),
   display_mode   ( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH ),
   title          ( string("Image Viewer") ),
   mouse_x        ( 0 ),
   mouse_y        ( 0 ),
   camera_fov     (35.0),
   camera_aspect  (1.0),
   camera_near    (0.01),
   camera_far     (10000.0),
   camera_eye_x   (0.0),
   camera_eye_y   (0.0),
   camera_eye_z   (-15.0),
   camera_view_x  (0.0),
   camera_view_y  (0.0),
   camera_view_z  (0.0),
   camera_up_x    (0.0),
   camera_up_y    (1.0),
   camera_up_z    (0.0),
   camera_right_x (1.0),
   camera_right_y (0.0),
   camera_right_z (0.0),
   frame          (0),
   default_x_dim(DEFAULT_NEW_IMAGE_SIZE),
    default_y_dim(DEFAULT_NEW_IMAGE_SIZE),
    default_num_chan(DEFAULT_NEW_IMAGE_NUM_CHANNELS),
    colorTable(blackWhite)
{
   cout << "StarterViewer Loaded\n";
}

StarterViewer::~StarterViewer(){}

void StarterViewer::Init( const std::vector<std::string>& args )
{
   int argc = (int)args.size();
   char** argv = new char*[argc];
   for( int i=0;i<argc;i++)
   {
      argv[i] = new char[args[i].length() + 1];
      std::strcpy(argv[i], args[i].c_str());
   }

   string window_title = title;

   glutInit( &argc, argv );
   glutInitDisplayMode( display_mode );
   glutInitWindowSize( width, height );
   std::cout << "Initializing Windows with size (" << width << ", " << height << ")" << std::endl;
   glutCreateWindow( window_title.c_str() );
   glClearColor(0.5,0.5,0.6,0.0);

   camera_aspect = (float)width/(float)height;

   glutDisplayFunc( &cbDisplayFunc );
   glutIdleFunc( &cbIdleFunc );
   glutKeyboardFunc( &cbKeyboardFunc );
   glutMotionFunc( &cbMotionFunc );
   glutMouseFunc( &cbMouseFunc );
   glutReshapeFunc( &cbReshapeFunc );

   initialized = true;
   cout << "StarterViewer Initialized\n";
}

void StarterViewer::MainLoop()
{
   Usage();
   glutMainLoop();
}


void StarterViewer::Display()
{


	++frame;
	glLoadIdentity();
	gluPerspective( camera_fov, camera_aspect, camera_near, camera_far );	
	gluLookAt( camera_eye_x, camera_eye_y, camera_eye_z,    // Camera eye point
               camera_view_x, camera_view_y, camera_view_z, // Camera view point
               camera_up_x, camera_up_y, camera_up_z        // Camera up direction
             );
	
	glEnable(GL_DEPTH_TEST);
	glDepthRange( camera_near, camera_far );
    
	// std::cout << "Display(), image loaded? " << image.is_loaded() << std::endl;
 	
	if(image.depth() == 3){	 
   		glDrawPixels(image.nx(), image.ny(),  GL_RGB, GL_FLOAT,image.get_raw_data());	
	}
	else if(image.depth() == 4){
		glDrawPixels(image.nx(), image.ny(),  GL_RGBA, GL_FLOAT,image.get_raw_data());
	}
	else{
		std::cout << "Error: Unsupported number of channels. Can suppport three or four channels, but found " << image.depth() << " channels " << std::endl; 
	}
		

}

void StarterViewer::SetDefaultXDimension(int value){
    default_x_dim = value;
}
void StarterViewer::SetDefaultYDimension(int value){
    default_y_dim = value;
}
void StarterViewer::SetDefaultNumChannels(int value){
    default_num_chan = value;
}

void StarterViewer::SetLUT(std::string name){
    colorTable = blackWhite;

    for(int i = 0; i < numberOfPalletes; i++){
        if(name.compare(palletes[i]->getName()) == 0){
           colorTable = palletes[i];
           return;
        }
    }

}


void StarterViewer::Load(std::string filename){
	ImgProc placeholder;
    bool success = placeholder.read_image(filename);
    if(success){
        std::cout << "Successfully loaded " << filename << " image." << std::endl;
    }
    else{

        std::cout << "Creating new image " << filename << " with dimensions " << default_x_dim << "," << default_y_dim << " with " << default_num_chan << " channels" << std::endl;

        placeholder.clear(default_x_dim, default_y_dim, default_num_chan);
    }
    image_filename = filename;
    img::flip_vertically(placeholder, image);
    std::cout << " Loading and flipping " << std::endl;
    width = image.nx();
    height = image.ny();


    Reshape(image.nx(), image.ny());


}

void StarterViewer::Reshape( int w, int h )
{
   width = w;
   height = h;
   camera_aspect = (float)width/(float)height;

//   std::cout << "Reshaping Window to (" << width << ", " << height << ") Aspect: " << camera_aspect << std::endl;
   glViewport( 0, 0, (GLsizei) width, (GLsizei) height );
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
}

void StarterViewer::Keyboard( unsigned char key, int x, int y )
{
   switch (key)
   {
      case 'f':
         camera_fov /= 1.01;
         break;
      case 'F':
         camera_fov *= 1.01;
         if( camera_fov > 170.0){ camera_fov = 170.0; }
	 break;
      case '+':
      case '=':
         ComputeEyeShift(0.07);
         break;
      case '-':
      case '_':
         ComputeEyeShift(-0.07);
         break;
      case 'r':
	     Reset();
        break;
      case 'h':
	     Home();
      break;
      case 'u':
	     Usage();
      break;
    
      // Image operation controls
      case 'g':
        apply_gamma(0.9);
      break;
        
      case 'G':
        apply_gamma(1.0 + (1.0/9.0));
      break;
	  
      case 'b':
        apply_brightness(0.9);
      break;
        
      case 'B':
        apply_brightness(1.0 + (1.0/9.0));
      break;

      case 's': {
        apply_random_linear_conv();
        break;
      }

      case 'J': {
        draw_julia_set();
        break;
      }
      // Image saving controls
      case 'j':
        image.write_image_jpeg(image_filename);
      break;
    
      case 'p':
        image.write_image_png(image_filename);
      break;

	  
   }
}


void StarterViewer::Motion( int x, int y )
{
   float dx = x - mouse_x;
   float dy = y - mouse_y;
   float pos_x = current_raster_pos[0] + dx;
   float pos_y = current_raster_pos[1] - dy;
   glRasterPos2f( pos_x, pos_y ); 

   // camera motion perp to view direction
   if(keystate == GLUT_ACTIVE_SHIFT )
   {
     ComputeEyeUpRight(dx,dy);
   }
   mouse_x = x;
   mouse_y = y;
}


void StarterViewer::Mouse( int b, int state, int x, int y )
{
   mouse_x = x;
   mouse_y = y;
   keystate = glutGetModifiers();
   button = b;
   mouse_state = state;
   glGetFloatv( GL_CURRENT_RASTER_POSITION, current_raster_pos );
}


void StarterViewer::Idle() {}


void StarterViewer::Usage()
{
   cout << "--------------------------------------------------------------\n";
   cout << "StarterViewer usage:\n";
   cout << "--------------------------------------------------------------\n";
   cout << "f/F           reduce/increase the camera FOV\n";
   cout << "+/=           move camera farther from the view point\n";
   cout << "-/_           move camera closer to the view point\n";
   cout << "SHIFT+mouse   move camera perpendicular to the view direction\n";
   cout << "r             reset sim parameters\n";
   cout << "h             home display parameters\n";
   cout << "s             apply random linear convolution\n";
   cout << "g/G           decrease/increase gamma\n";
   cout << "b/B           decrease/increase brightness\n";
   cout << "j             save image to " << DEFAULT_WRITE_FILENAME << ".jpeg \n";
   cout << "p             save image to " << DEFAULT_WRITE_FILENAME << ".png \n";
   cout << "u             display this usage message\n";
   cout << "--------------------------------------------------------------\n";
}

void StarterViewer::Reset()
{
   std::cout << "Reset\n";
   Load(image_filename);
   Home();
}

void StarterViewer::Home()
{
   std::cout << "Home\n";
   camera_fov     = 35.0;
   camera_near    = 0.01;
   camera_far     = 100.0;
   camera_eye_x   = 0.0;
   camera_eye_y   = 0.0;
   camera_eye_z   = -15.0;
   camera_view_x  = 0.0;
   camera_view_y  = 0.0;
   camera_view_z  = 0.0;
   camera_up_x    = 0.0;
   camera_up_y    = 1.0;
   camera_up_z    = 0.0;
   camera_right_x = 1.0;
   camera_right_y = 0.0;
   camera_right_z = 0.0;
}



void StarterViewer::ComputeEyeUpRight(int dx, int dy)
{

// dx --> rotation around y axis
// dy --> rotation about camera right axis


   float vvx = camera_eye_x-camera_view_x; 
   float vvy = camera_eye_y-camera_view_y; 
   float vvz = camera_eye_z-camera_view_z;
   float vvnorm = std::sqrt( vvx*vvx + vvy*vvy + vvz*vvz );
   vvx /= vvnorm;
   vvy /= vvnorm;
   vvz /= vvnorm;


// Rotate around y axis
//      Rotate view direction
   float cosx = std::cos( -dx*0.006 );
   float sinx = std::sin( -dx*0.006 );
   float nvvx = vvx*cosx + vvz*sinx;
   float nvvz = -vvx*sinx + vvz*cosx;
   float nrightx = camera_right_x*cosx + camera_right_z*sinx;
   float nrightz = -camera_right_x*sinx + camera_right_z*cosx;
   vvx = nvvx;
   vvz = nvvz;
   camera_right_x = nrightx;
   camera_right_z = nrightz;
//      Rotate up direction
   float crossx = camera_up_z;
   float crossy = 0.0;
   float crossz = -camera_up_x;
   float ydotup = camera_up_y;
   camera_up_x = camera_up_x*cosx + crossx*sinx;
   camera_up_y = camera_up_y*cosx + ydotup*(1.0-cosx) + crossy*sinx;
   camera_up_z = camera_up_z*cosx + crossz*sinx;
//      Rotate right direction
   crossx = camera_right_z;
   crossy = 0.0;
   crossz = -camera_right_x;
   ydotup = camera_right_y;
   camera_right_x = camera_right_x*cosx + crossx*sinx;
   camera_right_y = camera_right_y*cosx + ydotup*(1.0-cosx) + crossy*sinx;
   camera_right_z = camera_right_z*cosx + crossz*sinx;


// Rotate around camera-right axis
//     Rotate view direction
   cosx = std::cos( dy*0.006 );
   sinx = std::sin( dy*0.006 );
   float rightdotview = camera_right_x*vvx + camera_right_y*vvy + camera_right_z*vvz;
   crossx = camera_right_y*vvz - camera_right_z*vvy;
   crossy = camera_right_z*vvx - camera_right_x*vvz;
   crossz = camera_right_x*vvy - camera_right_y*vvx;
   nvvx = vvx*cosx + camera_right_x*rightdotview*(1.0-cosx) + crossx*sinx; 
   float nvvy = vvy*cosx + camera_right_y*rightdotview*(1.0-cosx) + crossy*sinx; 
   nvvz = vvz*cosx + camera_right_z*rightdotview*(1.0-cosx) + crossz*sinx; 
   vvx = nvvx;
   vvy = nvvy;
   vvz = nvvz;
//      Rotate up direction
   crossx = camera_right_y*camera_up_z - camera_right_z*camera_up_y;
   crossy = camera_right_z*camera_up_x - camera_right_x*camera_up_z;
   crossz = camera_right_x*camera_up_y - camera_right_y*camera_up_x;
   camera_up_x = camera_up_x*cosx + crossx*sinx; 
   camera_up_y = camera_up_y*cosx + crossy*sinx; 
   camera_up_z = camera_up_z*cosx + crossz*sinx; 


   camera_eye_x = vvx*vvnorm + camera_view_x;
   camera_eye_y = vvy*vvnorm + camera_view_y;
   camera_eye_z = vvz*vvnorm + camera_view_z;
}

void StarterViewer::ComputeEyeShift(float dz)
{
   float vvx = camera_eye_x-camera_view_x; 
   float vvy = camera_eye_y-camera_view_y; 
   float vvz = camera_eye_z-camera_view_z;
   float vvnorm = std::sqrt( vvx*vvx + vvy*vvy + vvz*vvz );
   vvx /= vvnorm;
   vvy /= vvnorm;
   vvz /= vvnorm;

   camera_eye_x += dz*vvx;
   camera_eye_y += dz*vvy;
   camera_eye_z += dz*vvz;
}



void StarterViewer::SetCameraEyeViewUp( float eyex, float eyey, float eyez, float viewx, float viewy, float viewz, float upx, float upy, float upz )
{
   camera_eye_x = eyex;   
   camera_eye_y = eyey;   
   camera_eye_z = eyez;   
   camera_view_x = viewx;   
   camera_view_y = viewy;   
   camera_view_z = viewz;   
   camera_up_x = upx;   
   camera_up_y = upy;   
   camera_up_z = upz;   
}



StarterViewer* CreateViewer() { return StarterViewer::Instance(); }



}


