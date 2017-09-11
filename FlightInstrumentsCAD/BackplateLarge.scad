deckHeight = 0.063;
mountingBlockHeight = 0.300;
boardHeight = 0.065;

module ledPort(xOffset, yOffset)
{
  translate([xOffset, yOffset, 0.15])
    union () {
      cube([0.25, 0.1, 0.2], center=true);
      cylinder(h = 0.5, d = 0.100, center=true, $fn=20);
    }
}

pedestalHeight = 0.625;
pedestalWell = 0.100;

module deckConnector(rot)
{
  rotate(a = rot, v = [0, 0, 1])
    translate([0, 1.35, 0])
      union () {
        cylinder(h=1.5, d=0.125, center=true, $fn=25);
        cylinder(h=0.9, d=0.4, center=true, $fn=25);
        translate([0, 0.25, 0])
          cube([0.4, 0.50, 0.9 ], center=true);

        translate([0, 0, pedestalHeight - pedestalWell])
          cylinder(h=0.25, d=0.350, center=false, $fn=25);
        translate([0, 0.25, pedestalHeight+0.025])
          cube([0.350, 0.5, 0.25 ], center=true);
      }
}

module deckConnectPedestal(rot)
{
  rotate([0, 0, rot])
    translate([0, 1.35, 0])
      cylinder(h=pedestalHeight, d=0.6, center=false, $fn=100);
}

module alignmentPin(x, y)
{
 translate([x, y, 0.25])
   cylinder(h = mountingBlockHeight, d = 0.093, center = true, $fn=50);
}

// Electronics E deck
intersection() {
  difference() {
    union() {
      // Deck disk
      cylinder(h = deckHeight, d = 3.000, center = false, $fn=250);

      // Add holes for connection to D deck.
      deckConnectPedestal(-40);
      deckConnectPedestal(-40 - 90);
      deckConnectPedestal(180 - 40);
      deckConnectPedestal(180 - 40 - 90);

      // Control board mounting block
      rotate([0, 0, 45])
        union () {
          difference() {
            translate([0, 0, mountingBlockHeight / 2])
              cube([2.25, 2.25, mountingBlockHeight], center=true);

            // Add mounting cavity.
            translate([0, 0, mountingBlockHeight - boardHeight/2])
             cube([1.75, 1.75, boardHeight + 0.010], center=true);
          }
          // Add alignment pins for retainer frame.
          alignmentPin(0.95 - 0.5, 0.95);
          alignmentPin(0.95, 0.95 - 0.5);
          alignmentPin(-0.95 + 0.5, 0.95);
          alignmentPin(-0.95, 0.95 - 0.5);
          alignmentPin(-0.95, -0.95 + 0.5);
          alignmentPin(0.95, -0.95 + 0.5);
          alignmentPin(-0.95 + 0.5, -0.95);
          alignmentPin(0.95 - 0.5, -0.95);
      }
    }

    rotate([0, 0, 45])
      union () {
        // Add hole for usb connector.
        translate([-1.75/2 + 0.350, -1.75/2 + 0.450, 0 ])
          cube([0.525, 0.525, 0.90], center=true);

        // Reset button access
        translate([-1.75/2 + 0.750, -1.75/2 + 1.175, 0 ])
          union () {
            // Add cavity for reset button block.
            translate([0, 0, mountingBlockHeight - boardHeight/2 - (0.200)/2])
              cube([0.355, 0.525, 0.250 - 0.064], center=true);

            // Add hole for reset button.
            cylinder(h = 0.5, d = 0.175, center=true, $fn=25);

            // Add recess for through-hole connectors
            translate([0.15, -0.275, 0.200 ])
              cube([1.6, 1.6, 0.065], center=true);
          }

       // LED access
       union () {
         ledPort(-1.75/2 + 0.2, -1.75/2 + 1.085);
         ledPort(-1.75/2 + 0.2, -1.75/2 + 1.235);
         ledPort(-1.75/2 + 0.2, -1.75/2 + 1.385);
         ledPort(-1.75/2 + 0.2, -1.75/2 + 1.550);
       }
    }

    // Add holes for connection to D deck.
    deckConnector(-40);
    deckConnector(-40 - 90);
    deckConnector(180 - 40);
    deckConnector(180 - 40 - 90);
  }

 cylinder(h = 2, d = 3.000, center = false, $fn=250);
}
