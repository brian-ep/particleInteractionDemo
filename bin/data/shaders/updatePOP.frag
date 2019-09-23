#version 330

//  ping pong inputs
uniform sampler2DRect posIn;
uniform sampler2DRect velIn;
uniform sampler2DRect colIn;
uniform sampler2DRect stateIn;

//  interaction data
uniform sampler2DRect kData;

//  uniforms
uniform vec2 mouse;
uniform float elapsed;
uniform vec2 res;

//  attributes
in vec2 texCoordVarying;

//  output
layout(location = 0) out vec4 posOut;
layout(location = 1) out vec4 velOut;
layout(location = 2) out vec4 colOut;
layout(location = 3) out vec4 stateOut;

///////////////////////////////////////////////////////////////////////
#define TWO_PI 6.283185307179586476925286766559

float rand(vec2 n) { 
    return fract(sin(dot(n, vec2(12.9898, 4.1414))) * 43758.5453);
}

vec3 hash33(vec3 p) { 
    float n = sin(dot(p, vec3(7, 157, 113)));    
    return fract(vec3(2097152, 262144, 32768)*n)*2. - 1.;
}

float noise(in vec3 p)
{
    vec3 i = floor(p + dot(p, vec3(0.333333)) );  p -= i - dot(i, vec3(0.166666)) ;
    vec3 i1 = step(p.yzx, p), i2 = max(i1, 1.0-i1.zxy); i1 = min(i1, 1.0-i1.zxy);    
    
    vec3 p1 = p - i1 + 0.166666, p2 = p - i2 + 0.333333, p3 = p - 0.5;
  
    vec4 v = max(0.5 - vec4(dot(p,p), dot(p1,p1), dot(p2,p2), dot(p3,p3)), 0.0);
    
    vec4 d = vec4(dot(p, hash33(i)), dot(p1, hash33(i + i1)), dot(p2, hash33(i + i2)), dot(p3, hash33(i + 1.)));
    
    return clamp(dot(d, v*v*v*8.)*1.732 + .5, 0., 1.); 
}

///////////////////////////////////////////////////////////////////////
void main()
{
    //  previous frame
    vec3 pos = texture(posIn, texCoordVarying.st).xyz;
    vec3 vel = texture(velIn, texCoordVarying.st).xyz;
    vec4 col = texture(colIn, texCoordVarying.st).rgba;
    float state = texture(stateIn, texCoordVarying.st).r;

    //  interaction data
    vec2 texPos = pos.xy;
    //texPos.y -= res.y;
    vec4 kVal = texture(kData, texPos.xy);
    
    //  forces
    vec3 force = vec3(0.0);

    //  Noise
    float angle = noise(vec3(pos.x * 10.0, pos.y * 10.0, 1.0)) * TWO_PI * 4.0;
    vec3 dir = vec3(cos(angle), sin(angle), 0);
    force += 128.0 * dir;

    //  gravity
    force -= vec3(0.0, 20.0 * 9.82, 0.0);

     //  Variance
    float variance = texture(velIn, texCoordVarying.st).a;
    force *= variance;

    //  accelerate
    vel += 0.1 * force;

    vel.y = -400.0 * variance;

    //  move
    pos += elapsed * vel;

    //  remap off-screen particles
    if(pos.x < -10.0){
        pos.x += 10.0 + res.x;
        col.a = 0.0;
    }
    else if (pos.x > 10.0 + res.x){
        pos.x -= 10.0 + res.x;
        col.a = 0.0;
    }
    if(pos.y > res.y + 10.0){
        pos.y -= 10.0 + res.y;
        col.a = 0.0;
    }
    else if(pos.y < -10.0){
        pos.y += 10.0 + res.y;
        col.a = 0.0;
    }
    
    //  damping
    vel *= 0.995;

    //  Interaction
    if(kVal == 1.0) col.a = 0.75;
    else col.a *= 0.995 * clamp(2.0 * variance, 0.0, 1.0);
    
    posOut = vec4(pos, 1.0);
    velOut = vec4(vel, variance);
    colOut = vec4(col);
    stateOut = vec4(state);
}