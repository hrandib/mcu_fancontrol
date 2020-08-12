import qbs
import qbs.FileInfo

Project {
    name: "Fancontrol"
    references: [
        "scmrtos/scmrtos.qbs",
        "stm8_libs/stm8_libs.qbs"
    ]

Product {
    name: "Common options"

    Export {
        Depends { name: "cpp" }

        cpp.defines: [
            "STM8S103"
        ]

        cpp.commonCompilerFlags: [
            "-e",
            "--eec++",
//            "--mfc"
        ]
    }
}

CppApplication {
    name: "Main app"

    Depends { name: "scmrtos" }
    Depends { name: "stm8lib" }
    Depends { name: "Common options" }

    cpp.positionIndependentCode: false
    cpp.debugInformation: false
    cpp.generateLinkerMapFile: true

    cpp.driverLinkerFlags: [
        "--config_def", "_CSTACK_SIZE=0x100",
        "--config_def", "_HEAP_SIZE=0",
    ]

    Group {
        name: "Linker script"
        prefix: cpp.toolchainInstallPath + "/../config/"
        fileTags: ["linkerscript"]
        files: ["lnkstm8s103f3.icf"]
    }

    files: ["main.cpp"]
} //CppApplication

} //Project
