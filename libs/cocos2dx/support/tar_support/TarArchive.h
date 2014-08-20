#pragma once

#include <string>
#include <vector>
#include <tr1/unordered_map>

enum class TarEntryType
{
    RegularFile,
    Directory,
    Unknown,
};

class TarEntry
{
public:
    std::string getName() const { return name; }
    TarEntryType getType() const { return type; }
    size_t getFileSize() const { return fileSize; }
    size_t getContentBlockId() const { return contentBlockId; }

    static TarEntry* createFromHeader(size_t blockId, unsigned char* buffer);
private:
    TarEntry() {};

private:
    std::string name;
    TarEntryType type;
    size_t fileSize;
    size_t contentBlockId;
};

class TarArchive
{
public:
    virtual ~TarArchive();
    void* getFileData(const std::string& path, size_t* size);
    bool fileExists(const std::string& path) const;
    std::vector<std::string> getFileList() const;
    std::string getDir() const { return dirPath; }

    static TarArchive* create(const std::string& path, std::string directoryRoot="");
private:
    TarArchive() {}

private:
    std::tr1::unordered_map<std::string, TarEntry*> entryMap;
    size_t tarSize;
    std::string tarPath;
    std::string dirPath;
};
