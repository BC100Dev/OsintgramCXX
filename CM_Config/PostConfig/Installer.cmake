install(TARGETS
        OsintgramCXX-core
        OsintgramCXX
        OsintgramCXX-interactive
        OsintgramCXX-security
        instagram-private-api
        dev-utils)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/Resources/commands.json"
        DESTINATION "share/OsintgramCXX/commands.json")