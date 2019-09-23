#version 330

out vec4 fragColor;

in vec2 texCoordVarying;

//  ping pong inputs
uniform sampler2DRect posIn;
uniform sampler2DRect velIn;
uniform sampler2DRect colIn;
uniform sampler2DRect stateIn;

//  interaction data
uniform sampler2DRect kData;

float circle(in vec2 _st, in float _radius){
    vec2 dist = _st-vec2(0.5);
	return 1.-smoothstep(0,
                         _radius+0.05,
                         dot(dist,dist)*4.0);
}

void main()
{
	vec4 col = texture(colIn, texCoordVarying.st).rgba;

    //	Circle shading
	col.a *= circle(gl_PointCoord, 0.45);

    fragColor = col;
}