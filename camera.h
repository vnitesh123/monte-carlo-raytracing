#include<iostream>
#include<vector>
#include<math.h>
#include "ray_vector.h"

class camera{

    public:
        // camera basis

        // camera origin
        glm::vec3 e;

        // up vector
        glm::vec3 up;

        // view direction
        glm::vec3 d;

        // right vector = (d x up) 
        glm::vec3 right;

        // view plane distance
        double dist;

        // Is perspective camera
        bool is_perspective_camera;

        camera(glm::vec3 e, glm::vec3 up, glm::vec3  d, double dist, bool is_perspective_camera){
            this->d = d;
            this->dist = dist;
            this->e = e;
            this->up = up;
            this->right = cross(d,up);
            this->is_perspective_camera = is_perspective_camera;
        }

        vector<vector<glm::vec3 > > get_view_plane(double w, double h, double res_x, double res_y){
            glm::vec3 view_plane_centre = add_vectors(e, scalar_product(d, dist));

            glm::vec3 view_plane_bottom_left = add_vectors(add_vectors(view_plane_centre, scalar_product(up, -1*h/2)), scalar_product(right, -1*w/2));

            vector<vector<glm::vec3 > >pixel_points(res_x,vector<glm::vec3 >(res_y, glm::vec3(0.0f,0.0f,0.0f)));

            for(int i=0;i<res_x;i++){
                for(int j=0;j<res_y;j++){
                    pixel_points[i][j] = add_vectors(view_plane_bottom_left, scalar_product(right, (i+0.5)*w/res_x));
                    pixel_points[i][j] = add_vectors(pixel_points[i][j], scalar_product(up, (j+0.5)*h/res_y));
                }
            }

            return pixel_points;
        } 

};