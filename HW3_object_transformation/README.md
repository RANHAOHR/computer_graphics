# computer_graphics HW3

## LEFT mouse button - swings the viewing axis based on the mouse motion. This would be equivalent to turning your head side to side if the mouse moves horizontally and nodding your head up and down if the mouse moves vertically.
## MIDDLE mouse button - moves the camera up and down in the plane perpendicular to its viewing axis. This is equivalent to moving sideways when the mouse moves horizontally and going up and down in a glass elevator when the mouse moves vertically. If the mouse moves diagonally, then these motions should be combined.
## RIGHT mouse button - moves the camera forward and back along the viewing vector.

## Use the following keys to control object transformations in your program:

## NOTE : the following are with respect to the WORLD coordinate system

"4" : negative translation along x axis
"6" : positive translation along x axis
"8" : positive translation along y axis
"2" : negative translation along y axis
"9" : positive translation along z axis
"1" : negative translation along z axis
"[" : negative rotation around x axis
"]" : positive rotation around x axis
";" : negative rotation around y axis
"'" : positive rotation around y axis
"." : negative rotation around z axis
"/" : positive rotation around z axis
"=" : increase uniform scaling
"-" : decrease uniform scaling

## NOTE : the following are with respect to the LOCAL object coordinate system

"i" : negative rotation around local x axis
"o" : positive rotation around local x axis
"k" : negative rotation around local y axis
"l" : positive rotation around local y axis
"m" : negative rotation around local z axis
"," : positive rotation around local z axis

## Other keyboard commands :

"a" : toggle display of coordinate axes, should display world and local object coordinate axes
"c" : snap camera to pointing at the world origin and pointing at the object at alternating order
"p" : toggle between perspective and orthogonal projection (already in the template)
"q" : exit program