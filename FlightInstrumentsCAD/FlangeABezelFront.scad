deckHeight = 0.150;

module HexNut440(x, y, rot)
{
  translate([x, y, 0 - 0.020])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.075 + 0.025, d = 0.200 * 1.15, center = false, $fn = 6);
        cylinder(h = 0.275, d = 0.125, center = true, $fn = 100);
      }
}

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([1.6, 0, -0.020])
      cylinder(h = 0.120, d = 0.135, center = false, $fn = 100);
}

union() {
  difference() {
    linear_extrude(height = deckHeight, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline.dxf");

    // cut center hole
    cylinder (h = 1.0, d=2.850, center=true, $fn = 250);

    // cut panel mounting holes
    translate([1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.25, 1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([1.25, -1.25, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
    translate([-1.325, -1.325, 0])
       cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);

    // assembly nuts
    HexNut440( 1,       2.75/2,   37);
    HexNut440(-1,       2.75/2,  -37);
    HexNut440( 1,      -2.75/2,   27);
    HexNut440(-1,      -2.75/2,  -27);
    HexNut440( 2.75/2,  1.000,    -7);
    HexNut440(-2.75/2,  1.000,     7);
    HexNut440( 2.75/2, -1.000,     7);
    HexNut440(-2.75/2, -1.000,    -7);

    // Alignment pins
    AlignmentPin(25);
    AlignmentPin(65);
    AlignmentPin(25 + 90);
    AlignmentPin(65 + 90);
    AlignmentPin(25+ 180);
    AlignmentPin(65 + 180);
    AlignmentPin(25 + 270);
    AlignmentPin(65+ 270);
  }

  // 12 teeth
  translate([-1.325, -1.325, -0.225])
    cylinder(h = 0.125, d = 10.65/25.4, center = true, $fn = 40);
  // 32 teeth
  translate([-0.85, -0.85, -0.225])
    cylinder(h = 0.125, d = 26.56/25.4, center = true, $fn = 40);
  // 48 teeth
  translate([0, 0, -0.225])
    difference() {
      cylinder(h = 0.125, d = 39.30/25.4, center = true, $fn = 40);
      cylinder(h = 1, d = 0.5, center = true, $fn = 40);
    }

  // rim for bezel glass
  translate([0, 0, deckHeight - 0.005])
    difference() {
      cylinder (h = 0.188/2 + 0.005, d=3.125, center = false, $fn = 250);
      cylinder (h = 1.0, d=3.005, center = true, $fn = 200);
  }

}

