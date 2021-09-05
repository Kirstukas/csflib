# csflib

This is a library to view, edit and write csf files.
For commands view the header file.

## Read example

```C
#include <stdio.h>
#include <stdlib.h>
#include <csflib.h>

int main(){
	csflib_data* data = csflib_readFile("file.csf");


	const char* string = csflib_get(data, "string");
	printf("%s\n", string);


	csflib_free(data);
}
```
This will print content of the string "string" from "file.csf".

## Write example

```C
#include <stdio.h>
#include <stdlib.h>
#include <csflib.h>

int main(){
	csflib_data* data = csflib_create();


	csflib_add(data, "string", "Hello, world!");

	csflib_writeFile(data, "file.csf");


	csflib_free(data);
}
```
this will create "file.csf" with "string" in it containing "Hello, world!".
