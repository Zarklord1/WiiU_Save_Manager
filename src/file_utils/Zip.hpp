#pragma once

#include <zip/zip.h>
#include <zip/unzip.h>

/*class Zip {
	public:
		Zip();
		Zip(const char * zipPath);
		~Zip();
		int AddFile(const char * internalPath,const char * path);
};*/
class UnZip {
	public:		
		UnZip(const char * zipPath);
		~UnZip();
		int ExtractFile(const char * internalPath,const char * path);
		int ExtractAll(const char * dirToExtract);
		int ExtractDir(const char * internalDir,const char * externalDir);
	private:
		int Extract(const char * path, unz_file_info_s * fileInfo);
		int makePath(const char * path);
		const char * GetFileName(unz_file_info_s * fileInfo);
		const char * GetFullFileName(unz_file_info_s * fileInfo);
		unz_file_info_s * GetFileInfo();
		unzFile fileToUnzip;
};

