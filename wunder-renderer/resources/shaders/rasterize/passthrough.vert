//-------------------------------------------------------------------------------------------------
// Draw a triangle larger than the screen

#version 450
layout(location = 0) out vec2 v_texCoord;
void main()
{
  v_texCoord  = vec2((gl_VertexIndex << 1) & 2, gl_VertexIndex & 2);
  gl_Position = vec4(v_texCoord * 2.0f - 1.0f, 1.0f, 1.0f);
}
