module CornerRounder(x, y, rot)
{
   translate ([x, y, -0.1])
     rotate(a = rot, v = [0, 0, 1])
       difference () {
         cube([0.4, 0.4, 0.5]);
         cylinder(h = 1.5, r = 0.2, center=true, $fn = 100);
       }
}

module FlatHead256(x, y, z)
{
  translate([x,y,z-(0.075-0.010)])
    union() {
      translate([0, 0, 0.075])
        cylinder(h = 0.200, d = 0.156, center = false, $fn = 50);
      cylinder(h = 0.075, d1 = 0.093, d2 = 0.156, center = false, $fn = 50);
      translate([0, 0, -0.200])
        cylinder(h = 0.300, d = 0.093, center = false, $fn = 50);
  }
}

module HexNut256(x, y, rot)
{
  translate([x, y, 0 - 0.010])
    rotate(a = rot, v = [0, 0, 1])
      union() {
        cylinder(h = 0.066 + 0.010, d = 0.156, center = false, $fn = 6);
        cylinder(h = 0.188 * 2, d = 0.093, center = true, $fn = 50);
      }
}

module MountHole(x, y)
{
  translate([x, y, 0])
    cylinder(h = 1.0, d = 0.1875, center=true, $fn = 25);
}

layer_height = 0.150;

union () {
  difference() {

    // Create main body.
    translate([0, 0, layer_height/2])
      cube([2.400, 2.400, layer_height], center=true);

    // Round off the corners
    CornerRounder(1.2 - 0.2, 1.2 - 0.2, 0);
    CornerRounder(-(1.2 - 0.2), 1.2 - 0.2, 90);
    CornerRounder(-(1.2 - 0.2), -(1.2 - 0.2), 180);
    CornerRounder((1.2 - 0.2), -(1.2 - 0.2), 270);

    // Add holes for mounting screws
    MountHole(1.875/2, 1.875/2);
    MountHole(1.875/2, -1.875/2);
    MountHole(-1.875/2, -1.875/2);
    MountHole(-1.875/2, 1.875/2);

    // Holes for assembly screws
    FlatHead256(   1.2 - 0.61,    1.2 - 0.15, layer_height);
    FlatHead256(   1.2 - 0.15,    1.2 - 0.61, layer_height);
    FlatHead256(-(1.2 - 0.61),    1.2 - 0.15, layer_height);
    FlatHead256(-(1.2 - 0.15),    1.2 - 0.61, layer_height);
    FlatHead256(   1.2 - 0.61, -(1.2 - 0.15), layer_height);
    FlatHead256(   1.2 - 0.15, -(1.2 - 0.61), layer_height);
    FlatHead256(-(1.2 - 0.61), -(1.2 - 0.15), layer_height);
    FlatHead256(-(1.2 - 0.15), -(1.2 - 0.61), layer_height);

    // Create center plate area.
    cylinder(h = 1.0, d = 2.250 - 0.125, center=true, $fn = 150);

    // Create actuator hole.
    *cylinder(h = 1.0, d=0.125, center=true, $fn = 25);

  }

  translate([0, 0, layer_height - 0.005])
    difference() {
      cylinder (h = 0.200/2 + 0.005, d=2.250, center = false, $fn = 250);
      cylinder (h = 1.0, d=2.250 - 0.075, center = true, $fn = 250);
  }

  // Add alignment notch
  translate([0, 1.1250 - 0.125, layer_height/2])
    cube([0.125, 0.125, layer_height], center = true);

  // Add indicator gear bushing.
  *translate([0, 0, 0.0625])
    difference() {
      cylinder(h = 0.125, d = 0.500, center=false, $fn = 50);
      cylinder(h = 1.000, d = 0.125, center=true,  $fn = 25);
    }

  // Add drive gear bushing.
  *translate([13.528/25.4, 0, 0.062])
    difference() {
      cylinder(h = 0.125, d = 0.500, center=false, $fn = 50);
      cylinder(h = 1.000, d = 0.125, center=true,  $fn = 25);
    }

}


// Add indicator gear
*translate([0, 0, 0.0625+0.125])
  color([1,0,0])
    cylinder(h = 0.188, d = 10.67/25.4, center=false, $fn=12);

// Add drive gear
*translate([13.528/25.4, 0, 0.062+0.125])
  color([1,0,0])
    cylinder(h = 0.188, d = 18.63/25.4, center=false, $fn=22);

