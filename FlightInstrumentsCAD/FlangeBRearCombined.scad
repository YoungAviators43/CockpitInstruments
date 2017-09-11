module FlatHead(x, y, z)
{
  translate([x,y,z-0.075])
    union() {
      translate([0, 0, 0.085])
        cylinder(h = 0.200, d = 0.225, center = false, $fn = 100);
      cylinder(h = 0.085, d1 = 0.125, d2 = 0.225, center = false, $fn = 100);
      translate([0, 0, -0.200])
        cylinder(h = 0.300, d = 0.125, center = false, $fn = 100);
  }
}

module HexNut440(x, y, rot)
{
  rotate(a = rot, v = [0, 0, 1])
    translate([x,y,0 - 0.010])
      union() {
        rotate([0, 0, 30])
          cylinder(h = 0.075 + 0.010, d = 0.200, center = false, $fn = 6);
        cylinder(h = 0.500, d = 0.125, center = true, $fn = 100);
      }
}

module GearBushing(h, c)
{
  difference() {
    cylinder(h = h, d = 0.500, center=false, $fn = 150);
    cylinder(h = 1.0, d = c, center=true,  $fn = 150);
  }
}

module IndicatorGear(d, h1, h2)
{
  difference() {
    union() {
      cylinder(h = h1, d = d, center=false, $fn = 20);
      translate([0, 0, h1])
        cylinder(h = h2, d=d*2/3, center=false, $fn = 50);
    }
    cylinder(h = 1.000, d = 0.125, center=true,  $fn = 50);
  }
}

module DriveGear(d, h1, h2)
{
  difference() {
    union() {
      cylinder(h = h1, d = d, center=false, $fn = 20);
      translate([0, 0, h1])
        cylinder(h = h2, d=d*2/3, center=false, $fn = 50);
    }
    cylinder(h = 1.000, d = 0.125, center=true,  $fn = 150);
  }
}

union() {
  difference() {
    linear_extrude(height = 0.1, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

    /* cut center hole */
/*
    cylinder (h = 0.5, d=2.35, center=true, $fn = 250);
*/

    /* cut mounting holes */
    translate([1.25, 1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([-1.25, 1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([1.25, -1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);
    translate([-1.25, -1.25, 0])
       cylinder(h = 0.5, d = 0.188, center = true, $fn = 40);

    /* access for assembly screws */
    FlatHead(1, 2.75/2, 0.100);
    FlatHead(-1, 2.75/2, 0.100);
    FlatHead(1, -2.75/2, 0.100);
    FlatHead(-1, -2.75/2, 0.100);
    FlatHead(2.75/2, 1, 0.100);
    FlatHead(-2.75/2, 1, 0.100);
    FlatHead(2.75/2, -1, 0.100);
    FlatHead(-2.75/2, -1, 0.100);

    /* Front gear deck mounting holes and nuts */
/*
    HexNut440(2.627 / 2, 0, 0);
    HexNut440(2.627 / 2, 0, 45);
    HexNut440(2.627 / 2, 0, 90);
    HexNut440(2.627 / 2, 0, 135);
    HexNut440(2.627 / 2, 0, 180);
    HexNut440(2.627 / 2, 0, 225);
    HexNut440(2.627 / 2, 0, 270);
    HexNut440(2.627 / 2, 0, 315);
*/

    // Cut indicator actuator hole
    cylinder (h = 0.5, d=0.125, center=true, $fn = 50);
  }

  // rim for front gear deck
  translate([0, 0, 0.05])
    difference() {
      cylinder (h = 0.193 - 0.05, d=3.125, center = false, $fn = 200);
      cylinder (h = 0.5, d=3.000, center = true, $fn = 200);
  }

  // Add indicator gear bushing.
  translate([0, 0, 0.100])
    GearBushing(0.188, 0.125);

  // Add drive gear bushing 1.
  translate([20.054/25.4*sin(45), 20.054/25.4*cos(45), 0.100])
    GearBushing(0.188, 0.125);

  // Add drive gear bushing 2.
  translate([-20.054/25.4*sin(45), -20.054/25.4*cos(45), 0.100])
    GearBushing(0.188 + 0.188, 0.125);

}

  // Add gears
  translate([0, 0, 0.100+0.188+0.010])
   color([1,0,0])
     IndicatorGear(14.95/25.4, 0.188, 0.093);

  //               plate+bushing+fit+gear1+fit+gear2
  translate([0, 0, 0.100+0.188+0.010+0.188+0.093+0.010+0.188+0.093])
   color([1,0,0])
     rotate(a = 180, v=[1,0,0])
       IndicatorGear(14.95/25.4, 0.188, 0.093);

translate([20.054/25.4*sin(45), 20.054/25.4*cos(45), 0.100+0.188+0.010])
  color([0,0,1])
    DriveGear(28.32/25.4,0.188,0.188);

translate([-20.054/25.4*sin(45), -20.054/25.4*cos(45), 0.100+0.188+0.093+0.010+0.188+0.093+0.010+0.188])
 color([0,0,1])
   rotate(a = 180, v=[1,0,0])
     DriveGear(28.32/25.4,0.188,0.188);

// Gear deck with bushings.
translate([0,0,0.100+0.188+0.093+0.010+0.188+0.093+0.010+0.188+0.188+0.010+0.093])
  color([0,1,1])
    rotate(a=180, v=[1,0,0])
      union() {
        difference() {
          cylinder(h=0.093, d=3.000, center = false, $fn=150);
          translate([-20.054/25.4*sin(45), 20.054/25.4*cos(45), 0])
            cylinder(h=1, d=0.125, center=true, $fn=50);
          translate([20.054/25.4*sin(45), -20.054/25.4*cos(45), 0])
            cylinder(h=1, d=0.125, center=true, $fn=50);
        }

        translate([0, 0, 0.093])
          GearBushing(0.188, 0.125);

        translate([-20.054/25.4*sin(45), 20.054/25.4*cos(45), 0.093])
          GearBushing(0.188, 0.125);

        translate([20.054/25.4*sin(45), -20.054/25.4*cos(45), 0.093])
          GearBushing(0.188+0.188, 0.125);
      }
