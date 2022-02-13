#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "spaceship.hpp"
#include "camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables

float rotation_x = 0.0;
float rotation_z = 0.0;

Camera camera(glm::vec3(0.0, 0.0, 3.0), glm::vec3(0.0, 1.0, 0.0), -90.0, 0.0);

void render_pyramid (Shader* shader, std::vector<BasicShape*> shapes,
                     unsigned int floor_texture, unsigned int side_texture,
                     glm::vec3 location);

int main () {
    int screen_width = 600;
    int screen_height = 600;
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",screen_width,screen_height);
    if (window == NULL) {
        return -1;
    }

    unsigned int wood_texture = GetTexture("./images/wood.jpg");
    unsigned int tile_texture = GetTexture("./images/tile.jpg");

    

    Shader shader("./vertexShader.glsl","./fragmentShader.glsl");
    Shader texture_shader("./textureVertexShader.glsl","./textureFragmentShader.glsl");

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);



    //Vertex Array Object for untextured VBOs
    VAOStruct position_vao;
    glGenVertexArrays(1,&(position_vao.id));
    AttributePointer position_attr = BuildAttribute(3,GL_FLOAT,false,3*sizeof(float),0);
    position_vao.attributes.push_back(position_attr);
 
    //Vertex Array Object for textured VBOs
    VAOStruct texture_vao;
    glGenVertexArrays(1,&(texture_vao.id));
    AttributePointer texture_pos_attr = BuildAttribute(3,GL_FLOAT,false,5*sizeof(float),0);
    AttributePointer texture_attr = BuildAttribute(2,GL_FLOAT,false,5*sizeof(float),3*sizeof(float));
    texture_vao.attributes.push_back(texture_pos_attr);
    texture_vao.attributes.push_back(texture_attr);

    BasicShape pyramid_floor = GetTexturedRectangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),0.5,0.5);
    BasicShape triangle_front = GetTexturedTriangle(texture_vao,glm::vec3(-0.25,-0.25,0.0),
                                                                glm::vec3(0.25,-0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_right = GetTexturedTriangle(texture_vao,glm::vec3(0.25,-0.25,0.0),
                                                                glm::vec3(0.25,0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_back = GetTexturedTriangle(texture_vao,glm::vec3( 0.25,0.25,0.0),
                                                                glm::vec3(-0.25,0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    BasicShape triangle_left = GetTexturedTriangle(texture_vao,glm::vec3( -0.25,0.25,0.0),
                                                                glm::vec3(-0.25,-0.25,0.0),
                                                                glm::vec3(0.0,0.0,0.5));
    
    std::vector<BasicShape*> shapes {&pyramid_floor,&triangle_back,
                                    &triangle_front,&triangle_left,
                                    &triangle_right};

    glm::mat4 model = glm::mat4(1.0);
    model = glm::rotate(model,glm::radians(-45.0f),glm::vec3(1.0,0.0,0.0));
    glm::mat4 view = glm::mat4(1.0);
    view = glm::translate(view,glm::vec3(0.0,0.0,-5.0));
    glm::mat4 project = glm::mat4(1.0);
    project = glm::perspective(glm::radians(45.0f),(1.0f*screen_width)/(1.0f*screen_height),0.1f,100.0f);


    std::vector<Shader*> shaders {&shader,&texture_shader};
    for (int i = 0; i < shaders.size(); i++)
    {
        shaders[i]->use();
        shaders[i]->setMat4("transform",glm::mat4(1.0));
        shaders[i]->setMat4("model",model);
        shaders[i]->setMat4("view",view);
        shaders[i]->setMat4("projection",project);

    }
    
    glEnable(GL_DEPTH_TEST);

    texture_shader.use();

    std::vector<glm::vec3> locations;
    locations.push_back(glm::vec3(0.0,0.0,0.0));
    locations.push_back(glm::vec3(0.7,0.0,0.0));
    locations.push_back(glm::vec3(-0.7,0.0,0.0));
    locations.push_back(glm::vec3(0.0,0.7,0.0));
    locations.push_back(glm::vec3(0.0,-0.7,0.0));

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);

        //set the clear color to wipe the window
        glClearColor(0.0,0.0,0.0,1.0);

        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        texture_shader.use();

        view = camera.GetViewMatrix();
        texture_shader.setMat4("view", view);

        for (int i = 0; i < locations.size(); i++){
            render_pyramid(&texture_shader,shapes,tile_texture,wood_texture,
                           locations[i]);
        }
        

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
    if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) 
        rotation_z+=0.1;
    if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS)
        rotation_z-=0.1;
    if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS) 
        rotation_x-=0.1;
    if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS)
        rotation_x+=0.1;

    if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, 0.001);
    
    if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, 0.001);
    
    if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, 0.001);
    
    if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, 0.001);

}

void render_pyramid (Shader* shader, std::vector<BasicShape*> shapes,
                     unsigned int floor_texture, unsigned int side_texture,
                     glm::vec3 location) 

{

        glm::mat4 transform = glm::mat4(1.0);

        transform = glm::translate(transform, location);
        transform = glm::rotate(transform,
                                        glm::radians(rotation_x),
                                         glm::vec3(1.0,0.0,0.0));
        transform = glm::rotate(transform,
                                         glm::radians(rotation_z),
                                         glm::vec3(0.0,0.0,1.0));

        
        shader->setMat4("transform",transform);
        glBindTexture(GL_TEXTURE_2D,floor_texture);
        shapes[0]->Draw();
        if (shapes.size()<=1)
           return;
        for (int i = 1; i < shapes.size(); i++) {
            glBindTexture(GL_TEXTURE_2D,side_texture);
            shapes[i]->Draw();
        }
        
        
}