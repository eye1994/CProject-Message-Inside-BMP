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

void encrypt(char *string, unsigned len, char *password, unsigned password_len)
{
  
}

void write_message(FILE *inFILE, FILE *outFILE)
{
  // GET SECRET MESSAGE
  printf("Enter the message you want to hide: ");
  char message[254];
  get_line(); // consume '\n'
  fgets(message, 253, stdin);
  unsigned len = strlen(message);

  // GET PASSWORD AND ECRYPE
  BYTE key;
  unsigned number;
  printf("Enter the key: (number between 0 - 255): ");
  scanf("%u", &number);
  key = number;
  for (unsigned i = 0, len = strlen(message); i < len; i++)
  {

    message[i] = (message[i] ^ key);
  }

  printf("%s\n", message);

  // PROCESS THE BMP HEADER
  BITMAPINFOHEADER bi;
  BITMAPFILEHEADER bf;
  // read BITMAPFILEHEADER
  fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFILE);
  fwrite(&bf, sizeof(BITMAPFILEHEADER), 1, outFILE);
  // read BITMAPINFOHEADER
  fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFILE);
  fwrite(&bi, sizeof(BITMAPINFOHEADER), 1, outFILE);

  BYTE string_size = len - 1;

  // Write the lenght of the message intro the image
  write_byte(inFILE, outFILE, string_size);
  // Write char by char in the image
  for (unsigned i = 0; i < string_size; i++)
  {
    BYTE character = message[i];
    write_byte(inFILE, outFILE, character);
  }
  // Copy remainging file
  while(1)
  {
    BYTE buffer;
    fread(&buffer, sizeof(BYTE), 1, inFILE);
    if (feof(inFILE))
      break;
    fwrite(&buffer, sizeof(BYTE), 1, outFILE);
  }
}

void read_message(FILE *inFILE)
{
  // PROCESS THE BMP HEADER
  BITMAPINFOHEADER bi;
  BITMAPFILEHEADER bf;
  // read BITMAPFILEHEADER
  fread(&bf, sizeof(BITMAPFILEHEADER), 1, inFILE);
  // read BITMAPINFOHEADER
  fread(&bi, sizeof(BITMAPINFOHEADER), 1, inFILE);
    
  // Get the lenght of the message
  BYTE len = read_byte(inFILE);
  char message[len + 1];
  // Read the message from the file and print it
  unsigned i = 0;
  for (i = 0; i < len; i++)
    message[i] = read_byte(inFILE);
  message[i] = '\0';
  
  //GET PASSWORD AND ECRYPT
  BYTE key;
  unsigned number;
  printf("Enter the key: ");
  scanf("%u", &number);
  key = number;
  for (unsigned i = 0, len = strlen(message); i < len; i++)
    message[i] = (message[i] ^ key);
  printf("The message is: %s\n", message);

}

int main(int argc, char **argv)
{
  // Print menu
  printf("MENU: \n1. Write message.\n2. Read Message\nYour option: ");
  unsigned mode;
  
  // READ THE MODE FROM USER
  if (scanf("%u", &mode) != 1)
    return 1;
  
  if (mode == 1)
  {
    char inFileName[100], outFileName[100];
    printf("Enter the name of the input file: ");
    scanf("%99s", inFileName);
    
    printf("Enter the name of the output file: ");
    scanf("%99s", outFileName);
    
    FILE *inFILE = fopen(inFileName, "rb");
    FILE *outFILE = fopen(outFileName, "wb");  
    
    if (!inFILE || !outFILE)
    {
      fprintf(stderr, "Error at opening the file\n");
      return 1;   
    }
    write_message(inFILE, outFILE);  
    fclose(inFILE);
    fclose(outFILE);
  }
  else if (mode == 2)
  {
    char inFileName[100];
    printf("Enter the name of the input file: ");
    scanf("%99s", inFileName);
    FILE *inFILE = fopen(inFileName, "rb");
    if (!inFILE)
    {
      fprintf(stderr, "Error at opening the file\n");
      return 1;   
    }
    read_message(inFILE);  
    fclose(inFILE);
  }
  else
  {
    printf("Wrong mode.\n");
    return 1;
  }

  return 0;
}
