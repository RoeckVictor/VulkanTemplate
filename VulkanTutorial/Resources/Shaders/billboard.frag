// Fragment
#version 450

layout(location = 0) in vec2 fragOffset;

layout(location = 0) out vec4 outColor;

struct PointLight {
	vec3 position;
	vec4 color;
};

layout(set = 0, binding = 0) uniform UniformBufferObject 
{
	mat4 projection;
    mat4 view;
    mat4 invView;
    vec4 ambientColor;
	PointLight lights[100];
	int numLights;
} ubo;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(push_constant) uniform Push
{
	vec4 position;
	vec4 color;
	float radius;
} push;


void main()
{
	vec4 tex = texture(texSampler, fragOffset);
	if (tex.a < 0.1)
		discard;
    outColor = vec4(push.color.xyz, 1.0) * texture(texSampler, fragOffset);
}