set(requires "")
idf_build_get_property(target IDF_TARGET)

if(${target} STREQUAL "linux")
    list(APPEND requires esp_stubs esp-tls esp_http_server protocol_examples_common nvs_flash)
endif()

idf_component_register(SRCS
                "src/main.cpp"
                "src/controller.cpp"
                "src/button_handler.cpp"
                "src/led_handler.cpp"
                "src/servo_handler.cpp"
                "src/analog_sound_handler.cpp"
                "src/tuning_handler.cpp" 
                "src/i2s_sound_handler.cpp"
                "src/sound_handler_base.cpp"
                "src/network_handler.cpp"
                "src/html.cpp"
            INCLUDE_DIRS
                "include"
            REQUIRES
                ${requires}
)

target_compile_options(${COMPONENT_LIB} PRIVATE -std=gnu++23 )#-Werror -Wall -Wextra -Wpedantic -Wno-unused-parameter -Wno-unused-variable -Wno-unused-function -Wno-unused-but-set-variable -Wno-unused-value
