# libkwargs

For instructions on building, formatting, testing, etc, see [`README.dev.md`](README.dev.md).

## Executables

- `example_kwargs`: Example program that does nothing except demonstrate the use of the libkwargs library.

## Libraries

- `kwargs`: Handling of command line arguments.

## `man` pages

- `kwargs.3`
- `kwargs_create.3`
- `kwargs_destroy.3`
- `kwargs_get_optional_value.3`
- `kwargs_get_positional_value.3`
- `kwargs_get_required_value.3`
- `kwargs_has_flag.3`
- `kwargs_has_optional.3`
- `kwargs_print_classifications.3`
- `kwargs_requires_help.3`

## Example

`example_kwargs` is an example program for the usage of the library. Its source code is available in
`src/example/main.c` and is reproduced below for convenience:

```c
#include "kwargs/kwargs.h"
#include <stdio.h>
#include <stdlib.h>


const char * get_basename (const Kwargs * kwargs);
const char * get_input_filename (const Kwargs * kwargs);
size_t get_nsamples (const Kwargs * kwargs);
const char * get_output_filename (const Kwargs * kwargs);
bool get_verbose (const Kwargs * kwargs);
void show_usage (void);


const char * get_basename (const Kwargs * kwargs) {
    const char * s = kwargs_get_optional_value("--basename", kwargs);
    if (s == nullptr) {
        return "sample.";
    }
    return s;
}


const char * get_input_filename (const Kwargs * kwargs) {
    return kwargs_get_positional_value(0, kwargs);
}


size_t get_nsamples (const Kwargs * kwargs) {
    const char * s = kwargs_get_required_value("--nsamples", kwargs);
    size_t nsamples;
    sscanf(s, " %zu", &nsamples);
    return nsamples;
}


const char * get_output_filename (const Kwargs * kwargs) {
    return kwargs_get_positional_value(1, kwargs);
}


bool get_verbose (const Kwargs * kwargs) {
    return kwargs_has_flag("--verbose", kwargs) > 0;
}


int main (const int argc, const char * argv[]) {
    const KwargsClass classes[] = {
        {
            .longname = "--nsamples",
            .shortname = "-n",
            .type = KWARGS_REQUIRED
        },
        {
            .longname = "--basename",
            .shortname = "-b",
            .type = KWARGS_OPTIONAL
        },
        {
            .longname = "--verbose",
            .shortname = "-v",
            .type = KWARGS_FLAG
        }
    };

    const size_t nclasses = sizeof(classes) / sizeof(classes[0]);
    const size_t npositionals = 2;
    const Kwargs * kwargs = kwargs_create(argc, argv, nclasses, &classes[0], npositionals);
    if (kwargs_requires_help(kwargs)) {
        show_usage();
        kwargs_destroy((Kwargs **) &kwargs);
        exit(EXIT_SUCCESS);
    }
    const size_t nsamples = get_nsamples(kwargs);
    const char * basename = get_basename(kwargs);
    const bool verbose = get_verbose(kwargs);
    const char * input_filename = get_input_filename(kwargs);
    const char * output_filename = get_output_filename(kwargs);
    fprintf(stdout,
        "Example program that does nothing except demonstrate the use of the libkwargs "
        "library.\n"
        "    nsamples = %zu\n"
        "    basename = \"%s\"\n"
        "    verbose = %s\n"
        "    input_filename = \"%s\"\n"
        "    output_filename = \"%s\"\n",
        nsamples, basename, verbose ? "true" : "false", input_filename, output_filename);
    return EXIT_SUCCESS;
}


void show_usage (void) {
    fprintf(stdout,
        "  Usage:\n"
        "    example_kwargs -h\n"
        "    example_kwargs --help\n"
        "    example_kwargs [OPTIONALS] REQUIREDS POSITIONALS\n"
        "\n"
        "  Synopsis\n"
        "    Example program that does nothing except demonstrate the\n"
        "    use of the libkwargs library by printing the values of\n"
        "    its command line arguments.\n"
        "\n"
        "  Requireds\n"
        "    -n, --nsamples SAMPLES      The number of samples. SAMPLES should be a\n"
        "                                positive integer.\n"
        "\n"
        "  Optionals\n"
        "    -b, --basename BASENAME     Value of the basename. Default value of\n"
        "                                BASENAME is \"sample.\"\n"
        "    -h, --help                  Show the help.\n"
        "    -v, --verbose               Verbose output.\n"
        "\n"
        "  Positionals (by order)\n"
        "    INPUT_FILENAME              The filepath to the input file.\n"
        "    OUTPUT_FILENAME             The filepath to the output file. Any\n"
        "                                directories must exist before running the\n"
        "                                command.\n");
}
```

After compiling, show the help with `-h` or `--help` flag:

```console
$ ./dist/bin/example_kwargs --help
  Usage:
    example_kwargs -h
    example_kwargs --help
    example_kwargs [OPTIONALS] REQUIREDS POSITIONALS

  Synopsis
    Example program that does nothing except demonstrate the
    use of the libkwargs library by printing the values of
    its command line arguments.

  Requireds
    -n, --nsamples SAMPLES      The number of samples. SAMPLES should be a
                                positive integer.

  Optionals
    -b, --basename BASENAME     Value of the basename. Default value of
                                BASENAME is "sample."
    -h, --help                  Show the help.
    -v, --verbose               Verbose output.

  Positionals (by order)
    INPUT_FILENAME              The filepath to the input file.
    OUTPUT_FILENAME             The filepath to the output file. Any
                                directories must exist before running the
                                command.
```

Let's try with minimal inputs:

```console
$ ./dist/bin/example_kwargs -n 3 infile.txt outfile.txt
Example program that does nothing except demonstrate the use of the libkwargs library.
    nsamples = 3
    basename = "sample."
    verbose = false
    input_filename = "infile.txt"
    output_filename = "outfile.txt"
```

Let's try with some custom inputs:

```console
$ ./dist/bin/example_kwargs -n 3 -v --basename "myname/" infile.txt outfile.txt
Example program that does nothing except demonstrate the use of the libkwargs library.
    nsamples = 3
    basename = "myname/"
    verbose = true
    input_filename = "infile.txt"
    output_filename = "outfile.txt"
$ ./dist/bin/example_kwargs -n 3 infile.txt outfile.txt extra.txt
ERROR: Expected 2 positional arguments but found 3.
Here are some hints to resolve this problem:
  1. Check the spelling of any parameter names
  2. Check whether any parameter names that require a value did in fact get one
$ ./dist/bin/example_kwargs -v --basename "myname/" infile.txt outfile.txt
ERROR: Required parameter "--nsamples" seems to be missing, aborting.
```

## Acknowledgements

_This project was initialized using [Copier](https://pypi.org/project/copier)
and the [Copier template for C projects](https://github.com/jspaaks/copier-template-for-c-projects)._
