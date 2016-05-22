#version 330 core

out vec2 vTexCoord;

const vec2 quad[4] = vec2[] (
    vec2(-1.0, 1.0),
    vec2(-1.0,-1.0),
    vec2( 1.0, 1.0),
    vec2( 1.0,-1.0)
);

void main()
{
    vec2 p = quad[ gl_VertexID ];

    vTexCoord   = p * vec2(0.5, 0.5) + vec2(0.5, 0.5);
    gl_Position = vec4(p, 0.0, 1.0);
}
