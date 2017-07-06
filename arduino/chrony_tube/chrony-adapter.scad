barrel = 11.3 /2;
barrel_c = 9 /2;
$fn = 55;
//$fs = .01;

difference()
{
    union()
    {
        cylinder(h=20, r=13.5 / 2, center=false);  // inside diameter of the tube
        translate([0,0,10]) cylinder(h=10, r=14 /2, center=false);
    }
    cylinder(h=20, r=barrel_c, center=false);  // stopper
    translate([0,0,2]) cylinder(h=20, r=barrel, center=false); // barrel
}
