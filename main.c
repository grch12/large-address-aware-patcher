#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <locale.h>

// You need a little-endian machine to run this

#define MZ_MAGIC_NUMBER ((int16_t)0x5a4d)     // "MZ"
#define PE_MAGIC_NUMBER ((int32_t)0x00004550) // "PE\0\0"

#define MACHINE_TYPE_I386 0x014c

#define LARGE_ADDRESS_AWARE 0x0020

#define PATH_LENGTH 32768

long FileSize(FILE* fp) {
  fpos_t pos;
  fgetpos(fp, &pos);
  fseek(fp, 0, SEEK_END);
  long size = ftell(fp);
  fsetpos(fp, &pos);
  return size;
}

uint32_t GetPEHeaderOffset(const char* image, size_t size) {
  if (size < 0x3c) return 0;

  if (*(int16_t*)image != MZ_MAGIC_NUMBER)
    return 0;

  uint32_t offset = *(uint32_t*)(image + 0x3c);

  if (offset > size) return 0;

  if (*(int32_t*)(image + offset) != PE_MAGIC_NUMBER)
    return 0;

  return offset;
}

uint16_t GetMachineType(const char* image, uint32_t offset) {
  return *(uint16_t*)(image + offset + 0x04);
}

uint16_t* GetCharacteristics(const char* image, uint32_t offset) {
  return (uint16_t*)(image + offset + 0x16);
}

int main(void) {
  setlocale(LC_ALL, ".UTF-8");
  printf("Input the path to the executable file you want to patch: ");
  char path[PATH_LENGTH];
  fgets(path, sizeof(path), stdin);
  char* newline = strchr(path, '\n');
  if (newline != NULL)
    *newline = '\0';
  FILE* fp = fopen(path, "rb");
  if (fp == NULL) {
    perror("Error opening file");
    fputs("Do not include quotes around the path", stderr);
    return 1;
  }
  size_t size = FileSize(fp);
  char* buf = malloc(size);
  fread(buf, size, 1, fp);
  fclose(fp);
  uint32_t offset = GetPEHeaderOffset(buf, size);
  if (offset == 0) {
    fputs("File is not a valid executable", stderr);
    free(buf);
    return 1;
  }
  uint16_t machineType = GetMachineType(buf, offset);
  if (machineType != MACHINE_TYPE_I386) {
    fputs("File is not a 32-bit executable", stderr);
    free(buf);
    return 1;
  }
  uint16_t* characteristics = GetCharacteristics(buf, offset);
  if (*characteristics & LARGE_ADDRESS_AWARE) {
    fputs("File is already large address aware", stderr);
    free(buf);
    return 1;
  }
  char backupPath[PATH_LENGTH];
  strcpy(backupPath, path);
  strcat(backupPath, ".bak");
  int result = rename(path, backupPath);
  if (result != 0) {
    printf("Error backing up original file, do you wish to overwrite it? (y/n)");
    int ch = getchar();
    if (ch != 'y') {
      free(buf);
      return 1;
    }
  }
  *characteristics |= LARGE_ADDRESS_AWARE;
  FILE* newFp = fopen(path, "wb");
  if (newFp == NULL) {
    perror("Error writing file");
    free(buf);
    return 1;
  }
  fwrite(buf, size, 1, newFp);
  fclose(newFp);
  free(buf);
  puts("File patched successfully");
  return 0;
}
