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

	std::string Utility::pathWithoutFile(const std::string& path)
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
		std::string ext = filePath.substr(found, filePath.size());
		return ext;
	}
}
