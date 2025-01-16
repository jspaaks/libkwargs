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
    const Kwargs * kwargs = kwargs_create(argc, argv, nclasses, &classes[0]);
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
        "    libkwargs-example -h\n"
        "    libkwargs-example --help\n"
        "    libkwargs-example [OPTIONALS] REQUIREDS POSITIONALS\n"
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
