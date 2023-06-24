// Fragment
#version 450

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec3 fragWorldPos;
layout(location = 2) in vec3 fragWorldNormal;

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


layout(push_constant) uniform Push 
{ 
    mat4 modelMatrix;
    mat4 normalMatrix;
} push;

void main()
{
    vec3 ambientLight = ubo.ambientColor.xyz * ubo.ambientColor.w;
    vec3 specularLight = vec3(0.0);
    vec3 diffuseLight = vec3(0.0);

    vec3 cameraPosWorld = ubo.invView[3].xyz;
    vec3 viewDir = normalize(cameraPosWorld - fragWorldPos);
    vec3 surfaceNormal = normalize(fragWorldNormal);

    for(int i = 0; i < ubo.numLights; i++)
	{
        PointLight light = ubo.lights[i];
        vec3 dirToLight = light.position.xyz - fragWorldPos;
        float attenuation = 1.0 / dot(dirToLight, dirToLight);
        dirToLight = normalize(dirToLight);

        // Diffuse
        float cosAngle = max(dot(surfaceNormal, dirToLight), 0);
        vec3 intensity = light.color.xyz * light.color.w * attenuation;
        diffuseLight += intensity * cosAngle;

        // Specular
        vec3 halfDir = normalize(dirToLight + viewDir);
        float cosAngleHalf = dot(surfaceNormal, halfDir);
        cosAngleHalf = clamp(cosAngleHalf, 0, 1);
        cosAngleHalf = pow(cosAngleHalf, 64.0);
        specularLight += intensity * cosAngleHalf;
	}

    vec3 lightValue = ambientLight + diffuseLight + specularLight;
    // outColor = vec4(lightValue * fragColor, 1.0);
}