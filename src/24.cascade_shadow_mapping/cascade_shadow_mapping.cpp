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
	std::array<std::shared_ptr<Model>, 10> venusShadows;
	std::array<std::shared_ptr<Model>, 10> venuses;

	std::shared_ptr<Model> planeShadow;
	std::shared_ptr<Model> plane;

	std::shared_ptr<Material> shadowMat;
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
		
		cascadeEnd[0] = mMainCamera->getNearPlane();
		cascadeEnd[1] = mMainCamera->getFarPlane() / 5.0f;
		cascadeEnd[2] = mMainCamera->getFarPlane() / 2.0f;
		cascadeEnd[3] = mMainCamera->getFarPlane();

		dirLight.color = glm::vec3(1.0f, 1.0f, 1.0f);
		dirLight.ambientIntensity = 0.3f;
		dirLight.diffuseIntensity = 0.8f;
		dirLight.direction = glm::vec3(1.0f, -1.0f, 0.0f);

		lightMapFBO = Framebuffer::create();
	
		lightMapArray = Texture2DArray::createFromData(lightMapSize, lightMapSize, MAX_SPLITS, 1, 1, GL_DEPTH_COMPONENT32F, GL_DEPTH_COMPONENT, GL_FLOAT, true);
		lightMapArray->setMinFilter(GL_LINEAR);
		lightMapArray->setMagFilter(GL_LINEAR);
		lightMapArray->setCompareMode(GL_COMPARE_REF_TO_TEXTURE);
		lightMapArray->setCompareFunc(GL_LEQUAL);
		lightMapArray->setWrapping(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);

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

			}
		);

		plane = Model::createFromFile("plane", modelsDirectory + "/rocks_plane/rocks_plane.obj", {}, false);
		plane->setMaterial(sceneMat);
		plane->setRotation(glm::vec3(-90.0f, 0.0f, 0.0f));
		plane->setScale(glm::vec3(2.0f, 3.0f, 1.0f));
	
		planeShadow = Model::clone("plane_shadow", plane.get());
		planeShadow->setMaterial(shadowMat);
		planeShadow->setRotation(plane->getRotation());
		planeShadow->setScale(plane->getScaling());

		std::shared_ptr<Model> venusTemplate = Model::createFromFile("venus_template", modelsDirectory + "/venus/venus.fbx", {}, false);
		
		for (std::size_t i = 0; i < venuses.size(); i++)
		{
			venuses[i] = Model::clone("venus_" + std::to_string(i), venusTemplate.get());
			venuses[i]->setMaterial(sceneMat);
			venuses[i]->setPosition(glm::vec3(0.0f, 0.0f, 20.0f - i * 3.0f));
			venuses[i]->setScale(glm::vec3(0.2f, 0.2f, 0.2f));

			venusShadows[i] = Model::clone("venus_shadow_" + std::to_string(i), venusTemplate.get());
			venusShadows[i]->setMaterial(shadowMat);
			venusShadows[i]->setPosition(venuses[i]->getPosition());
			venusShadows[i]->setScale(venuses[i]->getScaling());
		}

		sceneMat->setUniform("biasMatrix", biasMatrix);
		sceneMat->setUniform("dirLight.color", dirLight.color);
		sceneMat->setUniform("dirLight.direction", dirLight.direction);
		sceneMat->setUniform("dirLight.ambientIntensity", dirLight.ambientIntensity);
		sceneMat->setUniform("dirLight.diffuseIntensity", dirLight.diffuseIntensity);
		for (uint32_t i = 0; i < NUM_CASCADES; i++)
		{
			sceneMat->setTexture("depthMap[" + std::to_string(i) + "]", lightMaps[i]);

			glm::vec4 clipSpacePos = mMainCamera->getProjection() * glm::vec4(0.0f, 0.0f, -cascadeEnd[i + 1], 1.0f);
			sceneMat->setUniform("cascadeEndClipSpace[" + std::to_string(i) + "]", clipSpacePos.z);
		}
	}

	virtual void render(float deltaTime) override
	{
		glViewport(0, 0, lightMapSize, lightMapSize);
		//glCullFace(GL_FRONT);
		for (uint32_t i = 0; i < NUM_CASCADES; i++)
		{
			lightMapFBOs[i]->attachDepthRenderTarget(lightMaps[i].get(), 0, 0);
			lightMapFBOs[i]->bind();
			glClear(GL_DEPTH_BUFFER_BIT);

			glm::mat4 lightView = glm::lookAtLH<float>(dirLight.direction, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 lightProj = glm::ortho<float>(lightOrthoProjInfo[i].l / 10.0f, lightOrthoProjInfo[i].r / 10.0f, lightOrthoProjInfo[i].b / 10.0f, lightOrthoProjInfo[i].t / 10.0f, lightOrthoProjInfo[i].n / 10.0f, lightOrthoProjInfo[i].f / 10.0f);

			glm::mat4 lightMatrix = lightProj * lightView;

			shadowMat->setUniform("lightMatrix", lightMatrix);
			sceneMat->setUniform("lightMatrix[" + std::to_string(i) + "]", lightMatrix);

			planeShadow->render();
			for (std::size_t j = 0; j < venusShadows.size(); j++)
			{
				venusShadows[j]->render();
			}
			lightMapFBOs[i]->unbind();
		}
		//glCullFace(GL_BACK);
	
		glViewport(0, 0, mWindowWidth, mWindowHeight);
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
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

			glm::vec3 lightDirection = frustumCenter - glm::normalize(glm::vec3(-0.1f, -0.5f, 0.0f)) * -minExtents.z;
			lightViewMatrix = glm::mat4(1.0f);
			lightViewMatrix = glm::lookAt(lightDirection, frustumCenter, glm::vec3(0.0f, 1.0f, 0.0f));

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