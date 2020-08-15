import qbs
import qbs.FileInfo

Project {
    name: "Fancontrol"
    references: [
        "scmrtos/scmrtos.qbs",
        "stm8_libs/stm8_libs.qbs"
    ]

Product {
    name: "common_options"

    Export {
        Depends { name: "cpp" }

        cpp.defines: [
            "STM8S103",
            "F_CPU=2000000UL"
        ]

        cpp.commonCompilerFlags: [
            "-e",
            "--eec++",
            "--mfc"
        ]
    }
}

CppApplication {
    name: "main_app"

    Depends { name: "scmrtos" }
    Depends { name: "stm8lib" }
    Depends { name: "common_options" }

    cpp.positionIndependentCode: false
    cpp.debugInformation: false
    cpp.generateLinkerMapFile: true

    cpp.driverLinkerFlags: [
        "--config_def", "_CSTACK_SIZE=0x40",
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
