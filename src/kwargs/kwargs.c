#include "kwargs/kwargs.h"
#include "protected.h"
#include <assert.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


const Kwargs * kwargs_create (const int argc, const char * argv[], const size_t nclasses, const KwargsClass * classes, size_t npositionals) {
    errno = 0;
    Kwargs * kwargs = calloc(1, sizeof(Kwargs));
    if (kwargs == nullptr) {
        fprintf(stderr, "%s\nError allocating memory for instance of \"Kwargs\", aborting.\n", strerror(errno));
        errno = 0;
        exit(EXIT_FAILURE);
    }

    errno = 0;
    KwargsType * classifieds = calloc(argc, sizeof(KwargsType));
    if (classifieds == nullptr) {
        fprintf(stderr, "%s\nError allocating memory for array of \"KwargsType\", aborting.\n", strerror(errno));
        errno = 0;
        kwargs_destroy((Kwargs **) &kwargs);
        exit(EXIT_FAILURE);
    }
    kwargs->argc = argc;
    kwargs->argv = argv;
    kwargs->nclasses = nclasses;
    kwargs->classes = classes;
    kwargs->nclassifieds = (size_t) argc;
    kwargs->classifieds = classifieds;
    kwargs->npositionals = npositionals;
    classify(kwargs);
    return (const Kwargs *) kwargs;
}


void kwargs_destroy (Kwargs ** kwargs) {
    free((*kwargs)->classifieds);
    (*kwargs)->classifieds = nullptr;
    free(*kwargs);
    *kwargs = nullptr;
}


const char * kwargs_get_optional_value (const char * name, const Kwargs * kwargs) {
    int iarg = has_type(name, kwargs, KWARGS_OPTIONAL);
    if (iarg == 0) {
        return nullptr;
    }
    if (iarg + 1 >= kwargs->argc) {
        fprintf(stderr, "Value for \"%s\" seems to be missing, aborting.\n", name);
        kwargs_destroy((Kwargs **) &kwargs);
        exit(EXIT_FAILURE);
    }
    return kwargs->argv[iarg + 1];
}


const char * kwargs_get_positional_value (const size_t ipos, const Kwargs * kwargs) {
    size_t k = 0;
    for (size_t i = 0; i < kwargs->nclassifieds; i++) {
        if (kwargs->classifieds[i] == KWARGS_POSITIONAL) {
            if (k == ipos) {
                return kwargs->argv[i];
            } else {
                k++;
            }
        }
    }
    fprintf(stderr, "ERROR: requested positional argument %zu doesn't exist, aborting.\n", ipos);
    kwargs_destroy((Kwargs **) &kwargs);
    exit(EXIT_FAILURE);
}


const char * kwargs_get_required_value (const char * name, const Kwargs * kwargs) {
    int iarg = has_type(name, kwargs, KWARGS_REQUIRED);
    if (iarg == 0) {
        return nullptr;
    }
    if (iarg + 1 >= kwargs->argc) {
        fprintf(stderr, "Value for \"%s\" seems to be missing, aborting.\n", name);
        kwargs_destroy((Kwargs **) &kwargs);
        exit(EXIT_FAILURE);
    }
    return kwargs->argv[iarg + 1];
}


int kwargs_has_flag (const char * name, const Kwargs * kwargs) {
    return has_type(name, kwargs, KWARGS_FLAG);
}


int kwargs_has_optional (const char * name, const Kwargs * kwargs) {
    return has_type(name, kwargs, KWARGS_OPTIONAL);
}


void kwargs_print_classifications (FILE * stream, const Kwargs * kwargs) {
    int ell = 25;
    const char typestrings[][20] = {
        "other",
        "help",
        "exe",
        "flag",
        "optional",
        "positional",
        "required",
        "value"
    };
    for (size_t i = 0; i < kwargs->nclassifieds; i++) {
        int itype = kwargs->classifieds[i];
        fprintf(stream, "%-*s %s\n", ell, kwargs->argv[i], typestrings[itype]);
    }
}


bool kwargs_requires_help (const Kwargs * kwargs) {
    return kwargs->classifieds[1] == KWARGS_HELP;
}
