#include <math.h>
#include <assert.h>

#include "part4.h"

// Set the x and y fields of the point structure pointed to by pt
// to the x and y values passed in as arguments.  The point structure
// is defined in the part4.h header file.
//
// This should only take a couple of lines.
void
set_point(struct point *pt, double x, double y)
{
	//switch using ->
	pt -> x = x;
	pt -> y = y;
	return;
	assert(0);
}

// Compute the planar distance between two points. Recall that the
// distance between two points in a plane is computed as follows. Label
// the two points (x1, y1) and (x2, y2).  Let dx be the difference
// between x1 and x2 and dy be the difference between y1 and y2. The
// distance between the two points is the square root of the sum of dx
// squared plus dy squared.
//
// Hint: Use the sqrt function from math.h.  You do not need to use
// the pow function; x * x is sufficient to square a variable.
double
point_dist(struct point *pt1, struct point *pt2)
{	
	//sqrt((x2-x1)^2 + (y2-y1)^2)
	double dx,dy,ans;
	dx = pt2 -> x - pt1 -> x;
	dy = pt2 -> y - pt1 -> y;
	ans = sqrt((dx * dx) + (dy * dy));
	return ans;
	assert(0);
}
