
deckHeight = 0.063;
spacerCapHeight = 0.100;
bushingHeight = 0.093;
offsetMarginMM = 0.25;
gearOffsetMM = 15.915 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
///indicatorOffset = 1.045;
indicatorOffset = 1.35/2;

// Calculate position of drive gear bushings.
driveAngle = asin(indicatorOffset/(2*gearOffsetIN));
bushingOffsetX = gearOffsetIN*sin(driveAngle);
bushingOffsetY = gearOffsetIN*cos(driveAngle);

spacerHeight = deckHeight + 0.150 - deckHeight + bushingHeight + 0.375 + bushingHeight - spacerCapHeight;

module HexNut440(x, y, rot)
{
  translate([x, y, 0 - 0.020])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.075 + 0.025, d = 0.200 * 1.15, center = false, $fn = 6);
        cylinder(h = 0.275, d = 0.125, center = true, $fn = 100);
      }
}

module MountingScrewNut(rot)
{
  rotate(a = rot, v = [0, 0, 1])
    translate([0, 0.875, 0])
      cylinder(h = 0.500, d = 0.125, center = true, $fn = 50);
}

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([0, 0.75, spacerCapHeight - 0.010])
      cylinder(h = 0.093, d = 0.125, center = false, $fn = 100);
}


// Support wall caps.
translate([0, -1, 0])
  union() {
    difference() {
      cylinder(h = spacerCapHeight, d = 2.250 - 0.1750, center = false, $fn = 250);

      cylinder(h = 2, d = 2.2500 - 1.000, center = true, $fn = 250);

      rotate(a=-90, v=[0,0,1])
        cube([1.00, 4, 3], center = true);
      rotate(a=-45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);
      rotate(a=45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);

      translate([0, -1, 0]) cube([2,2,2], center = true);

      // Relief for spacer screw.
      translate([0, 0.925, 0])
        cylinder(h=0.250, d=0.130, center=true, $fn=50);

      // Mounting screw nut 1
      MountingScrewNut(12);
      MountingScrewNut(-12);
    }
    AlignmentPin(0);
}
