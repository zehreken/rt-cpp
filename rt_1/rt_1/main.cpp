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

bool refract(const vec3& v, const vec3& n, float ni_over_nt, vec3& refracted)
{
	vec3 uv = unit_vector(v);
	float dt = dot(uv, n);
	float discriminant = 1.0 - ni_over_nt * ni_over_nt * (1 - dt * dt);
	if (discriminant > 0)
	{
		refracted = ni_over_nt * (uv - dt * n) - sqrt(discriminant) * n;
		return true;
	}
	else
	{
		return false;
	}
}

float schlick(float cosine, float ref_idx)
{
	float r0 = (1 - ref_idx) / (1 + ref_idx);
	r0 = r0 * r0;
	
	return r0 + (1 - r0) * pow(1 - cosine, 5);
}

vec3 reflect(const vec3& v, const vec3& n)
{
	return v - 2 * dot(v, n) * n;
}

class material
{
public:
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const = 0;
};

class lambertian : public material
{
public:
	lambertian(const vec3& a) : albedo(a) { }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		vec3 target = rec.p + rec.normal + random_in_unit_sphere();
		scattered = ray(rec.p, target - rec.p);
		attenuation = albedo;
		
		return true;
	}
	
	vec3 albedo;
};

class metal : public material
{
public:
	metal(const vec3& a, float f) : albedo(a) { if (f < 1) fuzz = f; else fuzz = 1; }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		vec3 reflected  = reflect(unit_vector(r_in.direction()), rec.normal);
		scattered = ray(rec.p, reflected + fuzz * random_in_unit_sphere());
		attenuation = albedo;
		return (dot(scattered.direction(), rec.normal) > 0);
	}
	vec3 albedo;
	float fuzz;
};

class dielectric : public material
{
public:
	dielectric(float ri) : ref_idx(ri) { }
	virtual bool scatter(const ray& r_in, const hit_record& rec, vec3& attenuation, ray& scattered) const
	{
		vec3 outward_normal;
		vec3 reflected = reflect(r_in.direction(), rec.normal);
		float ni_over_nt;
		attenuation = vec3(1.0, 1.0, 1.0);
		vec3 refracted;
		float reflect_prob;
		float cosine;
		if (dot(r_in.direction(), rec.normal) > 0)
		{
			outward_normal = -rec.normal;
			ni_over_nt = ref_idx;
			cosine = ref_idx * dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		else
		{
			outward_normal = rec.normal;
			ni_over_nt = 1.0 / ref_idx;
			cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().length();
		}
		
		if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
		{
			reflect_prob = schlick(cosine, ref_idx);
		}
		else
		{
			scattered = ray(rec.p, reflected);
			reflect_prob = 1.0;
		}
		
		if (drand48() < reflect_prob)
		{
			scattered = ray(rec.p, reflected);
		}
		else
		{
			scattered = ray(rec.p, refracted);
		}
		
		return true;
	}
	float ref_idx;
};

vec3 color(const ray& r, hitable *world, int depth)
{
	hit_record rec;
	if (world->hit(r, 0.001, MAXFLOAT, rec))
	{
		ray scattered;
		vec3 attenuation;
		if (depth < 50 && rec.mat_ptr->scatter(r, rec, attenuation, scattered))
		{
			return attenuation * color(scattered, world, depth + 1);
		}
		else
		{
			return vec3(0, 0, 0);
		}
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
	
	/*
	float R = 0.5;
	hitable *list[4];
	list[0] = new sphere(vec3(0, 0, -1), R, new lambertian(vec3(0, 0, 1)));
	list[1] = new sphere(vec3(2 * R, 0, -1), R, new lambertian(vec3(1, 0, 0)));
	list[2] = new sphere(vec3(-2 * R, 0, -1), R, new lambertian(vec3(0, 1, 0)));
	list[3] = new sphere(vec3(0, -1000.5, -1), 1000, new metal(vec3(0.6, 0.6, 0.6), 1.0));
	hitable *world = new hitable_list(list, 4);
	//*/
	
	/*
	hitable *list[7];
	list[0] = new sphere(vec3(0, 0, -1), 0.5, new metal(vec3(0.7, 0.7, 0.7), 0));
	list[1] = new sphere(vec3(0, -1000.5, -1), 1000, new metal(vec3(0.6, 0.6, 0.6), 0.5));
	list[2] = new sphere(vec3(0.75, -0.25, -1), 0.25, new lambertian(vec3(0.9, 1, 0.2)));
	list[3] = new sphere(vec3(-0.75, -0.25, -0.75), 0.25, new metal(vec3(1, 0.0, 0.4), 0.5));
	list[4] = new sphere(vec3(0.3, -0.4, -0.6), 0.1, new dielectric(1.5));
	list[5] = new sphere(vec3(-1.8, 0.5, -1), 1, new metal(vec3(0.2, 0.6, 0.1), 0.7));
	list[6] = new sphere(vec3(0, -0.1, 0), 0.4, new dielectric(1.5));
	hitable *world = new hitable_list(list, 7);
	//*/
	
	//*
	float R = 0.5;
	hitable *list[100];
	for (int i = 0; i < 99; i++)
	{
		//list[i] = new sphere(vec3(drand48() * 20 - 10, 0, drand48() * 20 - 10), R, new lambertian(vec3(0, 0, 1)));
		list[i] = new sphere(vec3(drand48() * 20 - 10, 0, drand48() * 20 - 10), R, new metal(vec3(0.9, 0.4, 0.4), 1));
	}
	list[99] = new sphere(vec3(0, -1000.5, -1), 1000, new metal(vec3(0.6, 0.6, 0.6), 1.0));
	hitable *world = new hitable_list(list, 100);
	//*/
	
	vec3 look_from = vec3(10, 0, 10);
	vec3 look_at = vec3(0, 0, 0);
	float dist_to_focus = (look_from - look_at).length();
	float aperture = 1;
	camera cam(look_from, look_at, vec3(0, 1, 0), 60, float(nx) / float(ny), aperture, dist_to_focus);
	
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
				col = col + color(r, world, 0);
			}
			col = col / float(ns);
			col = vec3(sqrt(col[0]), sqrt(col[1]), sqrt(col[2]));
			int ir = int(255 * col[0]);
			int ig = int(255 * col[1]);
			int ib = int(255 * col[2]);
			
			imgData[index++] = ir;
			imgData[index++] = ig;
			imgData[index++] = ib;
//			std::cout << ir << " " << ig << " " << ib << " " << "\n";
		}
	}
	
	int r = stbi_write_png("basic.png", nx, ny, 3, imgData, nx * 3);
	
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end-start;
	std::cout << "Result in " << elapsed_seconds.count() << " seconds\nSuccess: " << r << "\n";
	
    return 0;
}
