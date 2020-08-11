import qbs

Project {
    name: "Fancontrol Project"
    references: [
        "scmrtos/scmrtos.qbs",
        "stm8_libs/stm8_libs.qbs"
    ]

CppApplication {
    name: "fancontrol"

    Depends { name: "scmrtos" }
    Depends { name: "stm8lib" }

    cpp.positionIndependentCode: false
    cpp.debugInformation: false
    cpp.generateLinkerMapFile: false

    cpp.commonCompilerFlags: [
        "-e",
        "--eec++",
        "--mfc"
    ]

    cpp.driverLinkerFlags: [
        "--config_def", "_CSTACK_SIZE=0x100",
        "--config_def", "_HEAP_SIZE=0",
    ]

    Group {
        name: "Linker Script"
        prefix: cpp.toolchainInstallPath + "/../config/"
        fileTags: ["linkerscript"]
        files: ["lnkstm8s103f3.icf"]
    }

    files: ["main.cpp"]
}

}
