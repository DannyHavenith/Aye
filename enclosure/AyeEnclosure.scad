pcb_width = 26;
pcb_length = 35.5;
pcb_thickness = 2;
pcb_dims = [pcb_length, pcb_width, pcb_thickness];
thin_wall = 1.2;
thin_walls = [thin_wall,thin_wall,thin_wall];
wall = 1.6;
walls = [wall, wall, wall];
d = .01;
d3 = [d,d,d];


/**
* essentially a cube with one of the edges cut off.
*/
module edged_box()
{
    rib = 4;
    sag = .4;
    dims = [rib,rib,pcb_thickness+sag];
    edge_off = 2;
    // somewhat convoluted, but it works
    mirror([0,1,0]) translate( [0, -rib, 0])
    difference()
    {
        cube( dims);
        translate([0,rib-edge_off,-d]) 
            rotate([0,0,45]) cube(dims + 2*d3);
    }
}

module frame_corner( height)
{
    clip = 1;
    edge = pcb_thickness + 2*clip;
    pillar_dims = [4,4,height + edge] + [thin_wall, thin_wall, 0] - d3;

    difference()
    {
        translate([-thin_wall,-thin_wall,-height]) cube (pillar_dims);
        edged_box();
    }
}

tab_width = 4; 
clip = 1; // radius of the clip that sticks out at the top of a pillar.
module pillar( height, doClip=true)
{
    edge = pcb_thickness + 2*clip;
    tab_dims = [thin_wall, 4, height + edge];
    if (doClip)
    {
        translate([0,-tab_width/2,edge - clip]) rotate([90,0,0]) cylinder(h = tab_width, r = clip, center=true, $fn = 50);
    }
    translate([0,-tab_width,-height]) cube( tab_dims);
    translate([-thin_wall+d, -tab_width, -height]) cube([thin_wall, tab_width, height]);
}

module wemos_d1_mini_clip( offset = 5)
{
    // frame corners on the NW, NE side
    frame_corner( offset);
    translate([0, pcb_width, 0]) mirror([0,1,0]) frame_corner(offset);
    
    //two tabs supporting the SE side
    translate([pcb_length, pcb_width, 0]) pillar(offset);
    translate([pcb_length-tab_width - 2*clip, pcb_width, 0]) rotate([0,0,90]) pillar(offset);
    
    // one pillar supporting the SW side (keep the west open for the reset button)
    translate([pcb_length, tab_width, 0]) pillar(offset);
    
    // two extra tabs holding the sides in the E and W position
    translate([pcb_length + tab_width, 0, 0] * .5) rotate([0,0,-90]) pillar( offset, false);
    translate([(pcb_length - tab_width)/2, pcb_width, 0] )rotate([0,0,90]) pillar( offset, false);
}

// a flat cube with rounded (cylindrical) corners.
module flatroundedcube( dimensions, r)
{
    rs = [r,r,0];
    inner = dimensions - 2 * rs;
    translate( -inner/2)
        hull()
            for ( x = [0:1]) for ( y = [0:1])
                translate(up( inner, [x,y,0])) cylinder( h = dimensions[2], r = r, $fn = 20);
}

module demo()
{
    offset = 5;
    wemos_d1_mini_clip( offset);
    
    // bottom to hold it all together
    translate([-thin_wall, -thin_wall,-offset]) cube([ pcb_length, pcb_width, 1] + 2*[thin_wall, thin_wall,0]);
}

function uniform_product( a, b) = [ a[0] * b[0], a[1] * b[1], a[2] * b[2]];
function up(a,b) = uniform_product( a,b);

inner_dims = [40,32,35];
outer_dims = inner_dims + 2*walls;

module case( bottom = true)
{
    difference()
    {
        flatroundedcube( outer_dims, 2);
        flatroundedcube( inner_dims, 2);
        translate( up( outer_dims, [0,0, bottom?.5:-.5])) 
            cube(up( outer_dims, [2,2,1]), center=true);
    }
}

module tab( expand = false)
{
    tab_dim = [2, 8 + (expand?1:0), 15];
    inset = .4;
    hook = .8 + (expand?.2:0);
    
    rotate([0,0,90]) translate( up( tab_dim, [-1, -.5, -.5])) cube( tab_dim + [ inset, 0, 0]);
    translate([0,0, -tab_dim[2]/2 + wall + hook - (expand?0:.1)]) 
        rotate([90,0,90]) 
            cylinder( r = hook, h = tab_dim[1] - 2* wall, center=true, $fn=4);
}

module tabs( expand = false)
{
    positions = up( inner_dims, [.3, .5, 0]);
    for ( x = [-1, 1]) for ( y = [-1, 1])
        translate(up(positions, [x, y, 0])) 
            rotate([0,0, y<0?180:0]) tab( expand);
}

module bottom()
{
    usb_hole = [0, 11, 7.5]; // hole size
    extent_length = 8;
    usb_extent = [usb_hole[2]+2, usb_hole[1], extent_length] + walls;
    hole_pcb_offset = [0, 12, -2.5,]; // offset of hole center from SE corner of pcb
    pcb_offset = 6; // height of pcb above floor
    pcb_SE_corner = up( pcb_dims, [.5, -.5, 0]) +
                    [0,0,-inner_dims[2]/2 + pcb_offset];
                   
    hole_position = up( pcb_SE_corner + hole_pcb_offset, [0,1,1]) +
                    up( inner_dims + outer_dims, [.25, 0, 0]);
    difference()
    {
        union() 
        {
            translate(hole_position + [extent_length/2, 0, 0]) rotate([0,90,0]) 
                flatroundedcube( usb_extent,1);
            case( true);
        }
        translate(hole_position)
        cube( usb_hole + [wall + 2*d + 2*extent_length, 0, 0], center=true);
        translate( up( outer_dims, [0,0, -1])) 
            cube(up( outer_dims, [2,2,1]), center=true);
        tabs( true);
        
    }
    
    translate( up(inner_dims, [ 0, 0 ,-.5]) + 
                up( pcb_dims, [-.5, -.5, 0]) + 
                [0,0,pcb_offset]) 
        wemos_d1_mini_clip( pcb_offset + d);
}

//tab();
//tabs();
bottom();
//case();
//demo();
//wemos_d1_mini_clip();
//frame_corner( 5);
//edged_box();