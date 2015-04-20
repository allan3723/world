// Input position vector
attribute vec3 coord3d;
attribute vec3 n_coord;

// Output color, to be interpolated
varying vec4 f_color;
// Input matrix by which cube should be moved
uniform mat4 m_transform;
uniform mat4 n_transform;
uniform float red, green, blue;

void main(void) {
	vec3 norm, light;  
	float wx,wy,wz,l, ndotl;

	norm = vec3(n_transform*vec4(n_coord,1.0));
	light = vec3(1.0, 1.0, 2.0);
	wx = light[0] - norm[0];
	wy = light[1] - norm[1];
	wz = light[2] - norm[2];
	l = sqrt(pow(wx,2) + pow(wy,2) + pow(wz,2));
	light = light/l;
	ndotl = dot(norm,light);
	
	if (ndotl < 0)
		ndotl = 0.0;

	vec3 comp_color = ndotl * vec3(red, green, blue);

    // use vertex position as color; pad to length 4 
    f_color = vec4(comp_color,1.0);


    // multiply matrix times position vector
    gl_Position  = m_transform * vec4(coord3d,1.0);
    }

