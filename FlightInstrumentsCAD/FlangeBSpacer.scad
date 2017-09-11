deckHeight = 0.100;

module AlignmentPin(rot)
{
  rotate(a = rot, v=[0,0,1])
    translate([1.6, 0, 0])
      cylinder(h = deckHeight + 0.075, d = 0.127, center = false, $fn = 16);
}

difference() {
  union() {
    // Basic frame
    linear_extrude(height = deckHeight, center = false, convexity = 10, slices = 10)
       import(file = "FlangeAOutline2.dxf");

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

  // cut center hole
  cylinder (h = 1.0, d=2.850, center=true, $fn = 250);

  // cut panel mounting holes
  translate([1.25, 1.25, 0])
     cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
  translate([-1.25, 1.25, 0])
     cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
  translate([1.25, -1.25, 0])
     cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);
  translate([-1.25, -1.25, 0])
     cylinder(h = 1.0, d = 0.188, center = true, $fn = 40);

  // assembly screw holes
  translate([1, 2.75/2, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([-1, 2.75/2, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([1, -2.75/2, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([-1, -2.75/2, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([2.75/2, 1.00, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([-2.75/2, 1.00, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([2.75/2, -1.00, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
  translate([-2.75/2, -1.00, 0])
    cylinder(h = 0.188 * 2, d = 0.125, center = true, $fn = 100);
}

