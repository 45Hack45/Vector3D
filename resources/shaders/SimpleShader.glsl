//#Begin_prop
(vec4       dye_color           myColor)
//#End_prop

//#Begin_vert

#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

out vec4 pos;
out vec3 Normal; // Pass normal to fragment shader

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat3 normalMatrix; // Transformed normal matrix (inverse transpose of model)

void main()
{
    vec4 position = projection * view * model * vec4(aPos, 1.0);
    gl_Position = position;
    pos = vec4(position.x, position.y, position.z, 1.0);
    
    Normal = normalize(normalMatrix * aNormal); // Transformed normal
}

//#End_vert

//#Begin_frag

#version 330 core
out vec4 FragColor;

uniform vec4 dye_color;
vec3 light_direction = vec3(-0.5, 1.0, -0.3);

in vec4 pos;
in vec3 Normal;

void main()
{
    vec3 lightDir = normalize(light_direction);
    float diff = max(dot(normalize(Normal), lightDir), 0.0);

    vec3 finalColor = dye_color.rgb * diff; // Basic diffuse shading
    FragColor = vec4(finalColor, dye_color.a);
}

//#End_frag
