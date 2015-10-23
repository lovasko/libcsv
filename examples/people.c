#include <stdlib.h>
#include <stdio.h>
#include <csv.h>

int
main(void)
{
	struct csv file;
	char** fields;
	char* error;
	int ret;

	csv_open(&file, "people.csv", ',', 4);
	csv_read_record(&file, &fields); /* header skip */

	while ((ret = csv_read_record(&file, &fields)) == CSV_OK) {
		printf("     ID: %s\n", fields[0]);
		printf("   Name: %s\n", fields[1]);
		printf("Country: %s\n", fields[2]);
		printf(" Gender: %s\n", fields[3]);
		printf("--\n");
	}

	if (ret == CSV_END) {
		csv_close(&file);
		return EXIT_SUCCESS;
	}

	csv_error_string(ret, &error);
	printf("ERROR: %s\n", error);
	csv_close(&file);
	return EXIT_FAILURE;
}

