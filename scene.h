#include "surface.h"
#include <glm/glm.hpp>

using namespace std;

void load_scene_one( vector<Surface*> &surfaces, Surface* &light_source){

    vector<Triangle*> triangles;
    vector<Sphere*> spheres;
    vector<Plane*> planes;

    //---Colors---
    glm::vec3 red = glm::vec3(120,0,0);
    glm::vec3 sand = glm::vec3(225, 208, 175);
    glm::vec3 white = glm::vec3(1.0f, 1.0f, 1.0f)* 255.0f;
    glm::vec3 silver = glm::vec3 (0.75f, 0.75f, 0.75f)*255.0f;
    glm::vec3 magenta = glm::vec3 (153, 5, 138);
    glm::vec3 plane_color = glm::vec3(160,160,160);

    //-------------------Surfaces--------------------

    //---Spheres---
    double ka = 0.4;
    double kd = 0.0;
    double ks = 0.0;
    double km = 0.5;

	spheres.push_back(new Sphere( glm::vec3(0.65,-0.7,0.4), 0.3f, silver, ka, kd, ks, km, 1, "OPAQUE", 1.0f));
	spheres.push_back(new Sphere( glm::vec3(-0.75, -0.7, 0.0), 0.3f, silver, 0.0, 0.0, 0.0, 0, 2, "GLASS", 1.5f));
    spheres.push_back(new Sphere( glm::vec3(0.0, -0.7, 0.7), 0.2f, magenta, 0.3, 0.4, 0.3, 0, 20, "OPAQUE", 1.5f));
    for( size_t i=0; i<spheres.size(); ++i ) {
        surfaces.push_back(spheres[i]);
	}
    
    //---Triangle Light---
    Triangle* light = new Triangle(glm::vec3(-0.5,1.49999,0.4), glm::vec3(0.5,1.49999,0.4), glm::vec3(0.0,1.49999,-0.5), white, 1.0, 0.0, 0.0, 0.0, 3, "LIGHT_SOURCE", 1.0f);
    surfaces.push_back(light);
    
    light_source = light;

    //---Planes---
    glm::vec3 ground_point = glm::vec3(1.0f,-1.0f,1.0f);
    glm::vec3 ground_normal = glm::vec3(0.0f,1.0f,0.0f);
    ground_normal = unit_vector(ground_normal);

    glm::vec3 dummy = glm::vec3(0.0f,1.0f,0.0f); // dummy value for UV axes for no texture planes

    planes.push_back(new Plane(glm::vec3(0,1,0), glm::vec3(0.0,-1.0,0.0), plane_color, 0.5, 0.4, 0, 0.0, 7, "OPAQUE", 1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(0,-1,0), glm::vec3(-1.5,1.52,-1), white, 0.5, 0.4, 0, 0.0, 10, "OPAQUE",1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(-1,0,0), glm::vec3(1.5,-1,-1), red, 0.4, 0.4, 0, 0.0, 8, "OPAQUE", 1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(1,0,0), glm::vec3(-1.5,-1,-1), red, 0.4, 0.4, 0, 0.0, 9, "OPAQUE", 1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(0,0,1), glm::vec3(-1.5f,-1,-1), sand, 0.5, 0.5, 0, 0.0, 11, "OPAQUE", 1.0f, "", dummy, dummy));

    for( size_t i=0; i<planes.size(); ++i ) {
        surfaces.push_back(planes[i]);
	}
}

void load_scene_two( vector<Surface*> &surfaces, Surface* &light_source){

    vector<Triangle*> triangles;
    vector<Sphere*> spheres;
    vector<Plane*> planes;

    //---Color---
    glm::vec3 red = glm::vec3(120,0,0);
    glm::vec3 yellow = glm::vec3( 127,255,0);
    glm::vec3 white = glm::vec3(  1.0f, 1.0f, 1.0f )* 255.0f;
    glm::vec3 silver = glm::vec3 ( 0.75f, 0.75f, 0.75f )*255.0f;
    glm::vec3 magenta = glm::vec3 (153, 5, 138);
    glm::vec3 plane_color = glm::vec3(160,160,160);

    //-------------------Surfaces--------------------

    //---Spheres---
    double ka = 0.4;
    double kd = 0.0;
    double ks = 0.0;
    double km = 0.5;

	spheres.push_back(new Sphere( glm::vec3(0.6,-0.7,0.4), 0.3f, silver, ka, kd, ks, km, 1, "OPAQUE", 1.0f));
	spheres.push_back(new Sphere( glm::vec3(-0.7, -0.7, 0.0), 0.3f, silver, 0.0, 0.0, 0.0, 0, 2, "GLASS", 1.5f));
    spheres.push_back(new Sphere( glm::vec3(0.0, -0.7, 0.7), 0.2f, magenta, 0.3, 0.4, 0.3, 0, 20, "OPAQUE", 1.5f));
    for( size_t i=0; i<spheres.size(); ++i ) {
        surfaces.push_back(spheres[i]);
	}
    
    //---Triangle Light---
    Triangle* light = new Triangle(glm::vec3(-0.5,1.49999,0.4), glm::vec3(0.5,1.49999,0.4), glm::vec3(0.0,1.49999,-0.5), white, 1.0, 0.0, 0.0, 0.0, 3, "LIGHT_SOURCE", 1.0f);
    surfaces.push_back(light);
    light_source = light;

   
    //---Planes---
    glm::vec3 ground_point = glm::vec3(1.0f,-1.0f,1.0f);
    glm::vec3 ground_normal = glm::vec3(0.0f,1.0f,0.0f);
    ground_normal = unit_vector(ground_normal);
    const char* imagepath1 = "images/image1.png";
    const char* imagepath2 = "images/image2.png";
    
    // UV axes for different planes
    glm::vec3 tex_u1 = glm::vec3(0.0f,0.0f,1.0f);
    glm::vec3 tex_v1 = glm::vec3(0.0f,1.0f,0.0f);

    glm::vec3 tex_u2 = glm::vec3(0.0f,0.0f,-1.0f);
    glm::vec3 tex_v2 = glm::vec3(0.0f,1.0f,0.0f);

    glm::vec3 tex_u3 = glm::vec3(1.0f,0.0f,0.0f);
    glm::vec3 tex_v3 = glm::vec3(0.0f,1.0f,0.0f);

    glm::vec3 dummy = glm::vec3(0.0f,1.0f,0.0f); // dummy value for UV axes for no texture planes

    planes.push_back(new Plane(glm::vec3(0,1,0), glm::vec3(0.0,-1.0,0.0), plane_color, 0.5, 0.4, 0, 0.0, 7, "OPAQUE", 1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(0,-1,0), glm::vec3(-1.5,1.52,-1), silver, 0.5, 0.4, 0, 0.0, 10, "OPAQUE",1.0f, "", dummy, dummy));
    planes.push_back(new Plane(glm::vec3(-1,0,0), glm::vec3(1.5,-1,-1), yellow, 0.4, 0.4, 0, 0.0, 8, "OPAQUE", 1.0f, imagepath2, tex_u1, tex_v1));
    planes.push_back(new Plane(glm::vec3(1,0,0), glm::vec3(-1.5,-1,-1), red, 0.4, 0.4, 0, 0.0, 9, "OPAQUE", 1.0f, imagepath2, tex_u2, tex_v2));
    planes.push_back(new Plane(glm::vec3(0,0,1), glm::vec3(-1.5f,-1,-1), plane_color, 0.5, 0.5, 0, 0.0, 11, "OPAQUE", 1.0f, imagepath1, tex_u3, tex_v3));

    for( size_t i=0; i<planes.size(); ++i ) {
        surfaces.push_back(planes[i]);
	}
}