#include "spaceship.hpp"
#include "Shader.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

Spaceship::Spaceship() {
    this->angle_z = 0.0;
    this->location = glm::vec3(0.0);
    this->velocity = glm::vec3(0.0);
    this -> bullet_loc = glm::vec3(0.0);
    this -> bullet_vel = glm::vec3(0.0);
}

float Spaceship::GetAngle(){
    return this->angle_z;
}

void Spaceship::CreateShip (std::vector<VAOStruct> vaos,std::vector<Shader> shaders, 
                           glm::vec3 location, glm::vec3 scale, float angle) {
    this->angle_z = angle;
    this->location = location;
    this->scale = scale;
    VAOStruct position_vao = vaos[0];
    VAOStruct texture_vao = vaos[1];
    //thruster,left_canon, right_canon, wing, hull, windshield
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(-0.11,-0.2,0.0),0.22,0.4));    
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(0.18,-0.1,0.0),0.05,0.2));
    this->shapes.push_back(GetRectangle(position_vao,glm::vec3(-0.23,-0.1,0.0),0.05,0.2));
    this->shapes.push_back(GetTriangle(position_vao,0.25,glm::vec3(0.0,0.15,0.0)));
    this->shapes.push_back(GetHull(texture_vao,glm::vec3(-0.13,-0.1,0.0),0.26,0.4,true));
    this->shapes.push_back(GetHull(position_vao,glm::vec3(-0.1,0.2,0.0),0.2,0.06));

    
    
    //A vector of colors for the shapes
    this->colors = std::vector<glm::vec4> {
                                   glm::vec4(0.0,1.0,1.0,1.0),
                                   glm::vec4(0.7,0.3,0.3,1.0),
                                   glm::vec4(0.7,0.3,0.3,1.0),
                                   glm::vec4(0.5,0.5,0.6,1.0),
                                   glm::vec4(0.5,0.5,0.6,1.0),
                                   glm::vec4(0.4,0.4,0.9,1.0)
                                   };
    
    //A vector of shaders for the shapes
    this->shaders = shaders;



}

void Spaceship::Draw(std::vector<unsigned int> textures) {
        glm::mat4 ship_transform = glm::mat4(1.0);
        //add user-induced movement here.
        ship_transform = glm::translate(ship_transform,this->location);
        ship_transform = glm::rotate(ship_transform,
                                     glm::radians(this->angle_z),
                                     glm::vec3(0.0,0.0,1.0));


        //orient and scale the ship before doing anything else.
        ship_transform = glm::scale(ship_transform,glm::vec3(0.2,0.2,1.0));
        ship_transform = glm::rotate(ship_transform,glm::radians(-90.0f),glm::vec3(0.0,0.0,1.0));
        ship_transform = glm::translate(ship_transform,glm::vec3(0.0,-0.1,0.0));
        
        Shader shader = this->shaders[0];
        Shader texture_shader = this->shaders[1];
        
        unsigned int hull_texture = textures[0];
        std::vector<Shader*> shader_ptrs = {&shader,&shader,&shader,&shader,&texture_shader,&shader};
        //Draw ship (later, you should handle this with a 'Ship' class)
        glBindTexture(GL_TEXTURE_2D,hull_texture);
        for (int i = 1; i < shapes.size(); i++) {
            shader_ptrs[i]->use();
            shader_ptrs[i]->setMat4("transform",ship_transform);
            shader_ptrs[i]->setVec4("set_color",colors[i]);
            this->shapes[i].Draw();
            shader_ptrs[i]->setBool("use_color",true);
            shader_ptrs[i]->setVec4("set_color",glm::vec4(0.0,0.0,0.0,1.0));
            this->shapes[i].DrawEBO();
            shader_ptrs[i]->setBool("use_color",false);
        }

}

void Spaceship::Thrust (float speed) {
    this->velocity += glm::vec3(speed*cos(glm::radians(angle_z)),speed*sin(glm::radians(angle_z)),0.0);
    this->last_thrust = glfwGetTime();
}

glm::vec3 Spaceship::GetLocation(){
    return this->location;
}

// Stops the ship when you release the button
void Spaceship::StopThrust() {
    this -> velocity = glm::vec3(0.0);
}

void Spaceship::Rotate (float angle_change) {
    this->angle_z += angle_change;
}

void Spaceship::Move(glm::vec2 bound_x, glm::vec2 bound_y) {
    if ((this->location.x+this->velocity.x > bound_x[0]) &&
        (this->location.x+this->velocity.x < bound_x[1])) {
            this->location.x += this->velocity.x;
    } else {
        this->velocity.x = 0.0;
    }
    if ((this->location.y+this->velocity.y > bound_y[0]) &&
        (this->location.y+this->velocity.y < bound_y[1])) {
            this->location.y += this->velocity.y;
    } else {
        this->velocity.y = 0.0;
    }

    if ((this -> bullet_loc.x + this -> bullet_vel.x > bound_x[0]) &&
        (this -> bullet_loc.x + this -> bullet_vel.x < bound_x[0])) {
            this -> bullet_loc += this -> bullet_vel.x;
    } else {
        this -> bullet_loc = glm::vec3(this -> location);
        this -> bullet_vel = glm::vec3(0.0);
    }

    if ((this -> bullet_loc.y + this -> bullet_vel.y > bound_y[0]) &&
        (this -> bullet_loc.y + this -> bullet_vel.y < bound_y[0])) {
            this -> bullet_loc += this -> bullet_vel.y;
    } else {
        this -> bullet_loc = glm::vec3(this -> location);
        this -> bullet_vel = glm::vec3(0.0);
    }

}

Spaceship::~Spaceship () {
    for (int i = 0; i < this->shapes.size(); i++) {
        unsigned int vbo_id = this->shapes[i].GetVBO();
        glDeleteBuffers(1,&vbo_id);
    }
}