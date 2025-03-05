
find_path(POPPLER_QT6_INCLUDE_DIR
    poppler-qt6.h
    PATH_SUFFIXES
    poppler/qt6
    PATHS
    ${POPPLER_ROOT}/include
    NO_DEFAULT_PATH
)

find_library(POPPLER_LIBRARY
    NAMES libpoppler
    PATHS
    ${POPPLER_ROOT}/lib
    NO_DEFAULT_PATH
)

find_library(POPPLER_QT6_LIBRARY
    NAMES libpoppler-qt6
    PATHS
    ${POPPLER_ROOT}/lib
    NO_DEFAULT_PATH
)

file(GLOB POPPLER_DLL ${POPPLER_ROOT}/bin/libpoppler-???.dll)
file(GLOB POPPLER_QT6_DLL ${POPPLER_ROOT}/bin/libpoppler-qt6*.dll)

message(STATUS "*** FindPoppler: POPPLER_QT6_INCLUDE_DIR=${POPPLER_QT6_INCLUDE_DIR}")
message(STATUS "*** FindPoppler: POPPLER_LIBRARY=${POPPLER_LIBRARY}")
message(STATUS "*** FindPoppler: POPPLER_DLL=${POPPLER_DLL}")
message(STATUS "*** FindPoppler: POPPLER_QT6_LIBRARY=${POPPLER_QT6_LIBRARY}")
message(STATUS "*** FindPoppler: POPPLER_QT6_DLL=${POPPLER_QT6_DLL}")

include(FindPackageHandleStandardArgs)

find_package_handle_standard_args(Poppler DEFAULT_MSG POPPLER_QT6_INCLUDE_DIR POPPLER_LIBRARY POPPLER_QT6_LIBRARY)

mark_as_advanced(  POPPLER_QT6_INCLUDE_DIR POPPLER_LIBRARY  POPPLER_QT6_LIBRARY)
if(Poppler_FOUND)
  if(NOT TARGET Poppler::Poppler)
     add_library(Poppler::Poppler UNKNOWN IMPORTED)

     set_target_properties(Poppler::Poppler PROPERTIES
                           IMPORTED_LINK_INTERFACE_LANGUAGES "C"
                           IMPORTED_IMPLIB "${POPPLER_LIBRARY}"
                           IMPORTED_LOCATION "${POPPLER_DLL}")
  endif()
  if(NOT TARGET Poppler::PopplerQt6)
   add_library(Poppler::PopplerQt6 UNKNOWN IMPORTED)

   set_target_properties(Poppler::PopplerQt6 PROPERTIES
                         IMPORTED_LINK_INTERFACE_LANGUAGES "CXX"
                         IMPORTED_IMPLIB "${POPPLER_QT6_LIBRARY}"
                         IMPORTED_LOCATION "${POPPLER_QT6_DLL}")

   target_include_directories(Poppler::PopplerQt6 INTERFACE  "${POPPLER_QT6_INCLUDE_DIR}" "${POPPLER_QT6_INCLUDE_DIR}/../" "${POPPLER_QT6_INCLUDE_DIR}/../../")
   target_link_libraries(Poppler::PopplerQt6 INTERFACE Poppler::Poppler)
 endif()
endif()

include(FeatureSummary)
set_package_properties(Poppler PROPERTIES
                      DESCRIPTION "A PDF rendering library" URL "http://poppler.freedesktop.org")
