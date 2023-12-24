#include<map>
#include<string>
#include "IntersectionInfo.hpp"
#include "stb_image.hpp"

using namespace std;

class Surface{
    public:
        virtual bool intersect(glm::vec3 e, glm::vec3 d, IntersectionInfo& result)= 0;
        virtual ~Surface(){}
};

/*
Object Types
1. LIGHT_SOURCE
2. OPAQUE
*/

class Sphere : public Surface{
    public:
        glm::vec3 c; 
        double r;
        glm::vec3 color;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 

        // surface’s specular coefficient
        double ks;

        // surface’s mirror coefficient
        double km;

        int obj_id;

        string object_type;

        float refractive_index;

        Sphere(glm::vec3 c, double r, glm::vec3 color, double ka, double kd, double ks, double km, int obj_id, string object_type, float refractive_index){
            this->c = c;
            this->color=color;
            this->r=r;
            this->ka = ka;
            this->kd =kd;
            this->ks = ks;
            this->km = km;
            this->obj_id = obj_id;
            this->object_type = object_type;
            this->refractive_index = refractive_index;
        }
        
        /**
         * To find normal to surface at intersection point
         *
         * @param intersection_point is point on surface of sphere where ray hits.
         * @return unit vector normal to sphere surface at intersection point
         */
        glm::vec3 normal(glm::vec3 intersection_point){
            glm::vec3 n;
            n = unit_vector(subtract_vectors(intersection_point, c));
            return n;
        }
        /**
         * To find if ray intersects sphere
         *
         * @param e represent ray_origin.
         * @param d represent ray_direction.
         * @param result contain closest intersection point info.
         * @return true if ray intersect sphere and intersection point is closer than intersection point in result, else false
         */
        bool intersect(glm::vec3 e, glm::vec3 d, IntersectionInfo& result)
        {
                double a = glm::dot(d, d);
                double b = 2 * ( glm::dot(d,e) - glm::dot(d, c) );
                double k = glm::dot(e, e) + glm::dot(c, c) - 2 * glm::dot(e, c) - r * r;
                double disc = ( b * b ) -  ( 4 * a * k );
                if (disc >= 0.0) {
                    double t;
                    t = ( 0.5 * (-1*b - sqrt(disc)) / a);
                    //if (t < result.t && t>0.0 && result.obj_id != this->obj_id){
                    if (t < result.t && t>0.0){
                        glm::vec3 intersection_point;
                        glm::vec3 v_normal;
                        intersection_point = add_vectors(e, scalar_product(d, t));
                        v_normal = normal(intersection_point);
                        result.setInfo(t, intersection_point, v_normal, this->color, this->ka, this->kd, this->ks, this->km, this->obj_id, this->object_type, this->refractive_index);
                        return true;
                    }
                };
                return false;
        }

};

class Triangle : public Surface{
    public:
        // vertices of triangle 
        glm::vec3 a;
        glm::vec3 b;
        glm::vec3 c;

        // color of surface
        glm::vec3 color;
        
        // outward normal to surface
        glm::vec3 normal;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 
        
        // surface’s specular coefficient
        double ks;

        // surface’s mirror coefficient
        double km;

        string object_type;
        int obj_id;

        float refractive_index;

        Triangle(glm::vec3 a, glm::vec3 b, glm::vec3 c, glm::vec3 color, double ka, double kd, double ks, double km, int obj_id, string object_type, float refractive_index){
            this->a = a;
            this->b = b;
            this->c = c;
            this->color=color;
            this->ka = ka;
            this->kd =kd;
            this->ks = ks;
            this->km = km;
            this->normal = unit_vector(cross(this->c - this->b, this->a - this->b));
            this->object_type = object_type;
            this->obj_id = obj_id;
            this->refractive_index = refractive_index;
        }

        /**
         * To find if ray intersects sphere
         *
         * @param e represent ray_origin.
         * @param d represent ray_direction.
         * @param result contain closest intersection point info.
         * @return true if ray intersect triangle and intersection point is closer than intersection point in result, else false
         */
        bool intersect(glm::vec3 e, glm::vec3 d, IntersectionInfo& result){
            map<string, double> mat;
            mat["a"] = a.x - b.x;
            mat["b"] = a.y - b.y;
            mat["c"] = a.z - b.z;
            mat["d"] = a.x - c.x;
            mat["e"] = a.y - c.y;
            mat["f"] = a.z - c.z;
            mat["g"] = d.x;
            mat["h"] = d.y;
            mat["i"] = d.z;
            mat["j"] = a.x - e.x;
            mat["k"] = a.y - e.y;
            mat["l"] = a.z - e.z;

            // This is value of M calculated in Ray-Triangle Intersection in the Book.
            double eihf = (mat["e"] * mat["i"]) - (mat["h"] * mat["f"]);
            double gfdi = (mat["g"] * mat["f"]) - (mat["d"] * mat["i"]);
            double dheg = (mat["d"] * mat["h"]) - (mat["e"] * mat["g"]);
            double akjb = (mat["a"] * mat["k"]) - (mat["j"] * mat["b"]);
            double jcal = (mat["j"] * mat["c"]) - (mat["a"] * mat["l"]);
            double blkc = (mat["b"] * mat["l"]) - (mat["k"] * mat["c"]);

            double m = mat["a"] * (eihf) + mat["b"] * (gfdi) + mat["c"] * (dheg);

            double t = -1 * (1/m) * (mat["f"] * (akjb) + mat["e"] * (jcal) + mat["d"] * (blkc));

            if (t < 0.0 || t > result.t){
                return false;
            }

            double gamma = (1/m) * (mat["i"] * (akjb) + mat["h"] * (jcal) + mat["g"] * (blkc));

            if ( gamma < 0.0 || gamma > 1.0){
                return false;
            }

            double beta = (1/m) * (mat["j"] * (eihf) + mat["k"] * (gfdi) + mat["l"] * (dheg));

            if (beta < 0.0 || beta > (1.0 - gamma)){
                return false;
            }

            if (t < result.t){
                glm::vec3 intersection_point;
                intersection_point = e + scalar_product(d, t);
    
                result.setInfo(t, intersection_point, normal, color, ka, kd, ks, km, this->obj_id, this->object_type, this->refractive_index);
                return true;
            }

            return false;
        
        }
};

class Texture {
public:
    int width, height, channels;
    unsigned char* data;
    
    Texture() : width(0), height(0), channels(0), data(nullptr) {};
    Texture(const char* imagePath) {
        stbi_set_flip_vertically_on_load(true);
        data = stbi_load(imagePath, &width, &height, &channels, 0);
    }

    ~Texture() {
        stbi_image_free(data);
    }

    glm::vec3 getColorAtUV(float u, float v) const {
        int x = static_cast<int>(u * width) % width;
        int y = static_cast<int>(v * height) % height;
        // Get the color channels
        unsigned char r = data[(y * width + x) * channels];
        unsigned char g = data[(y * width + x) * channels + 1];
        unsigned char b = data[(y * width + x) * channels + 2];

        // Normalize color values to the range [0, 1]
        return glm::vec3(r, g, b);
    }
};

class Plane : public Surface {
    public:
        glm::vec3 color;

        // surface’s ambient coefficient
        double ka;

        // surface’s diffuse coefficient
        double kd; 
        
        // surface’s specular coefficient
        double ks;

        // mirror refection coefficient
        double km;

        // plane's normal
        glm::vec3 normal;

        // point on a plane
        glm::vec3 a;

        int obj_id;

        string object_type;

        Texture texture;

        glm::vec3 tex_u; // u axis for texture

        glm::vec3 tex_v; // v axis for texture

        bool is_texture = false;

        float refractive_index;

        Plane(glm::vec3 normal, glm::vec3 a, glm::vec3 color, double ka, double kd, double ks, double km, int obj_id, string object_type, float refractive_index, const char* imagepath, glm::vec3 tex_u, glm::vec3 tex_v){

            this->ka = ka;
            this->kd = kd;
            this->ks = ks;
            this->km = km;
            this->normal = normal;
            this->color = color;
            this->a = a;
            this->obj_id = obj_id;
            this->object_type = object_type;
            this->refractive_index = refractive_index;
            if(strcmp(imagepath,"") != 0){
                this->texture = Texture(imagepath);
                this->is_texture = true;
            }
            this->tex_u = tex_u;
            this->tex_v = tex_v;
        }
            
        // To get texture color at given point in 3D space
        glm::vec3 get_color_at_intersection(glm::vec3 intersectionPoint){
        // UV coordinates on the plane
        float u = dot((intersectionPoint - this->a), this->tex_u);
        float v = dot((intersectionPoint - this->a), this->tex_v);
        return texture.getColorAtUV(u, v);
        }

        bool intersect(glm::vec3 e, glm::vec3 d, IntersectionInfo& result)
        {       
                double denominator = glm::dot(d,this->normal);
                if (denominator == 0.0){
                    return false;
                }
                double numerator = glm::dot(subtract_vectors(this->a,e), this->normal);
                double t = numerator/denominator;
                if (t < result.t && t>0.0001){

                    glm::vec3 intersection_point = add_vectors(e, scalar_product(d, t));
                    glm::vec3 point_color;
                    if(is_texture){
                        point_color = get_color_at_intersection(intersection_point);
                    }
                    else{
                        point_color = this->color;
                    }
                    result.setInfo(t, intersection_point, this->normal, point_color, this->ka, this->kd, this->ks, this->km, this->obj_id, this->object_type, this->refractive_index);
                    return true;
                }
                return false;
        }
};