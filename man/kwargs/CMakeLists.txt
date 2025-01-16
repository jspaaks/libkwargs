include(GNUInstallDirs)
install(
    FILES
        kwargs_create.3
        kwargs_destroy.3
        kwargs_get_optional_value.3
        kwargs_get_positional_value.3
        kwargs_get_required_value.3
        kwargs_has_flag.3
        kwargs_has_optional.3
        kwargs_print_classifications.3
        kwargs_requires_help.3
        kwargs.3

    DESTINATION ${CMAKE_INSTALL_MANDIR}/man3     # I'd prefer to use TYPE instead of DESTINATION but it seems I need the man3 subdirectory
)
