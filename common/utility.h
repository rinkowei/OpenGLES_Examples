#ifndef UTILITY_H_
#define UTILITY_H_

#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <cassert>
#include <algorithm>
#include <stdio.h>

namespace es
{
	class Utility
	{
	public:
		static std::string pathForResource(const std::string& resource);

		static std::string executablePath();

		static bool readFile(const std::string& path, std::string& out);

		//static bool preprocessShader(const std::string& path, const std::string& src, std::string& out);

		static std::string pathWithoutFile(std::string path);

		static std::string fileExtension(const std::string& filePath);

		static std::string execPath;
	};
}

#endif