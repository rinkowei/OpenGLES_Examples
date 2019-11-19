#include <examplebase.h>
#include <model.h>
#include <material.h>
using namespace es;

#define MAX_SPLITS 4
#define NUM_CASCADES 3
#define NUM_FRUSTUM_CORNERS 8
class Example final : public ExampleBase
{
public:
	std::array<std::shared_ptr<Model>, 10> venuses;

	std::shared_ptr<Model> plane;

	std::shared_ptr<Model> debugQuad;

	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> sceneMat;

	std::unique_ptr<Framebuffer> lightMapFBO;

	const uint32_t lightMapSize = 2048;
	std::shared_ptr<Texture2DArray> lightMapArray;

	glm::mat4 lightViewMatrix;
	glm::mat4 lightOrthoMatrix;

	struct DirectionalLight
	{
		glm::vec3 color;
		glm::vec3 direction;
		float ambientIntensity;
		float diffuseIntensity;
	};

	DirectionalLight dirLight;

	struct Cascade
	{
		float splitDepth;
		glm::mat4 viewProjMatrix;
	};

	std::array<Cascade, MAX_SPLITS> cascades;

	std::array<float, MAX_SPLITS> cascadeSplitArray;
	std::array<glm::mat4, MAX_SPLITS> cascadeMatrices;

	glm::mat4 biasMatrix = glm::mat4(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.5f, 1.0f
	);

	Example()
	{
		title = "cascaded shadow mapping";
		settings.vsync = true;
		settings.validation = true;
		defaultClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

		modelsDirectory = getResourcesPath(ResourceType::Model);
		shadersDirectory = getResourcesPath(ResourceType::Shader) + "/24.cascaded_shadow_mapping/";
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
		mMainCamera->setPosition(glm::vec3(0.0f, 15.0f, 30.0f));
		mMainCamera->setRotation(glm::vec3(30.0f, 0.0f, 0.0f));

		// enable depth test
		glEnable(GL_DEPTH_TEST);

		// enable cull face
		glEnable(GL_CULL_FACE);

		dirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
		dirLight.ambientIntensity = 0.3f;
		dirLight.diffuseIntensity = 0.8f;
		dirLight.direction = glm::vec3(1.0f, -1.0f, 0.0f);

		lightMapFBO = Framebuffer::create();
	
		lightMapArray = Texture2DArray::createFromData(lightMapSize, lightMapSize, MAX_SPLITS, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false);
		lightMapArray->setMinFilter(GL_NEAREST);
		lightMapArray->setMagFilter(GL_NEAREST);
		lightMapArray->setWrapping(GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT, GL_CLAMP_TO_BORDER_EXT);
		lightMapArray->setBorderColor(1.0f, 1.0f, 1.0f, 1.0f);

		lightPassMat = Material::createFromFiles("lightPass_mat",
			{
				shadersDirectory + "light_pass.vert",
				shadersDirectory + "light_pass.frag"
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
				
			}
		);

		plane = Model::createFromFile("plane", modelsDirectory + "/rocks_plane/rocks_plane.obj", {}, false);
		plane->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		plane->setScale(glm::vec3(2.0f, 3.0f, 1.0f));

		std::shared_ptr<Model> venusTemplate = Model::createFromFile("venus_template", modelsDirectory + "/venus/venus.fbx", {}, false);
		
		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i] = Model::clone("venus_" + std::to_string(i), venusTemplate.get());
			venuses[i]->setPosition(glm::vec3(0.0f, 0.0f, 20.0f - i * 3.0f));
			venuses[i]->setScale(glm::vec3(0.2f, 0.2f, 0.2f));
		}

		sceneMat->setUniform("dirLight.color", dirLight.color);
		sceneMat->setUniform("dirLight.direction", dirLight.direction);
		sceneMat->setUniform("dirLight.ambientIntensity", dirLight.ambientIntensity);
		sceneMat->setUniform("dirLight.diffuseIntensity", dirLight.diffuseIntensity);
		sceneMat->setUniform("biasMatrix", biasMatrix);

		sceneMat->setTexture("cascadedDepthMap", lightMapArray);

		debugQuad = Model::createFromFile("debug_quad", modelsDirectory + "/quadrangle/quadrangle.obj",
			{
				shadersDirectory + "debug_quad.vert",
				shadersDirectory + "debug_quad.frag"
			}
		);
		debugQuad->setTexture("cascadedDepthMap", lightMapArray);
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glCullFace(GL_BACK);

		for (unsigned int i = 0; i < MAX_SPLITS; ++i)
		{
			sceneMat->setUniform("cascadedSplits[" + std::to_string(i) + "]", cascades[i].splitDepth);
			sceneMat->setUniform("lightSpaceMatrices[" + std::to_string(i) + "]", cascades[i].viewProjMatrix);
		}
		sceneMat->setUniform("viewPos", mMainCamera->getPosition());

		plane->setMaterial(sceneMat);
		plane->render();
		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i]->setMaterial(sceneMat);
			venuses[i]->render();
		}

		//debugQuad->render();
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void lightMapPass()
	{
		Frustum cameraFrustum = mMainCamera->getFrustum();

		float lambda = 0.95f;

		float nearClip = cameraFrustum.mNear;
		float farClip = cameraFrustum.mFar;
		float clipRange = farClip - nearClip;

		float minZ = nearClip;
		float maxZ = nearClip + clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		for (uint32_t i = 0; i < MAX_SPLITS; i++)
		{
			float p = (i + 1) / static_cast<float>(MAX_SPLITS);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = lambda * (log - uniform) + uniform;
			cascadeSplitArray[i] = (d - nearClip) / clipRange;
		}

		float lastSplitDist = 0.0f;
		for (uint32_t i = 0; i < MAX_SPLITS; i++)
		{
			float splitDist = cascadeSplitArray[i];

			std::array<glm::vec3, 8> frustumCorners =
			{
				glm::vec3(-1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f,  1.0f, -1.0f),
				glm::vec3(1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f, -1.0f, -1.0f),
				glm::vec3(-1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f,  1.0f,  1.0f),
				glm::vec3(1.0f, -1.0f,  1.0f),
				glm::vec3(-1.0f, -1.0f,  1.0f)
			};

			glm::mat4 invCam = glm::inverse(mMainCamera->getProjection() * mMainCamera->getView());
			for (uint32_t i = 0; i < 8; i++) {
				glm::vec4 invCorner = invCam * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = invCorner / invCorner.w;
			}

			for (uint32_t i = 0; i < 4; i++) {
				glm::vec3 dist = frustumCorners[i + 4] - frustumCorners[i];
				frustumCorners[i + 4] = frustumCorners[i] + (dist * splitDist);
				frustumCorners[i] = frustumCorners[i] + (dist * lastSplitDist);
			}

			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (uint32_t i = 0; i < 8; i++) {
				frustumCenter += frustumCorners[i];
			}
			frustumCenter /= 8.0f;

			float radius = 0.0f;
			for (uint32_t i = 0; i < 8; i++) {
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDir = dirLight.direction;
			glm::mat4 lightViewMatrix = glm::lookAt(frustumCenter - lightDir * -minExtents.z, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));
			minExtents *= 2.0f;
			maxExtents *= 2.0f;
			glm::mat4 lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

			cascades[i].splitDepth = (nearClip + splitDist * clipRange) * 1.0f;
			cascades[i].viewProjMatrix = lightOrthoMatrix * lightViewMatrix;

			glViewport(0, 0, lightMapSize, lightMapSize);
			lightMapFBO->addAttachmentTextureLayer(GL_DEPTH_ATTACHMENT, lightMapArray->getID(), 0, i);
			lightMapFBO->bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			lightPassMat->setUniform("lightSpaceMatrix", cascades[i].viewProjMatrix);

			plane->setMaterial(lightPassMat);
			plane->render();
			for (std::size_t i = 0; i < venuses.size(); i++)
			{
				venuses[i]->setMaterial(lightPassMat);
				venuses[i]->render();
			}

			lastSplitDist = cascadeSplitArray[i];
		}
		lightMapFBO->unbind();
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