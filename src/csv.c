#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "csv.h"

int
csv_open(struct csv* file, char* path, char separator, size_t column_count)
{
	if (file == NULL || path == NULL)
		return CSV_E_NULL;

	if (column_count == 0)
		return CSV_E_COLUMN_COUNT;

	if (separator == '\n')
		return CSV_E_NEWLINE;

	file->fd = open(path, O_RDONLY);
	if (file->fd < 0)
		return CSV_E_PATH;

	file->cc = column_count;
	file->sep = separator;
	file->fields = malloc(sizeof(char*) * column_count);

	file->i = 0;
	file->bytes_read = 0;

	memset(file->buffer, '\0', 65536);
	file->fields[0] = &file->buffer[0];

	return CSV_OK;
}

int
csv_skip_line(struct csv* file, size_t n)
{
	char buffer[4096];
	size_t skipped;
	ssize_t buf_size;
	ssize_t i;

	if (file == NULL)
		return CSV_E_NULL;

	if (n == 0)
		return CSV_OK;

	skipped = 0;
	while (skipped != n) {
		buf_size = read(file->fd, buffer, 4096);

		if (buf_size == -1)
			return CSV_E_IO;

		if (buf_size == 0)
			return CSV_E_NOT_ENOUGH_LINES;			

		for (i = 0; i < buf_size; i++) {
			if (buffer[i] == '\n') {
				skipped++;
				if (skipped == n) {
					lseek(file->fd, buf_size - i, SEEK_CUR);
					return CSV_OK;
				}
			}
		}
	}

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
		file->bytes_read = read(file->fd, file->buffer, 65536);
		if (file->bytes_read == -1)
			return CSV_E_IO;

		if (file->bytes_read == 0)
			return CSV_END;

		file->i = 0;
	}

	file->fields[0] = &file->buffer[file->i];
	fields = 1;
	record_len = 0;
	for (; file->i < file->bytes_read; file->i++) {
		if (file->buffer[file->i] == file->sep) {
			file->buffer[file->i] = '\0';
			file->fields[fields] = &file->buffer[file->i+1];
			fields++;

			if (fields == file->cc) {
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
						memset(file->buffer, '\0', 65536);
						lseek(file->fd, -record_len, SEEK_CUR);
						return csv_read_record(file, out_fields);
					}
				}
			}
		}

		if (file->buffer[file->i] == '\n')
			return CSV_E_TOO_FEW_FIELDS;
	}

	return CSV_OK;
}

int
csv_error_string(int code, char** out_error_string)
{
	char* strings[] = {
		"OK",
		"Null"
	};
	if (out_error_string == NULL)
		return CSV_E_NULL;

	/* TODO define and check for CSV_E_MAX */
	*out_error_string = strings[code];
	return CSV_OK;
}

int
csv_close(struct csv* file)
{
	if (file == NULL)
		return CSV_E_NULL;

	free(file->fields);
	close(file->fd);
}

