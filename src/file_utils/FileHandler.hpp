#pragma once

#include "common/types.h"
#include <string>
#include <unistd.h>

#ifdef __cplusplus
extern "C" {
#endif

enum SeekType { SET,CUR,END };
enum OPENFLAG { READ,WRITE,READWRITE,APPEND };

int LoadToMem(const char * path, void** buff, u32 * size);
int WriteToFile(const char * path, void * buff, u32 size);

class BinaryFile {
	public:
		//constructors
		BinaryFile() {};
		BinaryFile(const char * FilePath, OPENFLAG openflag);
		~BinaryFile() {};
		//strings
		std::string ReadString(int maxLength = -1);
		void WriteString(std::string toWrite);
		//bytes
		u8 ReadUByte();
		s8 ReadSByte();
		void WriteUByte(u8 toWrite);
		void WriteSByte(s8 toWrite);
		//shorts
		u16 ReadU16LE();
		u16 ReadU16BE();
		s16 ReadS16LE();
		s16 ReadS16BE();
		void WriteU16LE(u16 toWrite);
		void WriteU16BE(u16 toWrite);
		void WriteS16LE(s16 toWrite);
		void WriteS16BE(s16 toWrite);
		//ints
		u32 ReadU32LE();
		u32 ReadU32BE();
		s32 ReadS32LE();
		s32 ReadS32BE();
		void WriteU32LE(u32 toWrite);
		void WriteU32BE(u32 toWrite);
		void WriteS32LE(s32 toWrite);
		void WriteS32BE(s32 toWrite);
		//longs
		u64 ReadU64LE();
		u64 ReadU64BE();
		s64 ReadS64LE();
		s64 ReadS64BE();
		void WriteU64LE(u64 toWrite);
		void WriteU64BE(u64 toWrite);
		void WriteS64LE(s64 toWrite);
		void WriteS64BE(s64 toWrite);
		//floats
		f32 ReadFloatLE();
		f32 ReadFloatBE();
		void WriteFloatLE(f32 toWrite);
		void WriteFloatBE(f32 toWrite);
		//doubles
		f64 ReadDoubleLE();
		f64 ReadDoubleBE();
		void WriteDoubleLE(f64 toWrite);
		void WriteDoubleBE(f64 toWrite);
		//closes the File and deletes the File()
		virtual int Close();
		//returns current file position
		int FTell();
		//returns current file position after seeking
		int Seek(int pos, SeekType seekType);
		
		bool IsEof();
		
		int GetFileSize();
	private:
		int readUByte(u8 **buff);
		int writeUByte(u8 *buff);
		
		int fileNumber;
		int fileSize;
		int extraOffset;
		
};

#ifdef __cplusplus
}
#endif
