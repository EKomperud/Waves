// WaveGenerator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

enum WaveExampleType
{
	SineWave = 0,
	SquareWave = 1,
	SawtoothWave = 2,
	TriangleWave = 3
};

class WaveGenerator
{
public:
	// Header, Format, Data chunks
	WaveHeader header;
	WaveFormatChunk format;
	WaveDataChunk data;

	WaveGenerator(WaveExampleType type, double frequency)
	{
		header = WaveHeader();
		format = WaveFormatChunk();
		data = WaveDataChunk();
		double PI = 4 * atan(1);

		uint32_t sampleCount = format.dwSamplesPerSec * format.wChannels;
		int amplitude = 32760;
		double t = (PI * 2 * frequency) / (format.dwSamplesPerSec * format.wChannels);
		data.shortArray = (short *)calloc(sampleCount, sizeof(short));

		switch (type)
		{
		case SineWave:
		{
			for (uint32_t i = 0; i < sampleCount - 1; i++)
			{
				for (int channel = 0; channel < format.wChannels; channel++)
				{
					data.shortArray[i + channel] = (short)(amplitude * sin(t * i));
				}
			}
			data.dwChunkSize = (uint32_t)(sampleCount * (format.wBitsPerSample / 8));
			break;
		}
		case SquareWave:
		{
			for (uint32_t i = 0; i < sampleCount - 1; i++)
			{
				for (int channel = 0; channel < format.wChannels; channel++)
				{
					short sign;
					if (sin(t * i) > 0)
						sign = 1;
					else if (sin(t * i) < 0)
						sign = -1;
					else
						sign = 0;
					data.shortArray[i + channel] = (short)(amplitude * sign);
				}
			}
			data.dwChunkSize = (uint32_t)(sampleCount * (format.wBitsPerSample / 8));
			break;
		}
		case SawtoothWave:
		{
			int samplesPerWavelength = (int)(format.dwSamplesPerSec / frequency);
			short amplitudeStep = (short)((amplitude * 2) / samplesPerWavelength);
			short sample = (short)-amplitude;
			uint32_t samples = 0;
			while (samples < sampleCount)
			{
				short sample = (short)-amplitude;
				for (int i = 0; i < samplesPerWavelength && samples < sampleCount; i++)
				{
					for (int channel = 0; channel < format.wChannels; channel++)
					{
						data.shortArray[samples] = sample;
						samples++;
					}
					sample += amplitudeStep;
				}
			}
			data.dwChunkSize = (uint32_t)(sampleCount * (format.wBitsPerSample / 8));
			break;
		}
		case TriangleWave:
		{
			int samplesPerWavelength = (int)(format.dwSamplesPerSec / frequency);
			short amplitudeStep = (short)-((amplitude * 4) / samplesPerWavelength);
			short sample = (short)-amplitude;
			for (uint32_t i = 0; i < sampleCount - 1; i+= format.wChannels)
			{
				for (int channel = 0; channel < format.wChannels; channel++)
				{
					data.shortArray[i + channel] = sample;
				}
				if ((int)abs(sample + amplitudeStep) >= amplitude)
				{
					short thisStep;
					if (sample < 0)
					{
						thisStep = amplitudeStep + (sample + amplitude);
						sample = -amplitude - thisStep;
					}
					else
					{
						thisStep = amplitudeStep + (sample - amplitude);
						sample = amplitude - thisStep;
					}
					amplitudeStep = (short)-amplitudeStep;
				}
				else
				{
					sample += amplitudeStep;
				}
			}
			data.dwChunkSize = (uint32_t)(sampleCount * (format.wBitsPerSample / 8));
			break;
		}
		default:
		{
			for (uint32_t i = 0; i < sampleCount - 1; i++)
			{
				for (int channel = 0; channel < format.wChannels; channel++)
				{
					data.shortArray[i + channel] = (uint16_t)(amplitude * sin(t * i));
				}
			}
			data.dwChunkSize = (uint32_t)(sizeof(data.shortArray) * (format.wBitsPerSample / 8));
			break;
		}
		}
	}

	void Save(const char* filePath)
	{
		int dataArraySize = (int)(format.dwSamplesPerSec * format.wChannels * 2);
		header.dwFileLength = 36 + dataArraySize;

		std::ofstream waveFile;
		waveFile.open(filePath, std::ios::out | std::ios::trunc | std::ios::binary);
		waveFile.write((char *)&header, 12);
		waveFile.write((char *)&format, 24);
		waveFile.write((char *)&data, 8);
		waveFile.write((char *)data.shortArray, dataArraySize);
		waveFile.close();
	}
};

int main(int argc, const char** argv)
{
	if (argc != 4)
		return 1;

	WaveExampleType waveType = WaveExampleType::SineWave;
	if (std::string(argv[2]) == "sine")
		waveType = WaveExampleType::SineWave;
	else if (std::string(argv[2]) == "square")
		waveType = WaveExampleType::SquareWave;
	else if (std::string(argv[2]) == "sawtooth")
		waveType = WaveExampleType::SawtoothWave;
	else if (std::string(argv[2]) == "triangle")
		waveType = WaveExampleType::TriangleWave;

	double frequency;
	frequency = strtod(argv[3], NULL);
	if (frequency > 0 && frequency < 1047)
	{
		WaveGenerator wg = WaveGenerator(waveType, frequency);
		wg.Save(argv[1]);
	}
    return 0;
}

