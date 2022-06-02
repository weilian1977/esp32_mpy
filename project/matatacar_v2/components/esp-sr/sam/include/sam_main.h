#ifndef SAM_TRANSLATE_H
#define SAM_TRANSLATE_H


#define DEFAULT_PITCH    64
#define DEFAULT_SPEED    72
#define DEFAULT_MOUTH    128
#define DEFAULT_THROAT   128

int sam_translate(const char *text,char *buffer,int buffersize);

#endif