intersection() {
  difference() {
    rotate_extrude(convexity = 100, $fn = 100)
      translate([1.5, 0, 0])
        circle(d = .150, $fn=50);

    translate([0, 0, -0.5])
      cube([4, 4, 1], center = true);
  }

  cylinder(d = 3.125, h = 1.0, center = true, $fn = 150);
}