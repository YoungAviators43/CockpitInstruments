
deckHeight = 0.093;
bushingHeight = 0.093 + deckHeight - 0.005;
offsetMarginMM = 0.25;
gearOffsetMM = 15.915 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
///indicatorOffset = 1.045;
indicatorOffset = 1.35/2;
lightenerDiameter = 0.225;
hexDeckRadius = 0.925;

// Calculate position of drive gear bushings.
driveAngle = asin(indicatorOffset/(2*gearOffsetIN));
bushingOffsetX = gearOffsetIN*sin(driveAngle);
bushingOffsetY = gearOffsetIN*cos(driveAngle);

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

module FlatHead(x, y, z)
{
  translate([x,y,z-0.05])
    rotate([180, 0, 0])
      union() {
        cylinder(h = 0.200, d1 = 0.125, d2 = 0.350, center = false, $fn = 100);
        translate([0, 0, -0.200])
          cylinder(h = 0.300, d = 0.125, center = false, $fn = 100);
  }
}

module MountingScrewHole(rot)
{
  rotate(a = rot - 40, v = [0, 0, 1])
    FlatHead(0, 0.875, deckHeight);
}

couplerHeight = 0.975 - 0.700 + 0.150;
module ServoCoupler()
{
  rotate([0, 180, 0])
    difference() {
      // Coupler barrel
      cylinder(h = couplerHeight, d = 0.700, center = false, $fn = 150);

      // Center hole
      cylinder(h = 2.0, d = 0.123, center = true, $fn = 100);
  }
}


module CheckInterference()
{
  // Show servo couplers to check for interference.
  translate([-bushingOffsetX, bushingOffsetY, - couplerHeight - 0.025])
    scale([1, 1, -1])
      ServoCoupler();
  translate([bushingOffsetX, -bushingOffsetY, - couplerHeight - 0.025])
    scale([1, 1, -1])
      ServoCoupler();

  // show spacers to check for interference.
  scale([1, 1, -1])
    rotate([0, 0, -40])
      translate([0, hexDeckRadius, 0.025])
         cylinder(h = 0.5, d = 0.275, center = false, $fn = 50);

  scale([1, 1, -1])
    rotate([0, 0, -40 - 90 + 20])
      translate([0, hexDeckRadius, 0.025])
         cylinder(h = 0.5, d = 0.275, center = false, $fn = 50);

  scale([1, 1, -1])
    rotate([0, 0, 180 - 40])
      translate([0, hexDeckRadius, 0.025])
         cylinder(h = 0.5, d = 0.275, center = false, $fn = 50);

  scale([1, 1, -1])
    rotate([0, 0, 180 - 40 - 90 + 20])
      translate([0, hexDeckRadius, 0.025])
         cylinder(h = 0.5, d = 0.275, center = false, $fn = 50);
}


union() {
  difference() {
    // Deck disk
    cylinder(h = deckHeight, d = 2.250 - 0.125 - 0.010, center = false, $fn = 250);

    // Cut center indicator actuator hole
    *cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);

    // Cut left indicator actuator hole
    *translate([indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);

    // Cut right indicator actuator hole
    *translate([-indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);

    // Cut left drive gear hole
    translate([-bushingOffsetX, bushingOffsetY, 0])
      cylinder (h = 0.5, d=0.125 + 0.010, center=true, $fn = 50);

    // Cut right drive gear hole
    translate([bushingOffsetX, -bushingOffsetY, deckHeight])
      cylinder (h = 0.5, d=0.125 + 0.010, center=true, $fn = 50);

    // Cut screw holes for mounting to B deck.
    MountingScrewHole(12);
    MountingScrewHole(-12);
    MountingScrewHole(180+12);
    MountingScrewHole(180-12);

    // Add hexnut mounting to D deck.
    rotate([0, 0, -40])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, hexDeckRadius, 0);
    rotate([0, 0, -40 - 90 + 20])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, hexDeckRadius, 0);
    rotate([0, 0, 180 - 40])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, hexDeckRadius, 0);
    rotate([0, 0, 180 - 40 - 90 + 20])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, hexDeckRadius, 0);

    // Lighteners
    rotate([0, 0, -10])
      translate([0, 0.475, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);
    rotate([0, 0, -50])
      translate([0, 0.475, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);
    rotate([0, 0, -10 + 180])
      translate([0, 0.475, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);
    rotate([0, 0, -50 + 180])
      translate([0, 0.475, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);
  }

  // Add center indicator gear bushing.
  GearBushing(bushingHeight, 0.093 + 0.005);

  // Add left indicator gear bushing
  translate([indicatorOffset, 0, 0])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add right indicator gear bushing
  translate([-indicatorOffset, 0, 0])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add drive gear bushing #1
  translate([-bushingOffsetX, bushingOffsetY, 0])
    GearBushing(bushingHeight, 0.125 + 0.010);

  // Add drive gear bushing #2 (for dual indicator gauge)
  translate([bushingOffsetX, -bushingOffsetY, 0])
    GearBushing(bushingHeight, 0.125 + 0.010);

  *CheckInterference();
}
