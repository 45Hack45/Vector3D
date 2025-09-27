//#Begin_prop
//#End_prop

//#Begin_vert

#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = vec4(aPos,1.f);
}

//#End_vert

//#Begin_frag

#version 330 core
out vec4 FragColor;

void main()
{
    FragColor = vec4(0,0,1,1.f);
}

//#End_frag
