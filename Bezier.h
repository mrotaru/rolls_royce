#ifndef XMX_BEZIER_H
#define XMX_BEZIER_H
#include <GL/glu.h>
#include "globals.h"

namespace xmx {

class BezierPath
{
    public:
        // constructor parameters:
        // e1,e2: end points
        // c1,c2: control points
        BezierPath( Point e1, Point c1, Point c2, Point e2 );
        ~BezierPath();

        // methods
        void print();
        void draw();
        void rotate( float );

        // array holding the vertices
        GLfloat **points;

        // sugar
        const static int END_PT_1 = 0;
        const static int CTRL_PT_1 = 1;
        const static int CTRL_PT_2 = 2;
        const static int END_PT_2 = 3;
};

} // namespace xmx
#endif /* XMX_BEZIER_H */
