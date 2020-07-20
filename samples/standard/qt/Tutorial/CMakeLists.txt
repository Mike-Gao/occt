project(Tutorial)

OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit)
OCCT_INCLUDE_CMAKE_FILE (adm/cmake/occt_toolkit_prepare_sample)

ADD_DEFINITIONS(-DNO_COMMONSAMPLE_EXPORTS -DNO_IESAMPLE_EXPORTS)


