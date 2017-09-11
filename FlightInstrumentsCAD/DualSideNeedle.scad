indicatorHeight = 0.095;
indicatorWidth  = 0.110;
indicatorLength = 0.950;

difference() {
  union() {
    translate([indicatorLength/2, 0, indicatorHeight/2])
      cube([indicatorLength, indicatorWidth, indicatorHeight], center = true);

    cylinder(h = indicatorHeight + 0.045, d = 0.300, center = false, $fn = 100);

    cylinder(h = indicatorHeight, d = 0.425, center = false, $fn = 100);
  }

  cylinder(h = 0.500, d = 0.075, center = true, $fn = 50);

  translate([indicatorLength + 0.05, indicatorWidth/2, 0])
    rotate([0,0,75])
     cube([indicatorWidth, indicatorLength/2, 2.5*indicatorHeight], center = true);

  translate([indicatorLength + 0.05, -indicatorWidth/2, 0])
    rotate([0,0,-75])
       cube([indicatorWidth, indicatorLength/2, 2.5*indicatorHeight], center = true);

}