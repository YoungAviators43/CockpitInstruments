// Gear8
//
// Tooth spacing 2.500 mm
// Contact angle 20 degrees
// Addendum 0.250 mm
// Diametric pitch 1.257 1/mm
// Pitch diameter 6.366 mm
// Overall diameter 7.470 mm
// Shaft spacing 19.099 mm (to 40 tooth gear)

drawingPositionIN = 40/25.4;

difference() {
  union () {
    // Gear body
    translate([-drawingPositionIN, -drawingPositionIN, 0])
      linear_extrude(height = 0.125, center = false, convexity = 25, slices = 5)
                     import(file = "Gear40and8.dxf", scale = 1/25.4, layer="Gear8");

    // Gear hub
    translate([0, 0, 0])
      cylinder(h = 0.188, d = 0.188, center = false, $fn = 150);
  }

  // idle bar hole
  cylinder(h = 2.0, d = 0.093, center = true, $fn = 100);
}
