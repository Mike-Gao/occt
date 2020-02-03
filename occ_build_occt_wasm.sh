#!/bin/bash

# Auxiliary script for semi-automated building of OCCT for WASM platform.
# Script should be placed into root of OCCT repository, edited with paths
# to CMake, 3rd-parties and Emscripten SDK.

# FreeType and RapidJSON should be specified as mandatory dependency (should be manually build or taken from earlier builds).

export aCasSrc="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"

if [ -f ${aCasSrc}/env_custom.sh ] ; then
	. ${aCasSrc}/env_custom.sh
fi


if [ ! -d "${aBuildRoot}" ] ; then 
	mkdir -p "${aBuildRoot}"
else
	rm -rf "${aBuildRoot}"
	mkdir -p "${aBuildRoot}"
fi

. ${EMSDK_ROOT}/emsdk_env.sh

export aToolchain="${EMSDK}/fastcomp/emscripten/cmake/Modules/Platform/Emscripten.cmake"

export aGitBranch=`git symbolic-ref --short HEAD`

echo "Compilation OCCT branch : $aGitBranch"

export aPlatformAndCompiler=wasm

export aWorkDir=${aCasSrc}/${aBuildRoot}/${aPlatformAndCompiler}-make
if [ ! -d "${aWorkDir}" ] ; then 
	mkdir "${aWorkDir}"
fi

export aDestDir=${aCasSrc}/${aBuildRoot}/${aPlatformAndCompiler}
if [ ! -d "${aDestDir}" ];then
	mkdir "${aDestDir}"
fi

export aLogFile=${aCasSrc}/${aBuildRoot}/build-${aPlatformAndCompiler}.log
if [ ! -f "${aLogFile}" ];then
	touch "${aLogFile}"
fi

echo Start building OCCT for ${aPlatformAndCompiler}
echo Start building OCCT for ${aPlatformAndCompiler}>> ${aLogFile}

cd ${aWorkDir}
pwd
echo toCMake=${toCMake}
if [ "${toCMake}" = "1" ] ; then

echo "Configuring OCCT for WASM..."
echo cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="${aToolchain}" \
-DCMAKE_BUILD_TYPE:STRING="Release" \
-DBUILD_LIBRARY_TYPE:STRING="Static" \
-DINSTALL_DIR:PATH="${aDestDir}" \
-DINSTALL_DIR_INCLUDE:STRING="inc" \
-DINSTALL_DIR_RESOURCE:STRING="src" \
-DBUILD_MODULE_ApplicationFramework:BOOL="${BUILD_AppFramework}" \
-DBUILD_MODULE_DataExchange:BOOL="${BUILD_DataExchange}" \
-DBUILD_MODULE_Draw:BOOL="${BUILD_Draw}" \
-DBUILD_MODULE_FoundationClasses:BOOL="${BUILD_FndClasses}" \
-DBUILD_MODULE_ModelingAlgorithms:BOOL="${BUILD_ModAlg}" \
-DBUILD_MODULE_ModelingData:BOOL="${BUILD_ModData}" \
-DBUILD_MODULE_Visualization:BOOL="${BUILD_Viz}" \
-DBUILD_DOC_Overview:BOOL="OFF" "${aCasSrc}"

cmake -G "Unix Makefiles" -DCMAKE_TOOLCHAIN_FILE:FILEPATH="${aToolchain}" \
-DCMAKE_BUILD_TYPE:STRING="Release" \
-DBUILD_LIBRARY_TYPE:STRING="Static" \
-DINSTALL_DIR:PATH="${aDestDir}" \
-DINSTALL_DIR_INCLUDE:STRING="inc" \
-DINSTALL_DIR_RESOURCE:STRING="src" \
-DBUILD_MODULE_ApplicationFramework:BOOL="${BUILD_AppFramework}" \
-DBUILD_MODULE_DataExchange:BOOL="${BUILD_DataExchange}" \
-DBUILD_MODULE_Draw:BOOL="${BUILD_Draw}" \
-DBUILD_MODULE_FoundationClasses:BOOL="${BUILD_FndClasses}" \
-DBUILD_MODULE_ModelingAlgorithms:BOOL="${BUILD_ModAlg}" \
-DBUILD_MODULE_ModelingData:BOOL="${BUILD_ModData}" \
-DBUILD_MODULE_Visualization:BOOL="${BUILD_Viz}" \
-DBUILD_DOC_Overview:BOOL="OFF" "${aCasSrc}"

	if [ $? -ne 0 ] ; then
		echo "Problem during configuration"
		exit 1
	fi

fi

if [ "${toClean}" = "1" ] ; then
  make clean
fi

if [ "${toMake}" = "1" ] ; then
  echo Building...
  make -j ${aNbJobs} 2>> ${aLogFile}
	if [ $? -ne 0 ] ; then
		echo "Problem during make operation"
		exit 1
	fi
  echo ${aLogFile}
fi

if [ "${toInstall}" = "1" ] ; then
  echo Installing into ${aDestDir}
  make install 2>> ${aLogFile}
fi
