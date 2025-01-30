#include "protected.h"
#include <assert.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>


void assert_correct_number_of_positionals (Kwargs * kwargs);
void assert_longnames_are_compliant (Kwargs * kwargs);
void assert_no_duplicate_names (Kwargs * kwargs);
void assert_no_unnameds (Kwargs * kwargs);
void assert_no_user_defined_help_class (Kwargs * kwargs);
void assert_shortnames_are_compliant (Kwargs * kwargs);
void assert_types_are_correct_subset (Kwargs * kwargs);
const KwargsClass * get_class (const char * name, const Kwargs * kwargs);
char * verify_required_args_are_present (Kwargs * kwargs);


void assert_correct_number_of_positionals (Kwargs * kwargs) {
    size_t actual = 0;
    for (size_t i = 0; i < kwargs->nclassifieds; i++) {
        if (kwargs->classifieds[i] == KWARGS_POSITIONAL) {
            actual++;
        }
    }
    if (actual != kwargs->npositionals) {
        fprintf(stderr,
                "ERROR: Expected %zu positional arguments but found %zu.\n"
                "Here are some hints to resolve this problem:\n"
                "  1. Check the spelling of any parameter names\n"
                "  2. Check whether any parameter names that require a value did in fact get one\n",
                kwargs->npositionals, actual);
        exit(EXIT_FAILURE);
    }
}


void assert_longnames_are_compliant (Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        bool present = kwargs->classes[i].longname != nullptr;
        if (!present) continue;
        if (strnlen(kwargs->classes[i].longname, 4) <= 3) {
            fprintf(stderr, "ERROR: longname \"%s\" should be at least 4 characters, aborting.\n", kwargs->classes[i].longname);
            exit(EXIT_FAILURE);
        }
        if (strnlen(kwargs->classes[i].longname, 65) > 64) {
            fprintf(stderr, "ERROR: longname \"%s\" should be at most 64 characters, aborting.\n", kwargs->classes[i].longname);
            exit(EXIT_FAILURE);
        }
        if (kwargs->classes[i].longname[0] != '-') {
            fprintf(stderr, "ERROR: longname \"%s\" should start with \"--\", aborting.\n", kwargs->classes[i].longname);
            exit(EXIT_FAILURE);
        }
        if (kwargs->classes[i].longname[1] != '-') {
            fprintf(stderr, "ERROR: longname \"%s\" should start with \"--\", aborting.\n", kwargs->classes[i].longname);
            exit(EXIT_FAILURE);
        }
    }
}


void assert_no_duplicate_names (Kwargs * kwargs) {
    size_t nclasses = kwargs->nclasses;
    size_t ncap = nclasses * 2;
    char ** seen = calloc(ncap, sizeof(char *));
    if (seen == nullptr) {
        fprintf(stderr, "ERROR: Problem allocating memory for array \"seen\", aborting.\n");
        exit(EXIT_FAILURE);
    }
    size_t nseen = 0;
    for (size_t icls = 0; icls < nclasses; icls++) {
        for (size_t j = 0; j < 2; j++) {
            char * name = nullptr;
            size_t ell = 0;
            if (j == 0) {
                name = kwargs->classes[icls].shortname;
                ell = 3;
            } else {
                name = kwargs->classes[icls].longname;
                ell = 65;
            }
            for (size_t iseen = 0; iseen < nseen; iseen++) {
                bool matches = strncmp(name, seen[iseen], ell) == 0;
                if (matches) {
                    fprintf(stderr, "ERROR: duplicate name \"%s\" found, aborting.\n", name);
                    exit(EXIT_FAILURE);
                }
            }
            seen[nseen] = name;
            nseen++;
        }
    }
    free(seen);
    assert(nseen == ncap && "Expected number of seen names to be equal to number of allocated items");
}


void assert_no_unnameds (Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        bool a = kwargs->classes[i].shortname == nullptr;
        bool b = kwargs->classes[i].longname == nullptr;
        if (a && b) {
            fprintf(stderr, "ERROR: Found KwargsClass instance without a shortname and without\n"
                            "a longname, aborting.\n");
            exit(EXIT_FAILURE);
        }
    }
}


void assert_no_user_defined_help_class (Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        bool a = strncmp(kwargs->classes[i].shortname, "-h", 3) == 0;
        bool b = strncmp(kwargs->classes[i].longname, "--help", 7) == 0;
        if (a || b) {
            fprintf(stderr, "-h / --help is a special case that mustn't be defined by the user, aborting.\n");
            exit(EXIT_FAILURE);
        }
    }
}


void assert_shortnames_are_compliant (Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        bool present = kwargs->classes[i].shortname != nullptr;
        if (!present) continue;
        if (strnlen(kwargs->classes[i].shortname, 3) != 2) {
            fprintf(stderr, "ERROR: shortname \"%s\" should be length 2, aborting.\n", kwargs->classes[i].shortname);
            exit(EXIT_FAILURE);
        }
        if (kwargs->classes[i].shortname[0] != '-') {
            fprintf(stderr, "ERROR: shortname \"%s\" should start with \'-\', aborting.\n", kwargs->classes[i].shortname);
            exit(EXIT_FAILURE);
        }
        if (!isalpha(kwargs->classes[i].shortname[1])) {
            fprintf(stderr, "ERROR: shortname \"%s\" character at index 1 should\n"
                            "be [a-zA-Z], aborting.\n", kwargs->classes[i].shortname);
            exit(EXIT_FAILURE);
        }
    }
}


void assert_types_are_correct_subset (Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        bool a = kwargs->classes[i].type == KWARGS_FLAG;
        bool b = kwargs->classes[i].type == KWARGS_OPTIONAL;
        bool c = kwargs->classes[i].type == KWARGS_REQUIRED;
        bool compliant = a || b || c;
        if (!compliant) {
            fprintf(stderr, "ERROR: all parameter types should be one of [KWARGS_FLAG,\n"
                            "KWARGS_OPTIONAL, KWARGS_REQUIRED], aborting.\n");
            exit(EXIT_FAILURE);
        }
    }
}


void classify (Kwargs * kwargs) {

    assert_no_unnameds(kwargs);
    assert_shortnames_are_compliant(kwargs);
    assert_longnames_are_compliant(kwargs);
    assert_types_are_correct_subset (kwargs);
    assert_no_duplicate_names(kwargs);
    assert_no_user_defined_help_class(kwargs);

    kwargs->classifieds[0] = KWARGS_EXE;

    for (size_t icurr = 1; icurr < kwargs->nclassifieds; icurr++) {
        size_t iprev = icurr - 1;
        // if previous arg was positional, so is this one
        if (kwargs->classifieds[iprev] == KWARGS_POSITIONAL) {
            kwargs->classifieds[icurr] = KWARGS_POSITIONAL;
            continue;
        }
        // if previous arg required a value, mark current as KWARGS_VALUE
        if ((kwargs->classifieds[iprev] == KWARGS_OPTIONAL) || (kwargs->classifieds[iprev] == KWARGS_REQUIRED)) {
            kwargs->classifieds[icurr] = KWARGS_VALUE;
            continue;
        }
        const KwargsClass * cls = get_class(kwargs->argv[icurr], kwargs);
        if (cls == nullptr) {
            bool a = strncmp(kwargs->argv[icurr], "-h", 3) == 0;
            bool b = strncmp(kwargs->argv[icurr], "--help", 7) == 0;
            if (a || b) {
                if (icurr != 1) {
                    fprintf(stderr, "Found reserved -h / --help parameter, but in an unexpected location (%zu)\n", icurr);
                    exit(EXIT_FAILURE);
                } else if (kwargs->nclassifieds != 2) {
                    fprintf(stderr, "Found reserved -h / --help parameter, but it's followed by other arguments which is illegal\n");
                    exit(EXIT_FAILURE);
                } else {
                    kwargs->classifieds[1] = KWARGS_HELP;
                    return; // verifying required arguments doesn't apply when -h / --help, skipping
                }
            } else {
                kwargs->classifieds[icurr] = KWARGS_POSITIONAL;
            }
        } else {
            kwargs->classifieds[icurr] = cls->type;
        }
    }
    char * missing = verify_required_args_are_present(kwargs);
    assert_correct_number_of_positionals(kwargs);
    if (missing != nullptr) {
        fprintf(stderr, "ERROR: Required parameter \"%s\" seems to be missing, aborting.\n", missing);
        kwargs_destroy(&kwargs);
        exit(EXIT_FAILURE);
    }
}


const KwargsClass * get_class (const char * name, const Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        {
            char * longname = kwargs->classes[i].longname;
            bool matches = longname != nullptr &&
                           strncmp(longname, name, 65) == 0;
            if (matches) return &kwargs->classes[i];
        }
        {
            char * shortname = kwargs->classes[i].shortname;
            bool matches = shortname != nullptr &&
                           strncmp(shortname, name, 3) == 0;
            if (matches) return &kwargs->classes[i];
        }
    }
    return nullptr;
}


int has_type (const char * name, const Kwargs * kwargs, KwargsType type) {
    const char typenames[][20] = {
        "other",
        "help",
        "exe",
        "flag",
        "optional",
        "positional",
        "required",
        "value"
    };
    const KwargsClass * cls = get_class(name, kwargs);
    // test whether name is known
    if (cls == nullptr) {
        fprintf(stderr, "ERROR: %s name \"%s\" doesn't exist, aborting.\n", typenames[type], name);
        exit(EXIT_FAILURE);
    }
    // test whether name is of type 'type'
    if (cls->type != type) {
        fprintf(stderr, "ERROR: name \"%s\" exists but is not a %s, aborting.\n", name, typenames[type]);
        exit(EXIT_FAILURE);
    }
    // test where name is used in argv
    char * names[2] = {cls->shortname, cls->longname};
    for (size_t iname = 0; iname < 2; iname++) {
        for (int iarg = 1; iarg < kwargs->argc; iarg++) {
            bool cond = strncmp(names[iname], kwargs->argv[iarg], 65) == 0;
            if (cond) {
                // test whether classification is as expected
                if (cls->type != kwargs->classifieds[iarg]) {
                    KwargsType itype = kwargs->classifieds[iarg];
                    fprintf(stdout, "ERROR: Name \"%s\" seems to have been misclassified as a \"%s\" argument.\n"
                                    "Here are some hints to resolve this problem:\n"
                                    "  1. Check the spelling of preceding parameter names\n"
                                    "  2. Check whether preceding parameter names that require a value did in fact get one\n",
                                    kwargs->argv[iarg], typenames[itype]);
                    exit(EXIT_FAILURE);
                }
                return iarg;
            }
        }
    }
    return 0;
}


char * verify_required_args_are_present(Kwargs * kwargs) {
    for (size_t i = 0; i < kwargs->nclasses; i++) {
        if (kwargs->classes[i].type != KWARGS_REQUIRED) continue;
        int iarg = has_type(kwargs->classes[i].longname, kwargs, KWARGS_REQUIRED);   // TODO does this work when longname == nullptr?
        if (iarg == 0) return kwargs->classes[i].longname;
    }
    return nullptr;
}
