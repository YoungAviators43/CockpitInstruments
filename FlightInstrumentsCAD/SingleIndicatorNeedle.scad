indicatorFrontLength = 1.175;
indicatorBackLength = 0.600;
indicatorWidth = 0.125;
indicatorHeight = 0.095;
indicatorCenterHubDiameter = 0.300;
indicatorMountingHubDiameter = 0.225;

difference() {
  union() {
    translate([-indicatorBackLength, -indicatorWidth/2, 0])
      cube([indicatorFrontLength + indicatorBackLength, indicatorWidth, indicatorHeight], center = false);

    cylinder(h = indicatorHeight, d = indicatorCenterHubDiameter, center = false, $fn = 100);
    cylinder(h = indicatorHeight + 0.025, d = indicatorMountingHubDiameter, center = false, $fn = 100);

    translate([-indicatorBackLength, 0, 0])
      cylinder(h = indicatorHeight, d = indicatorCenterHubDiameter, center = false, $fn = 100);
  }

  cylinder(h = 0.500, d = 0.075, center = true, $fn = 50);

  translate([1.825 - 0.525 - .1, 0, 0])
    rotate(a=75, v=[0,0,1])
      translate([0.5, 0, 0])
        cube([1, 1, 1], center = true);

  translate([1.825 - 0.525 - .1, 0, 0])
    rotate(a=-75, v=[0,0,1])
      translate([0.5, 0, 0])
        cube([1, 1, 1], center = true);

}