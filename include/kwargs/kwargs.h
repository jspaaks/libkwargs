#ifndef KWARGS_H
#define KWARGS_H
#include <stddef.h>
#include <stdio.h>

typedef struct kwargs Kwargs;

typedef enum {
    KWARGS_UNKNOWN = 0,
    KWARGS_EXE,
    KWARGS_HELP,
    KWARGS_FLAG,
    KWARGS_OPTIONAL,
    KWARGS_POSITIONAL,
    KWARGS_REQUIRED,
    KWARGS_VALUE
} KwargsType;

typedef struct {
    char * longname;
    char * shortname;
    KwargsType type;
} KwargsClass;

struct kwargs;  // forward declaration of an abstract data type

const Kwargs * kwargs_create (int argc, const char * argv[], size_t nclasses, const KwargsClass * classes, size_t npositionals);
void kwargs_destroy (Kwargs ** kwargs);
const char * kwargs_get_optional_value (const char * name, const Kwargs * kwargs);
const char * kwargs_get_positional_value (size_t ipos, const Kwargs * kwargs);
const char * kwargs_get_required_value (const char * name, const Kwargs * kwargs);
int kwargs_has_flag (const char * name, const Kwargs * kwargs);
int kwargs_has_optional (const char * name, const Kwargs * kwargs);
void kwargs_print_classifications (FILE * stream, const Kwargs * kwargs);
bool kwargs_requires_help (const Kwargs * kwargs);

#endif
