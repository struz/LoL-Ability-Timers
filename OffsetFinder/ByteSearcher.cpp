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
#include "ByteSearcher.h"

/// <summary>Creates a new ByteSearcher looking for the provided
/// pattern.</summary>
/// <param name="pattern">The byte pattern to search for.</param>
/// <param name="patternLen">The number of bytes in the pattern.</param>
ByteSearcher::ByteSearcher(const char* pattern, size_t patternLen)
{
	this->patternLen = patternLen;
	this->pattern = new unsigned char[patternLen];
	memcpy(this->pattern, pattern, patternLen);

	PreprocessPattern();
}

ByteSearcher::~ByteSearcher()
{
	delete[] pattern;
}

/// <summary>Peprocess the provided pattern to prepare for search
/// using the Booyer-Moore-Horspool algorithm.</summary>
void ByteSearcher::PreprocessPattern() {
	// Initialize the default value for each table entry
	for (size_t scan = 0; scan < MAX_CHAR; scan += 1) {
		bad_char_skip[scan] = patternLen;
	}

	size_t last = patternLen - 1; // the last byte of the array

	// Populate the table values for each character in the needle
	for (size_t scan = 0; scan < last; scan += 1) {
		bad_char_skip[pattern[scan]] = last - scan;
	}
}

/// <summary>Perform a Booyer-Moore-Horspool algorithm search
/// to find our pattern within the bytes provided.</summary>
/// <param name="bytes">Haystack to search in.</param>
/// <param name="bytesLen">Length of the haystack.</param>
/// <returns>The address within the haystack at which the
/// pattern was found.</returns>
long ByteSearcher::SearchBytes(const unsigned char* bytes, size_t bytesLen) {
	size_t last = patternLen - 1;
	long offsetIntoBytes = 0;

	while (bytesLen >= patternLen) {
		// scan from the end of the needle
		for (size_t scan = last; bytes[scan] == pattern[scan]; scan -= 1) {
			if (scan == 0)
				// subtract last because otherwise we get the addr of end of string
				return offsetIntoBytes;
		}

		// skip the right amount of bytes (according to our analysis)
		// and search again
		bytesLen -= bad_char_skip[bytes[last]];
		offsetIntoBytes += bad_char_skip[bytes[last]];
		bytes += bad_char_skip[bytes[last]];
	}

	return -1;
}