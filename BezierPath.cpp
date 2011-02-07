#include <iostream>
#include <memory>
#include <fstream>
#include <string>
using namespace std;

#include "BezierPath.h"
#include <GL/glu.h>

#include <boost/foreach.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>

namespace xmx {

void xmx::BezierPath::addCurve( 
        GLfloat e1_x, GLfloat e1_y,
        GLfloat c1_x, GLfloat c1_y,
        GLfloat c2_x, GLfloat c2_y,
        GLfloat e2_x, GLfloat e2_y )
{
    boost::shared_ptr<BezierCurve> mptr
        ( new BezierCurve( e1_x, e1_y, c1_x, c1_y, c2_x, c2_y, e2_x, e2_y ));

    curves.push_front( mptr );
}


void xmx::BezierPath::draw()
{
    BOOST_FOREACH( boost::shared_ptr<BezierCurve> curve_ptr, curves )
    {
        curve_ptr->draw();
    }
}


void xmx::BezierPath::print()
{
    cout<<"BezierPath @ "<<this<<endl;
    BOOST_FOREACH( boost::shared_ptr<BezierCurve> curve_ptr, curves )
    {
        curve_ptr->draw();
    }
}

// works with files exported from Inkscape as "PovRay ( paths and shapes only)"
// loads the first 'bezier_spline' found
void xmx::BezierPath::loadFromPovFile( char* filename )
{
    string line;
    int line_no = 0;
    int bezier_spline_line = -1; // on which line is 'bezier_spline'
    int nr_points_line = -1; // on which line is 'nr points'
    int nr_points = 0;
    int next_point = -1;

    ifstream my_file( filename, ios::in );
    if ( my_file.is_open())
    {
        while( my_file.good() )
        {
            getline( my_file, line );
            line_no++;
            if( line.find("bezier_spline") != -1 )
            {
                cout<<"bezier_spline on line "<< line_no <<endl;
                bezier_spline_line = line_no;
            }

            if( line.find("//nr points") != -1 && bezier_spline_line != -1 )
            {
                cout<<"//nr points @ line "<< line_no <<endl;
                static const boost::regex e( "points$", boost::regex::extended );
                boost::smatch what;
                cout<< regex_match( line, e ) <<endl;
            }
        
        }
        my_file.close();
    }
    else cout<<"Unable to open file: '" << filename << "'" << endl;
}


void xmx::BezierPath::resizeTo( int newWidth )
{
    return /* something */;
}

xmx::BezierPath::BezierPath()
{
}

xmx::BezierPath::~BezierPath()
{
}

} // namespace xmx