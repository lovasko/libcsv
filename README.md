# libcsv
Comma Separated Values implementation for the C89 language.

## Goals
 * ability to read arbitrary huge files
 * focus on speed by omitting unnecessary operations such as memory allocation
 * simplest possible API with proper error handling

## Documentation
### Opening a data file
In order to open a file, use the `csv_open` function. It expects four
arguments:
 * pointer to a `struct csv`
 * `path` of the CSV data file
 * the field `separator` (record separator is always `\n`)
 * `field_count` of every record

### Reading records
This is the main operation that has to be called separately for every record in
the data, essentially functioning as a stream of data. It only provides
pointers to a NULL-terminated array of strings (`char***`). None of these
strings are allocated, therefore all strings that need to preserved and not
only parsed (and acted upon) need to be `strdup`-ed. The function
`csv_read_record` expects a pointer to the `struct csv` and, as previously
mentioned`, it returns all field strings via the second argument.

Moreover, this function can be used to skip the optional headers.

### Finalisation
After reading all necessary records from the data set, use the `csv_close`
function to close the file descriptor and to deallocate all internal data
structures.

### Return codes
It may happen that one or more of the previously mentioned function will return
a value different from `CSV_OK`. In case you want to inform your user what kind
of error it is, use the `csv_error_string` that converts the `int` return code
into a meaningful English message.

## Example
The file [people.c](examples/people.c) provides a basic source code to a read a
data set stored in the [people.csv](examples/people.csv).

## Supported platforms
 * FreeBSD 10.0 with Clang 3.3
 * OS X 10.9 with Clang 3.5
 * Linux Gentoo 2.2 with Clang 3.6

If a platform does not appear to be in the previous list, it does not mean that
`libcsv` will not work in such environment. It only means that nobody tested
it - you are encouraged to do so and report either success or failure.

## Build & install
```
$ ninja
$ sudo ./install.sh
```

## License
2-clause BSD license. For more information please consult the
[LICENSE](LICENSE) file. In the case that you need a different license, feel
free to contact me.

## Author
Daniel Lovasko (daniel.lovasko@gmail.com)

