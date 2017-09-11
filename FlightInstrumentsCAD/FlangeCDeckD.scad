deckHeight = 0.093;
servoElevation = 0.1750 + 0.125 - deckHeight;

module servoBodyMount(x, y, rot)
{
  translate([x, y, servoElevation])
    rotate(a = rot, v = [0, 0, 1])
      union () {
        // Servo body.  Add in mounting margin.
        translate([0.225, 0, 0])  // move origin to output spline
          cube([0.90, 0.475, 0.90], center=true);

        // Servo mounting arms
        translate([0.225, 0, 0.275])
          cube([1.275, 0.50, 0.10], center=true);
      }
}

junctionAdjust = 0.025;
module servoRail(x, y, rot)
{
  translate([x, y, 0])
    rotate(a = rot, v = [0, 0, 1])
      // move origin to output spline
      translate([0.225, 0, deckHeight + servoElevation/2 - junctionAdjust/2])
        difference() {
          cube([1.375, 0.425, servoElevation + junctionAdjust], center = true);
          cube([0.90, 0.75, 0.90], center=true);
        }
}

module servoScrew(x, y, rot)
{
  translate([x, y, 0])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        // Mounting screw holes
        translate([0.450-0.225-1.075/2, 0, 0])
          cylinder(h=2.0, d=0.075, center= true, $fn=15);
        translate([0.450-0.225+1.075/2, 0, 0])
          cylinder(h=2.0, d=0.075, center= true, $fn=15);

        // Mounting screw recess
        translate([0.450-0.225-1.075/2, 0, 0])
          cylinder(h = 0.300, d = 0.250, center = true, $fn=50);
        translate([0.450-0.225+1.075/2, 0, 0])
          cylinder(h = 0.300, d = 0.250, center = true, $fn=50);
        translate([0.450-0.225, 0, 0])
          cube([0.9 + 0.200, 0.250, 0.300], center = true);
      }
}

mountingDeckRadius = 0.925;
module DeckMountingScrewHole(rot)
{
  rotate([0, 0, rot])
    translate([0, mountingDeckRadius, 0])
      cylinder(h = 0.275, d = 0.125, center = true, $fn = 100);
}


offsetMarginMM = 0.25;
gearOffsetMM = 15.915 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
///indicatorOffset = 1.045;
indicatorOffset = 1.35/2;
servo1rotate = 45 + 55 + 180;
servo2rotate = 225 + 55 + 180;

// Calculate position of drive gear bushings.
driveAngle = asin(indicatorOffset/(2*gearOffsetIN));
servoSplineOffsetX = gearOffsetIN*sin(driveAngle);
servoSplineOffsetY = gearOffsetIN*cos(driveAngle);

module CheckInterference()
{
  // Servo spline indicators
  translate([servoSplineOffsetX, servoSplineOffsetY, 0.093])
    cylinder(h=1, d=0.125, center=true, $fn=50);

  translate([-servoSplineOffsetX, -servoSplineOffsetY, 0.093])
    cylinder(h=1, d=0.125, center=true, $fn=50);

  // show spacers to check for interference.
  rotate([0, 0, 40])
    translate([0, mountingDeckRadius, 0])
      cylinder(h = 1, d = 0.275, center = true, $fn = 6);

  rotate([0, 0, 40 - (90 + 20)])
    translate([0, mountingDeckRadius, 0.025])
      cylinder(h = 1, d = 0.275, center = true, $fn = 6);

  rotate([0, 0, 180 + 40])
    translate([0, mountingDeckRadius, 0.025])
      cylinder(h = 1, d = 0.275, center = true, $fn = 6);

  rotate([0, 0, 180 + 40 - (90 + 20)])
    translate([0, mountingDeckRadius, 0.025])
      cylinder(h = 1, d = 0.275, center = true, $fn = 6);
}


// Servo D deck
intersection()
{
  cylinder(h = 1, d = 2.25 - 0.125 - 0.010, center = true, $fn = 250);

  union() {
    difference() {
      union() {
        // Deck disk
        cylinder(h = deckHeight, d = 2.250 - 0.125 - 0.010, center = false, $fn=250);

        // Servo 1 placement
        servoRail(servoSplineOffsetX, servoSplineOffsetY, servo1rotate);

        // Servo 2 placement
        servoRail(-servoSplineOffsetX, -servoSplineOffsetY, servo2rotate);
      }

      DeckMountingScrewHole(40);
      DeckMountingScrewHole(40 - (90 + 20));
      DeckMountingScrewHole(180 + 40);
      DeckMountingScrewHole(180 + 40 - (90 + 20));

      // Servo 1 placement
      servoBodyMount(servoSplineOffsetX, servoSplineOffsetY, servo1rotate);
      servoScrew(servoSplineOffsetX, servoSplineOffsetY, servo1rotate);

      // Servo 2 placement
      servoBodyMount(-servoSplineOffsetX, -servoSplineOffsetY, servo2rotate);
      servoScrew(-servoSplineOffsetX, -servoSplineOffsetY, servo2rotate);

      // Lighteners
      rotate([0, 0, 10])
        translate([0, 0.75, 0])
          cylinder(h=1, d = 0.225, center = true, $fn = 25);

      rotate([0, 0, 10])
        translate([0, 0.25, 0])
          cylinder(h=1, d = 0.225, center = true, $fn = 25);

      rotate([0, 0, 10])
        translate([0, -0.25, 0])
          cylinder(h=1, d = 0.225, center = true, $fn = 25);

      rotate([0, 0, 10])
        translate([0, -0.75, 0])
          cylinder(h=1, d = 0.225, center = true, $fn = 25);
    }

    *CheckInterference();
  }

}