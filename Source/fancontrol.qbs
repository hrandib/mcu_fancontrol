import qbs

CppApplication {
    name: "fancontrol"

    cpp.positionIndependentCode: false
    cpp.optimization: "small"
    cpp.debugInformation: false

    cpp.commonCompilerFlags: [
        "-e",
        "--eec++"
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
