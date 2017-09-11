intersection() {
  difference() {
    rotate_extrude(convexity = 100, $fn = 100)
      translate([2.135/2, 0, 0])
        circle(d = .125, $fn=50);

    translate([0, 0, -0.5])
      cube([4, 4, 1], center = true);
  }

  cylinder(d = 2.250, h = 1.0, center = true, $fn = 150);
}