//Step 2.g Create vertex_attribute.cpp

#include "vertex_attribute.hpp"

AttributePointer BuildAttribute (int values_per_vertex, int data_type, bool normal, int stride_bytes, int offset_bytes)
{
    AttributePointer attribute;
    attribute.values_per_vertex = values_per_vertex;
    attribute.type_data = data_type;
    attribute.normalized = normal;
    attribute.stride_bytes = stride_bytes;
    attribute.offset_bytes = offset_bytes;
    return attribute;
}

void BindVAO (VAOStruct vao, unsigned int vbo, int buffer_type)
{
    //bind the vao
    glBindVertexArray(vao.id);
    //bind the VBO
    glBindBuffer(buffer_type,vbo);  

    //iterate through all the attributes in the vector of attributes and invoke glVertexAttribPointer and
    //  glEnableVertexAttribArray for each
    //HINT:  the 'index' is just 'i' if you use int i = 0 as your iterator in a for loop.
    for (int i = 0; i < vao.attributes.size(); i++) {
        AttributePointer attr = vao.attributes[i];
        glVertexAttribPointer(i,attr.values_per_vertex,attr.type_data,
                               attr.normalized,attr.stride_bytes,(void*)(intptr_t)attr.offset_bytes);
        glEnableVertexAttribArray(i);
    }

}

