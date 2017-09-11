// Altimeter B deck

// gear height (all): 0.188 in
// 8-tooth gear dia.: 7.470 mm 0.294 in
// 32-tooth gear dia.: 26.56 mm 1.046 in
// 40-tooth gear dia.: 32.92 mm 1.296 in
// 16-tooth gear dia.: 13.83 mm 0.544 in

deckHeight = 0.100;
bushingHeight = 0.093;
offsetMarginMM = 0.25;
shaftSpacingMM = 19.099;
gearOffsetMM = shaftSpacingMM + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
gearHeight = 0.125;

// Calculate position of drive gear bushings.
driveAngle = 45;
bushingOffsetX = gearOffsetIN*sin(driveAngle);
bushingOffsetY = gearOffsetIN*cos(driveAngle);

spacerHeight = deckHeight + bushingHeight + gearHeight * 4 + bushingHeight - 0.100;

module gear(dia, x, y, z)
{
  translate([x, y, z])
    cylinder(h = gearHeight, d = dia, center=false, $fn =16);
}

module gear8(x, y, z)
{
  gear(0.294, x, y, z);
}

module gear16(x, y, z)
{
  gear(0.544, x, y, z);
}

module gear32(x, y, z)
{
  gear(1.046, x, y, z);
}

module gear40(x, y, z)
{
  gear(1.296, x, y, z);
}

module gears()
{
  gear40(0, 0, deckHeight + bushingHeight);
  gear16(0, 0, deckHeight + bushingHeight + gearHeight);
  gear40(0, 0, deckHeight + bushingHeight + gearHeight * 2);
  gear16(0, 0, deckHeight + bushingHeight + gearHeight * 3);

  gear8(-bushingOffsetX, -bushingOffsetY, deckHeight + bushingHeight);
  gear32(-bushingOffsetX, -bushingOffsetY, deckHeight + bushingHeight + gearHeight);
  gear8(-bushingOffsetX, -bushingOffsetY, deckHeight + bushingHeight + gearHeight * 2);
  gear32(-bushingOffsetX, -bushingOffsetY, deckHeight + bushingHeight + gearHeight * 3);
}

module GearBushing(h, c)
{
  difference() {
    cylinder(h = h, d = 0.450, center=false, $fn = 150);
    cylinder(h = h * 2 + 0.250, d = c, center=true,  $fn = 150);
  }
}

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
    translate([0, 0, deckHeight + bushingHeight + 0.375 + bushingHeight - 0.175])
      HexNut440(0, 1.2, 0);
}

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([0, 1.30, spacerHeight - 0.120])
      cylinder(h = 0.125, d = 0.135, center = false, $fn = 100);
}

union() {
  difference() {
    // Deck disk
    cylinder(h = deckHeight, d = 3.000 - 0.010, center = false, $fn = 250);

    // Add alignment notch
    translate([0, 3.000/2 - 0.135/2, 0])
      cube([0.135, 0.135 - 0.010/2, 1.000], center = true);

    // Cut center indicator actuator hole
    cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);
  }

  // Add center indicator gear bushing.
  translate([0, 0, deckHeight])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add drive gear bushing #2 (for dual indicator gauge)
  translate([-bushingOffsetX, -bushingOffsetY, deckHeight])
    GearBushing(bushingHeight, 0.125 + 0.010);

  // Add support walls.
  rotate(a=40, v=[0,0,1])
    difference() {
      cylinder(h = spacerHeight, d = 3.000 - 0.150, center = false, $fn = 250);

      cylinder(h = 2, d = 3.000 - 1.000, center = true, $fn = 250);

      rotate(a=-90, v=[0,0,1])
        cube([2.00, 4, 3], center = true);
      rotate(a=-45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);
      rotate(a=45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);

      // Mounting screw nut 1
      MountingScrewNut(15);
      MountingScrewNut(-15);
      MountingScrewNut(180+15);
      MountingScrewNut(180-15);

      AlignmentPin(-25);
      AlignmentPin(0);
      AlignmentPin(+25);

      AlignmentPin(180-25);
      AlignmentPin(180+0);
      AlignmentPin(180+25);

    }

    gears();
}
