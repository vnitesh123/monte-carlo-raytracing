#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<iostream>
#include<vector>
#include<math.h>
#include<string>
#include "ray_vector.h"
#include "IntersectionInfo.h"
//#include "stb/stb_image_write.h"

class image{
    public:
        // image dimensions
        int x_res; // resolution of the image along x
        int y_res; // resolution of the image along y
        double w; // width of the image
        double h; // height of the image

        vector<vector<glm::vec3 > > pixels;

        image (int x_res, int y_res, double w, double h){
            this->x_res = x_res;
            this->y_res = y_res;
            this->h = h;
            this->w = w;
            this->pixels = vector<vector<glm::vec3  > >(x_res, vector<glm::vec3 >(y_res, glm::vec3(0.0f, 0.0f, 0.0f)));
        }

        void set_pixel(int i, int j, glm::vec3 rgb){
            this->pixels[i][j] = rgb;
        }
};
