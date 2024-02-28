width = 100;
depth = 60;
height = 80;
tri_height = 250;
tri_hyp = sqrt(tri_height^2+(width/2)^2);
union(){
cube([width,depth,height],center=true);
translate([0,-depth/2,-height/2])rotate([-90,0,0])linear_extrude(depth)polygon([[width/2,0],[-width/2,0],[0,tri_height]]);
}

