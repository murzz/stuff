set(GLOG_INSTALL_ROOT ${CMAKE_CURRENT_BINARY_DIR}/_install)

include(ExternalProject)

set(GLOG_REPO_URL git@github.com:google/glog.git)
set(GLOG_TAG v0.3.4)

ExternalProject_Add(glog
    GIT_REPOSITORY ${GLOG_REPO_URL}
    GIT_TAG ${GLOG_TAG}
)