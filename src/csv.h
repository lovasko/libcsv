#include <stdlib.h>
#include <unistd.h>

#define CSV_OK                 0
#define CSV_END                1
#define CSV_E_NULL             2
#define CSV_E_NEWLINE          3
#define CSV_E_FIELD_COUNT      4
#define CSV_E_IO               5
#define CSV_E_TOO_FEW_FIELDS   6
#define CSV_E_TOO_MANY_FIELDS  7
#define CSV_E_NOT_ENOUGH_LINES 8
#define CSV_E_INVALID_CODE     9
#define CSV_E_MAX              9

#define __CSV_BUF_SIZE 65536

struct csv {
	char buffer[__CSV_BUF_SIZE];
	char** fields;
	size_t field_count;
	ssize_t i;
	ssize_t bytes_read;
	int fd;
	char sep;
	char padding[3];
};

int csv_open(struct csv* file, char* path, char separator, size_t field_count);
int csv_read_record(struct csv* file, char*** out_fields);
int csv_error_string(int code, char** out_error_string);
int csv_close(struct csv* file);

