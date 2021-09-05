#ifndef CSFLIB_H
#define CSFLIB_H
#include <stddef.h>
#include <stdint.h>

enum {
	CSFLIB_ERR_NOT_SUPPORTED=1,
	CSFLIB_ERR_INVALID_FILE,
	CSFLIB_ERR_FAILED_TO_OPEN_FILE,
	CSFLIB_ERR_STRING_NAME_TAKEN,
	CSFLIB_ERR_STRING_NAME_INVALID,
	CSFLIB_ERR_ARGUMMENT_MISSING,
	CSFLIB_ERR_ALLOC_FAIL
};

typedef struct {
	uint32_t name_len;
	char* name;
	uint32_t data_len;
	char* data;
}csflib_string;

typedef struct {
	uint32_t amount;
	csflib_string** strings;
}csflib_data;

csflib_data* csflib_read(FILE* stream);
csflib_data* csflib_readFile(const char* fileName);
#ifndef _WIN32
csflib_data* csflib_readMem(const unsigned char* file, size_t fileSize);
#endif

const char* csflib_get(csflib_data* data, const char* name);

int csflib_write(csflib_data* data, FILE* stream);
int csflib_writeFile(csflib_data* data, const char* fileName);
#ifndef _WIN32
unsigned char* csflib_writeMem(csflib_data* data, size_t* size);
#endif

csflib_data* csflib_create();
int csflib_add(csflib_data* data, const char* name, const char* string);
int csflib_remove(csflib_data* data, const char* name);
int csflib_replace(csflib_data* data, const char* name, const char* string);
int csflib_sort(csflib_data* data);

unsigned char csflib_exists(csflib_data* data, const char* name);
uint32_t csflib_index(csflib_data* data, const char* name);
int csflib_free(csflib_data* data);

int csflib_getError(char* log, int logSize);


#endif
