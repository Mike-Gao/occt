#version 450

layout(std140, binding=0) uniform Colors
{
  mat4 occWorldViewMatrix;  //!< World-view  matrix
  mat4 occProjectionMatrix; //!< Projection  matrix
  mat4 occModelWorldMatrix; //!< Model-world matrix
  vec4 uColor;
};

layout(location = 0) out vec4 occFragColor0;

void main()
{
  occFragColor0 = uColor;
}
