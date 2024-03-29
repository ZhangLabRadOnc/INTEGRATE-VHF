find_package(PkgConfig REQUIRED)
pkg_check_modules(HTSLIB REQUIRED IMPORTED_TARGET htslib)
pkg_check_modules(DIVSUFSORT REQUIRED IMPORTED_TARGET libdivsufsort)

set(SOURCES
    Alignment.cpp
    Artifact1.cpp
    BWT.cpp
    BreakPoint.cpp
    Dna.cpp
    FocalRegionHandler.cpp
    FusionGraph.cpp
    Gene.cpp
    HitsCounter.cpp
    LowComplexFinder.cpp
    MyBamHeader.cpp
    MyBamWrap.cpp
    Reference.cpp
    Result.cpp
    Rna.cpp
    RunCode.cpp
    SuffixArray2.cpp
    TidHandler.cpp
    Util.cpp
    Updator.cpp
    VirusLoader.cpp
    main.cpp
)

add_executable(${PROJECT_NAME}-bin ${SOURCES})
set_target_properties(${PROJECT_NAME}-bin PROPERTIES OUTPUT_NAME Integrate)
target_link_libraries(${PROJECT_NAME}-bin PkgConfig::HTSLIB)
target_link_libraries(${PROJECT_NAME}-bin PkgConfig::DIVSUFSORT)

install(TARGETS ${PROJECT_NAME}-bin DESTINATION bin)
