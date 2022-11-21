cmake_minimum_required(VERSION 3.20)
project(llvm-ir CXX C)
set (CMAKE_CXX_STANDARD 17)

# Not clean, I should find a better way to do this.
if(UNIX)
	set(CMAKE_CXX_FLAGS "-fuse-ld=gold")
endif()

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(LLVM_INSTALL_LOCATION "${CMAKE_CURRENT_LIST_DIR}/llvm")

include(ExternalProject)
ExternalProject_Add( llvm_from_source
	# The build system expects the llvm repo to be next to this repo
	# if this is not the case you can enable the GIT_REPOSITORY line instead
	# and download it directly from the github mirror.
    SOURCE_DIR "${CMAKE_CURRENT_LIST_DIR}/../../llvm-project"
	#GIT_REPOSITORY https://github.com/llvm/llvm-project.git
    GIT_TAG llvmorg-13.0.1
    SOURCE_SUBDIR llvm
    CMAKE_ARGS 
        -DCMAKE_ENABLE_PROJECTS='llvm'
        -G Ninja
        -DCMAKE_INSTALL_PREFIX=${LLVM_INSTALL_LOCATION}
)

set(LLVM_LIB_LIST
	LLVMAArch64AsmParser
	LLVMAArch64CodeGen
	LLVMAArch64Desc
	LLVMAArch64Disassembler
	LLVMAArch64Info
	LLVMAArch64Utils
	LLVMAggressiveInstCombine
	LLVMAMDGPUAsmParser
	LLVMAMDGPUCodeGen
	LLVMAMDGPUDesc
	LLVMAMDGPUDisassembler
	LLVMAMDGPUInfo
	LLVMAMDGPUTargetMCA
	LLVMAMDGPUUtils
	LLVMAnalysis
	LLVMARMAsmParser
	LLVMARMCodeGen
	LLVMARMDesc
	LLVMARMDisassembler
	LLVMARMInfo
	LLVMARMUtils
	LLVMAsmParser
	LLVMAsmPrinter
	LLVMAVRAsmParser
	LLVMAVRCodeGen
	LLVMAVRDesc
	LLVMAVRDisassembler
	LLVMAVRInfo
	LLVMBinaryFormat
	LLVMBitReader
	LLVMBitstreamReader
	LLVMBitWriter
	LLVMBPFAsmParser
	LLVMBPFCodeGen
	LLVMBPFDesc
	LLVMBPFDisassembler
	LLVMBPFInfo
	LLVMCFGuard
	LLVMCFIVerify
	LLVMCodeGen
	LLVMCore
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
	LLVMExegesisAArch64
	LLVMExegesisMips
	LLVMExegesisPowerPC
	LLVMExegesisX86
	LLVMExtensions
	LLVMFileCheck
	LLVMFrontendHLSL
	LLVMFrontendOpenACC
	LLVMFrontendOpenMP
	LLVMFuzzerCLI
	LLVMFuzzMutate
	LLVMGlobalISel
	LLVMHexagonAsmParser
	LLVMHexagonCodeGen
	LLVMHexagonDesc
	LLVMHexagonDisassembler
	LLVMHexagonInfo
	LLVMInstCombine
	LLVMInstrumentation
	LLVMInterfaceStub
	LLVMInterpreter
	LLVMipo
	LLVMIRPrinter
	LLVMIRReader
	LLVMJITLink
	LLVMLanaiAsmParser
	LLVMLanaiCodeGen
	LLVMLanaiDesc
	LLVMLanaiDisassembler
	LLVMLanaiInfo
	LLVMLibDriver
	LLVMLineEditor
	LLVMLinker
	LLVMLTO
	LLVMMC
	LLVMMCA
	LLVMMCDisassembler
	LLVMMCJIT
	LLVMMCParser
	LLVMMipsAsmParser
	LLVMMipsCodeGen
	LLVMMipsDesc
	LLVMMipsDisassembler
	LLVMMipsInfo
	LLVMMIRParser
	LLVMMSP430AsmParser
	LLVMMSP430CodeGen
	LLVMMSP430Desc
	LLVMMSP430Disassembler
	LLVMMSP430Info
	LLVMNVPTXCodeGen
	LLVMNVPTXDesc
	LLVMNVPTXInfo
	LLVMObjCARCOpts
	LLVMObjCopy
	LLVMObject
	LLVMObjectYAML
	LLVMOption
	LLVMOrcJIT
	LLVMOrcShared
	LLVMOrcTargetProcess
	LLVMPasses
	LLVMPowerPCAsmParser
	LLVMPowerPCCodeGen
	LLVMPowerPCDesc
	LLVMPowerPCDisassembler
	LLVMPowerPCInfo
	LLVMProfileData
	LLVMRemarks
	LLVMRISCVAsmParser
	LLVMRISCVCodeGen
	LLVMRISCVDesc
	LLVMRISCVDisassembler
	LLVMRISCVInfo
	LLVMRISCVTargetMCA
	LLVMRuntimeDyld
	LLVMScalarOpts
	LLVMSelectionDAG
	LLVMSparcAsmParser
	LLVMSparcCodeGen
	LLVMSparcDesc
	LLVMSparcDisassembler
	LLVMSparcInfo
	LLVMSupport
	LLVMSymbolize
	LLVMSystemZAsmParser
	LLVMSystemZCodeGen
	LLVMSystemZDesc
	LLVMSystemZDisassembler
	LLVMSystemZInfo
	LLVMTableGen
	LLVMTableGenGlobalISel
	LLVMTarget
	LLVMTextAPI
	LLVMTransformUtils
	LLVMVEAsmParser
	LLVMVECodeGen
	LLVMVectorize
	LLVMVEDesc
	LLVMVEDisassembler
	LLVMVEInfo
	LLVMWebAssemblyAsmParser
	LLVMWebAssemblyCodeGen
	LLVMWebAssemblyDesc
	LLVMWebAssemblyDisassembler
	LLVMWebAssemblyInfo
	LLVMWebAssemblyUtils
	LLVMWindowsDriver
	LLVMWindowsManifest
	LLVMX86AsmParser
	LLVMX86CodeGen
	LLVMX86Desc
	LLVMX86Disassembler
	LLVMX86Info
	LLVMX86TargetMCA
	LLVMXCoreCodeGen
	LLVMXCoreDesc
	LLVMXCoreDisassembler
	LLVMXCoreInfo
	LLVMXRay
	LTO
	Remarks)
add_library(llvm::llvm INTERFACE IMPORTED)

foreach(LLVM_LIB ${LLVM_LIB_LIST})
	add_library(llvm::${LLVM_LIB} SHARED IMPORTED)
	set_target_properties(llvm::${LLVM_LIB} PROPERTIES 
	 	INTERFACE_INCLUDE_DIRECTORIES "${LLVM_INSTALL_LOCATION}/include"
	 	IMPORTED_IMPLIB "${LLVM_INSTALL_LOCATION}/lib/${LLVM_LIB}.lib"
	)
	target_link_libraries(llvm::llvm INTERFACE llvm::${LLVM_LIB})
endforeach(LLVM_LIB)

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

add_subdirectory(tests)
add_subdirectory(src/LLVM_IR)

add_executable(driver ${CMAKE_CURRENT_LIST_DIR}/driver.cpp)
target_link_libraries(driver Infra fmt::fmt )
