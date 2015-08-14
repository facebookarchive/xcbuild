find_package(LLVM REQUIRED)

find_path(LibClang_INCLUDE_DIR "clang" HINTS "${LLVM_INCLUDE_DIRS}")
find_library(LibClang_LIBRARY NAMES clang "clang-${LLVM_VERSION_MAJOR}.${LLVM_VERSION_MINOR}" PATH "${LLVM_LIBRARY_DIRS}")

set(LibClang_LIBRARIES ${LibClang_LIBRARY})
set(LibClang_INCLUDE_DIRS ${LibClang_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(LibClang DEFAULT_MSG LibClang_LIBRARY LibClang_INCLUDE_DIR)

mark_as_advanced(LibClang_INCLUDE_DIR LibClang_LIBRARY)
if (NOT LibClang_INCLUDE_DIR OR NOT LibClang_LIBRARY)
    message(FATAL_ERROR "LibClang could not be found")
else ()
    message("-- Clang include path: ${LibClang_INCLUDE_DIR}")
    message("-- Clang library: ${LibClang_LIBRARY}")
endif ()
