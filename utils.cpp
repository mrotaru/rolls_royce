#include <fstream>
#include <iostream>
#include <sstream>
#include <list>
#include <vector>
#include <string>
#include <stdexcept>
using namespace std;

#include <GL/glu.h>

#include <boost/shared_ptr.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string/regex.hpp>
#include <boost/lexical_cast.hpp>

#include "globals.h"
#include "utils.h"
#include "Point.h"
#include "BezierCurve.h"
#include "Shape.h"
using namespace xmx;

// if `str` matches `myRegex`, try parsing the match into an integer
// NOTE: this should really be a template class; maybe later
int getIntRegex( string str, const boost::regex& myRegex )
{
//    cout<<"searching in " << str <<endl;
    boost::smatch match;
    if( regex_search( str, match, myRegex ))
        return boost::lexical_cast<int>( match[1] );
    else
        throw( "cannot convert; exiting");

}

// if `str` matches `myRegex`, try parsing the match into a GLfloat
GLfloat getGLfloatRegex( string str, const boost::regex& myRegex )
{
//    cout<<"searching in " << str <<endl;
    boost::smatch match;
    if( regex_search( str, match, myRegex ))
        return boost::lexical_cast<GLfloat>( match[1] );
    else
        throw( "cannot convert; exiting");
}

string getStringRegex( string str, const boost::regex& myRegex )
{
//    cout<<"searching in " << str <<endl;
    boost::smatch match;
    if( regex_search( str, match, myRegex ))
        return boost::lexical_cast<string>( match[1] );
    else
        throw( "cannot convert; exiting");
}


// assumes `line` is a string containing the coordinates 4 points,
// separated by commas, and enclosed in angled brackets, like this:
// /*   1*/ <140.00000000, 440.00000000>, <210.00000000, 370.00000000>, <490.00000000, 560.00000000>, <400.00000000, 410.00000000>,
boost::shared_ptr< BezierCurve > parseBezierCurve( string line )
{
    cout << "parsing line " << line <<" for a Bezier curve" << endl;

    static const boost::regex split_coords_regex ( ">, <|/ <|>,$|>$", boost::regex::extended );
    static const boost::regex curve_no_regex     ( "\\s*/\\*\\s*(\\d{1,})\\*/ <", boost::regex::extended );
    static const boost::regex comma_split_regex     ( ", ", boost::regex::extended );

    Point pointz[4];

    vector< string > result;
    
    boost::algorithm::split_regex( result, line, split_coords_regex );

    for( int i=1; i< result.size()-1; i++ )
    {   // split the pairs into numbers
        vector<string> result2;
        boost::algorithm::split_regex ( result2, result[i], comma_split_regex );

        pointz[i-1].x = boost::lexical_cast<GLfloat>( result2[0] );
        pointz[i-1].y = boost::lexical_cast<GLfloat>( result2[1] );

        if( DEBUG_POV_LOADING ) cout<<"P"<< i-1 <<": x: "<< pointz[i-1].x <<", y: "<< pointz[i-1].y <<endl;
    }

    boost::shared_ptr< BezierCurve > mbc( new BezierCurve( pointz[0], pointz[1], pointz[2], pointz[3] ));
    return mbc;
}

// try parsing a Shape from `lines`, starting with `start_line`
// and if parsing is successufll, put the last parsed line into `end_line`
boost::shared_ptr< Shape > parseShape( vector< string >& lines, int& line_no )
{
    cout<< "parsing Shape starting with line " << line_no << endl;
    string line = lines[ line_no ];

    static const boost::regex shape_end_regex    ( "#{3} end (path\\d{4,})", boost::regex::extended );
    static const boost::regex curve_regex        ( "\\s*/\\*\\s*(\\d{1,})\\*/ <", boost::regex::extended );
    static const boost::regex shape_name_regex   ( "\\s*#declare (path\\d{4,})\\s*=\\s*prism", boost::regex::extended );

    string shape_name = getStringRegex( line, shape_name_regex );
    cout<< "shape name: "<< shape_name << endl;
    boost::shared_ptr< Shape > sptr_shape( new Shape());
    sptr_shape->name = shape_name;

    bool shape_finished = false;

    while( line_no <= lines.size() && !shape_finished )
    {
        if( regex_search( line, curve_regex ))
            sptr_shape->addBezierCurve( parseBezierCurve( line ));

        else if( regex_search( line, shape_end_regex ) && ( getStringRegex( line, shape_end_regex ) == sptr_shape->name ))
            {
                cout<<"sptr_shape " << sptr_shape->name << " end at: " << line_no << endl;
                shape_finished = true;
            }
        else
            cout<<"no bezier, no end: " << line << endl;

        line_no ++;
        line = lines[ line_no ];
    }

    if( !shape_finished )
    {
        string msg = "Pov parsing error: sptr_shape " + sptr_shape->name + " not finished";
        throw runtime_error( msg );
    }

    return sptr_shape;

}

void loadPovFile( string filename, list< boost::shared_ptr< Shape > > shape_list )
{
    cout<< "loading " << filename << endl;
    int total_lines = 0;
    vector<string> lines;
    lines.push_back( filename );

    // read the whole file in memory
    ifstream my_file;
    my_file.open( filename.c_str(), ifstream::in );
    string line;
    while( getline( my_file, line ))
    {
        total_lines++;
        lines.push_back( line );
    }

    cout<< "total lines: " << total_lines << endl;

    // process the file
    int header_start               = 0;     // the line number where the header starts
    int num_shapes                 = 0;
    int num_segments               = 0;
    int num_nodes                  = 0;             

    // find the header
    int cline = 1; // the current line
    while( !header_start && cline <= total_lines )
    {
        cline++;
        if ( lines[ cline ].find( "##   Exports in this file" ) != -1 ) 
            header_start = cline;
    }

    // no header ?
    if( !header_start && STRICT_POV )
        throw runtime_error("Pov parsing error: can't find the header");

    cout << "header start: " << header_start << endl;

    // setup some regexes
    static const boost::regex shapes_regex       ( "^##\\s{1,}Shapes\\s{1,}: (\\d{1,})$", boost::regex::extended );
    static const boost::regex segments_regex     ( "^##\\s{1,}Segments\\s{1,}: (\\d{1,})$", boost::regex::extended );
    static const boost::regex nodes_regex        ( "^##\\s{1,}Nodes\\s{1,}: (\\d{1,})$", boost::regex::extended );
    static const boost::regex curve_no_regex     ( "\\s*/\\*\\s*(\\d{1,})\\*/ <", boost::regex::extended );

    // parse the header
    if( STRICT_POV )
    {
        if( lines[ header_start + 2 ].find( "Shapes" ) == -1 )
        {
            cout<<"line: "<< header_start + 2 << endl;
            throw runtime_error("Pov parsing error: expected 'Shapes'; line: '" + lines[ header_start + 2 ] + "'" );
        }
        else
        {
            cout<<"getting asdasd"<< endl;
            num_shapes = getIntRegex( lines[ header_start + 2 ], shapes_regex );
        }
    }

    if( STRICT_POV )
    {
        if( lines[ header_start + 3 ].find( "Segments" ) == -1 )
        {
            cout<<"line: "<< header_start + 3 << endl;
            throw runtime_error("Pov parsing error: expected 'Segments'; line: '" + lines[ header_start + 2 ]  + "'" );
        }
        else
            num_segments = getIntRegex( lines[ header_start + 3 ] , segments_regex );
    }

    if( STRICT_POV )
    {
        if( lines[ header_start + 4 ].find( "Nodes" ) == -1 )
        {
            cout<<"line: "<< header_start + 4 << endl;
            throw runtime_error("Pov parsing error: expected 'Nodes'; line: '" + lines[ header_start + 2 ] + "'" );
        }
        else
            num_nodes = getIntRegex( lines[ header_start + 4 ], nodes_regex );
    }

    // we're done with the header;
    // now let's look for those shapes

    // we'll need some regex'es
    static const boost::regex shape_start        ( "\\s*#declare (path\\d{4,})\\s*=\\s*prism", boost::regex::extended );

    bool inside_shape = false;

    // iterate over the rest of the lines
    cline = header_start + 5;                                                                                         
    while( cline <= total_lines )
    {
        cout<< "processing line: " << cline << " - '" << lines[ cline ] << "'" << endl;
        string line = lines[ cline ];
        
        // does the line contain a curve ?
        if ( regex_search( line, curve_no_regex ))
        {
            if( !inside_shape )
            {
                cout<<"line: "<< cline << endl;
                throw runtime_error( "Pov parsing error: points encoutered outside a shape definition" );
            }
        }

        // does the line define the start of a shape ?
        else if ( regex_search( line, shape_start ))
                if( inside_shape )
                {

                    cout<<"line: "<< cline << endl;
                    cout<<"Pov parsing warning: nested shapes not supported ( line " << cline + 1 << endl;
                    inside_shape = true;
                }
                else 
                {
                    inside_shape = true;

                    shape_list.push_back( parseShape( lines, cline ) );
                    cout << "parsed shape; " << cline << endl;
                    inside_shape = false;
                }

        cline ++;
    }

    cout<< "processing of " << filename << " complete" << endl;
}