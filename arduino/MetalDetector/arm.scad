

difference()
{
    union()
    {
    hull()
    {
        cylinder(r=20/2, h=22);
        translate([0,9.6/2,37]) rotate([90,0,0]) cylinder(r=11.8/2, h=9.6, $fn=30);
        
    }
    hull()
    {
        cylinder(r=20/2, h=3);
        translate([12,0,0]) cylinder(r=3,h=3,$fn=30);
    }
    }
    
    translate([0,15/2,37]) rotate([90,0,0]) cylinder(r=3, h=15, $fn=40);
    translate([0,-9.6/2,37]) rotate([90,0,0]) cylinder(r=14/2, h=9.6);
    translate([0,9.6+9.6/2,37]) rotate([90,0,0]) cylinder(r=14/2, h=9.6);
    cylinder(r=12/2,h=22);
    translate([0,0,22])sphere(r=12/2);

    translate([12,0,0]) cylinder(r=1.5,h=30,$fn=30);
    
    //cube(30);
}