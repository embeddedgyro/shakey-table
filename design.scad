$fs=0.25;
width = 100;
depth = 80;
height = 80;
tri_height = 150;
wall_width = 5;
mot_scale = 1.05;
amot_len=52;
amot_d=25;
mot_len = mot_scale*amot_len;
mot_d = mot_scale*amot_d;
mot_r=mot_d/2;
amot_nib_d = 7;
amot_nib_len=12;
mot_nib_len = mot_scale*amot_nib_len;
mot_nib_d = mot_scale*amot_nib_d;
mot_nib_r=mot_nib_d/2;
tri_hyp = sqrt(tri_height^2+(width/2)^2);
mot_bol_of=10;
mot_bol_r=1.5;
pi_pos=[[0,0],[0,49],[58,0],[58,49]];
module pi_standoff(d=2.5){
    translate([0,0,-1])
    difference(){
    cylinder(h=11,r2=d,r1=d+5);
    translate([0,0,2])cylinder(h=11,r=d/2);
    }
    }
difference(){
difference(){
union(){
cube([width,depth,height],center=true);
translate([0,-depth/2,-height/2])rotate([-90,0,0])linear_extrude(depth)polygon([[width/2,0],[-width/2,0],[0,tri_height]]);
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
