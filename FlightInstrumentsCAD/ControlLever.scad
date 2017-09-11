
difference() {
  union() {
    linear_extrude(height = 0.250, center = false, convexity = 25, slices = 5)
                   import(file = "ActuatorLever.dxf", scale = 1/25.4, layer="Lever");

    translate([0, 0, 0.189])
      cylinder(h = 0.125, d = 0.376, center = false, $fn = 150);
  }

  cylinder(h = 1, d = 0.125, center = true, $fn = 50);

  translate([-7.6225/25.4, 0, 0])
    cylinder(h = 1, d = 0.063, center=true, $fn = 25);

  translate([-13.37/25.4, 0, 0])
    cylinder(h = 1, d = 0.063, center=true, $fn = 25);

  translate([-19.11/25.4, 0, 0])
    cylinder(h = 1, d = 0.063, center=true, $fn = 25);

  translate([-24.855/25.4, 0, 0])
    cylinder(h = 1, d = 0.063, center=true, $fn = 25);
}
