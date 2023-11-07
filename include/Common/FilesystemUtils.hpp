#ifndef FS_UTILS_HPP
#define FS_UTILS_HPP

#include <algorithm>
#include <string>
#include <vector>

namespace FilesystemUtils
{
#ifdef _WIN32
#include <windows.h>
    static std::vector<std::string> files_in_directory(std::string path)
    {
        std::vector<std::string> files;

        // check directory exists
        char fullpath[MAX_PATH];
        GetFullPathName(path.c_str(), MAX_PATH, fullpath, 0);
        std::string fp(fullpath);
        if (GetFileAttributes(fp.c_str()) != FILE_ATTRIBUTE_DIRECTORY)
            return files;

        // get file names
        WIN32_FIND_DATA findfiledata;
        HANDLE hFind = FindFirstFile((LPCSTR)(fp + "\\*").c_str(), &findfiledata);
        if (hFind != INVALID_HANDLE_VALUE)
        {
            do
            {
                files.push_back(findfiledata.cFileName);
            } while (FindNextFile(hFind, &findfiledata));
            FindClose(hFind);
        }

        // delete current and parent directories
        files.erase(std::find(files.begin(), files.end(), "."));
        files.erase(std::find(files.begin(), files.end(), ".."));

        // sort in alphabetical order
        std::sort(files.begin(), files.end());

        return files;
    }
#else
#include <dirent.h>
    static std::vector<std::string> listFilesInDirectory(std::string directory)
    {
        std::vector<std::string> files;

        // open directory
        DIR *dir;
        dir = opendir(directory.c_str());
        if (dir == NULL)
            return files;

        // get file names
        struct dirent *ent;
        while ((ent = readdir(dir)) != NULL)
            files.push_back(ent->d_name);
        closedir(dir);

        // delete current and parent directories
        files.erase(std::find(files.begin(), files.end(), "."));
        files.erase(std::find(files.begin(), files.end(), ".."));

        // sort in alphabetical order
        std::sort(files.begin(), files.end());

        return files;
    }
#endif // _WIN32

    static std::string getFileBasename(std::string fullname)
    {
        auto dot_idx = fullname.find_first_of(".");
        auto slash_idx = fullname.find_last_of("/");

        if (slash_idx == std::string::npos)
        {
            slash_idx = 0;
        }
        else
        {
            slash_idx++;
        }

        return fullname.substr(slash_idx, dot_idx);
    }
}

#endif