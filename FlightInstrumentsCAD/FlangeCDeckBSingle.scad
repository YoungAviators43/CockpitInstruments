
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
    translate([0, 0, spacerHeight - 0.075])
      HexNut440(0, 0.875, 0);
}

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([0, 0.75, spacerHeight - 0.150])
      cylinder(h = 0.195, d = 0.125, center = false, $fn = 100);
}


union() {
  difference() {
    // Deck disk
    cylinder(h = deckHeight, d = 2.250 - 0.125 - 0.010, center = false, $fn = 250);

    // Add alignment notch
    translate([0, 2.250/2 - 0.130, 0])
      cube([0.135, 0.135 - 0.010/2, 1.000], center = true);

    // Cut center indicator actuator hole
    cylinder (h = 0.5, d=0.088 + 0.005, center=true, $fn = 50);

    // Cut left indicator actuator hole
    *translate([indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.088 + 0.005, center=true, $fn = 50);

    // Cut right indicator actuator hole
    *translate([-indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);
  }

  // Add center indicator gear bushing.
  translate([0, 0, deckHeight])
    GearBushing(bushingHeight + 0.150 - deckHeight, 0.088 + 0.005);

  // Add left indicator gear bushing
  *translate([indicatorOffset, 0, deckHeight])
    GearBushing(bushingHeight + 0.150 - deckHeight, 0.093 + 0.005);

  // Add right indicator gear bushing
  *translate([-indicatorOffset, 0, deckHeight])
    GearBushing(bushingHeight + 0.150 - deckHeight, 0.093 + 0.005);

  // Add drive gear bushing #1
  *translate([bushingOffsetX, bushingOffsetY, deckHeight])
    GearBushing(bushingHeight + 0.150 - deckHeight, 0.125 + 0.010);

  // Add drive gear bushing #2 (for dual indicator gauge)
  translate([-bushingOffsetX, -bushingOffsetY, deckHeight])
    GearBushing(bushingHeight + 0.150 - deckHeight, 0.125 + 0.010);

  // Add support walls.
  rotate(a=40, v=[0,0,1])
    difference() {
      cylinder(h = spacerHeight, d = 2.250 - 0.1750, center = false, $fn = 250);

      cylinder(h = 2, d = 2.2500 - 1.000, center = true, $fn = 250);

      rotate(a=-90, v=[0,0,1])
        cube([1.00, 4, 3], center = true);
      rotate(a=-45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);
      rotate(a=45, v=[0,0,1])
        cube([0.650, 4, 3], center = true);

      // Mounting screw nut 1
      MountingScrewNut(12);
      MountingScrewNut(-12);
      MountingScrewNut(180+12);
      MountingScrewNut(180-12);

      AlignmentPin(0);

      AlignmentPin(180+0);

    }

  // Add drive gears
  *translate([bushingOffsetX, bushingOffsetY, deckHeight + bushingHeight + 0.150 - deckHeight])
    cylinder(h = 0.375, d = 20.3/25.4, center = false, $fn = 20);

  *translate([-bushingOffsetX, -bushingOffsetY, deckHeight + bushingHeight + 0.150 - deckHeight])
    cylinder(h = 0.375, d = 20.3/25.4, center = false, $fn = 20);

  // Add indicator gears
  *translate([0, 0, deckHeight + bushingHeight + 0.150 - deckHeight])
    cylinder(h = 0.375, d = 13.93/25.4, center = false, $fn = 20);

  *translate([-indicatorOffset, 0, deckHeight + bushingHeight + 0.150 - deckHeight])
    cylinder(h = 0.375, d = 13.93/25.4, center = false, $fn = 20);

}
