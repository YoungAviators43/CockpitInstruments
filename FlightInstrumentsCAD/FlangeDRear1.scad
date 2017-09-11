module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([0.8, 0.8, 1.5]);
         cylinder(h = 1.5, r = 0.4, center=false, $fn = 150);
       }
}

/*
module HexNut440(x, y, rot)
{
  translate([x, y, 0 - 0.010])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.075 + 0.010, d = 0.210, center = false, $fn = 6);
        cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
      }
}
*/

module MountHole(x, y)
{
  translate([x, y, 0])
    cylinder(h = 1.0, d = 0.1875, center=true, $fn = 50);
}

union () {
  difference() {
    // Create main body.
    translate([0, 0, 0.125])
      cube([4.000, 4.000, 0.250], center = true);

    // Create center hole.
    cylinder(h = 1.0, d = 3.750 - 0.188 - 0.188, center=true, $fn = 250);

    // Round off the corners
    CornerRounder(2.0 - 0.4, 2.0 - 0.4, 0);
    CornerRounder(-(2.0 - 0.4), 2.0 - 0.4, 90);
    CornerRounder(-(2.0 - 0.4), -(2.0 - 0.4), 180);
    CornerRounder((2.0 - 0.4), -(2.0 - 0.4), 270);

    // Add holes for mounting screws
    MountHole(3.4/2, 3.4/2);
    MountHole(3.4/2, -3.4/2);
    MountHole(-3.4/2, -3.4/2);
    MountHole(-3.4/2, 3.4/2);

  }

  difference() {
    cylinder(h = 0.188/2 + 0.250, d = 3.750, center=false, $fn = 250);
    cylinder(h = 1.0, d = 3.750 - 0.188, center=true, $fn = 250);
  }

}
