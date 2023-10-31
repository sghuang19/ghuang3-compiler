#ifndef ENCODER_H
#define ENCODER_H

#define MAX_STRING_LEN 255

int string_decode(const char* es, char* s);

int string_encode(const char* s, char* es);

int decode(const char* filename);

#endif