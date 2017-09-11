
difference() {
    union () {
        // Gear body
        translate([-2, -1, 0])
            linear_extrude(height = 0.150, center = false, convexity = 25, slices = 5)
                import(file = "GearsInstrumentSmall.dxf", scale = 1/25.4, layer="Gear12");

        // Gear hub
        translate([0, 0, 0.057])
            cylinder(h = 0.3050, d = 0.450, center = false, $fn = 150);
    }

    // Notch for set screw 440 nut
    translate([0.135, 0, 0.360 - 0.200/2 - 0.040])
       cube(size = [0.075, 0.200, 0.300], center = true);

    // Set screw path
    translate([0, 0, 0.360 - 0.100 - 0.040])
       rotate ([0, 90, 0])
          cylinder(h = 2.0, d = 0.125, center = true, $fn = 100);

    // drive bar hole
    cylinder(h = 2.0, d = 0.125, center = true, $fn = 100);
}

