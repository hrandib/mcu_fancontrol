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
            "F_CPU=2000000UL",
            "__STDC_LIMIT_MACROS"
        ]

        cpp.commonCompilerFlags: [
            "-e",
            "--eec++",
            "--mfc",
            "--diag_suppress=Pa137,Go004,Go005",
        ]
    }
}

CppApplication {
    name: "fancontrol"

    Depends { name: "scmrtos" }
    Depends { name: "stm8lib" }
    Depends { name: "common_options" }

    cpp.positionIndependentCode: false
    cpp.debugInformation: false
    cpp.generateLinkerMapFile: true

    cpp.driverLinkerFlags: [
        "--config_def", "_CSTACK_SIZE=0x40",
        "--config_def", "_HEAP_SIZE=0",
        "--merge_duplicate_sections"
    ]

    cpp.includePaths: [
        "scm_aux"
    ]

    Group {
        name: "Linker script"
        prefix: cpp.toolchainInstallPath + "/../config/"
        fileTags: ["linkerscript"]
        files: ["lnkstm8s103f3.icf"]
    }

    Group {
        name: "Compiled object file"
        fileTagsFilter: "application"
        qbs.install: true
    }

    Group {
        name: "Aux"
        prefix: "scm_aux/"
        files: [
            "*.h",
            "*.cpp"
        ]
    }

    files: ["main.cpp"]
} //CppApplication

} //Project
