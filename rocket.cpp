#include "rocket.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "spaceship.hpp"

Rocket::Rocket(){
    this->angle_z = 0.0;
    this->location = glm::vec3(0.0);
    this->velocity = glm::vec3(0.0);
    this -> bullet_loc = glm::vec3(0.0);
    this -> bullet_vel = glm::vec3(0.0);
}

void Rocket::CreateRocket(std::vector<VAOStruct> vaos,std::vector<Shader> shaders, 
                           glm::vec3 location, glm::vec3 scale, float angle){
    this-> angle_z = angle;
    this-> location = location;
    this-> scale = scale;  
    VAOStruct position_vao = vaos[0];
    VAOStruct texture_vao = vaos[1];
    this->shapes.push_back(GetTriangle(position_vao, 0.1, glm::vec3(-0.045,-0.13,0.0)));
    this->shapes.push_back(GetHull(texture_vao,glm::vec3(-0.13,-0.1,0.0),0.15,0.3,true));  
    
    this -> colors = std::vector<glm::vec4>{
        glm::vec4(0.7, 0.3, 0.3, 1.0)
    };  

    this-> shaders = shaders;
    
}

void Rocket::Draw(std::vector<unsigned int> textures){
    glm::mat4 rocket_transform = glm::mat4(1.0);

    rocket_transform = glm::translate(rocket_transform, this -> location);
    rocket_transform = glm::rotate(rocket_transform, glm::radians(this->angle_z), glm::vec3(0.0, 0.0, 1.0));

    rocket_transform = glm::scale(rocket_transform, glm::vec3(0.2, 0.2, 1.0));
    rocket_transform = glm::rotate(rocket_transform, glm::radians(-90.0f), glm::vec3(0.0, 0.0, 1.0));
    rocket_transform = glm::translate(rocket_transform, glm::vec3(0.0, -0.1, 0.0));
    
    unsigned int rocket_texture = textures[0];
    unsigned int fire_texture = textures[1];


    Shader shader = this-> shaders[0];
    Shader texture_shader = this->shaders[1];


    std::vector<Shader*> shader_ptrs = {&texture_shader, &texture_shader};
    
    for (int i=0; i < shapes.size(); i++){
        if (i == 0){glBindTexture(GL_TEXTURE_2D, fire_texture);}
        else if (i == 1){glBindTexture(GL_TEXTURE_2D, rocket_texture);}
        shader_ptrs[i]->use();
        shader_ptrs[i]->setMat4("transform", rocket_transform);
        shader_ptrs[i]->setVec4("set_color", colors[i]);
        this->shapes[i].Draw();
        shader_ptrs[i]->setBool("use_color", true);
        shader_ptrs[i]->setVec4("set_color", glm::vec4(0.0,0.0,0.0,1.0));
        this->shapes[i].DrawEBO();
        shader_ptrs[i]->setBool("use_color", false);
    }
}

// Moves the rocket object to its associated ship
void Rocket::Shoot(glm::vec3 location, float angle){
    this -> location = location;
    this -> angle_z = angle;
}

void Rocket::Move(){
    float speed = 0.01;
    this -> location += glm::vec3(speed*cos(glm::radians(angle_z)),speed*sin(glm::radians(angle_z)),0.0);
}

glm::vec3 Rocket::GetLocation(){
    return this->location;
}