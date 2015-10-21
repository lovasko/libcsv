#include <stdlib.h>

#define CSV_OK 0
#define CSV_E_NULL 1

struct csv {
	char** fields;
	char* buffer;
	char* aux;
	size_t carry;
	size_t orl;
	size_t cc;
	int fd;
	char sep;
	char padding[3];
};

int csv_open(struct csv* file, char* path, char separator, int skip, size_t column_count);
int csv_error_string(int code, char** out_error_string);
int csv_skip_rows(struct csv* file, size_t n);
int csv_read_row(struct csv* file, char*** out_fields);
int csv_close(struct csv* file);

