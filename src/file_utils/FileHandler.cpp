#include "FileHandler.hpp"
#include <fcntl.h>
#include <string.h>
#include <malloc.h>
#include "logging/logger.h"
#include <cerrno>
#include <errno.h>

BinaryFile::BinaryFile(const char * filepath, OPENFLAG openflag) {
	switch(openflag){
		case READ: fileNumber = open(filepath, O_RDONLY); break;
		case WRITE: fileNumber = open(filepath, O_WRONLY | O_TRUNC); break;
		case READWRITE: fileNumber = open(filepath, O_RDWR); break;
		case APPEND: fileNumber = open(filepath, O_RDWR | O_APPEND); break;
		default: break;
	}
	
	if(fileNumber >= 0) {
		fileSize = lseek(fileNumber,0,SEEK_END); 
		Seek(0,SET);
	}
}

int BinaryFile::readUByte(u8 **buff){
	*buff = NULL;
	u8 *buffer = (u8 *) malloc(1);
	int stat = read(fileNumber, buffer, 1);
	if(stat <= 0){
		*buff = 0;
	} else {
		*buff = buffer;
	}
	return stat;
}

int BinaryFile::writeUByte(u8 * buff){
	int stat = 0;
	if(FTell() < fileSize) {
		stat = write(fileNumber, buff, 1);
		return stat;
	} else {
		//adjust filesize since its writing at the end of the file
		while(extraOffset) {
			u8 temp = 0;
			stat += write(fileNumber, &temp, 1);
			if(stat > 0) {
				fileSize += stat;
			}
			extraOffset--;
		}
		stat = write(fileNumber, buff, 1);
		fileSize += stat;
		return stat;
	}
}

int LoadToMem(const char * path,void** buff, u32 * size) {
	//! always initialze input
	*buff = NULL;
    if(size)
        *size = 0;

	int fileNumber = open(path, O_RDONLY);
	if(fileNumber == -1) 
		return -2;
	u32 filesize = lseek(fileNumber, 0, SEEK_END);
    lseek(fileNumber, 0, SEEK_SET);

	u8 *buffer = (u8 *) malloc(filesize);
	if (buffer == NULL)
	{
		return -2;
	}

    u32 blocksize = 0x4000;
    u32 done = 0;
    int readBytes = 0;

	while(done < filesize)
    {
        if(done + blocksize > filesize) {
            blocksize = filesize - done;
        }
        readBytes = read(fileNumber, buffer + done, blocksize);
        if(readBytes <= 0)
            break;
        done += readBytes;
    }
	close(fileNumber);

	if (done != filesize)
	{
		free(buffer);
		return -3;
	}

	*buff = buffer;

    //! sign is optional input
    if(size)
        *size = filesize;

	return filesize;
}

int WriteToFile(const char * path, void * buff, u32 size) {
	//! always check input
	if(buff == NULL || size == 0)
		return -1;

    u32 blocksize = 0x4000;
    u32 done = 0;
    int writeBytes = 0;
	
	int fileNumber = open(path, O_WRONLY | O_TRUNC);
	if(fileNumber == -1) 
		return -2;
	while(done < size)
    {
        if(done + blocksize > size) {
            blocksize = size - done;
        }
        writeBytes = write(fileNumber, buff + done, blocksize);
        if(writeBytes <= 0) {
            break;
		}
        done += writeBytes;
    }
	close(fileNumber);

	if (done != size)
	{
		return -3;
	}

	return 0;
}

int BinaryFile::FTell() {
	return lseek(fileNumber,0,SEEK_CUR) + extraOffset;
}

int BinaryFile::Seek(int pos, SeekType seektype) {
	int temp = lseek(fileNumber,0,SEEK_CUR);
	switch(seektype){
		case SET: 				
			if(pos > fileSize) {
				extraOffset = pos - fileSize;
				temp = lseek(fileNumber,fileSize,SEEK_SET); 
			} else {
				extraOffset = 0;
				temp = lseek(fileNumber,pos,SEEK_SET); 
			}
			return temp + extraOffset;
			break;
		case CUR:
			if(temp == fileSize) {
				extraOffset += pos;
			} else if(temp + pos > fileSize) {
				extraOffset = (temp + pos) - fileSize;
				temp = lseek(fileNumber,fileSize,SEEK_SET); 
			} else {
				extraOffset = 0;
				temp = lseek(fileNumber,pos,SEEK_CUR); 
			}
			return temp + extraOffset;
			break;
		case END: 
			if(pos >= 0) {
				extraOffset = pos;
				temp = lseek(fileNumber,fileSize,SEEK_SET);
			} else {
				extraOffset = 0;
				temp = lseek(fileNumber,fileSize + pos,SEEK_SET);
			}
			return temp + extraOffset;
			break;
	}
	return -1;
}

u8 BinaryFile::ReadUByte() {
	u8 * buff = (u8*) malloc(1);
	readUByte(&buff);
	return *buff;
}

s8 BinaryFile::ReadSByte() {
	u8 Byte = ReadUByte();
	return *reinterpret_cast<s8*>(&Byte);
}

void BinaryFile::WriteUByte(u8 toWrite) {
	writeUByte(&toWrite);
}

void BinaryFile::WriteSByte(s8 toWrite) {
	WriteUByte(*reinterpret_cast<u8*>(&toWrite));
}

u16 BinaryFile::ReadU16LE() {
	u8 temp1 = ReadUByte();
	u8 temp2 = ReadUByte();
	return (u16) temp2 << 8 | temp1;
}

u16 BinaryFile::ReadU16BE() {
	u8 temp2 = ReadUByte();
	u8 temp1 = ReadUByte();
	return (u16) temp2 << 8 | temp1;
}

s16 BinaryFile::ReadS16LE() {
	u16 temp = ReadU16LE();
	return *reinterpret_cast<s16*>(&temp);
}

s16 BinaryFile::ReadS16BE() {
	u16 temp = ReadU16BE();
	return *reinterpret_cast<s16*>(&temp);
}

void BinaryFile::WriteU16LE(u16 toWrite) {
	u8 temp1 = (toWrite & 0xFF);
	u8 temp2 = (toWrite & 0xFF00) >> 8;
	WriteUByte(temp1);
	WriteUByte(temp2);
}

void BinaryFile::WriteU16BE(u16 toWrite) {
	u8 temp1 = (toWrite & 0xFF);
	u8 temp2 = (toWrite & 0xFF00) >> 8;
	WriteUByte(temp2);
	WriteUByte(temp1);
}

void BinaryFile::WriteS16LE(s16 toWrite) {
	WriteU16LE(*reinterpret_cast<u16*>(&toWrite));
}

void BinaryFile::WriteS16BE(s16 toWrite) {
	WriteU16BE(*reinterpret_cast<u16*>(&toWrite));
}

u32 BinaryFile::ReadU32LE() {
	u16 temp1 = ReadU16LE();
	u16 temp2 = ReadU16LE();
	return (u32) temp2 << 16 | temp1;
}

u32 BinaryFile::ReadU32BE() {
	u16 temp2 = ReadU16BE();
	u16 temp1 = ReadU16BE();
	return (u32) temp2 << 16 | temp1;
}

s32 BinaryFile::ReadS32LE() {
	u32 temp = ReadU32LE();
	return *reinterpret_cast<s32*>(&temp);
}

s32 BinaryFile::ReadS32BE() {
	u32 temp = ReadU32BE();
	return *reinterpret_cast<s32*>(&temp);
}

void BinaryFile::WriteU32LE(u32 toWrite) {
	u16 temp1 = (toWrite & 0xFFFF);
	u16 temp2 = (toWrite & 0xFFFF0000) >> 16;
	WriteU16LE(temp1);
	WriteU16LE(temp2);
}

void BinaryFile::WriteU32BE(u32 toWrite) {
	u16 temp2 = (toWrite & 0xFFFF);
	u16 temp1 = (toWrite & 0xFFFF0000) >> 16;
	WriteU16BE(temp1);
	WriteU16BE(temp2);
}

void BinaryFile::WriteS32LE(s32 toWrite) {
	WriteU32LE(*reinterpret_cast<u32*>(&toWrite));
}

void BinaryFile::WriteS32BE(s32 toWrite) {
	WriteU32BE(*reinterpret_cast<u32*>(&toWrite));
}

u64 BinaryFile::ReadU64LE() {
	u64 temp1 = ReadU32LE();
	u64 temp2 = ReadU32LE();
	return ((u64) temp2) << 32 | temp1;
}

u64 BinaryFile::ReadU64BE() {
	u64 temp2 = ReadU32BE();
	u64 temp1 = ReadU32BE();
	return ((u64) temp2) << 32 | temp1;
}

s64 BinaryFile::ReadS64LE() {
	u64 temp = ReadU64LE();
	return *reinterpret_cast<s64*>(&temp);
}

s64 BinaryFile::ReadS64BE() {
	u64 temp = ReadU64BE();
	return *reinterpret_cast<s64*>(&temp);
}

void BinaryFile::WriteU64LE(u64 toWrite) {
	u32 temp2 = (toWrite & 0xFFFFFFFF);
	u32 temp1 = (toWrite & 0xFFFFFFFF00000000) >> 32;
	WriteU32LE(temp2);
	WriteU32LE(temp1);
}

void BinaryFile::WriteU64BE(u64 toWrite) {
	u32 temp1 = (toWrite & 0xFFFFFFFF);
	u32 temp2 = (toWrite & 0xFFFFFFFF00000000) >> 32;
	WriteU32BE(temp2);
	WriteU32BE(temp1);
}

void BinaryFile::WriteS64LE(s64 toWrite) {
	WriteU64LE(*reinterpret_cast<u64*>(&toWrite));
}

void BinaryFile::WriteS64BE(s64 toWrite) {
	WriteU64BE(*reinterpret_cast<u64*>(&toWrite));
}

f32 BinaryFile::ReadFloatLE() {
	u32 temp = ReadU32LE();
	return *reinterpret_cast<f32*>(&temp);
}

f32 BinaryFile::ReadFloatBE() {
	u32 temp = ReadU32BE();
	return *reinterpret_cast<f32*>(&temp);
}

void BinaryFile::WriteFloatLE(f32 toWrite) {
	WriteU32LE(*reinterpret_cast<u32*>(&toWrite));
}

void BinaryFile::WriteFloatBE(f32 toWrite) {
	WriteU32BE(*reinterpret_cast<u32*>(&toWrite));
}

f64 BinaryFile::ReadDoubleLE() {
	u64 temp = ReadU64LE();
	return *reinterpret_cast<f64*>(&temp);
}

f64 BinaryFile::ReadDoubleBE() {
	u64 temp = ReadU64BE();
	return *reinterpret_cast<f64*>(&temp);
}

void BinaryFile::WriteDoubleLE(f64 toWrite) {
	WriteU64LE(*reinterpret_cast<u64*>(&toWrite));
}

void BinaryFile::WriteDoubleBE(f64 toWrite) {
	WriteU64BE(*reinterpret_cast<u64*>(&toWrite));
}

std::string BinaryFile::ReadString(int maxLength) {
	std::string returnString("");
	u8 * character = (u8*) malloc(1);
	int readStat = 0;
	while(true) {
		int readStat = read(fileNumber,character,1);
		if(/*Is Null Char?*/*character == 0
			|| /*Is Eof?*/readStat < 1 
			|| /*If Max Length is defined is it greater than or equal to maxLength?*/(maxLength != -1 && (int) returnString.length() >= maxLength)) {
			Seek(-1,CUR);
			return returnString;
		}
		returnString += *character;
	}
}

void BinaryFile:: WriteString(std::string toWrite) {
	const char * writable = toWrite.c_str();
	for(int i = 0;i < (int)strlen(writable);i++) {
		char * currrentWrite = strdup(writable + i);
		WriteUByte(*reinterpret_cast<u8*>(currrentWrite));
	}
}

int BinaryFile::Close() {
	close(fileNumber);
	return 1;
}

bool BinaryFile::IsEof() {
	if(FTell() >= fileSize) {
		return true;
	}
	return false;
}

int BinaryFile::GetFileSize() {
	return fileSize;
}
