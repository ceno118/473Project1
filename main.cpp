#include <cmath> //cmath is used for absolute value calculations in collision detection
#include "environment.hpp"
#include "basic_shape.hpp"
#include "vertex_attribute.hpp"
#include "Shader.hpp"
#include "build_shapes.hpp"
#include "spaceship.hpp"
#include "rocket.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

//Processes user input.
//Receives a GLFWwindow pointer as input.
void processInput(GLFWwindow *window);

// Checks when the rockets leave the playable area
void checkRockets();

// Checks if a rocket hits a ship
void checkCollision(GLFWwindow *window);

//Global Variables

// Used to stop the ships
bool slowing_1 = false;
bool slowing_2 = false;

// Hold the ships' locations and angles
glm::vec3 ship_1_location;
glm::vec3 ship_2_location;
float ship_1_angle;
float ship_2_angle;

// Tracks whether or not there is a rocket on screen
bool rocket_1_fired = false;
bool rocket_2_fired = false;

Spaceship ship_1;
Spaceship ship_2;
Rocket rocket_1;
Rocket rocket_2;

int main () {
    GLFWwindow *window = InitializeEnvironment("Eno Project 1",800,800);
    if (window == NULL) {
        return -1;
    }

    // Background/planet textures
    unsigned int space_texture = GetTexture("./images/space2.jpg");
    unsigned int fire_texture = GetTexture("./images/fire_texture.jpg");
    unsigned int sun_texture = GetTexture("./images/sun.png");
    unsigned int planet_texture = GetTexture("./images/mars.jpg");
    unsigned int moon_texture = GetTexture("./images/moon.jpg");

    // Ship/rocket textures
    unsigned int hull_1_texture = GetTexture("./images/hull_1_texture.jpg");
    unsigned int hull_2_texture = GetTexture("./images/hull_2_texture.jpg");
    unsigned int rocket_1_texture = GetTexture("./images/rocket_1_texture.jpg");
    unsigned int rocket_2_texture = GetTexture("./images/rocket_2_texture.jpg");
    
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

    // Planets and Background
    BasicShape planet = GetCircle(texture_vao,0.2,true,40,glm::vec3(0.7,0.6,0.0));
    BasicShape moon = GetCircle(texture_vao,0.15,true,40,glm::vec3(-0.4,-0.5,0.0));
    BasicShape sun = GetCircle(texture_vao,0.35,true,40,glm::vec3(-0.85,0.9,0.0));
    BasicShape space = GetTexturedRectangle(texture_vao,glm::vec3(-1.0,-1.0,0.0),2.0,2.0);

    // Instantiates the ships and rockets
    ship_1.CreateShip(std::vector<VAOStruct>{position_vao,texture_vao},std::vector<Shader>{shader,texture_shader});
    rocket_1.CreateRocket(std::vector<VAOStruct>{position_vao,texture_vao},std::vector<Shader>{shader, texture_shader});
    ship_2.CreateShip(std::vector<VAOStruct>{position_vao,texture_vao},std::vector<Shader>{shader,texture_shader}, glm::vec3(0.0, 0.8, 0.0), glm::vec3(0.0), -90.0 );
    rocket_2.CreateRocket(std::vector<VAOStruct>{position_vao,texture_vao},std::vector<Shader>{shader, texture_shader});
    

    //The render loop -- the function in the condition checks if the 
    //  window has been set to close (does this each iteration)
    while (!glfwWindowShouldClose(window)) {
        // Input/boundary/collision checking
        processInput(window);
        checkRockets();
        checkCollision(window);
        
        glClearColor(0.0,0.0,0.0,1.0);
        glClear(GL_COLOR_BUFFER_BIT);

        //Draw the background
        glBindTexture(GL_TEXTURE_2D,space_texture);
        texture_shader.use();
        texture_shader.setMat4("transform",glm::mat4(1.0));
        space.Draw();

        // Vectors for planets and their textures
        std::vector<unsigned int> planet_textures = std::vector<unsigned int>{planet_texture, moon_texture, sun_texture};
        std::vector<BasicShape> planets = std::vector<BasicShape>{planet, moon, sun};
        
        // Draws planets
        for (int i = 0; i < planets.size(); i++){
            glBindTexture(GL_TEXTURE_2D, planet_textures[i]);
            texture_shader.use();
            planets[i].Draw();
            texture_shader.setBool("use_color",true);
            texture_shader.setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
            planets[i].DrawEBO();
            texture_shader.setBool("use_color",false);
        }
        
        
        // Moves and draws the ships and rockets
        if (rocket_1_fired){
            rocket_1.Move();
            rocket_1.Draw(std::vector<unsigned int>{rocket_1_texture, fire_texture});
        }
        ship_1.Draw(std::vector<unsigned int>{hull_1_texture});
        ship_1.Move(glm::vec2(-1.0,1.0),glm::vec2(-1.0,1.0));
        if (slowing_1){
            ship_1.StopThrust();
        }
        
        if (rocket_2_fired){
            rocket_2.Move();
            rocket_2.Draw(std::vector<unsigned int>{rocket_2_texture, fire_texture});
        }
        ship_2.Draw(std::vector<unsigned int>{hull_2_texture});
        ship_2.Move(glm::vec2(-1.0,1.0),glm::vec2(-1.0,1.0));
        if (slowing_2){
            ship_2.StopThrust();
        }
        
        

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //terminate your window when you leave the render loop
    glfwTerminate();
    //must have a return value 0 indicates successful run...in this case yes.
    return 0;

}

void processInput(GLFWwindow *window)
{
    
    ship_1_location = ship_1.GetLocation();
    ship_1_angle = ship_1.GetAngle();
    ship_2_location = ship_2.GetLocation();
    ship_2_angle = ship_2.GetAngle();

    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS) 
    {
        ship_1.Rotate(-2);
    }
    if (glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS)
    {
        ship_1.Rotate(2);
    }
    if ((glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS))
    {
        slowing_1 = false;
        ship_1.Thrust(0.00015);
    }
    if (glfwGetKey(window,GLFW_KEY_W)==GLFW_RELEASE) {
        slowing_1 = true;
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !rocket_1_fired){
        rocket_1.Shoot(ship_1_location, ship_1_angle);
        rocket_1_fired = true;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS && !rocket_2_fired){
        rocket_2.Shoot(ship_2_location, ship_2_angle);
        rocket_2_fired = true;
    }

    if (glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS) 
    {
        ship_2.Rotate(-2);
    }
    if (glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS) 
    {
        ship_2.Rotate(2);
    }
    if ((glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS))
    {
        slowing_2 = false;
        ship_2.Thrust(0.00015);
    }
    if (glfwGetKey(window,GLFW_KEY_UP)==GLFW_RELEASE) {
        slowing_2 = true;
    }


}

void checkRockets(){
    glm::vec3 rocket_1_loc = rocket_1.GetLocation();
    if (std::abs(rocket_1_loc.x) >= 1 || std::abs(rocket_1_loc.y) >= 1){
        rocket_1_fired = false;
    }
    glm::vec3 rocket_2_loc = rocket_2.GetLocation();
    if (std::abs(rocket_2_loc.x) >= 1 || std::abs(rocket_2_loc.y) >= 1){
        rocket_2_fired = false;
    }
}

void checkCollision(GLFWwindow *window){
    glm::vec3 rocket_1_loc = rocket_1.GetLocation();
    glm::vec3 rocket_2_loc = rocket_2.GetLocation();
    glm::vec3 ship_1_loc = ship_1.GetLocation();
    glm::vec3 ship_2_loc = ship_2.GetLocation();

    if ((rocket_1_loc.x - 0.08 < ship_2_loc.x && rocket_1_loc.x + 0.08 > ship_2_loc.x) 
        && (rocket_1_loc.y - 0.08 < ship_2_loc.y && rocket_1_loc.y + 0.08 > ship_2_loc.y) && rocket_1_fired){
            glfwSetWindowShouldClose(window, true);
            std::cout << "Player 1 Wins!";
        }
    if ((rocket_2_loc.x - 0.08 < ship_1_loc.x && rocket_2_loc.x + 0.08 > ship_1_loc.x) 
        && (rocket_2_loc.y - 0.08 < ship_1_loc.y && rocket_2_loc.y + 0.08 > ship_1_loc.y) && rocket_2_fired){
            glfwSetWindowShouldClose(window, true);
            std::cout << "Player 2 Wins!";
        }
}

