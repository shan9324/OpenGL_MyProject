
#include <GL\glew.h>
#include <GLFW\glfw3.h>
#include <stdio.h>
#include <string.h>
#include <cmath>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>
#include <glm\gtc\type_ptr.hpp>
#include "Mesh.h"
#include "Shader.h"
#include "Window.h"
#include "Camera.h"
#include "stb_image.h"
#include "Texture.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "CommonValues.h"

// Window dimensions
const GLint WIDTH = 800, HEIGHT = 600;
const float toRadians = 3.14159265f / 180.0f;

Light mainLight0;
GLuint shader, uniformModel, uniformProjection, uniformView, uniformEyePosition, uniformPointLightCount, uniformSpotLightCount;
GLfloat uniformSpecularIntensity, uniformSpecularShininess;

struct {
	GLuint uniformColour;
	GLuint uniformAmbientIntensity;
	GLuint uniformDiffuseIntensity;

	GLuint uniformDirection;
} uniformDirectionalLight;


struct {
	GLuint uniformColour;
	GLuint uniformAmbientIntensity;
	GLuint uniformDiffuseIntensity;

	GLuint uniformPosition;
	GLuint uniformConstant;
	GLuint uniformLinear;
	GLuint uniformExponent;
} uniformPointLight[MAX_POINT_LIGHTS];

struct {
	GLuint uniformColour;
	GLuint uniformAmbientIntensity;
	GLuint uniformDiffuseIntensity;

	GLuint uniformPosition;
	GLuint uniformDirection;
	GLuint uniformConstant;
	GLuint uniformLinear;
	GLuint uniformExponent;
	GLuint uniformEdge;
} uniformSpotLight[MAX_SPOT_LIGHTS];

char* brickLocation = const_cast<char*>("Textures/brick.png");
char* dirtLocation = const_cast<char*>("Textures/dirt.png");

Texture brickTexture(brickLocation);
Texture dirtTexture(dirtLocation);


bool direction = true;
float triOffset = 0.0f;
float triMaxOffset = 0.7f;
float triIncrement = 0.0005f;

float curAngle = 0.0f;

bool sizeDirection = true;
float curSize = 0.4f;
float maxSize = 0.8f;
float minSize = 0.1f;
GLfloat deltaTime = 0.0f;
GLfloat lastTime = 0.0f;

GLfloat intensityInc = 0.1f;

Mesh mesh, mesh1;


void SetDirectionalLight(DirectionalLight* dlight) {
	dlight->UseLight(uniformDirectionalLight.uniformAmbientIntensity,
		uniformDirectionalLight.uniformColour,
		uniformDirectionalLight.uniformDiffuseIntensity,
		uniformDirectionalLight.uniformDirection);
}
/*GLfloat ambientIntensityLocation, GLfloat ambientColourLocation, GLfloat diffuseIntensityLocation, GLfloat positionLocation,
GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation*/
void SetPointLight(PointLight* plight, GLuint lightCount) {
	if (lightCount > MAX_POINT_LIGHTS) lightCount = MAX_POINT_LIGHTS;

	glUniform1i(uniformPointLightCount, lightCount);
	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {
		plight[i].UseLight(uniformPointLight[i].uniformAmbientIntensity,
			uniformPointLight[i].uniformColour,
			uniformPointLight[i].uniformDiffuseIntensity,
			uniformPointLight[i].uniformPosition,
			uniformPointLight[i].uniformConstant, 
			uniformPointLight[i].uniformLinear, 
			uniformPointLight[i].uniformExponent );
	}
}

void SetSpotLight(SpotLight* slight, GLuint lightCount) {
	if (lightCount > MAX_SPOT_LIGHTS) lightCount = MAX_SPOT_LIGHTS;

	glUniform1i(uniformSpotLightCount, lightCount);
	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {
		slight[i].UseLight(uniformSpotLight[i].uniformAmbientIntensity,
			uniformSpotLight[i].uniformColour,
			uniformSpotLight[i].uniformDiffuseIntensity,
			uniformSpotLight[i].uniformPosition,
			uniformSpotLight[i].uniformDirection,
			uniformSpotLight[i].uniformConstant,
			uniformSpotLight[i].uniformLinear,
			uniformSpotLight[i].uniformExponent,
			uniformSpotLight[i].uniformEdge);
	}
}

// Vertex Shader code
static const char* vShader = "                                                \n\
#version 330                                                                  \n\
                                                                              \n\
layout (location = 0) in vec3 pos;											  \n\
layout (location = 1) in vec2 tex;											  \n\
layout (location = 2) in vec3 norm;											  \n\
																			  \n\
out vec4 vCol;																  \n\
out vec2 TexCoord;															  \n\
out vec3 Normal;															  \n\
out vec3 FragPos;															  \n\
                                                                              \n\
uniform mat4 model;                                                           \n\
uniform mat4 projection;                                                      \n\
uniform mat4 view;                                                            \n\
                                                                              \n\
void main()                                                                   \n\
{                                                                             \n\
    gl_Position = projection * view * model * vec4(pos, 1.0);			      \n\
	vCol = vec4(clamp(pos, 0.0f, 1.0f), 1.0);								  \n\
	TexCoord =tex;                           								  \n\
	Normal =  mat3(transpose(inverse(model))) * norm;			              \n\
	FragPos = (model * vec4(pos,1.0)).xyz;						              \n\
}";
// model *pos = World View
// FragPos is the position on the surface where the light is going to be incident
// 
// 
// Fragment Shader code below
static const char* fShader = "                                                \n\
#version 330																																					\n\
             																																					\n\
             																																					\n\
in vec4 vCol;																																									\n\
in vec2 TexCoord;																																									\n\
in vec3 Normal;																																									\n\
in vec3 FragPos;																																									\n\
             																																					\n\
out vec4 colour;																																									\n\
             																																					\n\
const int MAX_POINT_LIGHTS = 3;																																									\n\
const int MAX_SPOT_LIGHTS = 3;																																									\n\
             																																					\n\
struct Light             																																					\n\
{             																																					\n\
	vec3 colour;																																									\n\
	float ambientIntensity;																																									\n\
	float diffuseIntensity;																																									\n\
};																																									\n\
             																																					\n\
struct DirectionalLight             																																					\n\
{             																																					\n\
	Light base;																																									\n\
	vec3 direction;																																									\n\
};																																									\n\
             																																					\n\
struct PointLight             																																					\n\
{             																																					\n\
	Light base;																																									\n\
	vec3 position;																																									\n\
	float constant;																																									\n\
	float linear;																																									\n\
	float exponent;																																									\n\
};																																									\n\
             																																					\n\
struct SpotLight             																																					\n\
{             																																					\n\
	PointLight base;																																									\n\
	vec3 direction;																																									\n\
	float edge;																																									\n\
};																																									\n\
             																																					\n\
struct Material             																																					\n\
{             																																					\n\
	float specularIntensity;																																									\n\
	float shininess;																																									\n\
};																																									\n\
             																																					\n\
uniform int pointLightCount;																																									\n\
uniform int spotLightCount;																																									\n\
             																																				\n\
uniform DirectionalLight directionalLight;																																									\n\
uniform PointLight pointLights[MAX_POINT_LIGHTS];																																									\n\
uniform SpotLight spotLights[MAX_SPOT_LIGHTS];																																									\n\
             																																				\n\
uniform sampler2D theTexture;																																									\n\
uniform Material material;																																									\n\
             																																				\n\
uniform vec3 eyePosition;																																									\n\
             																																					\n\
vec4 CalcLightByDirection(Light light, vec3 direction)             																																					\n\
{             																																					\n\
	vec4 ambientColour = vec4(light.colour, 1.0f) * light.ambientIntensity;																																									\n\
             																																					\n\
	float diffuseFactor = max(dot(normalize(Normal), normalize(direction)), 0.0f);																																									\n\
	vec4 diffuseColour = vec4(light.colour * light.diffuseIntensity * diffuseFactor, 1.0f);																																									\n\
             																																				\n\
	vec4 specularColour = vec4(0, 0, 0, 0);																																									\n\
             																																				\n\
	if (diffuseFactor > 0.0f)             																																					\n\
	{             																																					\n\
		vec3 fragToEye = normalize(eyePosition - FragPos);																																									\n\
			vec3 reflectedVertex = normalize(reflect(direction, normalize(Normal)));																																									\n\
             																																				\n\
			float specularFactor = dot(fragToEye, reflectedVertex);																																									\n\
			if (specularFactor > 0.0f)             																																					\n\
			{             																																					\n\
				specularFactor = pow(specularFactor, material.shininess);																																									\n\
					specularColour = vec4(light.colour * material.specularIntensity * specularFactor, 1.0f);																																									\n\
			}             																																					\n\
	}             																																					\n\
	\n\
	return (ambientColour + diffuseColour + specularColour);																																									\n\             																																					\n\
}             																																					\n\
             																																					\n\
vec4 CalcDirectionalLight()             																																					\n\
{             																																					\n\
	return CalcLightByDirection(directionalLight.base, directionalLight.direction);																																									\n\
}             																																					\n\
             																																					\n\             																																					\n\
vec4 CalcPointLight(PointLight pLight)             																																					\n\
{             																																					\n\
	vec3 direction = FragPos - pLight.position;																																									\n\
	float distance = length(direction);																																									\n\
	direction = normalize(direction);																																									\n\
             																																				\n\
	vec4 colour = CalcLightByDirection(pLight.base, direction);																																									\n\
	float attenuation = pLight.exponent * distance * distance + pLight.linear * distance + pLight.constant;																																									\n\
             																																				\n\
	return (colour / attenuation);																																									\n\
}             																																					\n\
             																																					\n\
vec4 CalcSpotLight(SpotLight sLight)             																																					\n\
{             																																					\n\
	vec3 rayDirection = normalize(FragPos - sLight.base.position);																																									\n\
	float slFactor = dot(rayDirection, sLight.direction);																																									\n\
             																																				\n\
	if (slFactor > sLight.edge)             																																					\n\
	{             																																					\n\
		vec4 colour = CalcPointLight(sLight.base);																																									\n\
             																																				\n\
			return colour * (1.0f - (1.0f - slFactor) * (1.0f / (1.0f - sLight.edge)));																																									\n\
             																																				\n\
	}             																																					\n\
	else {             																																					\n\
		return vec4(0, 0, 0, 0);																																									\n\
	}             																																					\n\
}             																																					\n\
             																																					\n\
vec4 CalcPointLights()             																																					\n\
{             																																					\n\
	vec4 totalColour = vec4(0, 0, 0, 0);																																									\n\
	for (int i = 0; \n\ i < pointLightCount; \n\ i++)             																																					\n\
	{             																																					\n\
		totalColour += CalcPointLight(pointLights[i]);																																									\n\
	}             																																					\n\
             																																					\n\
	return totalColour;																																									\n\
}             																																					\n\
             																																					\n\
vec4 CalcSpotLights()             																																					\n\
{             																																					\n\
	vec4 totalColour = vec4(0, 0, 0, 0);																																									\n\
	for (int i = 0; \n\ i < spotLightCount; \n\ i++)             																																					\n\
	{             																																					\n\
		totalColour += CalcSpotLight(spotLights[i]);																																									\n\
	}             																																					\n\
             																																					\n\
	return totalColour;																																									\n\
}             																																					\n\
             																																					\n\
void main()             																																					\n\
{             																																					\n\
	vec4 finalColour = CalcDirectionalLight();																																									\n\
	finalColour += CalcPointLights();																																									\n\
	finalColour += CalcSpotLights();																																									\n\
             																																				\n\
	colour = texture(theTexture, TexCoord) * finalColour;																																									\n\
}																																					\n\
";


/*
* Ambient Colour is the percentage of light that is incident upon the surface.
* If texture color is purely red, but the ambient intensity of the red colour is 0, it will be shown black.
* 
* 
*/

void calcAverageNormals(unsigned int* indices, unsigned int indiceCount, GLfloat* vertices, unsigned int verticeCount,
	unsigned int vLength, unsigned int normalOffset)
{
	for (size_t i = 0; i < indiceCount; i += 3)
	{
		unsigned int in0 = indices[i] * vLength;
		unsigned int in1 = indices[i + 1] * vLength;
		unsigned int in2 = indices[i + 2] * vLength;
		glm::vec3 v1(vertices[in1] - vertices[in0], vertices[in1 + 1] - vertices[in0 + 1], vertices[in1 + 2] - vertices[in0 + 2]);
		glm::vec3 v2(vertices[in2] - vertices[in0], vertices[in2 + 1] - vertices[in0 + 1], vertices[in2 + 2] - vertices[in0 + 2]);
		glm::vec3 normal = glm::cross(v1, v2);
		normal = glm::normalize(normal);

		in0 += normalOffset; in1 += normalOffset; in2 += normalOffset;
		vertices[in0] += normal.x; vertices[in0 + 1] += normal.y; vertices[in0 + 2] += normal.z;
		vertices[in1] += normal.x; vertices[in1 + 1] += normal.y; vertices[in1 + 2] += normal.z;
		vertices[in2] += normal.x; vertices[in2 + 1] += normal.y; vertices[in2 + 2] += normal.z;
	}

	for (size_t i = 0; i < verticeCount / vLength; i++)
	{
		unsigned int nOffset = i * vLength + normalOffset;
		glm::vec3 vec(vertices[nOffset], vertices[nOffset + 1], vertices[nOffset + 2]);
		vec = glm::normalize(vec);
		vertices[nOffset] = vec.x; vertices[nOffset + 1] = vec.y; vertices[nOffset + 2] = vec.z;
	}
}
void CreateTriangle()
{
	unsigned int indices[] = {
		0, 3, 1,
		1, 3, 2,
		2, 3, 0,
		0, 1, 2
	};

	unsigned int indices1[] = {
		0, 2, 1,
		1, 2, 3
	};

	GLfloat vertices[] = {
		//x,y,z, textureU, textureV, Normal.x, Normal.y, Normal.z
		-1.0f, -1.0f,  -0.6f,  0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
	     0.0f, -1.0f,  1.0f,  0.5f, 0.0f, 0.0f, 0.0f, 0.0f,
		 1.0f, -1.0f,  -0.6f,  1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		 0.0f,  1.0f,  0.0f,  0.5f, 1.0f, 0.0f, 0.0f, 0.0f
	};

	GLfloat floorVertices[] = {
		//x,y,z, textureU, textureV, Normal.x, Normal.y, Normal.z
		-10.0f,  0.0f,  -10.0f,   0.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		 10.0f,  0.0f,  -10.0f,  10.0f,  0.0f, 0.0f, -1.0f, 0.0f,
		-10.0f,  0.0f,   10.0f,   0.0f, 10.0f, 0.0f, -1.0f, 0.0f,
		 10.0f,  0.0f,   10.0f,  10.0f, 10.0f, 0.0f, -1.0f, 0.0f
	};

	calcAverageNormals(indices, 12, vertices, 32, 8, 5);
	//calcAverageNormals(indices1, 12, vertices, 32, 8, 5);
	mesh.CreateMesh(vertices, indices, sizeof(vertices), sizeof(indices));
	mesh1.CreateMesh(floorVertices, indices1, sizeof(floorVertices), sizeof(indices1));
}

void AddShader(GLuint theProgram, const char* shaderCode, GLenum shaderType)
{
	GLuint theShader = glCreateShader(shaderType);

	const GLchar* theCode[1];
	theCode[0] = shaderCode;

	GLint codeLength[1];
	codeLength[0] = strlen(shaderCode);

	glShaderSource(theShader, 1, theCode, codeLength);
	glCompileShader(theShader);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glGetShaderiv(theShader, GL_COMPILE_STATUS, &result);
	if (!result)
	{
		glGetShaderInfoLog(theShader, 1024, NULL, eLog);
		fprintf(stderr, "Error compiling the %d shader: '%s'\n", shaderType, eLog);
		return;
	}

	glAttachShader(theProgram, theShader);
}

void CompileShaders()
{
	shader = glCreateProgram();

	if (!shader)
	{
		printf("Failed to create shader\n");
		return;
	}

	AddShader(shader, vShader, GL_VERTEX_SHADER);
	AddShader(shader, fShader, GL_FRAGMENT_SHADER);

	GLint result = 0;
	GLchar eLog[1024] = { 0 };

	glLinkProgram(shader);
	glGetProgramiv(shader, GL_LINK_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error linking program: '%s'\n", eLog);
		return;
	}

	glValidateProgram(shader);
	glGetProgramiv(shader, GL_VALIDATE_STATUS, &result);
	if (!result)
	{
		glGetProgramInfoLog(shader, sizeof(eLog), NULL, eLog);
		printf("Error validating program: '%s'\n", eLog);
		return;
	}

	uniformModel = glGetUniformLocation(shader, "model");
	uniformProjection = glGetUniformLocation(shader, "projection");
	uniformView = glGetUniformLocation(shader, "view");
	uniformDirectionalLight.uniformColour= glGetUniformLocation(shader, "directionalLight.base.colour");
	uniformDirectionalLight.uniformAmbientIntensity = glGetUniformLocation(shader, "directionalLight.base.ambientIntensity");
	uniformDirectionalLight.uniformDirection = glGetUniformLocation(shader, "directionalLight.direction");
	uniformDirectionalLight.uniformDiffuseIntensity = glGetUniformLocation(shader, "directionalLight.base.diffuseIntensity");
	uniformSpecularIntensity = glGetUniformLocation(shader, "material.specularIntensity");
	uniformSpecularShininess = glGetUniformLocation(shader, "material.shininess");
	uniformEyePosition = glGetUniformLocation(shader, "EyePos");
	uniformPointLightCount = glGetUniformLocation(shader, "pointLightCount");
	uniformSpotLightCount = glGetUniformLocation(shader, "spotLightCount");

	for (size_t i = 0; i < MAX_POINT_LIGHTS; i++) {

		char logbuff[100] = { '\0' };
		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].base.colour", i);
		uniformPointLight[i].uniformColour = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].base.ambientIntensity", i);
		uniformPointLight[i].uniformAmbientIntensity = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].base.diffuseIntensity", i);
		uniformPointLight[i].uniformDiffuseIntensity = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].position", i);
		uniformPointLight[i].uniformPosition = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].constant", i);
		uniformPointLight[i].uniformConstant = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].linear", i);
		uniformPointLight[i].uniformLinear = glGetUniformLocation(shader, logbuff);

		snprintf(logbuff, sizeof(logbuff), "pointLights[%d].exponent", i);
		uniformPointLight[i].uniformExponent = glGetUniformLocation(shader, logbuff);

	}

	for (size_t i = 0; i < MAX_SPOT_LIGHTS; i++) {

		char locBuff[100] = { '\0' };

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.colour", i);
		uniformSpotLight[i].uniformColour = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.ambientIntensity", i);
		uniformSpotLight[i].uniformAmbientIntensity = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.base.diffuseIntensity", i);
		uniformSpotLight[i].uniformDiffuseIntensity = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.position", i);
		uniformSpotLight[i].uniformPosition = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.constant", i);
		uniformSpotLight[i].uniformConstant = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.linear", i);
		uniformSpotLight[i].uniformLinear = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].base.exponent", i);
		uniformSpotLight[i].uniformExponent = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].direction", i);
		uniformSpotLight[i].uniformDirection = glGetUniformLocation(shader, locBuff);

		snprintf(locBuff, sizeof(locBuff), "spotLights[%d].edge", i);
		uniformSpotLight[i].uniformEdge = glGetUniformLocation(shader, locBuff);

	}
}

int main()
{
	Window window(1366, 768);
	window.Initialise();

	CreateTriangle();
	CompileShaders();

	Camera camera(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 1.0f, 0.0f),
		-60.0f,
		0.0f,
		5.0f,
		0.5f);

	brickTexture.LoadTexture();
	dirtTexture.LoadTexture();

	char* grassLocation = const_cast<char*>("Textures/grass.png");
	Texture grassTexture(grassLocation);
	grassTexture.LoadTexture();


	glm::mat4 projection = glm::perspective(glm::radians(45.0f), (GLfloat)(window.getBufferWidth()) / (GLfloat)(window.getBufferHeight()), 0.1f, 100.0f);

	// Loop until window closed
	while (!window.getShouldClose())
	{
		// Get + Handle user input events
		glfwPollEvents();

		if (direction)
		{
			triOffset += triIncrement;
		}
		else {
			triOffset -= triIncrement;
		}

		if (abs(triOffset) >= triMaxOffset)
		{
			direction = !direction;
		}

		curAngle += 0.01f;
		if (curAngle >= 360)
		{
			curAngle -= 360;
		}

		if (direction)
		{
			curSize += 0.0001f;
		}
		else {
			curSize -= 0.0001f;
		}

		if (curSize >= maxSize || curSize <= minSize)
		{
			sizeDirection = !sizeDirection;
		}

		GLfloat now = glfwGetTime();
		deltaTime = now - lastTime;
		lastTime = now;

		if (intensityInc >= 1.0f) {
			intensityInc = 0.1f;
		}
		else {
			intensityInc += 0.4f;
		}

		//DirectionalLight mainLight(1.0f, 1.0f, 1.0f, 0.2f, 0.3f, 2.0f, -1.0f, -2.0f);
		DirectionalLight mainLight = DirectionalLight(1.0f, 1.0f, 1.0f,
			0.3f, 0.4f,
			0.0f, 0.0f, -1.0f);
		PointLight pointLights[MAX_POINT_LIGHTS];
		unsigned int pointLightCount = 0;
		pointLights[0] = PointLight(0.0f, 0.0f, 1.0f,
			0.3f, 0.3f,
			1.0f, 2.0f, -3.0f,
			1.0f, 0.4f, 0.3f);
		pointLightCount++;
		pointLights[1] = PointLight(1.0f, 0.0f, 0.0f,
			0.3f, 0.3f,
			-1.0f, 2.0f, -3.0f,
			1.0f, 0.3f, 0.3f);
		pointLightCount++;

		SpotLight spotLights[MAX_POINT_LIGHTS];
		unsigned int spotLightCount = 0;
		spotLights[0] = SpotLight(1.0f, 1.0f, 1.0f,
			0.0f, 2.0f,
			0.0f, 0.0f, -2.0f,
			0.0f, -1.0f, 0.0f,
			1.0f, 0.0f, 0.0f,
			20.0f);
		spotLightCount++;
		/*spotLights[1] = SpotLight(1.0f, 0.0f, 0.0f,
			0.3f, 0.3f,
			-1.0f, 2.0f, -3.0f,
			1.0f, 0.3f, 0.3f);
		spotLightCount++;*/



		// Clear window
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		{
			glUseProgram(shader);


			Material shinyMaterial(4.0f, 256);
			glm::mat4 model(1.0f);
			glm::mat4 view = camera.calculateViewMatrix();
			camera.keyControl(window.getKeys(), deltaTime);
			camera.mouseControl(window.getXchange(), window.getYchange());

			model = glm::translate(model, glm::vec3(0.0f, 0.0f, -3.0f));
			model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));

			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			
			glm::vec3 lowerLight = camera.getCameraPosition();
			lowerLight.y -= 0.3f;
			spotLights[0].SetFlash(lowerLight, camera.getCameraDirection());
			SetDirectionalLight(&mainLight);
			SetPointLight(pointLights, pointLightCount);
			SetSpotLight(spotLights, spotLightCount);

			brickTexture.UseTexture();
			shinyMaterial.UseMaterial(uniformSpecularIntensity, uniformSpecularShininess);
			mesh.RenderMesh();
			glUseProgram(0);
		}
		{
			glUseProgram(shader);
			Material dullMaterial(0.3f, 4);
			glm::mat4 model(1.0f);
			glm::mat4 view = camera.calculateViewMatrix();
			camera.keyControl(window.getKeys(), deltaTime);
			camera.mouseControl(window.getXchange(), window.getYchange());

			model = glm::translate(model, glm::vec3(0.0f, 0.5f, -3.0f));
			model = glm::rotate(model, curAngle * toRadians, glm::vec3(0.0f, 1.0f, 0.0f));
			model = glm::scale(model, glm::vec3(0.2f, 0.2f, 0.2f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			glUniformMatrix4fv(uniformProjection, 1, GL_FALSE, glm::value_ptr(projection));
			glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(view));
			glUniform3f(uniformEyePosition, camera.getCameraPosition().x, camera.getCameraPosition().y, camera.getCameraPosition().z);
			SetDirectionalLight(&mainLight);
			SetPointLight(pointLights, pointLightCount);
			//SetSpotLight(spotLights, spotLightCount);

			dirtTexture.UseTexture();
			dullMaterial.UseMaterial(uniformSpecularIntensity, uniformSpecularShininess);
			mesh.RenderMesh();
			glUseProgram(0);
		}
		{
			glUseProgram(shader);
			glm::mat4 model(1.0f);

			model = glm::translate(model, glm::vec3(0.0f, -0.1f, -3.0f));

			glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
			dirtTexture.UseTexture();
			mesh1.RenderMesh();
			glUseProgram(0);
		}


		window.swapBuffers();
	}
	return 0;
}