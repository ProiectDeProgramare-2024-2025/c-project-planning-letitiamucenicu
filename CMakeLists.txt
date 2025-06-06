cmake_minimum_required(VERSION 3.10)
project(DentalClinicSystem CXX)

# Setează standardul C++
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Specifică directoarele de include
include_directories(include)

# Sursele pentru logica de bază (Scheduler și clasele dependente)
set(CORE_SOURCES
    src/Scheduler.cpp
    # Operation.cpp și Appointment.cpp nu sunt necesare dacă sunt header-only
)

# ----- Executabilul pentru Admin -----
add_executable(admin_app src/admin_main.cpp ${CORE_SOURCES})
# Specifică directoarele de include pentru admin_app (redundant dacă global, dar bun pentru specificitate)
target_include_directories(admin_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Comandă pentru a copia op_details.txt în directorul de build al executabilului admin_app
# Acesta va fi copiat lângă executabil la fiecare build dacă s-a modificat.
add_custom_command(TARGET admin_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/data/op_details.txt
    $<TARGET_FILE_DIR:admin_app>/op_details.txt
    COMMENT "Copying op_details.txt for admin_app"
)

# ----- Executabilul pentru User -----
add_executable(user_app src/user_main.cpp ${CORE_SOURCES})
# Specifică directoarele de include pentru user_app
target_include_directories(user_app PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Comandă pentru a copia op_details.txt în directorul de build al executabilului user_app
add_custom_command(TARGET user_app POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_CURRENT_SOURCE_DIR}/data/op_details.txt
    $<TARGET_FILE_DIR:user_app>/op_details.txt
    COMMENT "Copying op_details.txt for user_app"
)

# Opțional: Crearea unei biblioteci statice pentru logica de bază
# add_library(CoreLogic STATIC ${CORE_SOURCES})
# target_include_directories(CoreLogic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
# target_link_libraries(admin_app PRIVATE CoreLogic)
# target_link_libraries(user_app PRIVATE CoreLogic)

# Asigură-te că directoarele de output sunt ușor de găsit (opțional)
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
# set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)