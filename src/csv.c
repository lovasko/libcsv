#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

#include "csv.h"

int
csv_open(struct csv* file, char* path, char separator, size_t field_count)
{
	if (file == NULL || path == NULL)
		return CSV_E_NULL;

	if (field_count == 0)
		return CSV_E_FIELD_COUNT;

	if (separator == '\n')
		return CSV_E_NEWLINE;

	errno = 0;
	file->fd = open(path, O_RDONLY);
	if (file->fd < 0)
		return CSV_E_IO;

	file->field_count = field_count;
	file->sep = separator;
	file->fields = malloc(sizeof(char*) * field_count);

	file->i = 0;
	file->bytes_read = 0;

	memset(file->buffer, '\0', __CSV_BUF_SIZE);
	file->fields[0] = &file->buffer[0];

	return CSV_OK;
}

int
csv_read_record(struct csv* file, char*** out_fields)
{
	int record_len;
	unsigned int fields;

	if (file == NULL || out_fields == NULL)
		return CSV_E_NULL;

	if (file->i == file->bytes_read) {
		file->bytes_read = read(file->fd, file->buffer, __CSV_BUF_SIZE);
		if (file->bytes_read == -1)
			return CSV_E_IO;

		if (file->bytes_read == 0)
			return CSV_END;

		file->i = 0;
	}

	file->fields[0] = &file->buffer[file->i];
	fields = 1;
	record_len = 0;
	for (; file->i < file->bytes_read; file->i++, record_len++) {
		if (file->buffer[file->i] == file->sep) {
			file->buffer[file->i] = '\0';
			file->fields[fields] = &file->buffer[file->i+1];
			fields++;

			if (fields == file->field_count) {
				while (1) {
					if (file->buffer[file->i] == '\n') {
						file->buffer[file->i] = '\0';
						file->i++;
						*out_fields = file->fields;
						return CSV_OK;
					}

					if (file->buffer[file->i] == file->sep)
						return CSV_E_TOO_MANY_FIELDS;

					file->i++;
					record_len++;

					if (file->i == file->bytes_read) {
						memset(file->buffer, '\0', __CSV_BUF_SIZE);
						errno = 0;
						if (lseek(file->fd, -record_len, SEEK_CUR) == -1)
							return CSV_E_IO;

						return csv_read_record(file, out_fields);
					}
				}
			}
		}

		if (file->buffer[file->i] == '\n')
			return CSV_E_TOO_FEW_FIELDS;
	}

	if (fields < file->field_count) {
		memset(file->buffer, '\0', __CSV_BUF_SIZE);
		errno = 0;
		if (lseek(file->fd, -record_len, SEEK_CUR) == -1)
			return CSV_E_IO;

		return csv_read_record(file, out_fields);
	}

	return CSV_OK;
}

int
csv_error_string(int code, char** out_error_string)
{
	static const char* strings[] = {
		"OK",
		"End",
		"Null pointer",
		"Unexpected newline",
		"Field count can not be zero",
		"I/O error",
		"Too few fields for record",
		"Too many fields for record",
		"Not enough lines to skip",
		"Invalid return code"
	};

	if (out_error_string == NULL)
		return CSV_E_NULL;

	if (code < 0 || code > CSV_E_MAX)
		return CSV_E_INVALID_CODE;

	*out_error_string = (char*)strings[code];
	return CSV_OK;
}

int
csv_close(struct csv* file)
{
	if (file == NULL)
		return CSV_E_NULL;

	free(file->fields);
	close(file->fd);

	return CSV_OK;
}

