// Gmsh project created on Fri Aug  20 10:29:04 2021
Point(1) = {0, 0, 0, 1.0};
Point(2) = {5, 0, 0, 1.0};
Point(3) = {5, 0.2, 0, 1.0};
Point(4) = {0, 0.2, 0, 1.0};
Line(1) = {1, 2};
Line(2) = {2, 3};
Line(3) = {3, 4};
Line(4) = {4, 1};
Transfinite Line {1,3} = 641;
Transfinite Line {2,4} = 41;
Line Loop(5) = {4, 1, 2, 3};
Plane Surface(6) = {5};
Transfinite Surface {6} Alternated;
Recombine Surface {6};

Physical Line("1") = {4};
Physical Line("2") = {3};
Physical Line("3") = {2};
Physical Line("4") = {1};
Physical Surface("Regions") = {6};