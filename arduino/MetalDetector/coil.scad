$fn=30;

module spokes(NoArm=11, ArmWide=5, ArmHigh=10, ArmLong=121, CircSize=25)
{
    DGrees=360/NoArm; 

    union() 
    { 
        for (i=[1:NoArm]) 
        { 
            rotate([0,0,9 + DGrees*i]) cube([ArmWide,ArmLong,ArmHigh]); 
        } 

        cylinder(ArmHigh,CircSize,CircSize); 
    }
}

module torus(R=130, R1=120, H=10, R_ind=3)
{
    difference()
    {
        cylinder(r=R, h=H, $fn=40);
        translate([0,0,H/2]) rotate_extrude($fn=60) translate([R, 0, 0]) circle(r=R_ind,$fn=30);
        cylinder(r=R1, h=H);
    }
}

module arm(L=10, W=4, H=15, R=3, off=0)
{
    translate([0,W/2,0]) difference()
    {
        hull()
        {
            translate([-L*3/4, -W, 0]) cube([L*3/2, W, 1]);
            translate([off, 0, H - L/2]) rotate([90,0,0]) cylinder(r=L / 2, h=W);
        }
        translate([off, 0, H - L/2]) rotate([90,0,0]) cylinder(r=R, h=W+.1);
    }
}

R=120/2;
H=6;

difference()
{
    union()
    {
        torus(R=R, R1=R-10, H=H, R_ind=1.5);
        spokes(ArmLong=R-10, ArmHigh=3, ArmWide=3, CircSize=25/2);
        translate([0,-2-5, 3]) arm();
        translate([0,2+5, 3]) arm();
        
        translate([R - 18, 0, 1.5]) cube([20, 20, 3], center=true);
    }
    translate([20,-3,H/2]) cube([R, 1, 1]);
}