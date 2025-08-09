
add_library(
        blank_storage OBJECT
        blank_storage.cpp
        blank_storage.h
        $<TARGET_OBJECTS:basilisk_logging>
)

add_library(
        system_messaging OBJECT
        system_messaging.cpp
        system_messaging.h
        subscription_monitor.cpp
        $<TARGET_OBJECTS:basilisk_logging>
        $<TARGET_OBJECTS:blank_storage>
)

add_subdirectory(test)
