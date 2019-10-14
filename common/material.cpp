#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMaterialCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::vector<std::string>& textureFiles)
		:mName(name)
	{
		mProgram = Program::createFromFiles(shaderFiles);

		for (std::size_t i = 0; i < textureFiles.size(); i++)
		{
			std::shared_ptr<Texture2D> tex2d = Texture2D::createFromFile(textureFiles[i], 0, false);
			mTextures.push_back(tex2d);
		}
	}

	Material::~Material()
	{
		mProgram.reset(nullptr);

		for (std::size_t i = 0; i < mTextures.size(); i++)
		{
			mTextures[i].reset();
			mTextures[i] = nullptr;
		}
	}

	template<typename... T>
	std::shared_ptr<Material> Material::createFromFiles(const std::string& name, T &&... args)
	{
		if (mMaterialCache.find(name) == mMaterialCache.end())
		{
			std::shared_ptr<Material> mat = std::make_shared<Material>(name, std::forward<T>(args)...);
			mMaterialCache[name] = mat;
			return mat;
		}
		else
		{
			return mMaterialCache[name];
		}
	}
}