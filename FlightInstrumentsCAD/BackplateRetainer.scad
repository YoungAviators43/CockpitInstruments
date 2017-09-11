retainerHeight = 0.093;
mountingBlockHeight = 0.300;
boardHeight = 0.065;

module alignmentPin(x, y)
{
 translate([x, y, 0.25])
   cylinder(h = 1, d = 0.093, center = true, $fn=50);
}

module deckConnectPedestal(rot)
{
  rotate([0, 0, rot])
    translate([0, 1.35, -0.25])
      cylinder(h=1, d=0.7, center=false, $fn=100);
}


// Electronics E deck
intersection() {
  difference() {
    // Control board mounting block
    rotate([0, 0, 45])
      cube([2.25, 2.25, retainerHeight], center=true);

    rotate([0, 0, 45])
      translate([0, 0, mountingBlockHeight - boardHeight/2])
        cube([1.60, 1.60, 1], center=true);

    // Add alignment pins for retainer frame.
    alignmentPin(0.95 - 0.5, 0.95);
    alignmentPin(0.95, 0.95 - 0.5);
    alignmentPin(-0.95 + 0.5, 0.95);
    alignmentPin(-0.95, 0.95 - 0.5);
    alignmentPin(-0.95, -0.95 + 0.5);
    alignmentPin(0.95, -0.95 + 0.5);
    alignmentPin(-0.95 + 0.5, -0.95);
    alignmentPin(0.95 - 0.5, -0.95);

    deckConnectPedestal(-40);
    deckConnectPedestal(-40 - 90);
    deckConnectPedestal(180 - 40);
    deckConnectPedestal(180 - 40 - 90);
  }

 cylinder(h = 2, d = 3.000, center = false, $fn=250);
}
