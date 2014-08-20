#include "TarArchive.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

// const definition
#define TAR_BLOCK_SIZE                  512
#define TAR_TYPE_POSITION               156
#define TAR_NAME_POSITION               0
#define TAR_NAME_SIZE                   100
#define TAR_SIZE_POSITION               124
#define TAR_SIZE_SIZE                   12

static char getFileType(unsigned char* buffer)
{
    char type;
    memcpy(&type, buffer + TAR_TYPE_POSITION, 1);
    return type;
}

static std::string getFileName(unsigned char* buffer)
{
    char nameBytes[TAR_NAME_SIZE + 1]; // TAR_NAME_SIZE+1 for nul char at end
    
    memset(&nameBytes, '\0', TAR_NAME_SIZE + 1); // Fill byte array with nul char
    memcpy(&nameBytes, buffer + TAR_NAME_POSITION, TAR_NAME_SIZE);
    return std::string(nameBytes);
}

static size_t getFileSizeA(unsigned char* buffer)
{
    char sizeBytes[TAR_SIZE_SIZE + 1]; // TAR_SIZE_SIZE+1 for nul char at end
    
    memset(&sizeBytes, '\0', TAR_SIZE_SIZE + 1); // Fill byte array with nul char
    memcpy(&sizeBytes, buffer + TAR_SIZE_POSITION, TAR_SIZE_SIZE);
    return strtol(sizeBytes, NULL, 8); // Size is an octal number, convert to decimal
}


// TarEntry
TarEntry* TarEntry::createFromHeader(size_t bId, unsigned char* buffer)
{
    TarEntry* entry = new TarEntry();
    entry->name = getFileName(buffer);
    entry->fileSize = getFileSizeA(buffer);
    entry->contentBlockId = bId;

    // '0' -> regular file, '5' -> directory
    switch(getFileType(buffer))
    {
        case '0' : entry->type = TarEntryType::RegularFile; break;
        case '5' : entry->type = TarEntryType::Directory; break;
        default:
                   entry->type = TarEntryType::Unknown; break;
    }
    return entry;
}


// TarArchive
TarArchive::~TarArchive()
{
    for(auto x : entryMap)
        delete x.second;
}

TarArchive* TarArchive::create(const std::string& path, std::string directoryRoot)
{
    if(path.empty())
        return NULL;
    
    if (directoryRoot.empty())
        directoryRoot = path;

    TarArchive* tar = new TarArchive();
    tar->tarPath = path;

    size_t sepIndex = directoryRoot.find_last_of('/');
    if(sepIndex != std::string::npos)
    {
        tar->dirPath = directoryRoot.substr(0, sepIndex+1);
        printf("tar root %s\n", tar->dirPath.c_str());
    }

    FILE* file = fopen(path.c_str(), "rb");

    if (file)
    {
        unsigned char* buffer = (unsigned char*)calloc(TAR_BLOCK_SIZE, 1);
        
        fseek(file, 0L, SEEK_END);
        tar->tarSize = ftell(file);
        fseek(file, 0L, SEEK_SET);

        size_t blockId = 0;
        while(true)
        {
            size_t resultSize = fread(buffer, 1, TAR_BLOCK_SIZE, file);
            blockId++;

            if(resultSize == 0)
            {
                printf("result size is 0\n");
                break;
            }

            TarEntry* entry = TarEntry::createFromHeader(blockId, buffer);
            if(entry->getType() == TarEntryType::Unknown)
            {
                delete entry;
                entry = NULL;
                printf("unknown entry, break\n");
                break;
            }
            else
            {
                tar->entryMap[tar->dirPath + entry->getName()] = entry;
                //printf("%s, type %d, size %lu\n", entry->getName().c_str(), entry->getType(), entry->getFileSize());

                blockId += (size_t)ceil(entry->getFileSize() / (float)TAR_BLOCK_SIZE);
                int error = fseek(file, blockId * TAR_BLOCK_SIZE, SEEK_SET);

                if(error != 0)
                {
                    printf("error %d", error);
                    break;
                }
            }
        }
        fclose(file);
        free(buffer);
        
        return tar;
    }
    else
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
        // we need to bypass ccfileutils. which reads the whole file in memory
//        tar->tarSize = s_pZipFile->getFileSize(path);
//        
//        size_t blockId = 0;
//        auto entry = s_pZipFile->getFileEntry(path);
//        while(true)
//        {
//            s_pZipFile->getFileRegionData(path, 0/* offset has no effect right now. this is streaming. */, <#unsigned long length#>)
//        }
#endif
    }
    
    delete tar;
    return NULL;
}

bool TarArchive::fileExists(const std::string& path) const
{
    return entryMap.count(path) != 0;
}

std::vector<std::string> TarArchive::getFileList() const
{
    std::vector<std::string> fileList;
    fileList.reserve(entryMap.size());
    for(auto x : entryMap)
        fileList.push_back(x.first);
    return fileList;
}

void* TarArchive::getFileData(const std::string& key, size_t* outSize)
{
//    printf("getFileData %s\n", key.c_str());
    if(entryMap.count(key) == 0)
    {
        *outSize = 0;
        return NULL;
    }

    TarEntry* entry = entryMap[key];
    unsigned char* outBuffer = (unsigned char*) calloc(entry->getFileSize(), 1);

    {
        FILE* file = fopen(tarPath.c_str(), "rb");
        // skip header, seek for content block directly
        fseek(file, entry->getContentBlockId() * TAR_BLOCK_SIZE, SEEK_SET);

        unsigned char* pOut = outBuffer;
        unsigned char* buffer = (unsigned char*)calloc(TAR_BLOCK_SIZE, 1);

        for(size_t leftSize = entry->getFileSize(); leftSize > 0; /* inside loop */)
        {
            size_t expectSize = std::min<size_t>(TAR_BLOCK_SIZE, leftSize);
            size_t copySize = fread(buffer, 1, expectSize, file);
            if(copySize)
            {
                memcpy(pOut, buffer, copySize);
                pOut += copySize;
                leftSize -= copySize;
            }
        }
        free(buffer);
        fclose(file);
    }

    *outSize = entry->getFileSize();
    return outBuffer;
}

