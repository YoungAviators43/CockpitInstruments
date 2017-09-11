deckHeight = 0.150;

module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([0.4, 0.4, 0.5]);
         cylinder(h = 1.5, r = 0.2, center=true, $fn = 150);
       }
}

module HexNut256(x, y, rot)
{
  translate([x, y, 0 - 0.010])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.066 + 0.025, d = 0.165 * 1.15, center = false, $fn = 6);
        cylinder(h = 0.265, d = 0.093, center = true, $fn = 100);
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
    translate([1.125, 0, -0.020])
      cylinder(h = 0.120, d = 0.135, center = false, $fn = 100);
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

    // Add holes for mounting screws
    MountHole(1.875/2, 1.875/2);
    MountHole(1.875/2, -1.875/2);
    MountHole(-1.875/2, -1.875/2);
    MountHole(-1.875/2, 1.875/2);

    // Create center hole.
    cylinder(h = 1.0, d = 2.250 - 0.075 - 0.170, center=true, $fn = 250);

    // Cut assembly hex nut.
    HexNut256(1.2 - 0.61, 1.2 - 0.15, 0);
    HexNut256(1.2 - 0.15, 1.2 - 0.61, 30);
    HexNut256(-(1.2 - 0.61), 1.2 - 0.15, 0);
    HexNut256(-(1.2 - 0.15), 1.2 - 0.61, 30);
    HexNut256(1.2 - 0.61, -(1.2 - 0.15), 0);
    HexNut256(1.2 - 0.15, -(1.2 - 0.61), 30);
    HexNut256(-(1.2 - 0.61), -(1.2 - 0.15), 0);
    HexNut256(-(1.2 - 0.15), -(1.2 - 0.61), 30);

    // Alignment pins
    AlignmentPin(45);
    AlignmentPin(135);
    AlignmentPin(225);
    AlignmentPin(315);
  }

  // Bezel mounting ring.
  translate([0, 0, deckHeight - 0.005])
    difference() {
      cylinder (h = 0.085 + 0.005, d=2.250, center = false, $fn = 250);
      cylinder (h = 1.0, d=2.250 - 0.075, center = true, $fn = 250);
  }

}
