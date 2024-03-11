$fs=0.25;
$fa=2;
width = 100;
depth = 80;
height = 80;
bottom_rad = 5;
tri_height = 100;
wheel_rad = 80;
wheel_thick = 6;
wheel_rim_width = 15;
n_spokes = 6;
n_bolts = 26;
bolt_rad = 6/2;
mpu_width = 15;
wall_width = 5;
mot_scale = 1.05;
amot_len=52;
amot_d=25;
spoke_thick = 12.5;
shaft_rad = 4/2;
shaft_cut = 2*shaft_rad-3.5;
mot_len = mot_scale*amot_len;
mot_d = mot_scale*amot_d;
mot_r=mot_d/2;
amot_nib_d = 7;
amot_nib_len=12;
mot_nib_len = mot_scale*amot_nib_len;
mot_nib_d = mot_scale*amot_nib_d;
mot_nib_r=mot_nib_d/2;
tri_hyp = sqrt(tri_height^2+(width/2)^2);
mot_bol_of=17/2;
mot_bol_r=1.5;
pi_pos=[[0,0],[0,49],[58,0],[58,49]];
module pi_standoff(d=2.5,sheight=10){
    translate([0,0,-1])
    difference(){
    cylinder(h=1+sheight,r2=d,r1=d+sheight/2);
    translate([0,0,2])cylinder(h=1+sheight,r=d/2);
    }
    }
union(){
difference(){
difference(){
union(){
cube([width,depth,height],center=true);
translate([0,-depth/2,-height/2])rotate([-90,0,0])
    linear_extrude(depth)
    hull(){
        polygon([[width/2,0],[-width/2,0],[0,tri_height/2]]);
        translate([0,tri_height-bottom_rad])circle(r=bottom_rad);
    }
}
translate([0,0,2])cube([width-2*wall_width,depth-2*wall_width,height+2],center=true);
}
translate([0,wall_width-depth/2+mot_len,0])rotate([90,0,0])
union(){
cylinder(h=mot_len,r=mot_r);
translate([0,0,mot_len-1])
    union(){
        cylinder(h=mot_nib_len+1,r=mot_nib_r);
        translate([mot_bol_of,0,0])cylinder(h=mot_nib_len+1,r=mot_bol_r);
        translate([-mot_bol_of,0,0])cylinder(h=mot_nib_len+1,r=mot_bol_r);
    }
}
}
translate([3.5-85/2,3.5-56/2,-height/2])
for(pos=pi_pos){
   translate([pos.x,pos.y,0])pi_standoff();
}
translate([0,-depth/2+wall_width,height/3])rotate([-90,0,0])
union(){
    translate([mpu_width/2,0,0])pi_standoff(sheight=5);
    translate([-mpu_width/2,0,0])
    pi_standoff(sheight=5);
}

}
translate([0,-3-depth/2-wheel_thick/2,0])rotate([90,0,0])
difference(){
difference(){
union(){
difference(){
    cylinder(h=wheel_thick, r = wheel_rad,center=true);
    cylinder(h=wheel_thick+2, r= wheel_rad - wheel_rim_width,center=true);
    
}
for (i=[0:n_spokes/2]){
    rotate([0,0,i*(360/n_spokes)])cube([spoke_thick,2*(wheel_rad-wheel_rim_width),wheel_thick],center=true);
}
}
for(i=[0:n_bolts]){
    rotate([0,0,i*(360/n_bolts)])translate([wheel_rad-wheel_rim_width/2,0,0])cylinder(center=true,h=wheel_thick+2,r=bolt_rad);
}
}
difference(){
cylinder(h=wheel_thick+2,r=shaft_rad,center=true);
translate([0,3*shaft_rad/2-shaft_cut,0])cube([2*shaft_rad,shaft_rad,wheel_thick+4],center=true);
};
}