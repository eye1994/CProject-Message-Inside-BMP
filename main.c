#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include "bmp.h"

#define BLOCK 70

// Get the bit value from the give position in a BYTE
unsigned getBitValue(unsigned position, BYTE byte)
{	
	return byte & ~(~0u << 1) << position;
}

void setLastBitToOne(BYTE *byte)
{
	*byte = *byte | ~(~0u << 1);
}

void setLastBitToZero(BYTE *byte)
{
	*byte = *byte & (~0u << 1);
}

void setBitToOne(unsigned position, BYTE *byte)
{
	*byte = *byte | (~(~0u << 1) << position);
}

void setBitToZero(unsigned position, BYTE *byte)
{
	*byte = *byte & ~(~(~0u << 1) << position);
}

RGBTRIPLE readPixel(FILE *inFILE)
{
	RGBTRIPLE pixel;
  fread(&pixel, sizeof(RGBTRIPLE), 1, inFILE);
	return pixel;
} 

void writePixel(RGBTRIPLE pixel, FILE *outFILE)
{
	fwrite(&pixel, sizeof(pixel), 1, outFILE);
}

char * get_line(void) {
  char * tmp, * line = NULL;
  int mem = -1, size = 0;
  int c;
  while ((c = getchar()) != EOF) {
    if (mem <= size) {
      if ((tmp = realloc(line, (mem += BLOCK) + 1)) == NULL) {
        ungetc(c, stdin);
        fputs("Error at reallocating the memory", stderr);
        exit(1);
      } else {
        line = tmp;
      }
    }
    line[size++] = c;
    if (c == '\n') break;
  }
  if (line) {
    line[size++] = '\0';
    line = realloc(line, size);
  }
  return line;
}

void write_byte(FILE *inFILE, FILE *outFILE, BYTE byte)
{
  for (unsigned i = 0; i < 8; i++)
  {
    RGBTRIPLE pixel = readPixel(inFILE);
		unsigned bit_value = getBitValue(i, byte);
    if (bit_value)
			setLastBitToOne(&pixel.rgbtBlue);
    else 
      setLastBitToZero(&pixel.rgbtBlue);
		writePixel(pixel, outFILE);
    //fwrite(&pixel, sizeof(pixel), 1, outFILE); 
   } 
}

BYTE read_byte(FILE *inFILE)
{
  BYTE byte = 0u;
  for (unsigned i = 0; i < 8; i++)
  { 
    RGBTRIPLE pixel = readPixel(inFILE);
		unsigned bit_value = getBitValue(0, pixel.rgbtBlue);
    if (bit_value)
      setBitToOne(i, &byte);
    else
      setBitToZero(i, &byte);
  }
  return byte;
}

void encrypt(char *string, BYTE key)
{
  for (unsigned i = 0; string[i]; i++)
		string[i] = string[i] ^ key;
}

char *getMessage()
{
	printf("Enter the message you want to hide: ");
  char *message = malloc(sizeof(char) * 254);  
	if (!message) exit(1);	
	get_line(); // consume '\n'
  fgets(message, 253, stdin);
	return message;
}

BYTE getPassword()
{
  printf("Enter the key: (number between 0 - 255): ");
 	unsigned password;
	scanf("%u", &password);
	if (!password) exit(1);
	return (BYTE)password;
}

void writeHeader(FILE *inFILE, FILE *outFILE)
{
	BITMAPINFOHEADER bi;
  BITMAPFILEHEADER bf;
  // read BITMAPFILEHEADER
  fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFILE);
  fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outFILE);
  // read BITMAPINFOHEADER
  fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFILE);
  fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outFILE);
}

void skipHeader(FILE *inFILE)
{
	BITMAPINFOHEADER bi;
  BITMAPFILEHEADER bf;
  // read BITMAPFILEHEADER
  fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFILE);
  // read BITMAPINFOHEADER
  fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFILE);
}


void writeMessageToFile(FILE *inFILE, FILE *outFILE)
{
  char *message = getMessage();
	unsigned len = strlen(message);	

  BYTE key = getPassword();
  encrypt(message, key);

	writeHeader(inFILE, outFILE);
  
	BYTE string_len = len - 1;
  write_byte(inFILE, outFILE, string_len);

  // Write char by char in the image
  for (unsigned i = 0; i < string_len; i++)
    write_byte(inFILE, outFILE, message[i]);

  // Copy remainging file
  while(!feof(inFILE))
  {
    RGBTRIPLE pixel = readPixel(inFILE);
    writePixel(pixel, outFILE);
  }
}

void readMessageFromFile(FILE *inFILE)
{
	skipHeader(inFILE);
  // Get the lenght of the message
  BYTE len = read_byte(inFILE);
  char message[len + 1];

  // Read the message from the file and print it
  unsigned i = 0;
  for (i = 0; i < len; i++)
    message[i] = read_byte(inFILE);
  message[i] = '\0';
  
  BYTE key = getPassword();
  encrypt(message, key);
  printf("The message is: %s\n", message);

}

FILE *getTargetFile()
{
	char fileName[100];
  printf("Enter the name of the input file: ");
  scanf("%99s", fileName);
	FILE *file = fopen(fileName, "rb");
	if (!file) exit(1);
	return file;
}

FILE *getDestinationFile()
{
	char fileName[100];
  printf("Enter the name of the output file: ");
  scanf("%99s", fileName);
	FILE *file = fopen(fileName, "wb");
	if (!file) exit(1);
	return file;
}

void printMenu()
{
	printf("MENU: \n1. Write message.\n2. Read Message\nYour option: ");
}

unsigned getMode()
{
	unsigned mode;
  if (scanf("%u", &mode) != 1)
  	exit(1);
	return mode;
}

void hideMessage()
{
	FILE *inFILE = getTargetFile();
	FILE *outFILE = getDestinationFile();
	writeMessageToFile(inFILE, outFILE);  
	fclose(inFILE);
	fclose(outFILE);
}

void getHiddenMessage()
{
	FILE *inFILE = getTargetFile();
	readMessageFromFile(inFILE);  
	fclose(inFILE);
}

int main(int argc, char **argv)
{
  printMenu();
	unsigned mode = getMode();

	if (mode == 1)
		hideMessage();
	else if (mode == 2)
		getHiddenMessage();
	else 
		printf("Wrong mode.\n");

  return 0;
}
