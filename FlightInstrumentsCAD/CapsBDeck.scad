
deckHeight = 0.150;
bushingHeight = 0.093;
offsetMarginMM = 0.50;
gearOffsetMM = 20.054 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;

bushingOffsetX = gearOffsetIN*sin(45);
bushingOffsetY = gearOffsetIN*cos(45);
indicatorOffset = sqrt(2 * gearOffsetIN * gearOffsetIN);

spacerHeight = deckHeight + bushingHeight + 0.375 + bushingHeight - 0.100;

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([0, 1.30, 0.100 - 0.010])
      cylinder(h = 0.125, d = 0.120, center = false, $fn = 100);
}

module MountingScrewNut(rot)
{
  rotate(a = rot, v = [0, 0, 1])
    translate([0, 1.2, 0])
      cylinder(h = 0.500, d = 0.125, center = true, $fn = 50);
}

// Support walls caps.
translate([0, -1, 0])
  difference() {
    union() {
      cylinder(h = 0.100, d = 3.000 - 0.150, center = false, $fn = 250);
      AlignmentPin(-25);
      AlignmentPin(0);
      AlignmentPin(+25);
    }

    cylinder(h = 2, d = 3.000 - 1.000, center = true, $fn = 250);

    rotate(a=-90, v=[0,0,1])
      cube([2.00, 4, 3], center = true);
    rotate(a=-45, v=[0,0,1])
      cube([0.650, 4, 3], center = true);
    rotate(a=45, v=[0,0,1])
      cube([0.650, 4, 3], center = true);

    translate([0, -1, 0]) cube([2,2,2], center = true);

    MountingScrewNut(15);
    MountingScrewNut(-15);

    // Relief for spacer screw.
    translate([0, 1.35, 0])
      cylinder(h=0.50, d=0.130, center=true, $fn=50);
  }
