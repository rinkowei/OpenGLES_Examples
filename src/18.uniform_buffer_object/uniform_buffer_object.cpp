#include <examplebase.h>
#include <mesh.h>
#include <material.h>
using namespace es;

class Example final : public ExampleBase
{
public:
	std::shared_ptr<Mesh> cubeBlue;
	std::shared_ptr<Mesh> cubeGreen;
	std::shared_ptr<Mesh> cubeRed;
	std::shared_ptr<Mesh> cubeYellow;

	std::unique_ptr<UniformBuffer> uniformBuffer;

	Example()
	{
		title = "uniform buffer object";
		settings.vsync = true;
		defaultClearColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/18.uniform_buffer_object/";
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

		std::vector<float> vertexAttribs = {
			// positions         
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f
		};

		std::vector<Vertex> vertices = {};
		for (uint32_t i = 0; i < static_cast<uint32_t>(vertexAttribs.size() / 3); i++)
		{
			Vertex vertex;
			vertex.vPosition = glm::vec3(vertexAttribs[i * 3], vertexAttribs[i * 3 + 1], vertexAttribs[i * 3 + 2]);
			vertices.push_back(vertex);
		}

		std::shared_ptr<Material> blueMat = Material::createFromFiles("blue_mat",
			{
				shadersDirectory + "ubo.vert",
				shadersDirectory + "blue.frag"
			},
			{}
		);

		std::shared_ptr<Material> greenMat = Material::createFromFiles("green_mat",
			{
				shadersDirectory + "ubo.vert",
				shadersDirectory + "green.frag"
			},
			{}
		);

		std::shared_ptr<Material> redMat = Material::createFromFiles("red_mat",
			{
				shadersDirectory + "ubo.vert",
				shadersDirectory + "red.frag"
			},
			{}
		);

		std::shared_ptr<Material> yellowMat = Material::createFromFiles("yellow_mat",
			{
				shadersDirectory + "ubo.vert",
				shadersDirectory + "yellow.frag"
			},
			{}
		);
		
		std::array<const char* const, 2> names = {
			"additionalColor",
			"mixValue"
		};

		std::array<GLuint, 2> indices;
		GLES_CHECK_ERROR(glGetUniformIndices(blueMat->getProgram()->getID(), 2, names.data(), indices.data()));
		std::array<GLint, 2> offsets;
		GLES_CHECK_ERROR(glGetActiveUniformsiv(blueMat->getProgram()->getID(), 2, indices.data(), GL_UNIFORM_OFFSET, offsets.data()));

		glm::vec4 additionalColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		float mixValue = 0.5f;
		
		uniformBuffer = UniformBuffer::createWithData(GL_DYNAMIC_DRAW, blueMat->getProgram().get(), "mixColor", 0);
		uniformBuffer->setData(offsets[0], sizeof(glm::vec4), glm::value_ptr(additionalColor));
		uniformBuffer->setData(offsets[1], sizeof(float), &mixValue);
		
		// create cubeBlue mesh
		cubeBlue = Mesh::createWithData("cube_blue", vertices, {});
		cubeBlue->setDrawType(Mesh::DrawType::ARRAYS);
		cubeBlue->setMaterial(blueMat);
		cubeBlue->setPosition(glm::vec3(-0.75f, 0.75f, 0.0f));

		// create cubeGreen mesh
		cubeGreen = Mesh::createWithData("cube_green", vertices, {});
		cubeGreen->setDrawType(Mesh::DrawType::ARRAYS);
		cubeGreen->setMaterial(greenMat);
		cubeGreen->setPosition(glm::vec3(0.75f, 0.75f, 0.0f));

		// create cubeRed mesh
		cubeRed = Mesh::createWithData("cube_red", vertices, {});
		cubeRed->setDrawType(Mesh::DrawType::ARRAYS);
		cubeRed->setMaterial(redMat);
		cubeRed->setPosition(glm::vec3(-0.75f, -0.75f, 0.0f));

		// create cubeYellow mesh
		cubeYellow = Mesh::createWithData("cube_yellow", vertices, {});
		cubeYellow->setDrawType(Mesh::DrawType::ARRAYS);
		cubeYellow->setMaterial(yellowMat);
		cubeYellow->setPosition(glm::vec3(0.75f, -0.75f, 0.0f));
	}

	virtual void render(float deltaTime) override
	{
		SDL_GetWindowSize(window, &destWidth, &destHeight);
		glViewport(0, 0, destWidth, destHeight);
		glClearColor(defaultClearColor.r, defaultClearColor.g, defaultClearColor.b, defaultClearColor.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		
		cubeBlue->render();
		cubeGreen->render();
		cubeRed->render();
		cubeYellow->render();
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