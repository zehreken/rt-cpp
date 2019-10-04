#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "vec3.h"
#include "ray.h"

vec3 color(const ray& r)
{
	vec3 unit_direction = unit_vector(r.direction());
	float t = 0.5 * (unit_direction.y() + 1.0);
	return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
}

int main(int argc, const char * argv[])
{
	// Generate .ppm, see https://en.wikipedia.org/wiki/Netpbm_format for more
	int nx = 800;
	int ny = 600;
	
	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);
	
	unsigned char imgData[nx * ny * 3];
	int index = 0;
//	std::cout<< "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
//			vec3 color(float(i) / float(nx), float(j) / float(ny), 0.2);
			
			float u = float(i) / float(nx);
			float v = float(j) / float(ny);
			ray r(origin, lower_left_corner + u * horizontal + v * vertical);
			vec3 col = color(r);
			
			int ir = int(255.99 * col[0]);
			int ig = int(255.99 * col[1]);
			int ib = int(255.99 * col[2]);
			
			imgData[index++] = ir;
			imgData[index++] = ig;
			imgData[index++] = ib;
//			std::cout << ir << " " << ig << " " << ib << " " << "\n";
		}
	}
	
	int r = stbi_write_png("basic.png", nx, ny, 3, imgData, nx * 3);
	std::cout << "Result " << r << "\n";
	
    return 0;
}
