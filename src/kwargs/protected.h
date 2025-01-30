#ifndef KWARGS_PROTECTED_H
#define KWARGS_PROTECTED_H
#include "kwargs/kwargs.h"
#include <stddef.h>

struct kwargs {
    int argc;
    const char ** argv;
    size_t nclasses;
    const KwargsClass * classes;
    size_t nclassifieds;
    KwargsType * classifieds;
    size_t npositionals;
};

void classify (Kwargs * kwargs);
int has_type (const char * name, const Kwargs * kwargs, KwargsType type);

#endif
