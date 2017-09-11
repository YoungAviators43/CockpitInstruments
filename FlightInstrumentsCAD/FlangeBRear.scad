module FlatHead(x, y, z)
{
  translate([x,y,z-0.085])
    union() {
      cylinder(h = 0.200, d1 = 0.125, d2 = 0.350, center = false, $fn = 100);
      translate([0, 0, -0.200])
        cylinder(h = 0.300, d = 0.125, center = false, $fn = 100);
  }
}

union() {
  difference() {
    linear_extrude(height = 0.200, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

    // cut center hole
    cylinder (h = 0.5, d=3.000, center=true, $fn = 250);

    // add mounting holes
    translate([1.25, 1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([-1.25, 1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([1.25, -1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([-1.25, -1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);

    // Add assembly screw holes
    FlatHead( 1, 2.75/2, 0.175);
    FlatHead(-1, 2.75/2, 0.175);
    FlatHead(1, -2.75/2, 0.175);
    FlatHead(-1, -2.75/2, 0.175);
    FlatHead(2.75/2, 1, 0.175);
    FlatHead(-2.75/2, 1, 0.175);
    FlatHead(2.75/2, -1, 0.175);
    FlatHead(-2.75/2, -1, 0.175);

  }

  // rim for rear mount
  translate([0, 0, 0.05])
    difference() {
      cylinder (h = 0.293 - 0.05, d=3.125, center = false, $fn = 200);
      cylinder (h = 0.5, d=3.000, center = true, $fn = 200);
  }

  // Add alignment notch
  translate([0, 3.000/2 - 0.125/2, 0.125])
    cube([0.125, 0.125, 0.250], center = true);

}
