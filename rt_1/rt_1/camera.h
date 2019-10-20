#ifndef camera_h
#define camera_h

#include "ray.h"

class camera
{
public:
	camera(vec3 look_from, vec3 look_at, vec3 vup, float vfov, float aspect) // vfov is top to bottom in degrees
	{
		vec3 u, v, w;
		float theta = vfov * M_PI / 180; // conversion to radians
		float half_height = tan(theta / 2);
		float half_width = aspect * half_height;
		origin = look_from;
		w = unit_vector(look_from - look_at);
		u = unit_vector(cross(vup, w));
		v = cross(w, u);
//		lower_left_corner = vec3(-half_width, -half_height, -1.0);
		lower_left_corner = origin - half_width * u - half_height * v - w;
		horizontal = 2 * half_width * u;
		vertical = 2 * half_height * v;
	}
	
	ray get_ray(float u, float v)
	{
		return ray(origin, lower_left_corner + u * horizontal + v * vertical - origin);
	}
	
	vec3 origin;
	vec3 lower_left_corner;
	vec3 horizontal;
	vec3 vertical;
};

#endif /* camera_h */
