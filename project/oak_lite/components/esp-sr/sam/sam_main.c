#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "reciter.h"
#include "sam1.h"
#include "debug.h"
#include "sam_main.h"

int debug = 0;
extern int sam_bufferlength;

void WriteWav(char* filename, char* buffer, int bufferlength)
{
    FILE *file = fopen(filename, "wb");
    if (file == NULL) return;
    //RIFF header
    fwrite("RIFF", 4, 1,file);
    unsigned int filesize=bufferlength + 12 + 16 + 8 - 8;
    fwrite(&filesize, 4, 1, file);
    fwrite("WAVE", 4, 1, file);

    //format chunk
    fwrite("fmt ", 4, 1, file);
    unsigned int fmtlength = 16;
    fwrite(&fmtlength, 4, 1, file);
    unsigned short int format=1; //PCM
    fwrite(&format, 2, 1, file);
    unsigned short int channels=1;
    fwrite(&channels, 2, 1, file);
    unsigned int samplerate = 11050;//22050;
    fwrite(&samplerate, 4, 1, file);
    fwrite(&samplerate, 4, 1, file); // bytes/second
    unsigned short int blockalign = 1;
    fwrite(&blockalign, 2, 1, file);
    unsigned short int bitspersample=8;
    fwrite(&bitspersample, 2, 1, file);

    //data chunk
    fwrite("data", 4, 1, file);
    fwrite(&bufferlength, 4, 1, file);
    fwrite(buffer, bufferlength, 1, file);

    fclose(file);
}


int sam_translate(const char *text,char *buffer,int buffersize)
{
    int i;
    int ret;
    int phonetic = 0;
    char input_[259];
    debug=0;
    for(i=0; i<256; i++) input_[i] = 0;
    //SetMouth(20);
    //SetThroat(20);
    //SetMouth(DEFAULT_MOUTH);
    //SetThroat(DEFAULT_THROAT);
    //SetPitch(DEFAULT_PITCH);
    //SetSpeed(DEFAULT_SPEED);
    SetPitch(DEFAULT_PITCH);
    SetSpeed(100);
    phonetic = 0;
    for(i=0; text[i] != 0; i++)input_[i] = toupper((int)text[i]);
    if (debug)
    {
        if (phonetic) printf("phonetic input: %s\n", input_);
        else printf("text input: %s\n", input_);
    }

    if (!phonetic)
    {
        strncat(input_, "[", 256);
        if (!TextToPhonemes((unsigned char *)input_)) return 1;
        if (debug)printf("phonetic input: %s\n", input_);
    } else strncat(input_, "\x9b", 256);

    printf("phonetic input: %s\n", input_);
    SetInput(input_);
    ret=SAMMain(buffer,buffersize);
    if (ret!=0)
    {
        printf(" SAMMain=%d\n",ret);
        return ret;
    }
    return sam_bufferlength;
}
