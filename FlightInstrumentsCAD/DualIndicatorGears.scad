
difference() {
    union () {
        translate([-1, -1, 0])
            linear_extrude(height = 0.187, center = false, convexity = 25, slices = 5)
                import(file = "gears24.dxf", scale = 1/25.4, layer="Gear24");

        translate([0, 0, 0.057])
            cylinder(h = 0.3050, d = 0.750, center = false, $fn = 150);
    }

    translate([0.200, 0, 0.360 - 0.200/2])
       cube(size = [0.075, 0.210, 0.250], center = true);

    translate([0, 0, 0.357 - 0.100])
       rotate ([0, 90, 0])
          cylinder(h = 2.0, d = 0.125, center = true, $fn = 100);

    cylinder(h = 2.0, d = 0.125, center = true, $fn = 100);
}

