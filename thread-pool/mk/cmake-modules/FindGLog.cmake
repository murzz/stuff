# Find glog headers and libraries.
#
#  GLOG_INCLUDE_DIRS - where to find glog/logging.h, etc.
#  GLOG_LIBRARIES    - List of libraries when using GLOG.
#  GLOG_FOUND        - True if GLOG found.

IF (GLOG_INCLUDE_DIRS AND GLOG_LIBRARIES)
    SET(GLOG_FIND_QUIETLY TRUE)
ENDIF (GLOG_INCLUDE_DIRS AND GLOG_LIBRARIES)

FIND_PACKAGE (PkgConfig QUIET)
IF (PKGCONFIG_FOUND)
    PKG_CHECK_MODULES(PC_GLOG QUIET glog)
ENDIF (PKGCONFIG_FOUND)

FIND_PATH(GLOG_INCLUDE_DIRS
    NAMES
        glog/logging.h
    PATHS
        ${GLOG_ROOT}
        /usr/local/include
        /usr/include
        $ENV{GLOG}
        $ENV{GLOG}/include
    )

FIND_LIBRARY(GLOG_LIBRARIES
    NAMES
        glog
    PATHS
        ${GLOG_ROOT}
        /usr/local/lib
        /usr/lib
        $ENV{GLOG}
        $ENV{GLOG}/lib
    )

# handle the QUIETLY and REQUIRED arguments and set *_FOUND
INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(GLOG DEFAULT_MSG GLOG_LIBRARIES GLOG_INCLUDE_DIRS)

MARK_AS_ADVANCED(GLOG_INCLUDE_DIRS GLOG_LIBRARIES)
