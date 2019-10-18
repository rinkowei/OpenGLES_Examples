#include "utility.h"
#include "ogles.h"

#ifdef WIN32
#include <Windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#define ChangeWorkingDir _chdir
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#define ChangeWorkingDir chdir
#endif

#ifdef __APPLE__
#include <mach-o/dyld.h>
#endif

namespace es
{
	std::string Utility::execPath = "";

	std::string Utility::pathForResource(const std::string& resource)
	{
		return "";
	}

	std::string Utility::executablePath()
	{
#ifdef WIN32
		if (execPath == "")
		{
			char buffer[1024];
			GetModuleFileName(nullptr, &buffer[0], 1024);
			execPath = buffer;
			execPath = pathWithoutFile(execPath);
		}
#elif __APPLE__

#else

#endif

		return execPath;
	}

	bool Utility::readFile(const std::string& path, std::string& out)
	{
		struct stat info;
		if (stat(path.c_str(), &info) != 0)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to locate file in %s", path.c_str());
			return false;
		}

		std::ifstream ifs;
		ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		std::stringstream stream;
		try {
			ifs.open(path, std::ios::in | std::ios::binary);

			stream << ifs.rdbuf();
			out = stream.str();
			ifs.close();
		}
		catch (std::ifstream::failure e)
		{
			SDL_LogError(SDL_LOG_CATEGORY_ERROR, "failed to read file in %s", path.c_str());
			return false;
		}

		return true;
	}

	std::string Utility::pathWithoutFile(std::string path)
	{
#ifdef WIN32
		std::replace(path.begin(), path.end(), '\\', '/');
#endif
		std::size_t found = path.find_last_of("/\\");
		return path.substr(0, found);
	}

	std::string Utility::fileExtension(const std::string& filePath)
	{
		std::size_t found = filePath.find_last_of(".");
		std::string ext = filePath.substr(found + 1, filePath.size());
		return ext;
	}

	std::string Utility::createNewGUID()
	{
		char buf[64] = { 0 };
		GUID guid;
	
		if (CoCreateGuid(&guid))
		{
			return std::move(std::string(""));
		}

		sprintf(buf,
			"%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X",
			guid.Data1, guid.Data2, guid.Data3,
			guid.Data4[0], guid.Data4[1], guid.Data4[2],
			guid.Data4[3], guid.Data4[4], guid.Data4[5],
			guid.Data4[6], guid.Data4[7]);

		return std::move(std::string(buf));
	}
}
