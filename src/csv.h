#include <stdlib.h>

#define CSV_OK                0
#define CSV_E_NULL            1
#define CSV_E_NEWLINE         2
#define CSV_E_COLUMN_COUNT    3
#define CSV_E_ROW_LENGTH      4
#define CSV_E_PATH            5
#define CSV_E_UNKNOWN         6
#define CSV_E_TOO_MANY_FIELDS 7
#define CSV_E_IO              8
#define CSV_E_NOT_ENOUGH_LINES 9

struct csv {
	char buffer[65536];
	off_t overflow;
	size_t cc;
	int fd;
	char sep;
	char padding[3];
};

int csv_open(struct csv* file, char* path, char separator, int skip, size_t column_count, size_t optimal__length);
int csv_error_string(int code, char** out_error_string);
int csv_skip_line(struct csv* file, size_t n);
int csv_read_record(struct csv* file, char*** out_fields);
int csv_close(struct csv* file);

