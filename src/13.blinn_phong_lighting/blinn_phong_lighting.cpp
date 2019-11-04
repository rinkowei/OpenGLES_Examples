#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::vector<std::shared_ptr<Mesh>> cubes;
	std::shared_ptr<Material> cubeMat;

	Example()
	{
		title = "blinn phong lighting";
		settings.vsync = true;
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

		std::vector<float> vertexAttribs = {
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
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 8); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 8], vertexAttribs[i * 8 + 1], vertexAttribs[i * 8 + 2]);
			vertex.vTexcoord = glm::vec2(vertexAttribs[i * 8 + 6], vertexAttribs[i * 8 + 7]);
			vertex.vNormal = glm::vec3(vertexAttribs[i * 8 + 3], vertexAttribs[i * 8 + 4], vertexAttribs[i * 8 + 5]);
			vertices.push_back(vertex);
		}

		cubeMat = Material::createFromFiles("cube_mat",
			{
				shadersDirectory + "cube.vert",
				shadersDirectory + "cube.frag"
			},
			{
				{ "diffuseMap_0", texturesDirectory + "cube_diffuse.png" },
				{ "specularMap_0", texturesDirectory + "cube_specular.png" }
			}
		);

		cubeMat->setUniform("shininess", 32.0f);
		// directional light
		cubeMat->setUniform("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
		cubeMat->setUniform("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
		cubeMat->setUniform("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
		cubeMat->setUniform("dirLight.specular", glm::vec3(0.4f, 0.4f, 0.4f));
		// point light
		cubeMat->setUniform("pointLight.position", glm::vec3(0.0f, 0.0f, 0.0f));
		cubeMat->setUniform("pointLight.ambient", glm::vec3(0.5f, 0.5f, 0.5f));
		cubeMat->setUniform("pointLight.diffuse", glm::vec3(0.8f, 0.8f, 0.8f));
		cubeMat->setUniform("pointLight.specular", glm::vec3(0.8f, 0.8f, 0.8f));
		cubeMat->setUniform("pointLight.constant", 1.0f);
		cubeMat->setUniform("pointLight.linear", 0.022f);
		cubeMat->setUniform("pointLight.quadratic", 0.0019f);
		// spot light
		cubeMat->setUniform("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
		cubeMat->setUniform("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
		cubeMat->setUniform("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));
		cubeMat->setUniform("spotLight.constant", 1.0f);
		cubeMat->setUniform("spotLight.linear", 0.09f);
		cubeMat->setUniform("spotLight.quadratic", 0.032f);
		cubeMat->setUniform("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
		cubeMat->setUniform("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

		// create cube mesh
		std::shared_ptr<Mesh> cubeTemplate = Mesh::createWithData("cube_template", vertices, {});
		cubeTemplate->setDrawType(Mesh::DrawType::ARRAYS);
		cubeTemplate->setMaterial(cubeMat);

		for (unsigned int i = 0; i < cubePositions.size(); i++)
		{
			std::shared_ptr<Mesh> cube = Mesh::clone("cube_" + std::to_string(i), cubeTemplate.get());
			cube->setPosition(cubePositions[i]);
			cube->setRotation(glm::vec3(20.0f * i, 12.0f * i, 7.0f * i));
			cube->setScale(glm::vec3(0.7f));
			cubes.push_back(cube);
		}
	}

	virtual void render(float deltaTime) override
	{
		cubeMat->setUniform("spotLight.position", mMainCamera->getPosition());
		cubeMat->setUniform("spotLight.direction", mMainCamera->getForward());

		for (unsigned int i = 0; i < cubes.size(); i++)
		{
			cubes[i]->render();
		}
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}
};

Example* example;
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE, LPSTR, int)
{
	example = new Example();
	example->setupValidation();
	if (!example->setupSDL() ||
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