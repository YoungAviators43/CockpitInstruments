couplerHeight = 0.975 - 0.700 + 0.150;

rotate([0, 180, 0])
difference() {
    // Coupler barrel
    cylinder(h = couplerHeight, d = 0.700, center = false, $fn = 150);

    // Coupler barrel details
    translate([0, 0.375, 0])
      cylinder(h = 2.5, d = 0.325, center = true, $fn = 25);
    translate([0, -0.375, 0])
      cylinder(h = 2.5, d = 0.325, center = true, $fn = 25);

    // Center hole
    cylinder(h = 2.0, d = 0.123, center = true, $fn = 100);

    // Set screw nuts
    translate([0.150, 0, couplerHeight - 0.200/2 - 0.020])
       cube(size = [0.073, 0.187, 0.300], center = true);
    translate([-0.150, 0, couplerHeight - 0.200/2 - 0.020])
       cube(size = [0.073, 0.187, 0.300], center = true);

    // Set screw shaft.
    translate([0, 0, couplerHeight - 0.100 - 0.020])
       rotate ([0, 90, 0])
          cylinder(h = 2.0, d = 0.125, center = true, $fn = 100);

    cylinder(h = 0.200, d = 0.300, $fn = 50, center = false);

    cube(size = [0.300, 1.0, 0.350], center = true);

    cube(size = [1.0, 0.160, 0.350], center = true);
}

