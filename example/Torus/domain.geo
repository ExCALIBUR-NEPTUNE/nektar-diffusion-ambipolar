// Triangle mesh of a doughnut
//
// Run: gmsh -2 -order n Circular.geo

// Points
Point(1) = {0,0,0,1.0};
Point(2) = {1,0,0,1.0};
Point(3) = {-1,0,0,1.0};
Point(4) = {-2,0,0,1.0};
Point(5) = {2,0,0,1.0};
Point(6) = {2*Cos(60*Pi/180),-2*Sin(60*Pi/180),0,1.0};
Point(7) = {-2*Cos(60*Pi/180),-2*Sin(60*Pi/180),0,1.0};
Point(8) = {-Cos(60*Pi/180),-Sin(60*Pi/180),0,1.0};
Point(9) = {Cos(60*Pi/180),-Sin(60*Pi/180),0,1.0};
// Line Definitions
Circle(1) = {4, 1, 7};
Line(2) = {4, 3};
Line(3) = {2, 5};
Circle(4) = {5, 1, 6};
Circle(5) = {6, 1, 7};
Line(6) = {6, 9};
Line(7) = {7, 8};
Circle(8) = {3, 1, 8};
Circle(9) = {8, 1, 9};
Circle(10) = {9, 1, 2};
// Line Loops
Line Loop(13) = {2, 8, -7, -1};
Plane Surface(14) = {13};
Line Loop(15) = {10, 3, 4, 6};
Plane Surface(16) = {15};
Line Loop(17) = {9, -6, 5, 7};
Plane Surface(18) = {17};
// Physical Groups
Physical Line(1) = {8, 9, 10};
Physical Line(2) = {2};
Physical Line(3) = {3};
Physical Line(4) = {1, 5, 4};
// Characteristic Length {1, 6, 7, 8} = 1.0;
Physical Surface("Plasma") = {14, 16, 18};
// Structured Mesh
Transfinite Line {2, 7, 6, 3} = 30 Using Progression 1;
Transfinite Line {1, 8, 4, 10} = 50 Using Progression 1;
Transfinite Line {5, 9} = 50 Using Progression 1;
Transfinite Surface {14} = {4, 3, 8, 7};
Transfinite Surface {18} = {8, 9, 6, 7};
Transfinite Surface {16} = {9, 2, 5, 6};
Recombine Surface {14, 16, 18};
