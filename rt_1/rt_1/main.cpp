#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "sphere.h"
#include "hitable_list.h"
#include "camera.h"
#include "float.h"
#include <stdlib.h>
#include <chrono>

vec3 random_in_unit_sphere()
{
	vec3 p;
	do
	{
		p = 2.0 * vec3(drand48(), drand48(), drand48()) - vec3(1, 1, 1);
	}
	while (dot(p, p) >= 1.0);
	return p;
}

vec3 color(const ray& r, hitable *world)
{
	hit_record rec;
	if (world->hit(r, 0.0, MAXFLOAT, rec))
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		return 0.5 * color(ray(rec.p, target - rec.p), world);
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
	int nx = 600;
	int ny = 300;
	int ns = 10; // sampling size for anti-aliasing
	
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
//				vec3 p = r.point_at(2.0);
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
