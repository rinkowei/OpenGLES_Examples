
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	GLuint VAO, VBO, EBO, instanceVBO;
	Material* material;
	Example()
	{
		title = "triangle instancing";
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/06.triangle_instancing/";
	}
	~Example()
	{
		delete(material);
		glDeleteBuffers(1, &instanceVBO);
		glDeleteBuffers(1, &EBO);
		glDeleteBuffers(1, &VBO);
		glDeleteVertexArrays(1, &VAO);
	}
public:
	virtual void prepare() override
	{
		// create a array of 100 triangle locations
		std::array<glm::vec2, 100> locations;
		unsigned int index = 0;
		float offset = 0.1f;
		for (int x = -10; x < 10; x += 2)
		{
			for (int y = -10; y < 10; y += 2)
			{
				glm::vec2 position((float)x / 10.0f + offset, (float)y / 10.0f + offset);
				locations[index++] = position;
			}
		}

		std::vector<GLfloat> vertexAttrs = {
			// positions           // colors
			 0.05f, -0.05f, 0.0f,   1.0f, 0.0f, 0.0f, 1.0f,
			-0.05f, -0.05f, 0.0f,   0.0f, 1.0f, 0.0f, 1.0f,
			-0.05f,  0.05f, 0.0f,   0.0f, 0.0f, 1.0f, 1.0f,
		};

		std::vector<GLuint> indices = {
			0, 1, 2
		};

		glGenBuffers(1, &instanceVBO);
		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * locations.size(), locations.data(), GL_STATIC_DRAW);
		glBindBuffer(GL_ARRAY_BUFFER, 0);

		glGenVertexArrays(1, &VAO);
		glGenBuffers(1, &VBO);
		glGenBuffers(1, &EBO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferData(GL_ARRAY_BUFFER, vertexAttrs.size() * sizeof(GLfloat), vertexAttrs.data(), GL_STATIC_DRAW);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat)));
		glEnableVertexAttribArray(1);

		glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(2);
		glVertexAttribDivisor(2, 1);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "triangle.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "triangle.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths = {};

		// create triangle material
		material = Material::createWithFile(shaderPaths, texturePaths);

	}
	virtual void update() override
	{
		material->apply();
		glBindVertexArray(VAO);
		glDrawElementsInstanced(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0, 100);
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupGLFW() ||
		!example->loadGLESFunctions() ||
		!example->setupImGui())
	{
		return 0;
	}
	example->prepare();
	example->renderLoop();
	delete(example);
	return 0;
}




