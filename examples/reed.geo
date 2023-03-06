// solve the reed 1d benchmark
//
//     |         |    |         |    |         |
//  m  | src= 50 | 0  |    0    | 1  |    0    |    v
//  i  |         |    |         |    |         |    a
//  r  | tot= 50 | 5  |    0    | 1  |    1    |    c
//  r  |         |    |         |    |         |    u
//  o  | scat=0  | 0  |    0    | 0.9|   0.9   |    u
//  r  |         |    |         |    |         |    m
//     |         |    |         |    |         |
//     |    1    | 2  |    3    | 4  |    5    |
//     |         |    |         |    |         |
//     +---------+----+---------+----+---------+-------> x
//    x=0       x=2  x=3       x=5  x=6       x=8   

lc0 = 0.25;
f = 0.1;

Point(1) = {0, 0, 0, f*lc0};
Point(2) = {2, 0, 0, f*lc0};
Point(3) = {3, 0, 0, f*lc0};
Point(4) = {5, 0, 0, f*lc0};
Point(5) = {6, 0, 0, f*lc0};
Point(6) = {8, 0, 0, f*lc0};

Point(11) = {1,   0, 0, lc0};
Point(12) = {2.5, 0, 0, lc0};
Point(13) = {4,   0, 0, lc0};
Point(14) = {5.5, 0, 0, lc0};
Point(15) = {7,   0, 0, lc0};

Line(1) = {1, 11};
Line(11) = {11, 2};
Line(2) = {2, 12};
Line(12) = {12, 3};
Line(3) = {3, 13};
Line(13) = {13, 4};
Line(4) = {4, 14};
Line(14) = {14, 5};
Line(5) = {5, 15};
Line(15) = {15, 6};


Physical Line("source1")   = {1,11};
Physical Line("absorber")  = {2,12};
Physical Line("void")      = {3,13};
Physical Line("source2")   = {4,14};
Physical Line("reflector") = {5,15};

Physical Point("left")  = {1};
Physical Point("right") = {6};
