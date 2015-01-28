# FindLibPuzzle.cmake
#
# Locates the protobuf-c library.
#
# Defines the following variables
#
# LIBPUZZLE_FOUND - if the library was found
# LIBPUZZLE_LIBRARY - the library path
# LIBPUZZLE_INCLUDE_DIR - the include path

find_path (LIBPUZZLE_INCLUDE_DIR puzzle.h
  HINTS
    ENV LIBPUZZLE_DIR
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

find_library (LIBPUZZLE_LIBRARY
  NAMES puzzle
  HINTS
    ENV LIBPUZZLE_DIR
  PATH_SUFFIXES lib
  PATHS
    ~/Library/Frameworks
    /Library/Frameworks
    /opt/local
    /opt
)

include (FindPackageHandleStandardArgs)

find_package_handle_standard_args (LIBPUZZLE DEFAULT_MSG
  LIBPUZZLE_LIBRARY LIBPUZZLE_INCLUDE_DIR)

mark_as_advanced (LIBPUZZLE_INCLUDE_DIR LIBPUZZLE_LIBRARY)
