#include <stdlib.h>
#include <string.h>

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
	file->aux = NULL;
	file->buffer = malloc(sizeof(char) * optimal_row_length);
	file->orl = optimal_row_length;
	file->cc = column_count;
	file->sep = separator;
	file->fields = malloc(sizeof(char*) * column_count);
	memset(file->buffer, '\0', file->orl);

	return CSV_OK;
}

static void
skip_row(int fd)
{
	char c;

	c = '\0';
	while (c != '\n')
		read(fd, &c, 1);
}

int
csv_skip_rows(struct csv* file, size_t n)
{
	if (file == NULL)
		return CSV_E_NULL;

	if (n == 0)
		return CSV_OK;

	for (i = 0; i < n; i++)
		skip_row(file->fd);

	return CSV_OK;
}

static int
fill_fields(struct csv* file)
{
	size_t fields;
	size_t i;

	file->fields[0] = &file->buffer[0];
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
	if (file == NULL || out_fields == NULL)
		return CSV_E_NULL;

	if (file->aux != NULL) {
		free(file->aux);
		file->aux = NULL;
	}

	if (read(file->fd, file->buffer, file->orl));

	switch (fill_fields(file)) {
		case 
		default:
			return CSV_E_UNKNOWN;
	}
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

