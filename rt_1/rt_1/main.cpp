#include <iostream>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"
#include "vec3.h"
#include "ray.h"

int main(int argc, const char * argv[])
{
	// Generate .ppm, see https://en.wikipedia.org/wiki/Netpbm_format for more
	int nx = 400;
	int ny = 200;
	unsigned char imgData[nx * ny * 3];
	int index = 0;
//	std::cout<< "P3\n" << nx << " " << ny << "\n255\n";
	for (int j = ny - 1; j >= 0; j--)
	{
		for (int i = 0; i < nx; i++)
		{
			vec3 color(float(i) / float(nx), float(j) / float(ny), 0.2);
			int ir = int(255.99 * color[0]);
			int ig = int(255.99 * color[1]);
			int ib = int(255.99 * color[2]);
			
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
