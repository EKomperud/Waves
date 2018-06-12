#pragma once
#include <stdint.h>

class WaveHeader
{
public:
	char sGroupID[4] = { 'R','I','F','F' }; // RIFF
	uint32_t dwFileLength; // total file length minus 8, which is taken up by RIFF
	char sRiffType[4] = { 'W','A','V','E' }; // always WAVE

	/// <summary>
	/// Initializes a WaveHeader object with the default values.
	/// </summary>
	WaveHeader()
	{
		dwFileLength = 0;
	}
};

class WaveFormatChunk
{
public:
	char sChunkID[4] = { 'f','m','t',' ' };         // Four bytes: "fmt "
	uint32_t dwChunkSize;        // Length of header in bytes
	uint16_t wFormatTag;       // 1 (MS PCM)
	uint16_t wChannels;        // Number of channels
	uint32_t dwSamplesPerSec;    // Frequency of the audio in Hz... 44100
	uint32_t dwAvgBytesPerSec;   // for estimating RAM allocation
	uint16_t wBlockAlign;      // sample frame size, in bytes
	uint16_t wBitsPerSample;    // bits per sample

	/// <summary>
	/// Initializes a format chunk with the following properties:
	/// Sample rate: 44100 Hz
	/// Channels: Stereo
	/// Bit depth: 16-bit
	/// </summary>
	WaveFormatChunk()
	{
		dwChunkSize = 16;
		wFormatTag = 1;
		wChannels = 2;
		dwSamplesPerSec = 44100;
		wBitsPerSample = 16;
		wBlockAlign = (uint16_t)(wChannels * (wBitsPerSample / 8));
		dwAvgBytesPerSec = dwSamplesPerSec * wBlockAlign;
	}
};

class WaveDataChunk
{
public:
	char sChunkID[4] = {'d','a','t','a'};     // "data"
	uint32_t dwChunkSize;    // Length of header in bytes
	short* shortArray;  // 16-bit audio

	/// <summary>
	/// Initializes a new data chunk with default values.
	/// </summary>
	WaveDataChunk()
	{
		dwChunkSize = 0;
	}
};