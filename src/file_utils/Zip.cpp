
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string>

#include "Zip.hpp"
#include "common/common.h"
#include "common/types.h"
#include "logging/logger.h"

UnZip::UnZip(const char * zipPath) {
	fileToUnzip = unzOpen(zipPath);
}

UnZip::~UnZip() {
	unzClose(fileToUnzip);
}

int UnZip::ExtractFile(const char * internalPath,const char * path) {
	int code = unzLocateFile(fileToUnzip,internalPath,0);
	if(code == UNZ_END_OF_LIST_OF_FILE) 
		return -1;
	
	unz_file_info_s * fileInfo = GetFileInfo();
	
	char fullPath[strlen(path) + strlen(internalPath)];
	strcat(fullPath,GetFileName(fileInfo).c_str());
	code = Extract(fullPath,fileInfo);
	free(fileInfo);
	return code;
}

int UnZip::ExtractDir(const char * internalDir,const char * externalDir) {
	int i = 0;
	for(;;) {
		int code;
		if(i == 0) {
			code = unzGoToFirstFile(fileToUnzip);
			i++;
		} else {
			code = unzGoToNextFile(fileToUnzip);
		}
		if(code == UNZ_END_OF_LIST_OF_FILE) return -1;
		
		unz_file_info_s * fileInfo = GetFileInfo();
		if(fileInfo->uncompressed_size == 0 && fileInfo->compression_method == 0)
			continue;
		
		std::string zipName = GetFullFileName(fileInfo);
		if(zipName.compare(0,strlen(internalDir),internalDir) == 0) {
			zipName.erase(0,strlen(internalDir));
			std::string fileName(externalDir);
			fileName += "/";
			fileName += zipName;
			log_printf("%s\n",fileName.c_str());
			log_printf("%i\n",Extract(fileName.c_str(),fileInfo));
		}
		free(fileInfo);
	}
	return 0;
}

int UnZip::ExtractAll(const char * dirToExtract) {
	int i = 0;
	for(;;) {
		int code;
		if(i == 0) {
			code = unzGoToFirstFile(fileToUnzip);
			i++;
		} else {
			code = unzGoToNextFile(fileToUnzip);
		}
		if(code == UNZ_END_OF_LIST_OF_FILE) return -1;
		
		unz_file_info_s * fileInfo = GetFileInfo();
		if(fileInfo->uncompressed_size == 0 && fileInfo->compression_method == 0)
			continue;
		std::string fileName(dirToExtract);
		fileName += '/';
		fileName += GetFullFileName(fileInfo);
		
		Extract(fileName.c_str(),fileInfo);
		free(fileInfo);
	}
	return 0;
}

int UnZip::Extract(const char * path, unz_file_info_s * fileInfo) {
	//check to make sure filepath or fileInfo isnt null
	if(path == NULL || fileInfo == NULL)
		return -1;
		
	makePath(path);
	
	if(unzOpenCurrentFile(fileToUnzip) != UNZ_OK)
		return -2;
	
	u32 blocksize = 0x8000;
	u8 * buffer = (u8*)malloc(blocksize);
	if(buffer == NULL)
		return -3;
    u32 done = 0;
    int writeBytes = 0;
	
	int fileNumber = open(path, O_WRONLY | O_TRUNC);
	
	if(fileNumber == -1) {
		free(buffer);
		return -4;		
	}
		
	while(done < fileInfo->uncompressed_size)
    {
        if(done + blocksize > fileInfo->uncompressed_size) {
            blocksize = fileInfo->uncompressed_size - done;
        }
		unzReadCurrentFile(fileToUnzip,buffer,blocksize);
        writeBytes = write(fileNumber, buffer, blocksize);
		log_printf("Wrote %i bytes to file.\n",writeBytes);
        if(writeBytes <= 0) {
            break;
		}
        done += writeBytes;
    }
	close(fileNumber);
	free(buffer);

	if (done != fileInfo->uncompressed_size)
		return -4;		
	
	unzCloseCurrentFile(fileToUnzip);
	
	return 0;
}

int UnZip::makePath(const char * path) {
	log_printf("%s\n",path);	
	char correctPath[strlen(path)];
	strcpy(correctPath,path);
	char * pos = strrchr(correctPath, '/');
	if (pos != NULL) {
	    pos[0] = '\0';
	}
	
	char parentPath[strlen(correctPath)];
	strcpy(parentPath,correctPath);
	char * ptr = strrchr(parentPath, '/');
	if (ptr != NULL) {
	    ptr[1] = '\0';
		makePath(parentPath);
		return mkdir(correctPath,0x777);
	}
	return 0;
}

std::string UnZip::GetFileName(unz_file_info_s * fileInfo) {
	char * fileName = (char*)malloc(fileInfo->size_filename);
	std::string path;
	strcpy(fileName,GetFullFileName(fileInfo).c_str());
	char * pos = strrchr(fileName, '/');
	if (pos != NULL) {
		pos++;
		path = pos;
	} else {
		path = fileName;
	}
	free(fileName);
	return path;
}

std::string UnZip::GetFullFileName(unz_file_info_s * fileInfo) {
	char * filePath = (char*)malloc(fileInfo->size_filename);
	unzGetCurrentFileInfo(fileToUnzip,NULL,filePath,fileInfo->size_filename,NULL,0,NULL,0);
	log_printf("%s,%i\n",filePath,fileInfo->size_filename);
	std::string path(filePath);
	free(filePath);
	return path;
}

unz_file_info_s * UnZip::GetFileInfo() {
	unz_file_info_s * fileInfo = (unz_file_info_s*)malloc(sizeof(unz_file_info_s));
	unzGetCurrentFileInfo(fileToUnzip,fileInfo,NULL,0,NULL,0,NULL,0);
	return fileInfo;
}