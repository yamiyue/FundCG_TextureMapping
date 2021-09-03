#version 430 core

layout(location = 0) in vec3 vertex;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 uv;

vec3 vLightPosition = vec3(0,10,50);

uniform mat4 ProjectionMatrix;
uniform mat4 ModelViewMatrix;
uniform float OffsetFactor;

out vec3 vVaryingNormal;
out vec3 vVaryingLightDir;

out vec2 vUV;

void main(void)
{
	// Get surface normal in eye coordinates
	mat3 normalMatrix = mat3(ModelViewMatrix);//normal matrix is MV matrix's 3*3 excluding 'w' 
    vVaryingNormal = normalMatrix * normal;

    // Get vertex position in eye coordinates
    vec4 vPosition4 = ModelViewMatrix * vec4(vertex,1);
    vec3 vPosition3 = vPosition4.xyz / vPosition4.w;

    // Get vector to light source
    vVaryingLightDir = normalize(vLightPosition - vPosition3);
	
    gl_Position = ProjectionMatrix * ModelViewMatrix * vec4(vertex + normal * OffsetFactor, 1.0);
    vUV = uv;
}
