deckHeight = 0.093;
servoElevation = 0.200 + (0.125 - deckHeight);
servoLength = 0.900;
servoScrewDistance = 1.075;
servoWidth = 0.500;
servoSplineOffset = 0.175;  // from servo end

module servoBodyMount(x, y, rot)
{
  translate([x, y, servoElevation])
    rotate([0, 0, rot])
      union () {
        // Servo body.  Add in mounting margin.
        translate([servoLength/2 - servoSplineOffset, 0, 0])  // move origin to output spline
          cube([servoLength, servoWidth + 0.025, 0.90], center=true);

        // Servo mounting arms
        *translate([servoLength/2 - servoSplineOffset, 0, 0.275])
          cube([1.275, 0.50, 0.10], center=true);
      }
}

junctionAdjustZ = 0.025;
module servoRail(x, y, rot)
{
  translate([x, y, 0])
    rotate([0, 0, rot])
      // move origin to output spline
      translate([servoLength/2 - servoSplineOffset, 0, deckHeight + servoElevation/2 - junctionAdjustZ/2])
        cube([1.450, 0.450, servoElevation + junctionAdjustZ], center = true);
}

module servoScrew(x, y, rot)
{
  translate([x, y, 0])
    rotate([0, 0, rot])
      union() {
        // Mounting screw holes
       translate([(servoLength-servoScrewDistance)/2 - servoSplineOffset, 0, 0])
          cylinder(h=2.0, d=0.075, center= true, $fn=15);
        translate([(servoLength+servoScrewDistance)/2 - servoSplineOffset, 0, 0])
          cylinder(h=2.0, d=0.075, center= true, $fn=15);

        // Mounting screw recess
        translate([(servoLength-servoScrewDistance)/2 - servoSplineOffset, 0, 0])
          cylinder(h = 0.300, d = 0.250, center = true, $fn=50);
        translate([(servoLength+servoScrewDistance)/2 - servoSplineOffset, 0, 0])
          cylinder(h = 0.300, d = 0.250, center = true, $fn=50);
        translate([servoLength/2 - servoSplineOffset, 0, 0])
          cube([servoLength + 0.200, 0.250, 0.300], center = true);
      }
}

offsetMarginMM = 0.25;
gearOffsetMM = 20.054 + offsetMarginMM;
gearOffsetIN = gearOffsetMM / 25.4;
indicatorOffset = 1.045;

// Calculate position of drive gear bushings.
driveAngle = asin(indicatorOffset/(2*gearOffsetIN));
servoSplineOffsetX = gearOffsetIN*sin(driveAngle);
servoSplineOffsetY = gearOffsetIN*cos(driveAngle);

// Servo D deck
rotate([0, 180, 0])
union() {
  difference() {
    union() {
      // Deck disk
      cylinder(h = deckHeight, d = 3.000, center = false, $fn=250);

      // Servo 1 placement
      servoRail(servoSplineOffsetX, -servoSplineOffsetY, 45);

      // Servo 2 placement
      servoRail(-servoSplineOffsetX, servoSplineOffsetY, 225);
    }

    // Add holes for connection to E deck.
    rotate(a = -40, v = [0, 0, 1])
      translate([0, 1.35, 0])
        cylinder(h=1, d=0.125, center=true, $fn=50);
    rotate(a = -40 - 90, v = [0, 0, 1])
      translate([0, 1.35, 0])
        cylinder(h=1, d=0.125, center=true, $fn=50);
    rotate(a = 180 - 40, v = [0, 0, 1])
      translate([0, 1.35, 0])
        cylinder(h=1, d=0.125, center=true, $fn=50);
    rotate(a = 180 - 40 - 90, v = [0, 0, 1])
      translate([0, 1.35, 0])
        cylinder(h=1, d=0.125, center=true, $fn=50);

    // Servo 1 placement
    servoBodyMount(servoSplineOffsetX, -servoSplineOffsetY, 45);
    servoScrew(servoSplineOffsetX, -servoSplineOffsetY, 45);

    // Servo 2 placement
    servoBodyMount(-servoSplineOffsetX, servoSplineOffsetY, 225);
    servoScrew(-servoSplineOffsetX, servoSplineOffsetY, 225);

    // Lighteners
    *rotate([0, 0, 20])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, -25])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, -65])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    *rotate([0, 0, 180+20])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 180-25])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 180-65])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, -45])
      translate([0, 0.75, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 180-45])
      translate([0, 0.75, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 0])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 180])
      translate([0, 1.2, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 0])
      translate([0, 0.40, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 180])
      translate([0, 0.40, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, 90])
      translate([0, 0.40, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

    rotate([0, 0, -90])
      translate([0, 0.40, 0])
        cylinder(h=1, d = 0.300, center = true, $fn = 25);

  }

 // Servo spline indicators
 *translate([servoSplineOffsetX, -servoSplineOffsetY, 0.093])
   cylinder(h=1, d=0.125, center=true, $fn=50);

 *translate([-servoSplineOffsetX, servoSplineOffsetY, 0.093])
   cylinder(h=1, d=0.125, center=true, $fn=50);


}
