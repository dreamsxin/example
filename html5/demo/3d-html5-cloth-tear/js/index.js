/*
Copyright (c) 2014 Astralized
https://github.com/Astralized
https://codepen.io/Astralized/

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

canvas = document.getElementById('c');
ctx    = canvas.getContext('2d');

//settings
var physics_acc   = 5,
    tear_distance = 40,
    auto_rotate   = 1,
    field_of_view = 500,
    gravity       = 0.2,
    friction      = 0.99,
    cloth_rows    = 20,
    pm_locked_c   = 39; //pointmass locked count - prevents the cloth from falling to the ground

//arrays
var pointmass     = [],
    constraints   = [];
    vertex        = [];

//random
var halfx         = canvas.width / 2,
    halfy         = canvas.height / 2,
    rotatex       = 0,
    rotatey       = 0,
    rotatez       = 0,
    mouse         = {
              down: false,
              x: 0,
              y: 0,
              ox: 0,
              oy: 0,
              button: 0
            };

ctx.lineWidth     = 0.5;
ctx.strokeStyle   = "#ddd";

window.requestAnimFrame =
    window.requestAnimationFrame ||
    window.webkitRequestAnimationFrame ||
    window.mozRequestAnimationFrame ||
    window.oRequestAnimationFrame ||
    window.msRequestAnimationFrame ||
    function (callback) {
        window.setTimeout(callback, 1000 / 60);
};

function init() {

    //create top row
    for(i = 0; i < 40; i++) {

        x = Math.cos(2 * Math.PI * (i / 40)) * 100;
        z = Math.sin(2 * Math.PI * (i / 40)) * 100;

        create_pointmass(x, cloth_rows / 2 * -20, z);
    }

    //rest
    for(i = 0; i < 40; i++) {

        x = Math.cos(2 * Math.PI * (i / 40)) * 100;
        z = Math.sin(2 * Math.PI * (i / 40)) * 100;

        for(y = cloth_rows / 2 * -20 + 20; y < cloth_rows / 2 * 20; y+=20) {

            create_pointmass(x,y,z);
        }
    }

    //create constraints based on distance
    for(i = 0; i < pointmass.length; i++) {
    
        for(c = i + 1; c < pointmass.length; c++) {
          
             dist = Math.sqrt(
                    Math.pow(pointmass[i][0] - pointmass[c][0], 2) 
                  + Math.pow(pointmass[i][1] - pointmass[c][1], 2)
                  + Math.pow(pointmass[i][2] - pointmass[c][2], 2));

             if(dist < 21) {
                 create_constraint(i,c);
             } 

        }
    }

    //mouse
    canvas.onmousemove = function(e) {
        mouse.ox = mouse.x;
        mouse.oy = mouse.y;
        mouse.x  = e.pageX - canvas.offsetLeft,
        mouse.y  = e.pageY - canvas.offsetTop;
        e.preventDefault();
    };

    canvas.onmousedown = function(e) {
        mouse.button = e.which;
        mouse.down   = true;
        e.preventDefault();
    };

    canvas.oncontextmenu = function(e) {
        e.preventDefault();
    };

    canvas.onmouseup = function(e) {
        mouse.down = false;
        e.preventDefault();
    };

    render()
}

function draw3D() {

    vertex = [];
    
    for(i = 0; i < pointmass.length; i++) {

        vertex.push([pointmass[i][0],pointmass[i][1],pointmass[i][2]])
    }

    if(auto_rotate == 1)rotatey+= 0.01;
    
    for(i = 0; i < vertex.length; i++) {

        xyz    = vertex[i];
        x      = xyz[0];
        y      = xyz[1];
        z      = xyz[2];

        xcosa  = Math.cos(rotatex);
        xsina  = Math.sin(rotatex);
        ycosa  = Math.cos(rotatey);
        ysina  = Math.sin(rotatey);
        zcosa  = Math.cos(rotatez);
        zsina  = Math.sin(rotatez);

        xy     = xcosa*y - xsina*z; //x
        xz     = xsina*y + xcosa*z;

        yz     = ycosa*xz - ysina*x; //y
        yx     = ysina*xz + ycosa*x;

        zx     = zcosa*yx - zsina*xy; //z
        zy     = zsina*yx + zcosa*xy;

        xyz[0] = zx;
        xyz[1] = zy;
        xyz[2] = yz;
    }


    ctx.beginPath();
    for(i = 0; i < constraints.length; i++) {

        for(c = 0; c < 2; c++) {

            xyz = vertex[constraints[i][c]];
            fov = field_of_view / (field_of_view + xyz[2]);

            x   = xyz[0] * fov + halfx;
            y   = xyz[1] * fov + halfy;

            if(c == 0) {
                ctx.moveTo(x,y);
            } else {
                ctx.lineTo(x,y);
            }
        }
    }
    ctx.closePath();
    ctx.stroke();
}

function update_pointmass() {

    for(i = 0; i < pointmass.length; i++) {

        x  = pointmass[i][0];
        y  = pointmass[i][1];
        z  = pointmass[i][2];
        ox = pointmass[i][3];
        oy = pointmass[i][4];
        oz = pointmass[i][5];

        dx = x - ox;
        dy = y - oy;
        dz = z - oz;

        if(i > pm_locked_c) {
            pointmass[i][3] = x;
            pointmass[i][4] = y;
            pointmass[i][5] = z;

            pointmass[i][0] = x + dx * friction;
            pointmass[i][1] = y + dy + gravity;
            pointmass[i][2] = z + dz * friction;
        } else {
            pointmass[i][0] = ox;
            pointmass[i][1] = oy;
            pointmass[i][2] = oz;
        }
    }
}

function update_constraint() {

    for(i = 0; i < physics_acc; i++) {

        for(c = 0; c < constraints.length; c++) {

            c1    = pointmass[constraints[c][0]];
            c2    = pointmass[constraints[c][1]];

            diffx = c1[0] - c2[0];
            diffy = c1[1] - c2[1];
            diffz = c1[2] - c2[2];

            dist  = Math.sqrt(diffx * diffx + diffy * diffy + diffz * diffz);

            diff  = (constraints[c][2] - dist) / dist;

            dx    = c1[0] - c2[0];
            dy    = c1[1] - c2[1];
            dz    = c1[2] - c2[2];


            dx    = dx * 0.5;
            dy    = dy * 0.5;
            dz    = dz * 0.5;

            c1[0] = c1[0] + dx * diff;
            c1[1] = c1[1] + dy * diff;
            c1[2] = c1[2] + dz * diff;

            c2[0] = c2[0] - dx * diff;
            c2[1] = c2[1] - dy * diff; 
            c2[2] = c2[2] - dz * diff;

            constraints[c][3] = dist;

            if(dist > tear_distance) {
                constraints.splice(c, 1);
            }
        }
    }
}

function create_pointmass(x,y,z) {

    pointmass.push([x, y, z, x, y, z]);
}

function create_constraint(f,s) {

    constraints.push([f, s, Math.sqrt(
                            Math.pow((pointmass[f][0] - pointmass[s][0]), 2)
                          + Math.pow((pointmass[f][1] - pointmass[s][1]), 2) 
                          + Math.pow((pointmass[f][2] - pointmass[s][2]), 2)), 1])
}

function render() {

    ctx.clearRect(0, 0, canvas.width, canvas.height);

    update_mouse();
    update_pointmass();
    update_constraint();
    draw3D();

    requestAnimFrame(render);
}

function update_mouse() {

    if(mouse.down == true) {

        for(i = 0; i < pointmass.length; i++) {
    
            dist = Math.sqrt(Math.pow((vertex[i][0] - (mouse.x - halfx)), 2)
                           + Math.pow((vertex[i][1] - (mouse.y - halfy)), 2)
                           + Math.pow((vertex[i][2] + 50), 2));

            if(dist < 100 && mouse.button == 1) {
                pointmass[i][3] = pointmass[i][3] - Math.min(1, (mouse.x - mouse.ox) / 10);
                pointmass[i][4] = pointmass[i][4] - Math.min(1, (mouse.y - mouse.oy) / 10);
            }

            //lazy cut for m2
            if(dist < 21 && mouse.button == 3) {
                pointmass[i][4] = -1000;
                pointmass[i][3] = -1000;
            }

        }
    }
}