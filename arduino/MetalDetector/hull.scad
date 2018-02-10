module case(l=86, h=24, w=47, lcd_l=43, lcd_w=20, lcd_h=8)
{
    difference()
    {
        union()
        {
            translate([0,h,0]) scale([1,1,1]) cylinder(r=w/2, h=l);
            translate([-w/2,0,0]) cube([w,h,l]);
            translate([-lcd_w/2,h,0]) cube([lcd_w,lcd_l,lcd_h]);
            translate([0,h+lcd_l,0]) scale([1,1,1]) cylinder(r=lcd_w/2, h=lcd_h);
        }
        translate([-50,-10,0]) cube([100,10,100]);
    }
}


l=87;
h=20;
w=41.5;
lcd_l=40;
lcd_w=16;
lcd_h=4;

module hull()
{
    rotate([90,0,0]) difference()
    {
        case(l=l+4, h=h, w=w+4, lcd_l=lcd_l, lcd_w=lcd_w+4, lcd_h=lcd_h+4);
        union()
        {
            translate([0,0,2]) case(l=l, h=h, w=w, lcd_l=lcd_l, lcd_w=lcd_w, lcd_h=lcd_h);
            translate([-11/2,27,0]) cube([11,27,2]);
            translate([-8.7, 0, 63]) rotate([-90,0,0]) cylinder(r=5/2, h=60);
            translate([6,25.3,80]) cube([9, 3, 40], center=true);
        }
        //translate([-50,-10,0]) cube([100,10,100]);
        //cube([w, h*10, l]);
    }
}

module base()
{
    W = w + 12;
    L = l + 10;
    
    difference()
    {
        translate([-W/2,-L + 3,0]) cube([W, L, 8]);
        translate([0,0,4]) rotate([90,0,0]) case(l=l+4, h=h, w=w+5, lcd_l=lcd_l, lcd_w=lcd_w+4, lcd_h=lcd_h+4);
        //translate([-w/2,-l-2,5]) cube([w, l+2, 8]);
    }
}

base();