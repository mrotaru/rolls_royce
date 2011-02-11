# !/bin/env/python
#APPNAME = "fish"
#VERSION = "0.1"

from subprocess import *
from datetime import date
import re

top = '.'
out = 'build'

def options( opt ):
   # opt.load( "compiler_cxx" )
   pass

def configure( cnf ):
    print( "configuring the project in " + cnf.path.abspath())
    cnf.check_tool( "g++" )
    cnf.find_program( "git" )
    cnf.find_program( "date" )

def build( bld ):
    # determine version
    if bld.env[ 'GIT' ] !="":
        ver_short = Popen( "git describe --abbrev=0" , stdout=PIPE, stderr=PIPE ).stdout.read()
        ver_full = Popen( "git describe --abbrev=7 --dirty" , stdout=PIPE, stderr=PIPE ).stdout.read()
        #short_ver = re.sub('*?
        exe_name = 'rr_'+str(ver_short.strip())
        print "building " + exe_name + "..."
        ver_file = open( "VERSION", 'w' )
        ver_file.write( ver_short.strip() + '\n' )
        ver_file.write( ver_full.strip() + '\n')
        d = date.today()
        ver_file.write( d.strftime("%a, %d %b %Y, %H:%M %p") )

    a_path = bld.path.abspath()
    boost_path = "C:/pdev/boost_1_45_0"

    print( bld.path.abspath())
    bld.program(
            source       = [ 
                'main.cpp', 
                'globals.cpp',
                'utils.cpp',
                'Point.cpp',
                'Line.cpp',
                'BezierCurve.cpp', 
                'BezierPath.cpp',
                'Shape.cpp'
                ],
            
            target       = 'fish',
            features     = [ 'cxxprogram' ],
            includes     = [ 
                './freeglut/include', 
                boost_path ],
            lib          = [ 'freeglut','opengl32', 'glu32' ],
            libpath      = [ 
                a_path + '/freeglut/lib', 
                a_path + '/freeglut' ],
            stlib        = [ 'boost_regex-gcc-1_45'],
            stlibpath    = [ boost_path + '/libs/regex/build/gcc' ],
            linkflags    = [ '-W1,subsystem,windows' ],
            cxxflags     = [ '-c', '-g', '-O2', '-Wall' ],
            )

# vim:filetype=python
