#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include "vertex_attribute.hpp"


#include <iostream>
#include <vector>

//using standard namespace means you don't need
// to specify the namespace for applicable classes, functions, and variables.
//  (e.g., std::cout can be called just cout)
//USE sparingly...libraries sometimes use similar names...without the namespace
//  in front you can't tell which you are using.
using namespace std;

//Forward function declarations:


//Resizes the window.  Invoked when the user attempts to resize the window.
//Receives a GLFWwindow pointer, width integer, and height integer as input.
void framebuffer_size_callback(GLFWwindow* window, int width, int height); 

//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Displays the values of a color defined by a glm::vec4 of float values.
void PrintColor (glm::vec4 vec);

//Global Variables
glm::vec4 clear_color(0.0,0.0,0.0,1.0);
bool clear_key_pressed[] {false,false,false};

int main () {
    //initialize the GLFW library (MUST be done before GLFW functions are used!)
    glfwInit();

    //GLFW context options in option, value format -- here we set it to Core profile and
    //  OpenGL Version 3.3
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
    glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);

    //This next line is necessary for Mac OS X.  
    // glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    

    //Create the window object
    GLFWwindow *window = glfwCreateWindow(800,600,"LearnOpenGL",NULL,NULL);
    if (window == NULL) {
        cout << "Failed to create GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    //makes the context of the specified window current
    glfwMakeContextCurrent(window);

    //Initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }  
    

    //Set the size of the rendering window so OpenGL knows how we want to display the coordinates
    //first two parameters set the lower left corner of the window
    //then width then height of the window in pixels
    //Below this means a point at (-0.5, 0.5) in normalized device coordinates ends up at 
    //   location (200, 450)
    glViewport(0,0,800,600);

    //Set the window resize callback.
    glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);

    //Variables:
    //Vertex Shader:
    const char *vertexShaderSource = "#version 330 core\n"
    "layout (location = 0) in vec3 aPos;\n"
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
    "}\0";

    const char *fragmentShaderSource = "#version 330 core\n"
    "out vec4 FragColor;\n"
    "void main()\n"
    "{\n"
    "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
    "} \n\0";

    //Compile shaders:
    //1. define the shader
    unsigned int vertexShader;
    //2. create it
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    //3. Provide the source code
    glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
    // a. shader object
    // b. number of elements in a string array (1 for now)
    // c. String (or an array of strings)
    // d. specifies an array of string lengths

    //4. Compile
    glCompileShader(vertexShader);

    //check if it succeeded (get the log data if it did not)
    int  success;
    char infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //Fragment shader, same steps
    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    //check compilation here too
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if(!success)
    {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
    }

    //NOW a shader program object.
    //1. define the program
    unsigned int shaderProgram;
    //2. Create the program
    shaderProgram= glCreateProgram();
    //3. Attach your shaders to the program
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    //4. attaches the shaders to each other (matching input to outputs)
    glLinkProgram(shaderProgram);

    glUseProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);


    //vertex data in Normalized Device Coordinates (for now)
    // x, y, and z are in the range [-1,1] (outside this range and they get clipped)
    // each vertex ordered x,y,z
    float vertices[] {
        -0.8f, -0.8f, 0.0f,//bottom left vertex
        0.2f, -0.8f, 0.0f, //bottom right vertex
        -0.3f, 0.0f, 0.0f  //top vertex
    };

    float rect_vertices[] {
         0.9f,  0.9f, 0.0f,  // top right
        0.9f, 0.4f, 0.0f,  // bottom right
        0.4f, 0.4f, 0.0f,  // bottom left
        0.4f,  0.9f, 0.0f   // top left 
    };

    unsigned int indices[] {
        0,1,3,1,2,3
    };

    unsigned int rect_VBO;
    glGenBuffers(1,&rect_VBO);

    //to send this data to the vertex shader, it needs to be stored in memory 
    //vertex buffer objects is how to manage this memory
    //1. define the VBO
    unsigned int VBO;
    //2. generate the buffer
    glGenBuffers(1, &VBO);
    //3. bind the buffer object to the correct buffer
    //   the state is set! any calls we make on the GL_ARRAY_BUFFER
    //   will configure our bound buffer object

    unsigned int EBO;
    glGenBuffers(1,&EBO);


    //1. define your Vertex Array Object, used to hold attribute pointers
    //unsigned int VAO;
    //glGenVertexArrays(1, &VAO);
    //bind the vertex array object
    //glBindVertexArray(VAO);

    VAOStruct position_vao;
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);

    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //4. Copy the data into memory
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices, GL_STATIC_DRAW);
        //a. type of buffer
        //b. size in bytes of the vertices
        //c. data pointer
        //d. how to manage the data (static, dynamic, stream)
        //   static - data is set once and used many times
        //   dynamic - changed a lot and used many times
        //   stream - data is set once and used by the GPU at most a few times
    

    glBindVertexArray(position_vao.id);
    glBindBuffer(GL_ARRAY_BUFFER,rect_VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(rect_vertices),rect_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER,0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,0);


    

    glUseProgram(shaderProgram);


    
    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);

        //render commands here
        //set the clear color to wipe the window
        glClearColor(clear_color.r,clear_color.g,clear_color.b,clear_color.a);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT);
        //bind one VAO
        /*
        glBindVertexArray(VAO);

        //bind a new buffer
        glBindBuffer(GL_ARRAY_BUFFER,VBO);
        //You only need one VAO, but in OpenGL 3.3, the attribute pointer call associates 
        //  the currently bound buffer to the data....so it MUST be called each time 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
        //set the attribute pointer(s)
        //glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
        //a. index of the attribute
        //b. number of components PER VERTEX (not byte size)
        //c. type of data
        //d. Normalized? usually NO (clamps to -1 to 1 for signed and 0 to 1 for unsigned)
        //e. STRIDE in bytes (3 vertex components for x,y,z * size of each component)
        //f. offset to the first component of the first vertex for this attribute
        glEnableVertexAttribArray(0);
        */
        BindVAO(position_vao,VBO,GL_ARRAY_BUFFER);
        glDrawArrays(GL_TRIANGLES,0,3);

            
        glBindBuffer(GL_ARRAY_BUFFER,rect_VBO); 
        glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
        //Note -- YES you need to make the attribute pointer call each time you switch
        //  VBOs because the attribute gets associated with the currently bound VBO 
        //  (later versions of OpenGL did adjust this a bit, but it won't matter for us soon).
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
        glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
        

        //check and call events and swap the buffers

        //Applications are usually drawn in a double buffer...one for drawing one for displaying
        //you draw to one and then you swap the buffers...avoids flickering.
        glfwSwapBuffers(window);

        //checks if any events are triggered (keyboard or mouse input)
        glfwPollEvents();
    }

    //terminate your window when you leave the render loop
    glfwTerminate();
    //must have a return value 0 indicates successful run...in this case yes.
    return 0;

}

void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if(glfwGetKey(window,GLFW_KEY_SPACE)==GLFW_PRESS) {
        glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    }
    if(glfwGetKey(window,GLFW_KEY_SPACE) == GLFW_RELEASE){
        glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); 
    }
    //Color Key Processing
    int keys[] {GLFW_KEY_R,GLFW_KEY_G,GLFW_KEY_B};
    for (int i = 0; i < 3; i++) {
        if ((glfwGetKey(window,keys[i])==GLFW_PRESS) && (!clear_key_pressed[i])){
            clear_key_pressed[i] = !clear_key_pressed[i];
            clear_color[i] += 0.1;
            if (clear_color[i] > 1.1) {
                clear_color[i] = 0.0;
            }
            PrintColor(clear_color);
        }
        if (glfwGetKey(window,keys[i])==GLFW_RELEASE) {
            clear_key_pressed[i] = false;
        }
    }
}

//The window, width, and height are provided to this function when the window is resized.
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}  

void PrintColor (glm::vec4 vec) {
    cout << "Color("<<vec.r<<", "<<vec.g<<", ";
    cout << vec.b << ")" << endl;
}