
deckHeight = 0.093;
bushingHeight = 0.093;
offsetMarginMM = 0.25;
gearOffsetMM = 20.054 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;

bushingOffsetX = gearOffsetIN*sin(45);
bushingOffsetY = gearOffsetIN*cos(45);

gearHeight = 0.375;
lightenerDiameter = 0.275;

module GearBushing(h1, c)
{
  difference() {
    cylinder(h = h1, d = 0.450, center=false, $fn = 150);
    cylinder(h = (h1 * 2) + 0.250, d = c, center=true,  $fn = 150);
  }
}

module HexNut440(x, y, rot)
{
  translate([x, y, 0 - 0.020])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.075 + 0.025, d = 0.200 * 1.15, center = false, $fn = 6);
        cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 50);
      }
}

module FlatHead(x, y, z)
{
  translate([x,y,z-0.085])
    rotate([180, 0, 0])
      union() {
        cylinder(h = 0.200, d1 = 0.125, d2 = 0.350, center = false, $fn = 100);
        translate([0, 0, -0.200])
          cylinder(h = 0.300, d = 0.125, center = false, $fn = 100);
  }
}

module MountingScrewNut(rot)
{
  rotate(a = rot, v = [0, 0, 1])
    FlatHead(0, 1.2, deckHeight);
}

// Gear C deck with bushings.
union() {
  difference() {
    // Deck disk
    cylinder(h=deckHeight, d=3.000, center = false, $fn=250);

    // Hole for servo drive shaft #1.
    translate([-bushingOffsetX, bushingOffsetY, deckHeight])
      cylinder(h = 0.5, d = 0.125 + 0.005, center=true,  $fn = 50);

    // Hole for servo drive shaft #2.
    translate([bushingOffsetX, -bushingOffsetY, deckHeight])
      cylinder(h = 0.5, d = 0.125 + 0.005, center=true,  $fn = 50);

    // Add holes for connection to B deck.
    MountingScrewNut(180 - (-15 + 40));
    MountingScrewNut(180 - (15+40));
    MountingScrewNut(180 - (180+15+ 40));
    MountingScrewNut(180 - (180-15 + 40));

    // Add hexnut mounting to D deck.
    rotate(a = -40, v = [0, 0, 1])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, 1.35, 0);
    rotate(a = -40 - 90, v = [0, 0, 1])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, 1.35, 0);
    rotate(a = 180 - 40, v = [0, 0, 1])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, 1.35, 0);
    rotate(a = 180 - 40 - 90, v = [0, 0, 1])
      translate([0, 0, deckHeight - 0.040])
        HexNut440(0, 1.35, 0);

    // Lighteners
    rotate([0, 0, 0])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 25])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 75])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 100])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 75 - 180])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 100 - 180])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 180])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 205])
      translate([0, 1.2, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 180])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 0])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 135])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 135 - 180])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 90])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);

    rotate([0, 0, 270])
      translate([0, 0.6, 0])
        cylinder(h=1, d = lightenerDiameter, center = true, $fn = 25);


 }

 // Indicator shaft bushing
 translate([0, 0, deckHeight - 0.025])
   GearBushing(bushingHeight + 0.025, 0.093);

 // Drive shaft #1 bushing
 translate([-bushingOffsetX, bushingOffsetY, deckHeight])
   GearBushing(bushingHeight + gearHeight, 0.125);

 // Drive shaft #2 bushing
 translate([bushingOffsetX, -bushingOffsetY, deckHeight])
   GearBushing(bushingHeight, 0.125);
}
