indicatorHeight = 0.063;
indicatorWidth  = 0.110;
indicatorLength = 0.950;

module needle()
{
  difference() {
    union() {
      translate([indicatorLength/2, 0, indicatorHeight/2])
        cube([indicatorLength, indicatorWidth, indicatorHeight], center = true);

      cylinder(h = indicatorHeight + 0.045, d = 0.300, center = false, $fn = 100);

      cylinder(h = indicatorHeight, d = 0.425, center = false, $fn = 100);
    }

    // center mounting shaft with relief slots
    cylinder(h = 0.500, d = 0.085, center = true, $fn = 50);
    translate([0, 0.090, 0])
      cube([0.225, 0.035, 1], center=true);
    cube([0.275, 0.025, 1], center=true);
    translate([0, -0.090, 0])
      cube([0.225, 0.035, 1], center=true);

    // Form indicator point.
    translate([indicatorLength + 0.05, indicatorWidth/2, 0])
      rotate([0,0,75])
       cube([indicatorWidth, indicatorLength/2, 2.5*indicatorHeight], center = true);

    translate([indicatorLength + 0.05, -indicatorWidth/2, 0])
      rotate([0,0,-75])
        cube([indicatorWidth, indicatorLength/2, 2.5*indicatorHeight], center = true);
  }
}

needle();

translate([.750, 0.350, 0])
  rotate([0, 0, 180])
    needle();