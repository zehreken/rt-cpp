#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "float.h"
#include <stdlib.h>
#include <chrono>

vec3 color(const ray& r, hitable *world)
{
	hit_record rec;
	if (world->hit(r, 0.0, MAXFLOAT, rec))
	{
		return 0.5 * vec3(rec.normal.x() + 1, rec.normal.y() + 1, rec.normal.z() + 1);
	}
	else
	{
		vec3 unit_direction = unit_vector(r.direction());
		float t = 0.5 * (unit_direction.y() + 1.0);
		return (1.0 - t) * vec3(1.0, 1.0, 1.0) + t * vec3(0.5, 0.7, 1.0);
	}
}

int main(int argc, const char * argv[])
{
	auto start = std::chrono::system_clock::now();
	// Generate .ppm, see https://en.wikipedia.org/wiki/Netpbm_format for more
	int nx = 800;
	int ny = 400;
	int ns = 100; // sampling size for anti-aliasing
	
	vec3 lower_left_corner(-2.0, -1.0, -1.0);
	vec3 horizontal(4.0, 0.0, 0.0);
	vec3 vertical(0.0, 2.0, 0.0);
	vec3 origin(0.0, 0.0, 0.0);
	
	hitable *list[2];
	list[0] = new sphere(vec3(0, 0, -1), 0.5);
	list[1] = new sphere(vec3(0, -100.5, -1), 100);
	hitable *world = new hitable_list(list, 2);
	camera cam;
	
	unsigned char imgData[nx * ny * 3];
	int index = 0;
//	std::cout<< "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 col(0, 0, 0);
			for (int s = 0; s < ns; s++)
			{
				float u = float(i + drand48()) / float(nx);
				float v = float(j + drand48()) / float(ny);
				ray r = cam.get_ray(u, v);
				vec3 p = r.point_at(2.0);
				col = col + color(r, world);
			}
			col = col / float(ns);
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
	
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Result in " << elapsed_seconds.count() << " seconds\n";
	
    return 0;
}
