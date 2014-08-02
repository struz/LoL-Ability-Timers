/*  LoL Ability Timers. Injects into League of Legends to show ability
timers for all champions.
Copyright (C) 2014  Matthew Whittington

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include "stdafx.h"
#include "FileHelper.h"
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <algorithm>
#include "ByteSearcher.h"

#define BUFSIZE 1048576 // read 1MB of file at a time

/// <summary>Read an N byte value from a file at a given address
/// within the file. N must be less than the size of a long long.</summary>
/// <param name="filename">Path to the file to read from.</param>
/// <param name="addressToRead">The address of the value to read.</param>
/// <param name="numBytesToRead">The number of bytes to read (N).</param>
/// <returns>The value at the given address as an unsigned long long
/// (the max amount of bytes this function can process).</returns>
unsigned long long FileHelper::ReadValueFromFile(std::string filename,
	size_t addressToRead, size_t numBytesToRead) {
	std::ifstream file;
	unsigned int result = 0;

	if (filename.length() == 0)
		throw std::invalid_argument("Invalid filename");
	if (numBytesToRead > sizeof(long long))
		throw std::invalid_argument("Too many bytes requested");

	file.open(filename, std::ios::binary);
	if (file.is_open()) {
		file.seekg(0, file.end);
		size_t fileSize = (size_t)file.tellg();
		file.seekg(0, file.beg);

		if (addressToRead + sizeof(result) > fileSize)
			throw std::invalid_argument("Invalid address - address to \
										 read greater than file size");

		file.seekg(addressToRead, file.beg);
		file.read((char*)&result, numBytesToRead);
	}
	else
		throw std::runtime_error("Could not open file");

	file.close();
	return result;
}

/// <summary>Read a 4 byte value from a file at a given address
/// within the file.</summary>
/// <param name="filename">Path to the file to read from.</param>
/// <param name="addressToRead">The address of the value to read.</param>
/// <returns>The value at the given address as an unsigned int.</returns>
unsigned int FileHelper::ReadIntFromFile(std::string filename,
		size_t addressToRead) {
	unsigned long long result = ReadValueFromFile(filename,
		addressToRead, sizeof(unsigned int));
	return (unsigned int)result;
}

/// <summary>Read a 1 byte value from a file at a given address
/// within the file.</summary>
/// <param name="filename">Path to the file to read from.</param>
/// <param name="addressToRead">The address of the value to read.</param>
/// <returns>The value at the given address as an unsigned int.</returns>
unsigned char FileHelper::ReadByteFromFile(std::string filename,
	size_t addressToRead) {
	unsigned long long result = ReadValueFromFile(filename,
		addressToRead, sizeof(unsigned char));
	return (unsigned char)result;
}

/// <summary>Open and scan a file for a particular byte pattern.</summary>
/// <param name="filename">The filename of the file to open and scan.</param>
/// <param name="bytes">The byte pattern to look for.</param>
/// <param name="bytesLen">The length of the byte pattern to look for.</param>
/// <returns>The address in the file at which the byte pattern was found,
/// -1 if not found.</returns>
long long FileHelper::ScanFileForBytes(std::string filename,
		const char* bytes, size_t bytesLen) {
	std::ifstream file;
	unsigned char* buffer = new unsigned char[BUFSIZE];
	size_t bytesToRead = 0;
	size_t totalBytesRead = 0;
	size_t prevBytesRead = 0;
	long long result = -1;

	// Input check
	if (bytesLen > BUFSIZE || !bytes || bytesLen <= 0)
		return -1;

	ByteSearcher searcher = ByteSearcher(bytes, bytesLen);

	file.open(filename, std::ios::binary);
	if (file.is_open()) {
		file.seekg(0, file.end);
		size_t fileSize = (size_t)file.tellg();
		file.seekg(0, file.beg);

		//std::cout << "fileSize = " << fileSize << std::endl;
		size_t plen = bytesLen - 1;

		// Read the file in chunks so that we can theoretically handle
		// files of any size.
		while (totalBytesRead < fileSize && file.good()) {
			bytesToRead = std::min((size_t)BUFSIZE, fileSize - totalBytesRead);

			// Since the searcher uses the Boyer Moore Hoorspool algorithm
			// which searches from the end of the pattern, to get proper
			// matching we need to seek back bytesLen - 1 bytes
			// to get enough overlap to match the pattern if it cuts
			// across buffer read boundaries.
			bool backtrack = (prevBytesRead > plen);
			bool readLess = (backtrack && (bytesToRead + plen > BUFSIZE));
			if (backtrack) {
				memcpy(buffer, buffer + prevBytesRead, plen);
				if (readLess)
					file.read((char*)buffer + plen, bytesToRead - plen); // read adjusted
				else
					file.read((char*)buffer + plen, bytesToRead);
			}
			else {
				file.read((char*)buffer, bytesToRead); // read normally
			}

			// for end of file special case
			size_t adjustBytes = (backtrack && !readLess) ? plen : 0;
			result = searcher.SearchBytes(buffer, bytesToRead + adjustBytes);
			if (result > -1) {
				result += totalBytesRead;
				if (backtrack)
					result -= plen; // get the proper alignment for result
				break;
			}

			if (readLess)
				bytesToRead -= plen; // adjust the bytes we say we've read
			totalBytesRead += bytesToRead;
			prevBytesRead = bytesToRead;
			//std::cout << "totalBytesRead = " << std::hex << totalBytesRead << std::endl;
		}
		//unsigned char* buffer2 = new unsigned char[fileSize];
		//file.read((char*)buffer2, fileSize);
		//result = searcher.SearchBytes(buffer2, fileSize);
	}
	file.close();

	delete[] buffer;
	return result;
}

/// <summary>Scan the given file for a string and return the address at
/// which the first occurance was found.</summary>
/// <param name="filename">The filename to search.</param>
/// <param name="str">The string to search for.</param>
/// <returns>The address of the first occurance at which the string
/// was found.</returns>
long long FileHelper::ScanFileForString(std::string filename, std::string str) {
	const char* cstr = str.c_str();
	size_t strlen = str.length();
	return ScanFileForBytes(filename, cstr, strlen);
}

/// <summary>Scan the given file for references to an address and return
/// the first found reference.</summary>
/// <param name="filename">The path of the file to scan.</param>
/// <param name="addr">The address to look for references to.</param>
/// <param name="baseAddr">The base of the program. Usually defaults to
/// 0x400000 for most exe files.</param>
/// <returns>The address at which the reference was found, -1 if not found.</returns>
long long FileHelper::FindReferenceToAddress(std::string filename,
		unsigned int addr, long baseAddr) {
	unsigned int realAddr = addr + baseAddr;
	char* addressBytes = new char[sizeof(realAddr)];

	// copy the bytes, taking care of endianness automatically
	memcpy(addressBytes, &realAddr, sizeof(realAddr));
	long long result = ScanFileForBytes(filename, addressBytes, sizeof(addr));

	delete[] addressBytes;
	return result;
}