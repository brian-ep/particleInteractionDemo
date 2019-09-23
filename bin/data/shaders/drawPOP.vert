#version 330

uniform mat4 modelViewProjectionMatrix;

uniform vec2 res;
uniform sampler2DRect posIn;

in vec4  position;
in vec2  texcoord;

out vec2 texCoordVarying;

void main()
{
    texCoordVarying = texcoord;
    //gl_Position = modelViewProjectionMatrix * vec4(texture(posIn, texcoord).xyz, 1.0);
    gl_Position = vec4((texture(posIn, texcoord).xy / res * 2.0) - vec2(1.0), 0.0, 1.0);
}