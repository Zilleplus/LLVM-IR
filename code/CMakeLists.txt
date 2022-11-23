cmake_minimum_required(VERSION 3.20)
project(llvm-ir CXX C)
set (CMAKE_CXX_STANDARD 17)


# Not clean, I should find a better way to do this.
if(UNIX)
	#set(CMAKE_CXX_FLAGS "-fuse-ld=gold")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(EXTERNAL_LIBS "${CMAKE_SOURCE_DIR}/deps")

set(LLVM_INSTALL_LOCATION "${EXTERNAL_LIBS}/llvm/build")
set(LLVM_LIB_LIST
	LLVMCore
	LLVMSupport
	LLVMAggressiveInstCombine
	LLVMAnalysis
	LLVMAsmParser
	LLVMAsmPrinter
	LLVMBinaryFormat
	LLVMBitReader
	LLVMBitstreamReader
	LLVMBitWriter
	LLVMCFGuard
	LLVMCFIVerify
	LLVMCodeGen
	LLVMCoroutines
	LLVMCoverage
	LLVMDebugInfoCodeView
	LLVMDebuginfod
	LLVMDebugInfoDWARF
	LLVMDebugInfoGSYM
	LLVMDebugInfoLogicalView
	LLVMDebugInfoMSF
	LLVMDebugInfoPDB
	LLVMDemangle
	LLVMDiff
	LLVMDlltoolDriver
	LLVMDWARFLinker
	LLVMDWP
	LLVMExecutionEngine
	LLVMExegesis
	LLVMExegesisX86
	LLVMExtensions
	LLVMFileCheck
	LLVMFrontendHLSL
	LLVMFrontendOpenACC
	LLVMFrontendOpenMP
	LLVMFuzzerCLI
	LLVMFuzzMutate
	LLVMGlobalISel
	LLVMInstCombine
	LLVMInstrumentation
	LLVMInterfaceStub
	LLVMInterpreter
	LLVMipo
	LLVMIRPrinter
	LLVMIRReader
	LLVMJITLink
	LLVMLibDriver
	LLVMLineEditor
	LLVMLinker
	LLVMLTO
	LLVMMC
	LLVMMCA
	LLVMMCDisassembler
	LLVMMCJIT
	LLVMMCParser
	LLVMMIRParser
	LLVMObjCARCOpts
	LLVMObjCopy
	LLVMObject
	LLVMObjectYAML
	LLVMOption
	LLVMOrcJIT
	LLVMOrcShared
	LLVMOrcTargetProcess
	LLVMPasses
	LLVMProfileData
	LLVMRemarks
	LLVMRuntimeDyld
	LLVMScalarOpts
	LLVMSelectionDAG
	LLVMSymbolize
	LLVMTableGen
	LLVMTableGenGlobalISel
	LLVMTarget
	LLVMTextAPI
	LLVMTransformUtils
	LLVMVectorize
	LLVMWindowsDriver
	LLVMWindowsManifest
	LLVMX86AsmParser
	LLVMX86CodeGen
	LLVMX86Desc
	LLVMX86Disassembler
	LLVMX86Info
	LLVMX86TargetMCA
	)

# Ninja on linux will error on depencies if you 
# don't put the so files as byproducts.
set(LLVM_LIBS_LOCATIONS "")
if(UNIX)
	foreach(LIB ${LLVM_LIB_LIST})
		list(APPEND LLVM_LIBS_LOCATIONS "${LLVM_INSTALL_LOCATION}/lib/${LIB}.so")
	endforeach()
endif()

set(LLVM_ENABLE_CCACHE 0)
find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
	set(LLVM_ENABLE_CCACHE 1)
	message(STATUS "found ccache -> using ccache to compile llvm")
endif()

include(ExternalProject)
ExternalProject_Add(llvm_from_source
	# The build system expects the llvm repo to be next to this repo
	# if this is not the case you can enable the GIT_REPOSITORY line instead
	# and download it directly from the github mirror.
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../llvm-project"
	#GIT_REPOSITORY https://github.com/llvm/llvm-project.git
	GIT_TAG llvmorg-13.0.1
	SOURCE_SUBDIR llvm
    CMAKE_ARGS
		-DLLVM_ENABLE_PROJECTS='llvm'
        -G Ninja
        -DCMAKE_INSTALL_PREFIX=${LLVM_INSTALL_LOCATION}
		-DCMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE}
		-DLLVM_CCACHE_BUILD=${LLVM_ENABLE_CCACHE}
		-DLLVM_TARGETS_TO_BUILD=X86
		-DBUILD_SHARED_LIB=OFF
	TEST_AFTER_INSTALL 1
	TEST_COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}	
	BUILD_ALWAYS 1
	BUILD_BYPRODUCTS ${LLVM_LIBS_LOCATIONS}
)

add_library(llvm_lib INTERFACE)

# Create the include dir, this hack can prob be avoided by using a fetch.
file(MAKE_DIRECTORY "${LLVM_INSTALL_LOCATION}/include")

foreach(LLVM_LIB ${LLVM_LIB_LIST})
	add_library(llvm::${LLVM_LIB} STATIC IMPORTED)
	add_dependencies(llvm::${LLVM_LIB} llvm_from_source)
 	set_target_properties(llvm::${LLVM_LIB} PROPERTIES
		INTERFACE_INCLUDE_DIRECTORIES "${LLVM_INSTALL_LOCATION}/include"
 		)
 	set_target_properties(llvm::${LLVM_LIB} PROPERTIES
 		IMPORTED_LOCATION "${LLVM_INSTALL_LOCATION}/lib/lib${LLVM_LIB}.a" # linux
		)
	target_link_libraries(llvm_lib INTERFACE llvm::${LLVM_LIB})
endforeach(LLVM_LIB)

# llvm Remarks seems to be a shared library on linux
add_library(llvm::Remarks SHARED IMPORTED)
add_dependencies(llvm::Remarks llvm_from_source)
set_target_properties(llvm::Remarks PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${LLVM_INSTALL_LOCATION}/include")
set_target_properties(llvm::Remarks PROPERTIES
	IMPORTED_LOCATION "${LLVM_INSTALL_LOCATION}/lib/libRemarks.so")
target_link_libraries(llvm_lib INTERFACE llvm::Remarks)

add_library(llvm::LTO SHARED IMPORTED)
add_dependencies(llvm::LTO llvm_from_source)
set_target_properties(llvm::LTO PROPERTIES
	INTERFACE_INCLUDE_DIRECTORIES "${LLVM_INSTALL_LOCATION}/include")
set_target_properties(llvm::LTO PROPERTIES
	IMPORTED_LOCATION "${LLVM_INSTALL_LOCATION}/lib/libLTO.so")
target_link_libraries(llvm_lib INTERFACE llvm::LTO)


include(${CMAKE_CURRENT_LIST_DIR}/conan.cmake)

conan_cmake_configure(REQUIRES
                        fmt/8.0.0
                        catch2/3.1.0
                      GENERATORS
                        cmake_find_package
                        cmake
                        cmake_paths
                      )

conan_cmake_autodetect(settings)

conan_cmake_install(PATH_OR_REFERENCE .
                    BUILD missing
                    REMOTE conancenter
                    SETTINGS ${settings}
                    )

find_package(fmt REQUIRED)
find_package(Catch2 REQUIRED)

add_subdirectory(src/LLVM_IR)
#add_subdirectory(tests)

add_executable(driver ${CMAKE_CURRENT_LIST_DIR}/driver.cpp)
target_link_libraries(driver Infra fmt::fmt)
