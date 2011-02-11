#ifndef UTILS_H
#define UTILS_H

#include <list>
#include <string>
using namespace std;

#include <boost/shared_ptr.hpp>

#include "Shape.h"
using namespace xmx;

typedef boost::shared_ptr<Shape> sptrShape;

void loadPovFile( string filename, list< sptrShape >& shape_list );

#endif /* UTILS_H */
