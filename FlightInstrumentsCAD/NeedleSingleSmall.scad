indicatorFrontLength = 0.875;
indicatorBackLength = 0.450;
indicatorWidth = 0.125;
indicatorHeight = 0.063;
indicatorCenterHubDiameter = 0.275;
indicatorMountingHubDiameter = 0.200;

cornerRadius = 0.100;

module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([cornerRadius + 0.010, cornerRadius + 0.010, 0.2]);
         cylinder(h = 0.5, r = cornerRadius, center=true, $fn = 150);
       }
}

needleBadgeLength = 0.325;
needleBadgeWidth = 0.250;

module needleBadge(offset)
{
  translate([offset, 0, indicatorHeight/2])
    difference() {
      cube([needleBadgeLength, needleBadgeWidth, indicatorHeight], center = true);

      CornerRounder(needleBadgeLength/2 - cornerRadius, needleBadgeWidth/2 - cornerRadius, 0);
      CornerRounder(-needleBadgeLength/2 + cornerRadius, needleBadgeWidth/2 - cornerRadius, 90);
      CornerRounder(-needleBadgeLength/2 + cornerRadius, -needleBadgeWidth/2 + cornerRadius, 180);
      CornerRounder(needleBadgeLength/2 - cornerRadius, -needleBadgeWidth/2 + cornerRadius, 270);
   }
}

module needleIndicator()
{
  difference() {
    union() {
      translate([-indicatorBackLength, -indicatorWidth/2, 0])
        cube([indicatorFrontLength + indicatorBackLength, indicatorWidth, indicatorHeight], center = false);

      cylinder(h = indicatorHeight, d = indicatorCenterHubDiameter, center = false, $fn = 100);
      cylinder(h = indicatorHeight + 0.025, d = indicatorMountingHubDiameter, center = false, $fn = 100);

      translate([-indicatorBackLength, 0, 0])
        cylinder(h = indicatorHeight, d = indicatorCenterHubDiameter, center = false, $fn = 100);
    }

    cylinder(h = 0.500, d = 0.085, center = true, $fn = 50);
    cube([0.275, 0.025, 1], center=true);

    translate([indicatorFrontLength - 0.175, indicatorWidth/2 + 0.50, 0])
      rotate([0,0,75])
        cube([1, 1, 1], center = true);

    translate([indicatorFrontLength - 0.175, -(indicatorWidth/2 + 0.50), 0])
      rotate([0,0,-75])
        cube([1, 1, 1], center = true);

  }
}

module needleWithBadge(badgeOffset)
{
  union () {
    needleIndicator();
    needleBadge(badgeOffset);
  }
}

*translate([0, -0.175, 0])
  needleWithBadge(0.55);
*translate([0, 0.175, 0])
  needleWithBadge(0.40);

needleIndicator();
