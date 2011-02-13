#include <iostream>
#include <fstream>
using namespace std;

#include <GL/glut.h>
#include <GL/glu.h>

#include <boost/shared_ptr.hpp>
#include <boost/foreach.hpp>

#include "globals.h"
#include "utils.h"
#include "Point.h"
#include "Line.h"
#include "BezierCurve.h"
#include "Shape.h"
#include "Group.h"
#include "Layer.h"
using namespace xmx;

Line my_line( 100, 150, 200, 300 );
typedef boost::shared_ptr<Shape> sptrShape;

string build_info;

// bezier stuff
Point e1(  30, 70  );
Point c1(   0, 270 );
Point c2( 290, 110 );
Point e2( 200, 100 );
BezierCurve bez_path( e1, c1, c2, e2 );
//Shape shape;
//Shape rolls;
Group rolls;
list< sptrShape > shapez;

void myDisplayFunc( void )
{
    glClear( GL_COLOR_BUFFER_BIT );
    glRenderMode( GL_RENDER );

    // set line colour red( r=1, g=0,b=0 ).
    glColor3f( dcol.R, dcol.G, dcol.B );

    // draw a line from point( 100,150 ) to point( 200, 300 )
    my_line.draw();
//    my_line.rotate( 20 );
    
    // draw the bezier curve ! ( man, was I excited )
//    bez_path.draw();
//    shape.draw();
    
//    cout << "drawing shapes " << endl;
//    BOOST_FOREACH( boost::shared_ptr< Shape > xshape, shapez )
//    {
//        cout << "drawing shape " << xshape -> name << endl;
//        xshape -> draw();
//    }
    rolls.draw();

    printText( 10, glutGet( GLUT_WINDOW_HEIGHT ) - 18, VERSION );
    printText( 10, glutGet( GLUT_WINDOW_HEIGHT ) - 32, build_info );

    // keep showing( flushing ) line on the screen instead of showing just once.
    glutSwapBuffers();
    glutPostRedisplay();
}

void init( void )
{
    // select clearing color
    glClearColor( 0.0, 0.0, 0.0, 0.0 );

    // initialize viewing values
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0,640,0,480 );

    // read version number and build info
    ifstream ver_file( "VERSION", ifstream::in );
    getline( ver_file, VERSION );
    VERSION = "Rolls Royce Motor Cars, " + VERSION;
    getline( ver_file, BUILD_ID );
    getline( ver_file, BUILD_TIME );
    build_info = "Build info: " + BUILD_ID + " @ " + BUILD_TIME;

    // load a pov file
//    loadPovFile( "vector/rolls_logo.pov", shapez );
    rolls.loadFromPovFile( "vector/rolls_full.pov" );
//    rolls.primitives.splice( rolls.primitives.begin(), shapez, shapez.begin());
    cout <<"loaded pov" << endl;

//    bp1.loadFromPovFile("vector/body.pov");

}

void myKeyboardFunc( unsigned char key, int x, int y )
{
    switch( key )
    {
        case 'j':
            my_line.rotate( 5 );
            break;
        case 'k':
            my_line.rotate( -5 );
            break;
        case 'l':
//            shape.move( 5, 0 );
            break;
        case 'h':
//            shape.move( -5, 0);
            break;
        case 27:
            exit(0);
            break;
    }
    glutPostRedisplay();
}

void myReshape( int nWidht, int nHeight )
{
    glViewport( 0, 0, (GLsizei)nWidht, (GLsizei)nHeight );
    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    gluOrtho2D( 0, nWidht, 0, nHeight);
    glutPostRedisplay();
}

int main( int argc, char** argv )
{
    // gl stuff
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGB | GLUT_MULTISAMPLE );

    // window stuff
    window_width = 640;
    window_height = 480;
    glutInitWindowSize( window_width, window_height );
    glutInitWindowPosition( 100, 100 );
    glutCreateWindow( "Hello" );
    init();

    // callbacks
    glutDisplayFunc( myDisplayFunc );
    glutKeyboardFunc( myKeyboardFunc );
    glutReshapeFunc( myReshape );

    glutMainLoop();
    return 0;   // ANSI C requires main to return int.
}
