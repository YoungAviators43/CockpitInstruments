
cornerRadius = 0.100;

module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([cornerRadius + 0.010, cornerRadius + 0.010, 0.2]);
         cylinder(h = 0.5, r = cornerRadius, center=true, $fn = 150);
       }
}

module needleBadge(x)
{
  translate([x, 0, 0.065/2])
    difference() {
      cube([0.375, 0.250, 0.065], center = true);

      CornerRounder(0.375/2 - cornerRadius, 0.250/2 - cornerRadius, 0);
      CornerRounder(-0.375/2 + cornerRadius, 0.250/2 - cornerRadius, 90);
      CornerRounder(-0.375/2 + cornerRadius, -0.250/2 + cornerRadius, 180);
      CornerRounder(0.375/2 - cornerRadius, -0.250/2 + cornerRadius, 270);
   }
}

union() {
  difference() {
    union() {
      translate([0.525/2, 0, 0.065/2])
        cube([1.825, 0.125, 0.065], center = true);

      cylinder(h = 0.110, d = 0.300, center = false, $fn = 100);

      translate([-0.575, 0, 0])
        cylinder(h = 0.075, d = 0.295, center = false, $fn = 100);
    }

    cylinder(h = 0.500, d = 0.075, center = true, $fn = 50);

    translate([1.825 - 0.525 - .1, 0, 0])
      rotate(a=75, v=[0,0,1])
        translate([0.5, 0, 0])
          cube([1, 1, 1], center = true);

    translate([1.825 - 0.525 - .1, 0, 0])
      rotate(a=-75, v=[0,0,1])
        translate([0.5, 0, 0])
          cube([1, 1, 1], center = true);

  }

  needleBadge(0.45);
}