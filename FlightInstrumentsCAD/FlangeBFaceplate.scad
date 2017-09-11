union() {
  difference() {
    linear_extrude(height = 0.175, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

    // cut center hole
    cylinder (h = 1.0, d=3.000, center=true, $fn = 250);

    // cut panel mounting holes
    translate([1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([1.25, -1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.25, -1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);

    // assembly screw holes
    translate([1, 2.75/2, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([-1, 2.75/2, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([1, -2.75/2, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([-1, -2.75/2, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([2.75/2, 1, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([-2.75/2, 1, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([2.75/2, -1, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
    translate([-2.75/2, -1, 0])
       cylinder(h = 0.5, d = 0.125, center = true, $fn = 40);
  }

  // Add faceplate key (for notch)
  translate([-1.5, 0, 0])
     cube([0.075, 0.075, 0.175]);
}

