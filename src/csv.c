#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "csv.h"

int
csv_open(struct csv* file,
         char* path,
         char separator,
         size_t column_count,
         size_t optimal_row_length)
{
	if (file == NULL || path == NULL)
		return CSV_E_NULL;

	if (column_count == 0)
		return CSV_E_COLUMN_COUNT;

	if (separator == '\n')
		return CSV_E_NEWLINE;

	if (optimal_row_length == 0)
		return CSV_E_ROW_LENGTH;

	file->fd = open(path, O_RDONLY);
	if (file->fd < 0)
		return CSV_E_PATH;

	file->aux = NULL;
	file->buffer = malloc(sizeof(char) * optimal_row_length);
	file->orl = optimal_row_length;
	file->cc = column_count;
	file->sep = separator;
	file->fields = malloc(sizeof(char*) * column_count);

	memset(file->buffer, '\0', file->orl);
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
		buf_size = read(fd, buffer, 4096);

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

static int
fill_fields(struct csv* file)
{
	size_t fields;
	size_t i;

	fields = 1;
	for (i = 1; i < file->orl; i++) {
		if (file->buffer[i] == file->sep) {
			file->buffer[i] = '\0';	
			file->fields[fields] = &file->buffer[i+1];
			fields++;

			if (fields > file->cc)
				return CSV_E_TOO_MANY_FIELDS;
		}

		if (file->buffer[i] == '\n') {
			if (fields == file->cc)
				return CSV_OK;
			else
				return CSV_E_TOO_FEW_FIELDS;
		}
	}

	/* being here means that we are in the middle of a field,
	 * there is no new line and we need to alloc more to the aux */
	if (fields < file->cc)
		return CSV_NEED_MORE;
}

int
csv_read_row(struct csv* file, char*** out_fields)
{
	int ret;

	if (file == NULL || out_fields == NULL)
		return CSV_E_NULL;

	while (1) {
		bytes_read = read(file->fd, file->buffer, 65536);
		if (bytes_read == -1)
			return CSV_E_IO;
	}

	if (file->aux != NULL) {
		memmove(&file->buffer[0], file->aux[], );
		free(file->aux);
		file->aux = NULL;
	}

	if (read(file->fd, file->buffer, file->orl));

	ret = fill_fields(file);
	*out_fields = file->fields;
	
	return CSV_OK;
}

int
csv_close(struct csv* file)
{
	if (file == NULL)
		return CSV_E_NULL;

	free(file->buffer);
	free(file->aux);
	free(file->fields);

	close(file->fd);
}

