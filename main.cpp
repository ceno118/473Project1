#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "spaceship.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Processes user input (keyboard currently).
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

//Global Variables

bool thrusters_fired = false;

Spaceship ship;

int main () {
    GLFWwindow *window = InitializeEnvironment("LearnOpenGL",600,600);
    if (window == NULL) {
        return -1;
    }

    unsigned int space_texture = GetTexture("./images/space.jpg");
    unsigned int hull_texture = GetTexture("./images/hull_texture.png");
    unsigned int planet_texture = GetTexture("./images/mars.jpg");

    
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

    
    //Planet
     BasicShape planet = GetCircle(texture_vao,0.3,true,40,glm::vec3(0.7,0.6,0.0));
    
     //Star field
     BasicShape stars = GetStars(position_vao,200);
     //Space background
     BasicShape space = GetTexturedRectangle(texture_vao,glm::vec3(-1.0,-1.0,0.0),2.0,2.0);

    ship.CreateShip(std::vector<VAOStruct>{position_vao,texture_vao},std::vector<Shader>{shader,texture_shader});
    

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        //input
        processInput(window);

        //render commands here
        //set the clear color to wipe the window
        glClearColor(0.0,0.0,0.0,1.0);


        //clear the color buffer (where things are drawn) using the current clear color.
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw the background
        glBindTexture(GL_TEXTURE_2D,space_texture);
        texture_shader.use();
        texture_shader.setMat4("transform",glm::mat4(1.0));
        space.Draw();
        
        //Draw the starfield
        glPointSize(3.0);
        shader.use();
        shader.setMat4("transform",glm::mat4(1.0));
        shader.setVec4("set_color",glm::vec4(1.0));
        stars.Draw();
        glPointSize(1.0);

        //Draw the planet
        glBindTexture(GL_TEXTURE_2D,planet_texture);
        texture_shader.use();
        
        planet.Draw();
        texture_shader.setBool("use_color",true);
        texture_shader.setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
        planet.DrawEBO();
        texture_shader.setBool("use_color",false);

        ship.Draw(std::vector<unsigned int>{hull_texture});
        ship.Move(glm::vec2(-1.0,1.0),glm::vec2(-1.0,1.0));

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
    if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) 
    {
        //angle_z-=0.1;
        ship.Rotate(-0.1);
    }
    if (glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS)
    {
        //angle_z += 0.1;
        ship.Rotate(0.1);
    }
    if ((glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS) && (!thrusters_fired))
    {
        thrusters_fired = true;
        ship.Thrust(0.0001);
    }
    if (glfwGetKey(window,GLFW_KEY_UP)==GLFW_RELEASE) {
        thrusters_fired = false;
    }

}



