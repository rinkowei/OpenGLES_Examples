#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

#define NUM_CASCADES 3
#define NUM_FRUSTUM_CORNERS 8
class Example final : public ExampleBase
{
public:
	std::array<std::shared_ptr<Model>, 5> venusShadows;
	std::array<std::shared_ptr<Model>, 5> venuses;

	std::shared_ptr<Model> planeShadow;
	std::shared_ptr<Model> plane;

	std::shared_ptr<Material> shadowMat;
	std::shared_ptr<Material> sceneMat;

	std::unique_ptr<Framebuffer> lightMapFBO;

	const uint32_t lightMapWidth = 1024;
	const uint32_t lightMapHeight = 1024;
	std::array<std::shared_ptr<Texture2D>, 3> lightMaps;

	struct OrthoProjInfo
	{
		float r;
		float l;
		float b;
		float t;
		float n;
		float f;
	};

	std::array<OrthoProjInfo, NUM_CASCADES> lightOrthoProjInfo;

	struct DirectionalLight
	{
		glm::vec3 color;
		glm::vec3 direction;
		float ambientIntensity;
		float diffuseIntensity;
	};

	DirectionalLight dirLight;

	glm::vec3 lightPos = glm::vec3(-2.0f, 5.0f, 0.0f);

	std::array<float, NUM_CASCADES + 1> cascadeEnd;

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "cascade shadow mapping";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/24.cascade_shadow_mapping/";
		texturesDirectory = getResourcesPath(ResourceType::Texture);
	}
	~Example()
	{

	}
public:
	virtual void prepare() override
	{
		ExampleBase::prepare();
		
		// setup camera
		mMainCamera->setPosition(glm::vec3(0.0f, 10.0f, 25.0f));
		mMainCamera->setRotation(glm::vec3(30.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);
		
		cascadeEnd[0] = mMainCamera->getNearPlane();
		cascadeEnd[1] = mMainCamera->getFarPlane() / 20.0f;
		cascadeEnd[2] = mMainCamera->getFarPlane() / 10.0f;
		cascadeEnd[3] = mMainCamera->getFarPlane();

		dirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
		dirLight.ambientIntensity = 0.5f;
		dirLight.diffuseIntensity = 0.9f;
		dirLight.direction = glm::vec3(1.0f, -1.0f, 0.0f);

		lightMapFBO = Framebuffer::create();
	
		for (std::size_t i = 0; i < lightMaps.size(); i++)
		{
			lightMaps[i] = Texture2D::createFromData(lightMapWidth, lightMapHeight, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false);
			lightMaps[i]->setMinFilter(GL_LINEAR);
			lightMaps[i]->setMagFilter(GL_LINEAR);
			lightMaps[i]->setCompareMode(GL_NONE);
			lightMaps[i]->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		}

		lightMapFBO->attachDepthRenderTarget(lightMaps[0].get(), 0, 0);

		shadowMat = Material::createFromData("shadow_mat",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			},
			{

			}
		);

		sceneMat = Material::createFromData("scene_mat",
			{
				shadersDirectory + "scene.vert",
				shadersDirectory + "scene.frag"
			},
			{
				{ "depthMap0", lightMaps[0] },
				{ "depthMap1", lightMaps[1] },
				{ "depthMap2", lightMaps[2] }
			}
		);

		plane = Model::createFromFile("plane", modelsDirectory + "/rocks_plane/rocks_plane.obj", {}, false);
		plane->setMaterial(sceneMat);
		plane->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		plane->setScale(glm::vec3(1.5f, 2.0f, 1.0f));

		planeShadow = Model::clone("plane_shadow", plane.get());
		planeShadow->setMaterial(shadowMat);
		planeShadow->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		planeShadow->setScale(glm::vec3(1.5f, 2.0f, 1.0f));

		std::shared_ptr<Model> venusTemplate = Model::createFromFile("venus_template", modelsDirectory + "/venus/venus.fbx", {}, false);
		
		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i] = Model::clone("venus_" + std::to_string(i), venusTemplate.get());
			venuses[i]->setMaterial(sceneMat);
			venuses[i]->setPosition(glm::vec3(0.0f, 0.0f, 15.0f - i * 7.0f));
			venuses[i]->setScale(glm::vec3(0.2f, 0.2f, 0.2f));

			venusShadows[i] = Model::clone("venus_shadow_" + std::to_string(i), venusTemplate.get());
			venusShadows[i]->setMaterial(shadowMat);
			venusShadows[i]->setPosition(glm::vec3(0.0f, 0.0f, 15.0f - i * 7.0f));
			venusShadows[i]->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}
		/*
		playgroundShadow = Model::createFromFile("playground_shadow", modelsDirectory + "/playground/Playground.obj",
			{
				shadersDirectory + "depth_map.vert",
				shadersDirectory + "depth_map.frag"
			}
		);

		playground = Model::clone("playground", playgroundShadow.get());
		std::shared_ptr<Material> mat = Material::createFromData("mat",
			{
				shadersDirectory + "playground.vert",
				shadersDirectory + "playground.frag"
			},
			{
				{ "depthMap", lightMap },
			}
			);
		playground->setMaterial(mat);
		playground->setUniform("biasMatrix", biasMatrix);
		*/

		sceneMat->setUniform("dirLight.color", dirLight.color);
		sceneMat->setUniform("dirLight.direction", dirLight.direction);
		sceneMat->setUniform("dirLight.ambientIntensity", dirLight.ambientIntensity);
		sceneMat->setUniform("dirLight.color", dirLight.diffuseIntensity);
		for (uint32_t i = 0; i < NUM_CASCADES; i++)
		{
			//sceneMat->setTexture("depthMap" + std::to_string(i), lightMaps[i]);

			glm::vec4 clipSpacePos = mMainCamera->getProjection() * glm::vec4(0.0f, 0.0f, cascadeEnd[i + 1], 1.0f);
			sceneMat->setUniform("cascadeEndClipSpace[" + std::to_string(i) + "]", clipSpacePos.z);
		}
	}

	virtual void render(float deltaTime) override
	{
		calculateOrthoProjs();

		glViewport(0, 0, lightMapWidth, lightMapHeight);
		for (uint32_t i = 0; i < NUM_CASCADES; i++)
		{
			lightMapFBO->attachDepthRenderTarget(lightMaps[i].get(), 0, 0);
			lightMapFBO->bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 lightView = glm::lookAt(dirLight.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProj = glm::ortho<float>(lightOrthoProjInfo[i].l, lightOrthoProjInfo[i].r, lightOrthoProjInfo[i].b, lightOrthoProjInfo[i].t, lightOrthoProjInfo[i].n, lightOrthoProjInfo[i].f);
			glm::mat4 lightMatrix = lightProj * lightView;

			shadowMat->setUniform("lightMatrix", lightMatrix);

			planeShadow->render();
			for (std::size_t j = 0; j < venusShadows.size(); j++)
			{
				venusShadows[i]->render();
			}
		}

		glViewport(0, 0, mWindowWidth, mWindowHeight);
		lightMapFBO->unbind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		/*
		// change light position over time
		lightPos = glm::vec3(sin(timePassed) * 2.0f, 5.0f + cos(timePassed) * 1.0f, cos(timePassed) * 1.0f);
		glm::mat4 lightProj = glm::ortho<float>(-10.0f, 10.0f, -10.0f, 10.0f, 0.1f, 10.0f);
		glm::mat4 lightView = glm::lookAt(lightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 lightSpaceMatrix = lightProj * lightView;

		glViewport(0, 0, lightMapWidth, lightMapHeight);
		lightMapFBO->bind();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		playgroundShadow->setUniform("lightSpaceMatrix", lightSpaceMatrix);
		
		glCullFace(GL_FRONT);
		playgroundShadow->render();
		glCullFace(GL_BACK);

		lightMapFBO->unbind();
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
		*/
		sceneMat->setUniform("viewPos", mMainCamera->getPosition());
		plane->render();

		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i]->render();
		}
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void calculateOrthoProjs()
	{
		glm::mat4 camView = mMainCamera->getView();
		glm::mat4 camViewInv = glm::inverse(camView);

		glm::mat4 lightView = glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), dirLight.direction, glm::vec3(0.0f, 1.0f, 0.0f));

		float aspectRatio = mMainCamera->getAspectRatio();
		float tanHalfHFov = glm::tan(glm::radians(mMainCamera->getFov() / 2.0f));
		float tanHalfVFov = glm::tan(glm::radians(mMainCamera->getFov() * aspectRatio / 2.0f));

		for (uint32_t i = 0; i < NUM_CASCADES; i++)
		{
			float xn = cascadeEnd[i] * tanHalfHFov;
			float xf = cascadeEnd[i + 1] * tanHalfHFov;
			float yn = cascadeEnd[i] * tanHalfVFov;
			float yf = cascadeEnd[i + 1] * tanHalfVFov;

			std::array<glm::vec4, NUM_FRUSTUM_CORNERS> frustumCorners = {
				glm::vec4(xn, yn, cascadeEnd[i], 1.0f),
				glm::vec4(-xn,  yn, cascadeEnd[i], 1.0),
				glm::vec4(xn,  -yn, cascadeEnd[i], 1.0),
				glm::vec4(-xn, -yn, cascadeEnd[i], 1.0),

				glm::vec4(xf,   yf, cascadeEnd[i + 1], 1.0),
				glm::vec4(-xf,  yf, cascadeEnd[i + 1], 1.0),
				glm::vec4(xf,  -yf, cascadeEnd[i + 1], 1.0),
				glm::vec4(-xf, -yf, cascadeEnd[i + 1], 1.0)
			};

			std::array<glm::vec4, NUM_FRUSTUM_CORNERS> frustumCornersL;

			float minX = std::numeric_limits<float>::max();
			float maxX = std::numeric_limits<float>::min();
			float minY = std::numeric_limits<float>::max();
			float maxY = std::numeric_limits<float>::min();
			float minZ = std::numeric_limits<float>::max();
			float maxZ = std::numeric_limits<float>::min();

			for (uint32_t j = 0; j < NUM_FRUSTUM_CORNERS; j++)
			{
				glm::vec4 vW = camViewInv * frustumCorners[j];
				frustumCornersL[j] = lightView * vW;

				minX = glm::min(minX, frustumCornersL[j].x);
				maxX = glm::max(maxX, frustumCornersL[j].x);
				minY = glm::min(minY, frustumCornersL[j].y);
				maxY = glm::max(maxY, frustumCornersL[j].y);
				minZ = glm::min(minZ, frustumCornersL[j].z);
				maxZ = glm::max(maxZ, frustumCornersL[j].z);
			}

			lightOrthoProjInfo[i].r = maxX;
			lightOrthoProjInfo[i].l = minX;
			lightOrthoProjInfo[i].b = minY;
			lightOrthoProjInfo[i].t = maxY;
			lightOrthoProjInfo[i].f = maxZ;
			lightOrthoProjInfo[i].n = minZ;
		}
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