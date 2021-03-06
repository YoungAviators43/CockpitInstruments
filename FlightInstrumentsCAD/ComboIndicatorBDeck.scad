
deckHeight = 0.150;
bushingHeight = 0.093;
offsetMarginMM = 0.25;
gearOffsetMM = 20.054 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
indicatorOffset = 1.045;

// Calculate position of drive gear bushings.
driveAngle = asin(indicatorOffset/(2*gearOffsetIN));
bushingOffsetX = gearOffsetIN*sin(driveAngle);
bushingOffsetY = gearOffsetIN*cos(driveAngle);

spacerHeight = deckHeight + bushingHeight + 0.375 + bushingHeight - 0.100;

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

// 440 flat head screw
module FlatHead(x, y, z)
{
  translate([x,y,z-0.085])
    union() {
      cylinder(h = 0.200, d1 = 0.125, d2 = 0.350, center = false, $fn = 100);
      translate([0, 0, -0.200])
        cylinder(h = 0.300, d = 0.125, center = false, $fn = 100);
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


module FlangeMountB()
{
  union() {
    difference() {
      linear_extrude(height = 0.200, center = false, convexity = 10, slices = 10)
         import(file = "FlangeAOutline2.dxf");

      // cut center hole
      cylinder (h = 0.5, d=3.000, center=true, $fn = 250);

      // add mounting holes
      translate([1.25, 1.25, 0])
        cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
      translate([-1.25, 1.25, 0])
        cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
      translate([1.25, -1.25, 0])
        cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
      translate([-1.25, -1.25, 0])
        cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);

      // Add assembly screw holes
      FlatHead( 1, 2.75/2, 0.175);
      FlatHead(-1, 2.75/2, 0.175);
      FlatHead(1, -2.75/2, 0.175);
      FlatHead(-1, -2.75/2, 0.175);
      FlatHead(2.75/2, 1, 0.175);
      FlatHead(-2.75/2, 1, 0.175);
      FlatHead(2.75/2, -1, 0.175);
      FlatHead(-2.75/2, -1, 0.175);
    }

    // rim for rear mount
    translate([0, 0, 0.05])
      difference() {
        cylinder (h = 0.293 - 0.05, d=3.125, center = false, $fn = 200);
        cylinder (h = 0.5, d=3.000, center = true, $fn = 200);
      }
  }
}

union() {
  FlangeMountB();

  difference() {
    // Deck disk
    cylinder(h = deckHeight, d = 3.000 + 0.010, center = false, $fn = 250);

    // Cut center indicator actuator hole
    *cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);

    // Cut left indicator actuator hole
    translate([indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);

    // Cut right indicator actuator hole
    translate([-indicatorOffset, 0, 0])
      cylinder (h = 0.5, d=0.063 + 0.005, center=true, $fn = 50);
  }

  // Add center indicator gear bushing.
  *translate([0, 0, deckHeight])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add left indicator gear bushing
  translate([indicatorOffset, 0, deckHeight])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add right indicator gear bushing
  translate([-indicatorOffset, 0, deckHeight])
    GearBushing(bushingHeight, 0.093 + 0.005);

  // Add drive gear bushing #1
  translate([bushingOffsetX, bushingOffsetY, deckHeight])
    GearBushing(bushingHeight, 0.125 + 0.010);

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
}
