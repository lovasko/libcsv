#include <stdlib.h>
#include <unistd.h>

#define CSV_OK                  0
#define CSV_E_NULL              1
#define CSV_E_NEWLINE           2
#define CSV_E_COLUMN_COUNT      3
#define CSV_E_ROW_LENGTH        4
#define CSV_E_PATH              5
#define CSV_E_UNKNOWN           6
#define CSV_E_TOO_MANY_FIELDS   7
#define CSV_E_IO                8
#define CSV_E_NOT_ENOUGH_LINES  9
#define CSV_E_TOO_FEW_FIELDS   10
#define CSV_END                11

struct csv {
	char buffer[65536];
	char** fields;
	size_t cc;
	ssize_t i;
	ssize_t bytes_read;
	int fd;
	char sep;
	char padding[3];
};

int csv_open(struct csv* file, char* path, char separator, size_t column_count);
int csv_error_string(int code, char** out_error_string);
int csv_skip_line(struct csv* file, size_t n);
int csv_read_record(struct csv* file, char*** out_fields);
int csv_close(struct csv* file);

