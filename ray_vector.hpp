#ifndef ray_vector_H
#define ray_vector_H

#include<iostream>
#include<vector>
#include<math.h>
#include <glm/glm.hpp>
using namespace std;

class Ray {
    public:
    glm::vec3 source;
    glm::vec3 direction;
    Ray(glm::vec3 source, glm::vec3 direction){
        this->source = source;
        this->direction = direction;
    }
};

glm::vec3 add_vectors(glm::vec3 a, glm::vec3 b){
    glm::vec3 res = a + b;
    return res;
}

glm::vec3 subtract_vectors(glm::vec3 a, glm::vec3 b){
    glm::vec3 res = a - b;
    return res;
}

double dot(glm::vec3 a, glm::vec3 b){
    return glm::dot(a,b);
}

float magnitude(glm::vec3 a){
    return sqrt(dot(a,a));
}

glm::vec3 unit_vector(glm::vec3 a){
    double mag = magnitude(a);
    float factor = 1/mag;

    return a * factor;
}

glm::vec3 scalar_product(glm::vec3 a, float k){
    return a * k;
}

glm::vec3 cross(glm::vec3 a, glm::vec3 b){
    glm::vec3 x = glm::cross(a,b);
    return x;
}

#endif // ray_vector_H
