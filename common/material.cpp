#include "material.h"

namespace es
{
	std::unordered_map<std::string, std::shared_ptr<Material>> Material::mMaterialCache;

	Material::Material(const std::string& name, const std::vector<std::string>& shaderFiles, const std::unordered_map<std::string, std::string>& textureFiles)
		:mName(name)
	{
		mProgram = Program::createFromFiles(shaderFiles);

		for (auto iter = textureFiles.begin(); iter != textureFiles.end(); iter++)
		{
			std::shared_ptr<Texture2D> tex2d = Texture2D::createFromFile(iter->second, 0, false);
			mTextures[iter->first] = tex2d;
		}
	}

	Material::~Material()
	{
		mProgram.reset(nullptr);

		for (auto iter = mTextures.begin(); iter != mTextures.end(); iter++)
		{
			iter->second.reset();
			iter->second = nullptr;
		}
		mTextures.swap(std::unordered_map<std::string, std::shared_ptr<Texture2D>>());
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

	void Material::apply()
	{

	}
}