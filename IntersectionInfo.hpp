#ifndef IntersectionInfo_H
#define IntersectionInfo_H

#include "ray_vector.hpp"
#include<iostream>
#include<vector>
#include<math.h>
#include <glm/glm.hpp>

class IntersectionInfo{
    public :
        // scalar in e + td at intersection
        double t;

        // point at which ray intersects with object
        glm::vec3 hit_loc;

        // normal to the surface at intersection point
        glm::vec3 normal;

        IntersectionInfo(double t){
            this->t = t;
        }

        glm::vec3 color;
        double ka,kd,ks,km;

        int obj_id;

        string object_type;

        float refractive_index;

        void setInfo(double t, glm::vec3 hit_loc, glm::vec3 normal, glm::vec3 color, double ka, double kd, double ks, double km, int obj_id, string object_type, float refractive_index){
            this->t = t;
            this->normal=normal;
            this->hit_loc = hit_loc;
            this->ka=ka;
            this->kd=kd;
            this->color=color;
            this->ks=ks;
            this->km = km;
            this->obj_id = obj_id;
            this->object_type = object_type;
            this->refractive_index = refractive_index;
        }

};

#endif // IntersectionInfo_H