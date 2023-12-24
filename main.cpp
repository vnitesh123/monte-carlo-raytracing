#include "image.hpp"
#include "camera.hpp"
#include "scene.hpp"
#include "ray_vector.hpp"
#include<iostream>
#include <random>
#include <omp.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.hpp"
#include<vector>
#include <GL/glew.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <chrono>

using namespace std;

#define EPSILON 0.00001f

double DOUBLE_MAX = numeric_limits<double>::max();

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 800;

glm::vec3 DEFAULT_COLOUR = glm::vec3(10.0f,10.0f,10.0f);

std::default_random_engine gen = std::default_random_engine(static_cast<unsigned int>(time(NULL)));

std::uniform_real_distribution<float> uni = std::uniform_real_distribution<float>(0, 1);

// time stamps to calculate the duration of elsapsed time for performing the ray tracing
chrono::steady_clock::time_point startTime;
chrono::steady_clock::time_point endTime;

// To find a random point inside triangle 
glm::vec3 Random_Point_Inside_Triangle(Surface* surface) {
    Triangle *triangle = dynamic_cast< Triangle *>(surface);
    if (triangle != nullptr) {
        float r1 = uni(gen);
        float r2 = uni(gen);
        float alpha = 1 - sqrtf(r1);
        float beta = sqrtf(r1) * (1 - r2) ;
        float gamma = 1 - alpha - beta;
        return alpha * triangle->a + beta * triangle->b + gamma * triangle->c;
    }
    return glm::vec3(0,0,0);
}

const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "layout (location = 1) in vec3 aColor;\n"
    "layout (location = 2) in vec2 aTexCoord;\n"
    "out vec3 ourColor;\n"
    "out vec2 TexCoord;\n"
    "void main()\n"
    "{\n"
	"gl_Position = vec4(aPos, 1.0);\n"
	"ourColor = aColor;\n"
	"TexCoord = vec2(aTexCoord.x, aTexCoord.y);\n"
    "}\0";

const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "in vec3 ourColor;\n"
    "in vec2 TexCoord;\n"
    "uniform sampler2D texture1;\n"
    "void main()\n"
    "{\n"
    "   FragColor = texture(texture1, TexCoord);\n"
    "}\n\0";


// performs ray object intersection
bool hit_surface(vector<Surface*> surfaces, Ray r, IntersectionInfo &info){
    bool surface_hit=false;
    for (auto& s:surfaces){
        if(s->intersect(r.source,r.direction,info)){
            surface_hit = true;
        }
    }
    return surface_hit;
}

// To get the shading color at a point
glm::vec3 get_shading_color(Ray ray, glm::vec3 light_source, int recursive_depth, vector<Surface*> surfaces, IntersectionInfo intersection_pt){

    glm::vec3 light_direction = unit_vector(light_source - intersection_pt.hit_loc);

    IntersectionInfo new_info = IntersectionInfo(DOUBLE_MAX);
    Ray shadow_ray = Ray(intersection_pt.hit_loc + (light_direction * 0.00001f), light_direction);

    bool is_point_in_shadow = hit_surface(surfaces, shadow_ray, new_info);

    glm::vec3 ambient_shading = intersection_pt.color * float(intersection_pt.ka);
    
    // check if the shdow ray intersects any object
    if (is_point_in_shadow && new_info.object_type != "LIGHT_SOURCE") {
        return ambient_shading;
    }

    glm::vec3 diffuse_shading = glm::vec3(0.0f, 0.0f, 0.0f);
    
    glm::vec3 specular_shading = glm::vec3(0.0f, 0.0f, 0.0f);

    diffuse_shading = diffuse_shading + scalar_product(intersection_pt.color, max(0.0f,glm::dot(light_direction,intersection_pt.normal)) * intersection_pt.kd);
    glm::vec3 bisector = unit_vector(light_direction - ray.direction);
    specular_shading = add_vectors(specular_shading, scalar_product(intersection_pt.color, pow(max(0.0f,glm::dot(bisector,intersection_pt.normal)),10) *  intersection_pt.ks));

    glm::vec3 result = ambient_shading + diffuse_shading + specular_shading;


    return result;
}


// To get the color at pixel point for the incident ray
glm::vec3 trace_ray(Ray ray, Surface* light_source, int recursive_depth, vector<Surface*> surfaces){
    if(recursive_depth > 3){
        return DEFAULT_COLOUR;
    }

    glm::vec3 result = DEFAULT_COLOUR;

    IntersectionInfo new_info = IntersectionInfo(DOUBLE_MAX);

    bool surface_hit = hit_surface(surfaces, ray, new_info);

    if( !surface_hit){
        return DEFAULT_COLOUR;
    }

    if (new_info.object_type == "LIGHT_SOURCE"){
        result = new_info.color;
        return result;
    }

    glm::vec3 sample_color = glm::vec3(0.0f,0.0f,0.0f);

    // sampling for soft shadowing
    int shadow_sampling = 4;
    #pragma omp parallel for
    for(int i=0; i<shadow_sampling;i++){
        glm::vec3 light_point = Random_Point_Inside_Triangle(light_source);
        sample_color += get_shading_color(ray, light_point, recursive_depth, surfaces, new_info);
    }

    result = result + (sample_color * float(1.0f/float(shadow_sampling)));

    // handling reflections
    if (new_info.km > 0){
            glm::vec3 R = ray.direction -  (new_info.normal * float(2.0f * dot(ray.direction, new_info.normal)));
            if (recursive_depth < 3){
                glm::vec3 reflect_result = trace_ray(Ray(new_info.hit_loc + R * EPSILON, R), light_source, recursive_depth + 1, surfaces);
                result += (reflect_result * float(new_info.km) );
            }
    }

    // refraction calculations
    if (new_info.object_type == "GLASS") {
        if (recursive_depth < 3){
            float n2 = new_info.refractive_index;
            // consider the air medium with refractive index = 1
            float n1 = 1.0f;
            bool is_ray_inside = false;
            if (dot(new_info.normal, ray.direction) > 0){ // The ray is inside an object
			    is_ray_inside = true;
                n1 = n2;
                n2 = 1.0f;
            }

            // snell's law calculations
            float n = n1 / n2;
            float cosi = dot(new_info.normal * 1.0f, ray.direction);
            float cosr = 1.0f - ((n * n) * (1.0f - (cosi * cosi)));
            if (cosr > 0.0f){
                glm::vec3 transmitted = (n * (ray.direction + new_info.normal * cosi)) - (new_info.normal * sqrtf( cosr ));
                glm::vec3 refraction_color = trace_ray( Ray( new_info.hit_loc + transmitted * EPSILON, transmitted ), light_source, recursive_depth + 1, surfaces);
                result += refraction_color;
            }
        }
    }

    return result;
}

// Rendering function to find the colour at each pixel
void findImagePixels(vector<Surface*> surfaces, Surface* light, image &img, camera cam, vector<vector<glm::vec3>> points){
    random_device rand_dev;
    mt19937 generator(rand_dev());
    uniform_real_distribution<float> distr(-0.5, 0.5);

    cout<< "Ray tracing started ..."<<endl;

    startTime = chrono::steady_clock::now();

    #pragma omp parallel for
    for(int i=0;i<img.x_res;i++){
        #pragma omp parallel for
        for(int j=0;j<img.y_res;j++){
            img.set_pixel(i,j,DEFAULT_COLOUR);
            glm::vec3 pixel_color = glm::vec3(0.0f,0.0f,0.0f);
            int ray_samples = 4;
            #pragma omp parallel for
            for (int k = 0; k < ray_samples; ++k) {
                // random samples for anti aliasing
                float x_rand= distr(generator);
                float y_rand = distr(generator);
                glm::vec3 x_displacement = glm::vec3(2.0/500.0,0,0);
                glm::vec3 y_displacement = glm::vec3(0,-2.0/500.0,0);
                pixel_color += trace_ray(Ray(cam.e, unit_vector(points[i][j]+(x_displacement*float(x_rand)+y_displacement*float(y_rand))-cam.e)), light, 0, surfaces);
                // pixel_color += trace_ray(Ray(cam.e, unit_vector(points[i][j]-cam.e)), light, 0, surfaces);
            }
            pixel_color = pixel_color * float(1.0f/float(ray_samples));
            img.set_pixel(i,j,pixel_color);
        }

    }
    cout<< "Ray tracing completed."<<endl;

    endTime = chrono::steady_clock::now();
    auto duration = chrono::duration_cast<chrono::seconds>(endTime - startTime);
    cout<< "time taken for execution is - "<<duration.count()<< " seconds" <<endl;
}


int main(){
    // Scene 1 
    glm::vec3 e = glm::vec3(0, 0, 2);
    glm::vec3 d= glm::vec3(0,0,-1);
    glm::vec3 up= glm::vec3(0,1,0);

    up = unit_vector(up);
    d = unit_vector(d);

    bool is_perspective_camera = true;
    camera cam = camera(e,up,d, 2, is_perspective_camera);
    image img = image(500,500,4,4);

    vector<vector<glm::vec3>>points = cam.get_view_plane(img.w, img.h, img.x_res, img.y_res);

    vector<Surface*> surfaces;
    Surface* light;

    cout<< "Choose the scene that need to be rendered" << endl;
    cout << "1. Press 1 to load Scene One which has cornell box with 3 spheres and no texture" << endl;
    cout << "2. Any other key entered will load Scene Two as default scene with 3 spheres and texture for planes" << endl;
    string scene_type;
    cin >> scene_type;

    if(scene_type == "1"){
        load_scene_one(surfaces, light);
        cout << "Rendering Scene One" << endl;
    }
    else{
        load_scene_two(surfaces, light);
        cout << "Rendering Scene Two" << endl;
    }

    findImagePixels(surfaces, light, img, cam, points);
    
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Display RGB Array", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // // GLEW: load all OpenGL function pointers
    glewInit();

    // build and compile the shaders
    // ------------------------------------
    // vertex shader
    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check for shader compile errors
    int success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // fragment shader
    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check for shader compile errors
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }
    // link shaders
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check for linking errors
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
    }
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left 
    };
    unsigned int indices[] = {  
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture 
    // -------------------------
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture); // all upcoming GL_TEXTURE_2D operations now have effect on this texture object
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // Create the image (RGB Array) to be displayed
    const int width  = img.x_res; // keep it in powers of 2!
    const int height = img.y_res; // keep it in powers of 2!
    unsigned char image[width*height*3];
    for(int j = width-1; j >= 0; j--)
    {
        for (int i = height-1; i >= 0; i--)
        {
            int idx = (j * width + i) * 3;
            image[idx] = (unsigned char) img.pixels[i][j].x  ; //((i+j) % 2) * 255;
            image[idx+1] = (unsigned char) img.pixels[i][j].y;
            image[idx+2] = (unsigned char) img.pixels[i][j].z;
        }
    }

    unsigned char *data = &image[0];
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }



    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind Texture
        glBindTexture(GL_TEXTURE_2D, texture);

        // render container
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}