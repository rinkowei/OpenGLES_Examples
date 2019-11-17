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

	std::shared_ptr<Material> lightPassMat;
	std::shared_ptr<Material> sceneMat;

	std::array<std::unique_ptr<Framebuffer>, MAX_SPLITS> lightMapFBOs;

	const uint32_t lightMapSize = 2048;
	std::array<std::shared_ptr<Texture2D>, MAX_SPLITS> lightMaps;

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

		for (std::size_t i = 0; i < lightMapFBOs.size(); i++)
		{
			lightMapFBOs[i] = Framebuffer::create();
		}
	
		for (std::size_t i = 0; i < lightMaps.size(); i++)
		{
			lightMaps[i] = Texture2D::createFromData(lightMapSize, lightMapSize, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, false);
			lightMaps[i]->setMinFilter(GL_LINEAR);
			lightMaps[i]->setMagFilter(GL_LINEAR);
			lightMaps[i]->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
			lightMaps[i]->setCompareFunc(GL_LEQUAL);
			lightMaps[i]->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		}

		lightPassMat = Material::createFromFiles("lightPass_mat",
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

		//sceneMat->setUniform("biasMatrix", biasMatrix);
		sceneMat->setUniform("dirLight.color", dirLight.color);
		sceneMat->setUniform("dirLight.direction", dirLight.direction);
		sceneMat->setUniform("dirLight.ambientIntensity", dirLight.ambientIntensity);
		sceneMat->setUniform("dirLight.diffuseIntensity", dirLight.diffuseIntensity);

		sceneMat->setUniform("windowSize", glm::vec2(mWindowWidth, mWindowHeight));
		sceneMat->setUniform("cascadedSplits", glm::vec4(cascadeSplitArray[0], cascadeSplitArray[1], cascadeSplitArray[2], cascadeSplitArray[3]));
		sceneMat->setUniform("numOfCascades", MAX_SPLITS);

		for (unsigned int i = 0; i < MAX_SPLITS; i++)
		{
			sceneMat->setTexture("cascadedDepthMap[" + std::to_string(i) + "]", lightMaps[i]);
		}
	}

	virtual void render(float deltaTime) override
	{
		lightMapPass();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glCullFace(GL_BACK);

		for (unsigned int i = 0; i < MAX_SPLITS; ++i)
		{
			sceneMat->setUniform("lightMatrices[" + std::to_string(i) + "]", cascadeMatrices[i]);
		}
		sceneMat->setUniform("viewPos", mMainCamera->getPosition());

		plane->setMaterial(sceneMat);
		plane->render();

		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i]->setMaterial(sceneMat);
			venuses[i]->render();
		}
	}

	virtual void windowResized() override
	{
		ExampleBase::windowResized();
	}

	void lightMapPass()
	{
		Frustum cameraFrustum = mMainCamera->getFrustum();

		std::array<float, MAX_SPLITS> cascadeSplits;

		float lambda = 1.0f;

		float minDistance = 0.0f;
		float maxDistance = 1.0f;

		float nearClip = cameraFrustum.mNear;
		float farClip = cameraFrustum.mFar;
		float clipRange = farClip - nearClip;

		float minZ = nearClip + minDistance * clipRange;
		float maxZ = nearClip + maxDistance * clipRange;

		float range = maxZ - minZ;
		float ratio = maxZ / minZ;

		for (unsigned int i = 0; i < MAX_SPLITS; ++i)
		{
			float p = (i + 1) / static_cast<float>(MAX_SPLITS);
			float log = minZ * std::pow(ratio, p);
			float uniform = minZ + range * p;
			float d = lambda * (log - uniform) + uniform;
			cascadeSplits[i] = (d - nearClip) / clipRange;
		}

		glViewport(0, 0, lightMapSize, lightMapSize);
		glCullFace(GL_FRONT);
		for (unsigned int cascadeIterator = 0; cascadeIterator < MAX_SPLITS; ++cascadeIterator)
		{
			float prevSplitDistance = cascadeIterator == 0 ? minDistance : cascadeSplits[cascadeIterator - 1];
			float splitDistance = cascadeSplits[cascadeIterator];

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

			glm::mat4 invViewProj = glm::inverse(mMainCamera->getProjection() * mMainCamera->getView());
			for (std::size_t i = 0; i < frustumCorners.size(); ++i)
			{
				glm::vec4 inversePoint = invViewProj * glm::vec4(frustumCorners[i], 1.0f);
				frustumCorners[i] = glm::vec3(inversePoint / inversePoint.w);
			}

			for (unsigned int i = 0; i < 4; ++i)
			{
				glm::vec3 cornerRay = frustumCorners[i + 4] - frustumCorners[i];
				glm::vec3 nearCornerRay = cornerRay * prevSplitDistance;
				glm::vec3 farCornerRay = cornerRay * splitDistance;
				frustumCorners[i + 4] = frustumCorners[i] + farCornerRay;
				frustumCorners[i] = frustumCorners[i] + nearCornerRay;
			}

			glm::vec3 frustumCenter = glm::vec3(0.0f);
			for (std::size_t i = 0; i < frustumCorners.size(); ++i)
			{
				frustumCenter += frustumCorners[i];
			}
			frustumCenter /= 8.0f;

			float farInfinity = -std::numeric_limits<float>::infinity();
			float nearInfinity = std::numeric_limits<float>::infinity();

			float radius = 0.0f;
			for (std::size_t i = 0; i < frustumCorners.size(); ++i)
			{
				float distance = glm::length(frustumCorners[i] - frustumCenter);
				radius = glm::max(radius, distance);
			}
			radius = std::ceil(radius * 16.0f) / 16.0f;

			glm::vec3 maxExtents = glm::vec3(radius, radius, radius);
			glm::vec3 minExtents = -maxExtents;

			glm::vec3 lightDirection = frustumCenter - glm::normalize(dirLight.direction) * -minExtents.z;
			lightViewMatrix = glm::mat4(1.0f);
			lightViewMatrix = glm::lookAt<float>(lightDirection, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

			glm::vec3 cascadeExtents = maxExtents - minExtents;

			lightOrthoMatrix = glm::ortho(minExtents.x, maxExtents.x, minExtents.y, maxExtents.y, 0.0f, cascadeExtents.z);

			glm::mat4 lightMatrix = lightOrthoMatrix * lightViewMatrix;
			glm::vec4 shadowOrigin = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			shadowOrigin = lightMatrix * shadowOrigin;
			shadowOrigin = shadowOrigin * static_cast<float>(lightMapSize) / 2.0f;

			glm::vec4 roundedOrigin = glm::round(shadowOrigin);
			glm::vec4 roundOffset = roundedOrigin - shadowOrigin;
			roundOffset = roundOffset * 2.0f / static_cast<float>(lightMapSize);
			roundOffset.z = 0.0f;
			roundOffset.w = 0.0f;

			glm::mat4 shadowProj = lightOrthoMatrix;
			shadowProj[3] += roundOffset;
			lightOrthoMatrix = shadowProj;

			const float clipDist = cameraFrustum.mFar - cameraFrustum.mNear;
			cascadeSplitArray[cascadeIterator] = (cameraFrustum.mNear + splitDistance * clipDist) * -1.0f;
			cascadeMatrices[cascadeIterator] = lightOrthoMatrix * lightViewMatrix;

			lightMapFBOs[cascadeIterator]->attachDepthRenderTarget(lightMaps[cascadeIterator].get(), 0, 0);
			//lightMapFBO->addAttachmentLayer(GL_DEPTH_ATTACHMENT, lightMapArray->getID(), 0, cascadeIterator);
			lightMapFBOs[cascadeIterator]->bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			plane->setMaterial(lightPassMat);
			plane->setUniform("lightMatrix", lightOrthoMatrix * lightViewMatrix);
			plane->render();

			for (std::size_t i = 0; i < venuses.size(); i++)
			{
				venuses[i]->setMaterial(lightPassMat);
				venuses[i]->setUniform("lightMatrix", lightOrthoMatrix * lightViewMatrix);
				venuses[i]->render();
			}

			lightMapFBOs[cascadeIterator]->unbind();
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