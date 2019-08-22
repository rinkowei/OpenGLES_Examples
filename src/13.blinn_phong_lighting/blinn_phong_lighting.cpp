
#include <common.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Material> material;
	Example()
	{
		title = "blinn phong lighting";
		settings.vsync = false;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/13.blinn_phong_lighting/";
		texturesDirectory = getResourcesPath(ResourceType::Texture) + "/13.blinn_phong_lighting/";
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();

		// setup camera
		camera->rotationSpeed = 0.5f;
		camera->setPosition(glm::vec3(0.0f, 0.0f, 4.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// all positions of cubes
		std::array<glm::vec3, 10> cubePositions = {
			glm::vec3(0.0f,  3.0f,  -3.0f),
			glm::vec3(0.8f,  3.0f, -4.0f),
			glm::vec3(-2.5f, 2.2f, -3.2f),
			glm::vec3(-3.5f, -2.0f, -3.3f),
			glm::vec3(2.0f, -0.4f, -2.0f),
			glm::vec3(-1.7f,  3.0f, -3.5f),
			glm::vec3(1.3f, -3.0f, -2.8f),
			glm::vec3(1.8f,  2.0f, 0.5f),
			glm::vec3(2.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
		};

		std::vector<GLfloat> vertexAttrs = {
			// positions          // normals           // texture coordinates
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
			 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttrs.size() / 8); i++)
		{
			Vertex vertex;
			vertex.Position = glm::vec3(vertexAttrs[i * 8], vertexAttrs[i * 8 + 1], vertexAttrs[i * 8 + 2]);
			vertex.TexCoords = glm::vec2(vertexAttrs[i * 8 + 6], vertexAttrs[i * 8 + 7]);
			vertex.Normal = glm::vec3(vertexAttrs[i * 8 + 3], vertexAttrs[i * 8 + 4], vertexAttrs[i * 8 + 5]);
			vertices.push_back(vertex);
		}

		std::unordered_map<Material::ShaderType, std::string> shaderPaths =
		{
			{ Material::ShaderType::Vertex, shadersDirectory + "cube.vert" },
			{ Material::ShaderType::Fragment, shadersDirectory + "cube.frag" }
		};

		std::vector<std::pair<Texture::Type, std::string>> texturePaths =
		{
			std::make_pair(Texture::Type::Diffuse, texturesDirectory + "cube_diffuse.png"),
			std::make_pair(Texture::Type::Specular, texturesDirectory + "cube_specular.png")
		};

		// create cube material
	    material = std::make_shared<Material>(shaderPaths, texturePaths);

		for (unsigned int i = 0; i < cubePositions.size(); i++)
		{
			Mesh* cube = Mesh::createWithData(vertices, {}, Mesh::DrawType::Arrays, material);
			cube->setPosition(cubePositions[i]);
			cube->setRotation(glm::vec3(20.0f * i, 12.0f * i, 7.0f * i));
			cube->setScale(glm::vec3(0.7f));

			addObject(static_cast<Object*>(cube));
		}
	}

	virtual void update(float deltaTime) override
	{
		material->setVec3("viewPos", camera->getPosition());
		material->setFloat("shininess", 32.0f);
		// directional light
		material->setVec3("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		material->setVec3("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		material->setVec3("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		material->setVec3("dirLight.specular", glm::vec3(0.4f, 0.4f, 0.4f));
		// point light
		material->setVec3("pointLight.position", glm::vec3(0.0f, 0.0f, 0.0f));
		material->setVec3("pointLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		material->setVec3("pointLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		material->setVec3("pointLight.specular", glm::vec3(0.8f, 0.8f, 0.8f));
		material->setFloat("pointLight.constant", 1.0f);
		material->setFloat("pointLight.linear", 0.022f);
		material->setFloat("pointLight.quadratic", 0.0019f);
		// spot light
		material->setVec3("spotLight.position", camera->getPosition());
		material->setVec3("spotLight.direction", camera->getFrontVector());
		material->setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
		material->setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
		material->setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
		material->setFloat("spotLight.constant", 1.0f);
		material->setFloat("spotLight.linear", 0.09f);
		material->setFloat("spotLight.quadratic", 0.032f);
		material->setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		material->setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
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