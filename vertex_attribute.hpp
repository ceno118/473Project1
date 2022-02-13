#ifndef VERTEX_ATTRIBUTE_HPP
#define VERTEX_ATTRIBUTE_HPP

//Step 2.b
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/common.hpp>
#include <iostream>
#include <vector>
//Step 2.c Define Attribute Pointer

//AttributePointer contains all the values needed to 
//  interpret data in a VBO (values per vertex, type of data,
//  whether it is normalized, stride bytes, and offset to first value)
struct AttributePointer {
    int values_per_vertex;
    int type_data;
    bool normalized;
    int stride_bytes;
    int offset_bytes;
};

//Step 2.d Define a structure for vertex array objects

//VAOStruct contains all identity and attribute pointers for a 
// vertex array object (note that you could add EBO information here 
// as well--I leave that to the shape class to figure it out).
struct VAOStruct {
    unsigned int id;
    std::vector<AttributePointer> attributes;
};

//Builds and returns an AttributePointer structure given the necessary data members.
//  Note: stride and offset are in bytes!
AttributePointer BuildAttribute (int values_per_vertex, int data_type, bool normal, int stride_bytes, int offset_bytes);

//For a given VAO and VBO binds them (in preparation to draw the shape defined by the VBO as interpretted by the 
//  attributes in the VAO)
void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type = GL_ARRAY_BUFFER);




#endif //VERTEX_ATTRIBUTE_HPP
