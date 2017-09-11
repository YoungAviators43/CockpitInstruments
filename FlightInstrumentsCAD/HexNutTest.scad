module HexNut440(x, y, scale)
{
  rotate(a = rot, v = [0, 0, 1])
    translate([x, y, 0 - 0.010])
      union() {
        cylinder(h = 0.075 + 0.010, d = 0.200 * scale, center = false, $fn = 6);
        cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
      }
}

module HexNut256(x, y, scale)
{
  translate([x, y, 0 - 0.010])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.066 + 0.010, d = 0.165 * scale, center = false, $fn = 6);
        cylinder(h = 0.188 * 2, d = 0.093, center = true, $fn = 100);
      }
}

difference() {
    // Create test block.
    translate([-1.0, -0.5, 0])
      cube([2.000, 1.000, 0.125]);

    HexNut256(- 0.50, - 0.25, 1.05);
    HexNut256(- 0.25, - 0.25, 1.10);
    HexNut256(- 0.00, - 0.25, 1.15);
    HexNut256(  0.25, - 0.25, 1.20);
    HexNut256(  0.50, - 0.25, 1.25);
    HexNut256(  0.75, - 0.25, 1.30);

    HexNut440(- 0.75,   0.25, 1.05);
    HexNut440(- 0.50,   0.25, 1.10);
    HexNut440(- 0.20,   0.25, 1.15);
    HexNut440(  0.10,   0.25, 1.20);
    HexNut440(  0.45,   0.25, 1.25);
    HexNut440(  0.80,   0.25, 1.30);
}