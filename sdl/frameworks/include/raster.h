

#include <vector>;

#pragma once

const double PI = 3.14159265358979323846;
// Format: 0xRRGGBBAA
#define COLOR_BLACK       0x000000FF
#define COLOR_WHITE       0xFFFFFFFF
#define COLOR_RED         0xFF0000FF
#define COLOR_GREEN       0x00FF00FF
#define COLOR_BLUE        0x0000FFFF
#define COLOR_YELLOW      0xFFFF00FF
#define COLOR_MAGENTA     0xFF00FFFF
#define COLOR_CYAN        0x00FFFFFF
#define COLOR_GRAY        0x808080FF
#define COLOR_CORNFLOWER  0x6495EDFF // The classic "clear screen" blue
#define COLOR_TRANSPARENT 0x00000000

struct error {
	int code = -1;
	std::string error_str = "";
};

struct style {

};

struct point2d {
	float x;
	float y;
};

struct point3d{
	float x;
	float y;
	float z;
};


struct poly2d {
	point2d p1;
	point2d p2;
	point2d p3;
	bool line = false;
};

struct poly3d {
	point3d p1;
	point3d p2;
	point3d p3;
	bool line = false;
};

struct cube {
	point3d vertices[8];
};


struct object3d {
	std::vector<poly3d> polys3d;
	std::vector<poly2d> polys2d;
	
};


poly3d create_poly3d(point3d p1, point3d p2) {
	poly3d l; l.p1 = p1; l.p2 = p2;
	l.line = true;
	return l;
}
poly3d create_poly3d(point3d p1, point3d p2, point3d p3) {
	poly3d l; l.p1 = p1; l.p2 = p2; l.p3 = p3;
	l.line = false;
	return l;
}
poly2d create_poly2d(point2d p1, point2d p2) {
	poly2d l = { p1,p2 };
	l.line = true;
	return l;
}
poly2d create_poly2d(point2d p1, point2d p2, point2d p3) {
	poly2d l = { p1,p2,p3 };
	l.line = false;
	return l;
}

void print_poly3d(const poly3d* poly) {
	if (poly->line) {
		printf("poly3d:LINE P1(%.2f, %.2f, %.2f) -> P2(%.2f, %.2f, %.2f)\n",
		poly->p1.x, poly->p1.y, poly->p1.z,
		poly->p2.x, poly->p2.y, poly->p2.z);
	}
	else {
		printf("poly3d:POLY P1(%.2f, %.2f, %.2f) -- P2(%.2f, %.2f, %.2f) -- P3(%.2f, %.2f, %.2f)\n",
		poly->p1.x, poly->p1.y, poly->p1.z,
		poly->p2.x, poly->p2.y, poly->p2.z,
		poly->p3.x, poly->p3.y, poly->p3.z);
	}
}
void print_poly2d(const poly2d* poly) {
	if (poly->line) {
		printf("poly2d:LINE P1(%.2f, %.2f, %.2f) -> P2(%.2f, %.2f, %.2f)\n",
			poly->p1.x, poly->p1.y,
			poly->p2.x, poly->p2.y);
	}
	else {
		printf("poly2d:POLY P1(%.2f, %.2f, %.2f) -- P2(%.2f, %.2f, %.2f) -- P3(%.2f, %.2f, %.2f)\n",
			poly->p1.x, poly->p1.y,
			poly->p2.x, poly->p2.y,
			poly->p3.x, poly->p3.y);
	}
}

void print_object3d(const object3d* o) {
	for(const auto& poly: o->polys3d){
		print_poly3d(&poly);
	}
}

//void print_object3d_2d(const object3d* o) {
//	for (const auto& line : o->lines2d) {
//		print_line2d(&line);
//	}
//}


object3d create_cube_from_points(std::vector<point3d> points) {
	object3d o;
	o.polys3d.push_back(create_poly3d(points[0], points[1]));
	o.polys3d.push_back(create_poly3d(points[1], points[2]));
	o.polys3d.push_back(create_poly3d(points[2], points[3]));
	o.polys3d.push_back(create_poly3d(points[3], points[0]));

	o.polys3d.push_back(create_poly3d(points[4], points[5]));
	o.polys3d.push_back(create_poly3d(points[5], points[6]));
	o.polys3d.push_back(create_poly3d(points[6], points[7]));
	o.polys3d.push_back(create_poly3d(points[7], points[4]));

	o.polys3d.push_back(create_poly3d(points[0], points[4]));
	o.polys3d.push_back(create_poly3d(points[1], points[5]));
	o.polys3d.push_back(create_poly3d(points[2], points[6]));
	o.polys3d.push_back(create_poly3d(points[3], points[7]));

	return o;
}

object3d create_cube_from_center_point(point3d p, float width) {
	float a = width / 2;
	std::vector<point3d> points = {
		// Top points (Y = 500)
		{ p.x - a,  p.y + a,  p.z - a }, // p0: Top-Front-Left
		{ p.x - a,  p.y + a,  p.z + a }, // p1: Top-Front-Right
		{ p.x + a,  p.y + a,  p.z + a }, // p2: Top-Back-Right
		{ p.x + a,  p.y + a,  p.z - a },
		// Bottom points (Y = 300)
		{ p.x - a,  p.y - a,  p.z - a }, // p0: Top-Front-Left
		{ p.x - a,  p.y - a,  p.z + a }, // p1: Top-Front-Right
		{ p.x + a,  p.y - a,  p.z + a }, // p2: Top-Back-Right
		{ p.x + a,  p.y - a,  p.z - a },
	};
	object3d o;
	o.polys3d.push_back(create_poly3d(points[0], points[1]));
	o.polys3d.push_back(create_poly3d(points[1], points[2]));
	o.polys3d.push_back(create_poly3d(points[2], points[3]));
	o.polys3d.push_back(create_poly3d(points[3], points[0]));

	o.polys3d.push_back(create_poly3d(points[4], points[5]));
	o.polys3d.push_back(create_poly3d(points[5], points[6]));
	o.polys3d.push_back(create_poly3d(points[6], points[7]));
	o.polys3d.push_back(create_poly3d(points[7], points[4]));

	o.polys3d.push_back(create_poly3d(points[0], points[4]));
	o.polys3d.push_back(create_poly3d(points[1], points[5]));
	o.polys3d.push_back(create_poly3d(points[2], points[6]));
	o.polys3d.push_back(create_poly3d(points[3], points[7]));

	return o;


}



// projections return new objects to populate object 2d polys vector
// need to change d to be view or fov thing
point2d project_point(point3d p,float d) {
	point2d p2d = { p.x * d / p.z ,p.y * d / p.z };
	return p2d;
}

poly2d project_poly(poly3d poly3d,float d) {

	if (poly3d.line) {
		return create_poly2d(project_point(poly3d.p1, d), project_point(poly3d.p2, d));
	}
	else {
		return create_poly2d(project_point(poly3d.p1, d), project_point(poly3d.p2, d), project_point(poly3d.p3, d));
	}
}

void project_object(object3d* o, float d) {
	// For every "poly" in the "polys3d" vector...f
	for (const auto& poly3d : o->polys3d) {
		o->polys2d.push_back(project_poly(poly3d, d));
	}
}

void translate_poly3d(poly3d* poly, float dx, float dy, float dz) {
	// Shift Point 1
	poly->p1.x += dx;
	poly->p1.y += dy;
	poly->p1.z += dz;

	// Shift Point 2
	poly->p2.x += dx;
	poly->p2.y += dy;
	poly->p2.z += dz;

	if (!poly->line) {
		poly->p2.x += dx;
		poly->p2.y += dy;
		poly->p2.z += dz;
	}

}
void translate_poly2d(poly2d* poly, float dx, float dy) {
	// Shift Point 1
	poly->p1.x += dx;
	poly->p1.y += dy;

	// Shift Point 2
	poly->p2.x += dx;
	poly->p2.y += dy;

	if (!poly->line) {
		poly->p2.x += dx;
		poly->p2.y += dy;
	}

}


void translate_object(object3d* o, float dx, float dy, float dz) {
	// We iterate through the 3D polys and shift the points
	for (auto& poly3d : o->polys3d) {
		translate_poly3d(&poly3d, dx, dy, dz);
	}
}

void translate_object2d(object3d* o, float dx, float dy) {
	for (auto& poly : o->polys2d) {
		translate_poly2d(&poly, dx, dy);
		
	}
}

void matrix_rotate(point3d* p, std::vector<float> m) {
	float x = p->x; float y = p->y; float z = p->z;
	p->x = m[0] * x + m[1] * y + m[2] * z;
	p->y = m[3] * x + m[4] * y + m[5] * z;
	p->z = m[6] * x + m[7] * y + m[8] * z;

}

std::vector<float> getRotationMatrix(float xdeg, float ydeg, float zdeg) {

	float radx = xdeg * PI / 180.0;
	float rady = ydeg * PI / 180.0;
	float radz = zdeg * PI / 180.0;
	float cx = std::cosf(radx);
	float sx = std::sinf(radx);
	float cy = std::cosf(rady);
	float sy = std::sinf(rady);
	float cz = std::cosf(radz);
	float sz = std::sinf(radz);

	// Initializing a 9-item vector in row-major order
	return {
		(cy * cz), (sx * sy * cz - cx * sz), (cx * sy * cz + sx * sz), // Row 0
		(cy * sz), (sx * sy * sz + cx * cz), (cx * sy * sz - sx * cz), // Row 1
		(-sy),     (sx * cy),                (cx * cy)                 // Row 2
	};
}


void rotate_object(object3d* o, float xdeg, float ydeg, float zdeg) {
	float radx = xdeg * PI / 180.0;
	float rady = ydeg * PI / 180.0;
	float radz = zdeg * PI / 180.0;

	//translate_object(o,);
	std::vector<float> rotation = getRotationMatrix(xdeg, ydeg, zdeg);
 
	for (auto& poly : o->polys3d) {
		matrix_rotate(&poly.p1, rotation);
	}
}



