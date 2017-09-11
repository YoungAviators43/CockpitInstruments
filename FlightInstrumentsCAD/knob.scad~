
n=5;

difference(){
  intersection(){
    union(){
      cylinder(d=12,h=15,$fn=n);
      rotate(a=360/n/2,v=[0,0,1]) cylinder(d=12,h=15,$fn=n);
      cylinder(d=15,h=2,$fn=50);
      translate([0,0,2]) cylinder(d1=15,d2=0,h=6,$fn=50);
    };
    cylinder(d1=30,d2=8,h=15);
  };
}

/** 
  difference(){
    union(){
      cylinder(d=7.5,h=11,$fn=50);
      cylinder(d1=8.5,d2=0,h=8.5,$fn=50);
    }
    translate([7,0,6]) cube([10,10,12],center=true);
  }
**/
