/*
 * Simple sound playback using ALSA API and libasound.
 *
 * Compile:
 * $ cc -o play sound_playback.c -lasound
 * 
 * Usage:
 * $ ./play <sample_rate> <channels> <seconds> < <file>
 * 
 * Examples:
 * $ ./play 44100 2 5 < /dev/urandom
 * $ ./play 22050 1 8 < /path/to/file.wav
 *
 * Copyright (C) 2009 Alessandro Ghedini <alessandro@ghedini.me>
 * --------------------------------------------------------------
 * "THE BEER-WARE LICENSE" (Revision 42):
 * Alessandro Ghedini wrote this file. As long as you retain this
 * notice you can do whatever you want with this stuff. If we
 * meet some day, and you think this stuff is worth it, you can
 * buy me a beer in return.
 * --------------------------------------------------------------
 */

#include <alsa/asoundlib.h>
#include <stdio.h>
#include <iostream>

#define PCM_DEVICE "plughw:0"

typedef unsigned int UINT32 ;
typedef unsigned short UINT16;
typedef unsigned char UINT8;
typedef signed int INT32 ;
typedef signed short INT16;
typedef signed char INT8;

#define FORMAT_PMC_CODE 	1
#define FORMAT_EXTENSIBLE 	0xFFFE

#pragma pack (1)

struct PCM_FORMAT_BLOCK
{
	/* 
		typical value:
	  	1 - Mono ;   
		2 - Steoro ;  
		6 - 5.1 ;  
		8 - 7.1  ; 
		or other value. 
	*/
	UINT16 m_channel;		/* typically, for steoro, channel = 2 */
	UINT32 SamplePerSecond;	/* Normally 48K or 48000. */
	UINT32 BytesPerSecond;	/* for 24bits depth, 2 channel, 48Kbps, this value = 3 * 2 * 48000 = 288000 (00 65 04 00) */
	UINT16 BytesPerSample;	/* for 24bits 2 channel, typically this value = 3 * 2 = 6 (06 00) (Each sample cost 3Bytes / Channel * 2 Channels ) */
	UINT16 BitsPerSample;	/* Sample Depth in bits, normally 8bit / 16bit / 24bit / 32bit */
};

struct WAVE_HEADER
{
	char m_RiffID[4];	// "RIFF"
	unsigned int m_dataLen;
	char m_waveID[4];	// "WAVE"
	struct FMT_BLOCLK
	{
		char m_fmtID[4];	// "fmt "
		UINT32 m_fmtBlockLen;	

		/*  FormatTag: 2 bytes:
			1: PCM Code;  
			3: IEEE float; 
			6: 8-bit ITU-T g.711 A-law;
			7: 8-bit ITU-T g.711 u-law;
			0xFFFE: WAVE_FORMAT_EXTENSIBLE 
		*/
		UINT16 m_formatTag;
	} m_formatBlock;
};
#pragma pack ()

bool g_format_le = true;
double g_amplifier[] = {1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0};

bool readWaveHeader(FILE *fp, unsigned int *rate, unsigned int *channels, unsigned int * sampleDepthInByte, unsigned int *sampleCount, unsigned short *format)
{
	WAVE_HEADER header;
	int nLen = fread(&header, 1, sizeof(header), fp);
	if (nLen != sizeof(header))
	{
		return (false);
	}

	printf("total File Len = %d\n", header.m_dataLen + 8);
	printf("format block len %d\n", header.m_formatBlock.m_fmtBlockLen);

	int nFormatBlockSize = header.m_formatBlock.m_fmtBlockLen;
	
	*format = header.m_formatBlock.m_formatTag;
	/* Here we only study the PMC_CODE */
#if 0	
	if (header.m_formatBlock.m_formatTag != FORMAT_PMC_CODE)
	{
		printf("format_tag = %d, Not Support! (Only support PCM_CODE (01 00)\n", header.m_formatBlock.m_formatTag);
		return (false);
	}
#endif	

	UINT8 *formatBlock = new UINT8[nFormatBlockSize];
	if (fread(formatBlock, 1, nFormatBlockSize - 2, fp) != (nFormatBlockSize - 2))
	{
		delete []formatBlock;
		printf("File format Block error!\n");
		return (false);
	}

	PCM_FORMAT_BLOCK *fmtBlock = (PCM_FORMAT_BLOCK *)formatBlock;
	printf("channel = %d\n", fmtBlock->m_channel);
	printf("SamplePerSecond = %d\n", fmtBlock->SamplePerSecond);
	printf("BytesPerSecond = %d\n", fmtBlock->BytesPerSecond);
	printf("BytesPerSample = %d\n", fmtBlock->BytesPerSample);
	printf("BitsPerSample = %d\n", fmtBlock->BitsPerSample);

	*rate = fmtBlock->SamplePerSecond;
	*channels = fmtBlock->m_channel;
	*sampleDepthInByte = fmtBlock->BytesPerSample / fmtBlock->m_channel;
	*format = header.m_formatBlock.m_formatTag;

	if (header.m_formatBlock.m_formatTag == FORMAT_EXTENSIBLE)
	{
		/* For WAVE_FORMAT_EXTENSIBLE, we need to read some extra field! */
		UINT8 listBlockHeader[8];
		if (fread(listBlockHeader, 1, 8, fp) != 8)
		{
			printf("File list Block error!\n");
			delete []formatBlock;
			return (false);
		}
		if (memcmp(listBlockHeader, "LIST", 4) != 0)
		{
			printf("Invalid list block tag!\n");
			delete []formatBlock;
			return (false);
		}
		UINT32 listBlockLen = *(UINT32 *)(listBlockHeader + 4);
		printf("list Block Len = %d\n", listBlockLen);

		// Let's just skip the slip block.
		fseek(fp, listBlockLen, SEEK_CUR);
	}

	UINT8 dataBlockHeader[8];
	if (fread(dataBlockHeader, 1, 8, fp) != 8)
	{
		printf("File data Block error!\n");
		delete []formatBlock;
		return (false);
	}
	if (memcmp(dataBlockHeader, "data", 4) != 0)
	{
		printf("Invalid data block tag!\n");
		delete []formatBlock;
		return (false);
	}
	UINT32 sampleLen = *(UINT32 *)(dataBlockHeader + 4);
	printf("data Block Len = %d\n", sampleLen);
	*sampleCount = sampleLen / fmtBlock->BytesPerSample;
	printf("sampleCount = %d\n", *sampleCount);

	delete []formatBlock;
	return (true);
}

FILE * getWaveData(char * waveFileName, unsigned int *rate, unsigned int *channels, unsigned int * sampleDepthInByte, unsigned int *sampleCount)
{
	UINT16 format;	// PCM Code;
	FILE * fp = fopen(waveFileName, "rb");
	if (fp == NULL)
	{
		return (NULL);
	}

	bool ret = readWaveHeader(fp, rate, channels, sampleDepthInByte, sampleCount, &format);
	{
		if (ret != true)
		{
			fclose(fp);
			return (NULL);
		}
	}

	return (fp);
}

void addGain(int channelIndex, UINT8 *sampleData, int sampleDepthInByte)
{
	int value1, value2;

	static int index = 0;

	switch (sampleDepthInByte)
	{
		case 1:
		{
			/*
			 sampleData = [7f], value = -1  
			 sampleData = [80], value = 0  
			 sampleData = [81], value = 1  
			 */
			UINT8* pData = (UINT8 *)sampleData;
			*pData = (UINT8)((double)(*pData - 0x80) * g_amplifier[channelIndex]) + 0x80;
			break; 
		}
		case 2:
		{
			/*
			 sampleData = [01 00], value = 1  
			 sampleData = [00 00], value = 0  
			 sampleData = [ff ff], value = -1  
			 */
#if 0			
			value1 = (sampleData[1] << 8) | sampleData[0];
			if (value1 > 0x8000)
			{
				value1 = value1 - 0x10000;
			}
			value2 = (int)((double)value1 * g_amplifier[channelIndex]);
			if (value2 <= 0)
			{
				value2 = value2 + 0x10000;
			}
			sampleData[0] = value2 & 0xff;
			sampleData[1] = (value2 >> 8) & 0xff;
#else
			INT16* pData = (INT16 *)sampleData;
			*pData = (INT16)((double)(*pData) * g_amplifier[channelIndex]);
#endif						
			break;
		}
		case 3:
		{
			/*
			 sampleData = [01 00 00], value = 1  
			 sampleData = [00 00 00], value = 0  
			 sampleData = [ff ff ff], value = -1  
			 */
			value1 = (sampleData[2] << 16) | (sampleData[1] << 8) | sampleData[0];
			if (value1 > 0x800000)
			{
				value1 = value1 - 0x1000000;
			}

			value2 = (int)((double)value1 * g_amplifier[channelIndex]);
#if 0
			// just for debug.
			if (value2 != 0)
			{
				index++;

				if ((index & 0xff) == 0)
					printf("%d [%02x %02x %02x] => %d", value1, sampleData[0], sampleData[1], sampleData[2], value2);
			}
#endif			

			if (value2 < 0)
			{
				value2 = value2 + 0x1000000;
			}
			sampleData[0] = value2 & 0xff;
			sampleData[1] = (value2 >> 8) & 0xff;
			sampleData[2] = (value2 >> 16) & 0xff;
#if 0	
			// just for debug.		
			if (value1 != 0)
			{
				if ((index & 0xff) == 0)
					printf(" [%02x %02x %02x] \n", sampleData[0], sampleData[1], sampleData[2]);
			}
#endif			
			break;
		}

		case 4:
		{
			/*
			 sampleData = [01 00 00 00], value = 1  
			 sampleData = [00 00 00 00], value = 0  
			 sampleData = [ff ff ff ff], value = -1  
			 */

			INT32* pData = (INT32 *)sampleData;
			*pData = (INT32)((double)(*pData) * g_amplifier[channelIndex]);
			break;
		}
	}
}


void LE2BE(UINT8 *sampleData, int bytesPerSample)
{
	UINT8 temp;
	if (bytesPerSample == 1)
	{
		
	}
	else if (bytesPerSample == 2)
	{
		temp = sampleData[0];
		sampleData[0] = sampleData[1];
		sampleData[1] = temp;
	}
	else if (bytesPerSample == 3)
	{
		temp = sampleData[0];
		sampleData[0] = sampleData[2];
		sampleData[2] = temp;
	}
	else if (bytesPerSample == 4)
	{
		temp = sampleData[0];
		sampleData[0] = sampleData[3];
		sampleData[3] = temp;

		temp = sampleData[1];
		sampleData[1] = sampleData[2];
		sampleData[2] = temp;
	}
	else 
	{
		printf("Invalid bytesPerSample = %d\n", bytesPerSample);
	}
}

int main(int argc, char **argv) {
	unsigned int pcm, tmp, dir;
	unsigned int rate, channels, seconds, sampleDepthInByte, sampleCount;
	snd_pcm_t *pcm_handle;
	snd_pcm_hw_params_t *params;
	snd_pcm_uframes_t frames;
	char *buff;
	int buff_size, loops;
	FILE * wave_file;

	if (argc < 3) {
		printf("\nUsage: %s <wave_file_name> [format_le|format_be] <channel_0_amplifier> <channel_1_amplifier> ...\n", argv[0]);
		printf("    for example: %s 48k_8bit.wav format_be 0.5 2.0 \n\n", argv[0]);
		return -1;
	}
	char *waveFileName = argv[1];
	printf("fileName = %s\n", waveFileName);
	wave_file = getWaveData(waveFileName, &rate, &channels, &sampleDepthInByte, &sampleCount);
	if (wave_file == NULL)
	{
		printf("Failed to read file: %s\n", waveFileName);
		exit(0);
	}

	char captureFileName[128];
	strcpy(captureFileName, waveFileName);
	strcat(captureFileName, ".capture");
	FILE *capture_file = fopen(captureFileName, "wb");

	if (strcmp(argv[2], "format_le") == 0)
	{
		g_format_le = true;
	} 
	else if (strcmp(argv[2], "format_be") == 0)
	{
		g_format_le = false;
	}
	else
	{
		printf("Invalid format param: [%s]. Valid format must be \"format_le\", or \"format_be\"!\n", argv[2]);
		exit(0);
	}

	for (int i = 3; i < argc; i++)
	{
		if ((i - 3) >= 8)
		{
			// We support 8 channels at most!
			break;
		}

		double f = atof(argv[i]);
		if ((f < -5) || (f > 5))
		{
			printf("Channel %d is set to unsuitable value: %f\n", i - 3, f);
			exit (0);
		}
		else if (f == 0.0)
		{
			printf("Channel %d will be muted!\n", i - 3);
		}
		else 
		{
			printf("Channel %d will be amplified at %f\n", i - 3, f);
		}
		g_amplifier[i - 3] = f;
	}

	seconds  = sampleCount / rate;

	std::cout << "rate = " << rate << std::endl;
	std::cout << "channels = " << channels << std::endl;
	std::cout << "sampleDepthInByte = " << sampleDepthInByte << std::endl;
	std::cout << "sampleCount = " << sampleCount << std::endl;
	std::cout << "seconds = " << seconds << std::endl;

	/* Open the PCM device in playback mode */
	if (pcm = snd_pcm_open(&pcm_handle, PCM_DEVICE,
					SND_PCM_STREAM_PLAYBACK, 0) < 0) 
	{
		printf("ERROR: Can't open \"%s\" PCM device. %s\n",
					PCM_DEVICE, snd_strerror(pcm));
		exit (0);
	}

	/* Allocate parameters object and fill it with default values*/
	snd_pcm_hw_params_alloca(&params);

	snd_pcm_hw_params_any(pcm_handle, params);

	/* Set parameters */
	if (pcm = snd_pcm_hw_params_set_access(pcm_handle, params,
					SND_PCM_ACCESS_RW_INTERLEAVED) < 0) 
		printf("ERROR: Can't set interleaved mode. %s\n", snd_strerror(pcm));

	snd_pcm_format_t pcm_format;
	printf("!!-----------------------------");
	if (g_format_le == true)
	{
		if (sampleDepthInByte == 1)
		{
			pcm_format = SND_PCM_FORMAT_S8;
			printf("pcm_format = 8_LE!\n");
		}
		else if (sampleDepthInByte == 2)
		{
			pcm_format = SND_PCM_FORMAT_S16_LE;
			printf("pcm_format = 16_LE!\n");
		}
		else if (sampleDepthInByte == 3)
		{
			pcm_format = SND_PCM_FORMAT_S24_3LE;
			printf("pcm_format = 24_LE!\n");
		}
		else if (sampleDepthInByte == 4)
		{
			pcm_format = SND_PCM_FORMAT_S32_LE;
			printf("pcm_format = 32_LE!\n");
		}
		else
		{
			printf("Invalid sampleDepthInByte = %d", sampleDepthInByte);
			return (0);
		}
	}
	else
	{
		if (sampleDepthInByte == 1)
		{
			pcm_format = SND_PCM_FORMAT_S8;
			printf("pcm_format = S8!\n");
		}
		else if (sampleDepthInByte == 2)
		{
			pcm_format = SND_PCM_FORMAT_S16_BE;
			printf("pcm_format = S16_BE!\n");
		}
		else if (sampleDepthInByte == 3)
		{
			pcm_format = SND_PCM_FORMAT_S24_3BE;
			printf("pcm_format = S24_3BE!\n");
		}
		else if (sampleDepthInByte == 4)
		{
			pcm_format = SND_PCM_FORMAT_S32_BE;
			printf("pcm_format = S32_BE!\n");
		}
		else
		{
			printf("Invalid sampleDepthInByte = %d", sampleDepthInByte);
			return (0);
		}

	}

	if (pcm = snd_pcm_hw_params_set_format(pcm_handle, params, pcm_format) < 0) 
	{
		printf("ERROR: Can't set format. %s\n", snd_strerror(pcm));
		exit (-1);
	}

	if (pcm = snd_pcm_hw_params_set_channels(pcm_handle, params, channels) < 0) 
		printf("ERROR: Can't set channels number. %s\n", snd_strerror(pcm));

	if (pcm = snd_pcm_hw_params_set_rate_near(pcm_handle, params, &rate, 0) < 0) 
		printf("ERROR: Can't set rate. %s\n", snd_strerror(pcm));

	/* Write parameters */
	if (pcm = snd_pcm_hw_params(pcm_handle, params) < 0)
		printf("ERROR: Can't set harware parameters. %s\n", snd_strerror(pcm));

	/* Resume information */
	printf("PCM name: '%s'\n", snd_pcm_name(pcm_handle));

	printf("PCM state: %s\n", snd_pcm_state_name(snd_pcm_state(pcm_handle)));

	snd_pcm_hw_params_get_channels(params, &tmp);
	printf("channels: %i ", tmp);

	if (tmp == 1)
		printf("(mono)\n");
	else if (tmp == 2)
		printf("(stereo)\n");

	snd_pcm_hw_params_get_rate(params, &tmp, 0);
	printf("rate: %d bps\n", tmp);

	printf("seconds: %d\n", seconds);	

	/* Allocate buffer to hold single period */
	snd_pcm_hw_params_get_period_size(params, &frames, 0);

	printf("frames = %ld\n", frames);

	buff_size = frames * channels * sampleDepthInByte;
	printf("buff_size = %d\n", buff_size);
	buff = (char *) malloc(buff_size);

	snd_pcm_hw_params_get_period_time(params, &tmp, NULL);
	printf("period time = %d\n", tmp);

	loops = sampleCount * channels * sampleDepthInByte / buff_size;
	printf("loops = %d\n", loops);

	for ( ; loops > 0; loops--) 
	{
		int channelIndex = 0;

		pcm = fread(buff, 1, buff_size, wave_file);
		if (pcm != buff_size) 
		{
			printf("Early end of file.\n");
			break;
		}
		
		UINT8 *sampleData = (UINT8 *)buff;
		for (int i = 0; i < pcm; i+= sampleDepthInByte)
		{
			channelIndex++;
			if (channelIndex == channels)
			{
				channelIndex = 0;
			}
			addGain(channelIndex, sampleData, sampleDepthInByte);

			if (g_format_le == false)
			{
				LE2BE(sampleData, sampleDepthInByte);
			}

			sampleData += sampleDepthInByte;
		}

		frames = pcm / channels / sampleDepthInByte;
		printf("left %d, read = %d, fames = %ld\n", loops, pcm, frames);

		// Save to capture file.
		fwrite(buff, 1, pcm, capture_file);
		pcm = snd_pcm_writei(pcm_handle, buff, frames);
		if (pcm < 0)
		{
			if (pcm == -EPIPE) 
			{
				printf("XRUN.\n");
				snd_pcm_prepare(pcm_handle);
			}
			else
			{
				printf("ERROR. Can't write to PCM device. %s\n", snd_strerror(pcm));
			}
		}
//		usleep(500);
	}

	fclose(wave_file);
	fclose(capture_file);
	snd_pcm_drain(pcm_handle);
	snd_pcm_close(pcm_handle);
	free(buff);

	return 0;
}