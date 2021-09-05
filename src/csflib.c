#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <csflib.h>

static unsigned int err;

csflib_data* csflib_read(FILE* stream){
	if (!stream){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return NULL;
	}
	
	char check[6];
	fread(check, sizeof(unsigned char), 5, stream);
	check[5] = 0;
	if (strcmp(check, " FSC\x03")){
		err = CSFLIB_ERR_INVALID_FILE;
		return NULL;
	}

	csflib_data* data = malloc(sizeof(csflib_data));
	if (!data){
		err = CSFLIB_ERR_ALLOC_FAIL;
		return NULL;
	}


	fseek(stream, 3, SEEK_CUR);	
	fread(&data->amount, sizeof(uint32_t), 1, stream);
	data->strings = malloc(sizeof(void*)*data->amount);
	
	fseek(stream, 12, SEEK_CUR);

	for (uint32_t i = 0; i < data->amount; i++){
		data->strings[i] = malloc(sizeof(csflib_string));

		fseek(stream, 8, SEEK_CUR);
		fread(&data->strings[i]->name_len, sizeof(uint32_t), 1, stream);
		data->strings[i]->name = malloc(data->strings[i]->name_len+1);
		fread(data->strings[i]->name, sizeof(char), data->strings[i]->name_len, stream);
		data->strings[i]->name[data->strings[i]->name_len] = 0;

		fseek(stream, 4, SEEK_CUR);
		fread(&data->strings[i]->data_len, sizeof(uint32_t), 1, stream);
		data->strings[i]->data = malloc(data->strings[i]->data_len+1);
		for (uint32_t c = 0; c < data->strings[i]->data_len; c++){
			data->strings[i]->data[c] = 0xFF-getc(stream);
			getc(stream);
		}
		data->strings[i]->data[data->strings[i]->data_len] = 0;
	}

	err = 0;
	return data;
}

csflib_data* csflib_readFile(const char* fileName){
	if (!fileName){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return NULL;
	}
	FILE* stream = fopen(fileName, "rb");
	if (stream){
		csflib_data* data = csflib_read(stream);
		fclose(stream);
		err = 0;
		return data;
	}else{
		err = CSFLIB_ERR_FAILED_TO_OPEN_FILE;
		return NULL;
	}
}

#ifndef _WIN32
csflib_data* csflib_readMem(const unsigned char* file, size_t fileSize){
	if (!file || fileSize){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return NULL;
	}
	FILE* stream = fmemopen((void*)(file), fileSize, "rb");
	if (stream){
		csflib_data* data = csflib_read(stream);
		fclose(stream);
		err = 0;
		return data;
	}else{
		err = CSFLIB_ERR_FAILED_TO_OPEN_FILE;
		return NULL;
	}
}
#endif



const char* csflib_get(csflib_data* data, const char* name){
	if (!data || !name){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return NULL;
	}
	if (!csflib_exists(data, name)){
		err = CSFLIB_ERR_STRING_NAME_INVALID;
		return NULL;
	}
	return data->strings[csflib_index(data, name)]->data;
}



int csflib_write(csflib_data* data, FILE* stream){
	if (!data || !stream){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	uint32_t amount_sub = data->amount - 1;
	fwrite(" FSC\x03\0\0\0", sizeof(char), 8, stream);
	fwrite(&data->amount, sizeof(uint32_t), 1, stream);
	fwrite(&amount_sub, sizeof(uint32_t), 1, stream);
	fwrite("\0\0\0\0\0\0\0\0", sizeof(char), 8, stream);

	for (uint32_t i = 0; i < data->amount; i++){
		fwrite(" LBL\x01\0\0\0", sizeof(char), 8, stream);
		fwrite(&data->strings[i]->name_len, sizeof(uint32_t), 1, stream);
		fwrite(data->strings[i]->name, sizeof(char), data->strings[i]->name_len, stream);
		fwrite(" RTS", sizeof(char), 4, stream);
		fwrite(&data->strings[i]->data_len, sizeof(uint32_t), 1, stream);
		for (uint32_t n = 0; n < data->strings[i]->data_len; n++){
			putc(0xFF-data->strings[i]->data[n], stream);
			putc('\xFF', stream);
		}
	}

	err = 0;
	return 1;
}

int csflib_writeFile(csflib_data* data, const char* fileName){
	if (!data || !fileName){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	FILE* file = fopen(fileName, "wb");
	if (file){
		int status = csflib_write(data, file);
		fclose(file);
		return status;
	}else{
		err = CSFLIB_ERR_FAILED_TO_OPEN_FILE;
		return 0;
	}
}

#ifndef _WIN32
unsigned char* csflib_writeMem(csflib_data* data, size_t* size){
	if (!data || !size){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return NULL;
	}
	char* output;
	FILE* file = open_memstream(&output, size);
	if (file){
		csflib_write(data, file);
		fclose(file);
		err = 0;
		return output;
	}else{
		err = CSFLIB_ERR_FAILED_TO_OPEN_FILE;
		return NULL;
	}
}
#endif



csflib_data* csflib_create(){
	csflib_data* data = malloc(sizeof(csflib_data));
	data->amount = 0;
	data->strings = NULL;

	err = 0;
	return data;
}

unsigned char csflib_exists(csflib_data* data, const char* name){
	if (!data || !name){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	err = 0;
	for (uint32_t i = 0; i < data->amount; i++)
		if (!strcmp(data->strings[i]->name, name))
			return 1;
	return 0;
}

uint32_t csflib_index(csflib_data* data, const char* name){
	if (!data || !name){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	err = 0;
	for (uint32_t i = 0; i < data->amount; i++)
		if (!strcmp(data->strings[i]->name, name))
			return i;
	return 0;
}

int csflib_add(csflib_data* data, const char* name, const char* string){
	if (!data || !name || !string){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	if (csflib_exists(data, name)){
		err = CSFLIB_ERR_STRING_NAME_TAKEN;
		return 0;
	}
	data->amount++;
	data->strings = realloc(data->strings, sizeof(data->strings) * data->amount);
	csflib_string* stringData = malloc(sizeof(csflib_string));
	data->strings[data->amount-1] = stringData;
	stringData->name_len = strlen(name);
	stringData->name = malloc(stringData->name_len+1);
	stringData->data_len = strlen(string);
	stringData->data = malloc(stringData->data_len+1);
	strcpy(stringData->name, name);
	strcpy(stringData->data, string);

	err = 0;
	return 1;
}

int csflib_remove(csflib_data* data, const char* name){
	if (!data || !name){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	if (!csflib_exists(data, name)){
		err = CSFLIB_ERR_STRING_NAME_INVALID;
		return 0;
	}

	uint32_t pos = csflib_index(data, name);
	free(data->strings[pos]->name);
	free(data->strings[pos]->data);
	free(data->strings[pos]);

	data->amount--;

	data->strings[pos] = data->strings[data->amount];

	data->strings = realloc(data->strings, sizeof(csflib_string*) * data->amount);

	err = 0;
	return 1;
}

int csflib_replace(csflib_data* data, const char* name, const char* string){
	if (!data || !name || !string){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}
	if (!csflib_exists(data, name)){
		err = CSFLIB_ERR_STRING_NAME_INVALID;
		return 0;
	}

	uint32_t pos = csflib_index(data, name);
	data->strings[pos]->data_len = strlen(string);
	data->strings[pos]->data = realloc(data->strings[pos]->data, data->strings[pos]->data_len+1);
	strcpy(data->strings[pos]->data, string);

	err = 0;
	return 1;
}

static int csflib_sort_cmp(const void* str1, const void* str2){
	return strcmp((*(csflib_string**)(str1))->name, (*(csflib_string**)(str2))->name);
}

int csflib_sort(csflib_data* data){
	if (!data){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return 0;
	}

	qsort(data->strings, data->amount, sizeof(csflib_string*), csflib_sort_cmp);

	err = 0;
	return 1;
}



int csflib_getError(char* log, int logLen){
	if (!log)
		return err;
	char* errStr;
	switch (err){
		case 0:
			errStr = "No error";
			break;
		case CSFLIB_ERR_NOT_SUPPORTED:
			errStr = "The function is not supported on this OS";
			break;
		case CSFLIB_ERR_INVALID_FILE:
			errStr = "The file format does not match csf file format";
			break;
		case CSFLIB_ERR_FAILED_TO_OPEN_FILE:
			errStr = "Failed to open file eather for read or write";
			break;
		case CSFLIB_ERR_STRING_NAME_TAKEN:
			errStr = "The string with the specified name already exists";
			break;
		case CSFLIB_ERR_STRING_NAME_INVALID:
			errStr = "The string with specified name could not be found";
			break;
		case CSFLIB_ERR_ARGUMMENT_MISSING:
			errStr = "One or more required argumments are missing";
			break;
		case CSFLIB_ERR_ALLOC_FAIL:
			errStr = "Failed to allocate memory";
			break;
		default:
			errStr = "Unknown error";
			break;
	}
	strncpy(log, errStr, logLen);
	return err;
}

int csflib_free(csflib_data* data){
	if (!data){
		err = CSFLIB_ERR_ARGUMMENT_MISSING;
		return CSFLIB_ERR_ARGUMMENT_MISSING;
	}

	if (data->strings){
		for (uint32_t i = 0; i < data->amount; i++){
			free(data->strings[i]->name);
			free(data->strings[i]->data);
			free(data->strings[i]);
		}
		free(data->strings);
	}
	free(data);

	err = 0;
	return 1;
}
