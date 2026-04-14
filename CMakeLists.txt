cmake_minimum_required(VERSION 3.20)
project(DnDMaster VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 1. Trova i pacchetti scaricati da vcpkg
find_package(SDL3 REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-skia CONFIG REQUIRED)
find_package(unofficial-enet CONFIG REQUIRED)

# Per SQLite3 usiamo il modulo standard di CMake, che vcpkg intercetta correttamente
find_package(SQLite3 REQUIRED)
find_package(SQLiteCpp CONFIG REQUIRED)

find_package(glad CONFIG REQUIRED)

# 2. Trova tutti i file .cpp e .h ricorsivamente
file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS "src/*.cpp" "src/*.h")

# 3. Crea l'eseguibile
add_executable(DnDMaster ${SOURCES})

# 4. Aggiunge src/ come directory di include
target_include_directories(DnDMaster PRIVATE src)

# 5. Collega le librerie usando i target corretti
# Nota: SQLite3::SQLite3 è il nome del target standard di CMake
target_link_libraries(DnDMaster PRIVATE 
    SDL3::SDL3 
    imgui::imgui 
    nlohmann_json::nlohmann_json
    unofficial::enet::enet
    SQLiteCpp
    SQLite::SQLite3
    unofficial::skia::skia
    glad::glad
)

# --- Configurazione CPack (Installazione) ---
include(InstallRequiredSystemLibraries)

install(TARGETS DnDMaster DESTINATION bin)
install(DIRECTORY assets/ DESTINATION bin/assets)

set(CPACK_PACKAGE_NAME "DnDMaster")
set(CPACK_PACKAGE_VENDOR "Luiz/InProds")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Professional DnD master tools")
set(CPACK_PACKAGE_VERSION "0.1.0")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
elseif(APPLE)
    set(CPACK_GENERATOR "DragNDrop")
else()
    set(CPACK_GENERATOR "TGZ")
endif()

include(CPack)