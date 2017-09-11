indicatorOffset = 1.35;
indicatorTipDistance = 0.200;

indicatorHeight = 0.095;
indicatorWidth  = 0.100;
indicatorLength = indicatorOffset/2 - indicatorTipDistance/2;
indicatorMountingShaftDiameter = 0.093;

difference() {
  union() {
    translate([indicatorLength/2, 0, indicatorHeight/2])
      cube([indicatorLength, indicatorWidth, indicatorHeight], center = true);

    cylinder(h = indicatorHeight + 0.045, d = 0.225, center = false, $fn = 100);

    cylinder(h = indicatorHeight, d = 0.300, center = false, $fn = 100);
  }

  cylinder(h = 0.500, d = indicatorMountingShaftDiameter - 0.010, center = true, $fn = 50);
  cube([0.225, 0.025, 1], center=true);

  translate([indicatorLength + 0.05, indicatorWidth/2, 0])
    rotate([0,0,75])
     cube([indicatorWidth, indicatorLength, 2.5*indicatorHeight], center = true);

  translate([indicatorLength + 0.05, -indicatorWidth/2, 0])
    rotate([0,0,-75])
       cube([indicatorWidth, indicatorLength, 2.5*indicatorHeight], center = true);

}