deckHeight = 0.100;

module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([0.4, 0.4, 0.5]);
         cylinder(h = 1.5, r = 0.2, center=true, $fn = 150);
       }
}

module MountHole(x, y)
{
  translate([x, y, 0])
    cylinder(h = 1.0, d = 0.1875, center=true, $fn = 50);
}

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([1.125, 0, 0])
      cylinder(h = deckHeight + 0.075, d = 0.120, center = false, $fn = 16);
}

union () {
  difference() {
    // Create main body.
    translate([-1.2, -1.2, 0])
      cube([2.400, 2.400, deckHeight]);

    // Round off the corners
    CornerRounder(1.2 - 0.2, 1.2 - 0.2, 0);
    CornerRounder(-(1.2 - 0.2), 1.2 - 0.2, 90);
    CornerRounder(-(1.2 - 0.2), -(1.2 - 0.2), 180);
    CornerRounder((1.2 - 0.2), -(1.2 - 0.2), 270);

    // Create center hole.
    cylinder(h = 1.0, d = 2.250 - 0.075 - 0.170, center=true, $fn = 250);

    // Add holes for mounting screws
    MountHole(1.875/2, 1.875/2);
    MountHole(1.875/2, -1.875/2);
    MountHole(-1.875/2, -1.875/2);
    MountHole(-1.875/2, 1.875/2);

    // Cut assembly hex nut.
    translate([1.2 - 0.61, 1.2 - 0.15, 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([1.2 - 0.15, 1.2 - 0.61, 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([-(1.2 - 0.61), 1.2 - 0.15, 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([-(1.2 - 0.15), 1.2 - 0.61, 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([1.2 - 0.61, -(1.2 - 0.15), 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([1.2 - 0.15, -(1.2 - 0.61), 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([-(1.2 - 0.61), -(1.2 - 0.15), 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
    translate([-(1.2 - 0.15), -(1.2 - 0.61), 0])
      cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);

  }

  // Alignment pins
  AlignmentPin(45);
  AlignmentPin(135);
  AlignmentPin(225);
  AlignmentPin(315);
}
