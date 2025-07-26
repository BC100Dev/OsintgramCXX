## install files
# do nothing for now

install(TARGETS
        OsintgramCXX-core
        OsintgramCXX
        OsintgramCXX-interactive
        OsintgramCXX-security
        OsintgramCXX-commons
        OsintgramCXX-networking)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Resources/commands.json" DESTINATION "share/OsintgramCXX/commands.json")