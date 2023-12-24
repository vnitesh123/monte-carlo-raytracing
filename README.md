# Computer Graphics - CAP5705
## Project - Monte Carlo Ray Tracing

## Introduction 

In this project, realistic rendering was performed by implementing Monte Carlo ray tracing algorithm to render a Cornell box scene with reflection, refraction, soft shadowing effects. Random probabilistic sampling has been added for soft shadowing and anti aliasing effects. Added texture mapping to the side planes (walls) of cornell box. To reduce the rendering time for the scence and handle the excessive number of computations efficently and making them faster, a parallelization framework called OpenMP was used.

## Requirements

C++ version - 11 & above
Compiler path - /usr/bin/clang++
MacOS BigSur and above

We will be needing few OpenGL libraries for visualization of the image rendered from the scene

### Library Installation Commands

GLEW 
```
brew install glew
```
GLFW 
```
brew install glfw
```

GLM 
```
brew install glm
```

LIBOMP
```
brew install libomp
```

## Execution Steps

Checkout to the project folder and follow the below steps


Command to compile the program:

```
clang++ -std=c++14 -Xpreprocessor -fopenmp -I<LIBOMP INCLUDE PATH> -I<GLFW INCLUDE PATH> -I<GLEW INCLUDE PATH> -I<GLM INCLUDE PATH> -L<LIBOMP LIBRARY PATH> -L<GLFW LIBRARY PATH> -L<GLEW LIBRARY PATH> -L<GLM LIBRARY PATH> -lglfw -lglew -lomp -framework OpenGL main.cpp
```

Replace the variables : `<LIBOMP INCLUDE PATH> <GLFW INCLUDE PATH>  <GLEW INCLUDE PATH> <GLM INCLUDE PATH> <LIBOMP LIBRARY PATH> <GLFW LIBRARY PATH>  <GLEW LIBRARY PATH> <GLM LIBRARY PATH>` with their respective paths

Values for the above variables can be found by executing

```
brew info libomp
brew info glew
brew info glfw
brew info glm
```

An example for the final command based on the values in our local machine

For Mac Intel Processor
```
clang++ -std=c++14  -Xpreprocessor -fopenmp -I/usr/local/opt/libomp/include  -I/usr/local/Cellar/glfw/3.3.8/include/ -I/usr/local/Cellar/glew/2.2.0_1/include/ -I/usr/local/Cellar/glm/0.9.9.8/include/ -L/usr/local/opt/libomp/lib -L/usr/local/Cellar/glfw/3.3.8/lib/ -L/usr/local/Cellar/glew/2.2.0_1/lib/ -L/usr/local/Cellar/glm/0.9.9.8/lib/  -lglfw -lglew -lomp  -framework OpenGL main.cpp
```

For Apple Silicon Processor
```
clang++ -std=c++14  -Xpreprocessor -fopenmp -I/opt/homebrew/Cellar/libomp/17.0.6/include  -I/opt/homebrew/Cellar/glfw/3.3.8/include/ -I/opt/homebrew/Cellar/glew/2.2.0_1/include/ -I/opt/homebrew/Cellar/glm/0.9.9.8/include/ -L/opt/homebrew/Cellar/glfw/3.3.8/lib/ -L/opt/homebrew/Cellar/glew/2.2.0_1/lib/ -L/opt/homebrew/Cellar/glm/0.9.9.8/lib/ -L/opt/homebrew/Cellar/libomp/17.0.6/lib -lglfw -lglew -lomp  -framework OpenGL main.cpp
```

After compiling the program execute the program using the command

```
./a.out
```

After this you will be prompted to select the scene as follows.
```
Choose the scene that need to be rendered
1. Press 1 to load Scene One which has cornell box with 3 spheres and no texture
2. Any other key entered will load Scene Two as default scene with 3 spheres and texture for planes
```

### Feature / Effects present in this project

1. Shading
2. Soft Shadowing by random sampling
3. Reflection
4. Refraction using snells's law
5. Antialiasing by random sampling

### Result

Scene - 1 without texture mapping

![Alt text](./results/Scene1_With_Sampling.png?raw=true "result")

Scene - 2 with texture mapping on the walls

![Alt text](./results/scene2_with_sampling.png?raw=true "result")
