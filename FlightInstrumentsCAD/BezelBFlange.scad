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

union() {
  difference() {
    linear_extrude(height = 0.1, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

    /* cut center hole */
    cylinder (h = 0.5, d=2.35, center=true, $fn = 250);

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
    HexNut440(2.627 / 2, 0, 0);
    HexNut440(2.627 / 2, 0, 45);
    HexNut440(2.627 / 2, 0, 90);
    HexNut440(2.627 / 2, 0, 135);
    HexNut440(2.627 / 2, 0, 180);
    HexNut440(2.627 / 2, 0, 225);
    HexNut440(2.627 / 2, 0, 270);
    HexNut440(2.627 / 2, 0, 315);

  }

  /* rim for front gear deck */
  translate([0, 0, 0.05])
    difference() {
      cylinder (h = 0.193 - 0.05, d=3.125, center = false, $fn = 200);
      cylinder (h = 0.5, d=3.000, center = true, $fn = 200);
  }

}
