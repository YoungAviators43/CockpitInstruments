
module MountingScrewNut(rot)
{
  rotate(a = rot, v = [0, 0, 1])
    union() {
      // Nut access
      cube(size = [0.210, 0.350, 0.075], center = true);

      // Screw shaft
      cylinder(h = 0.225, d = 0.125, center = true, $fn = 50);
    }
}

union() {
  difference() {
    linear_extrude(height = 0.25, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

    // cut center hole
    cylinder (h = 1.0, d=2.850, center=true, $fn = 250);

    // cut panel mounting holes
    translate([1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([1.25, -1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.25, -1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);

    // assembly nuts
    translate([1, 2.75/2, 0.1])
      MountingScrewNut(-22);
    translate([-1, 2.75/2, 0.1])
      MountingScrewNut(22);
    translate([1, -2.75/2, 0.1])
      MountingScrewNut(22);
    translate([-1, -2.75/2, 0.1])
      MountingScrewNut(-22);
    translate([2.75/2, 1, 0.1])
      MountingScrewNut(-67);
    translate([-2.75/2, 1, 0.1])
      MountingScrewNut(67);
    translate([2.75/2, -1, 0.1])
      MountingScrewNut(67);
    translate([-2.75/2, -1, 0.1])
      MountingScrewNut(-67);
  }

  // rim for bezel glass
  translate([0, 0, 0.05])
    difference() {
      cylinder (h = 0.188/2 + 0.250 - 0.05, d=3.125, center = false, $fn = 200);
      cylinder (h = 1.0, d=3.000, center = true, $fn = 200);
  }

}

