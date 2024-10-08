import qbs
import qbs.FileInfo
import qbs.TextFile

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
            "F_CPU=8000000UL",
            "__STDC_LIMIT_MACROS",
        ]

        cpp.commonCompilerFlags: [
            "-e",
            "--eec++",
            "--mfc",
            "--diag_suppress=Pa137,Go004,Go005",
        ]

        cpp.includePaths: [
            "config"
        ]

        Group {
            name: "DeviceInfo"
            prefix: "config/"
            files: [
                "device_info.h",
                "device_info.cpp"
            ]
        }
    }
}

CppApplication {
    name: "fancontrol"

    Depends { name: "scmrtos" }
    Depends { name: "stm8lib" }
    Depends { name: "common_options" }

    type: ["print_size"]

    cpp.positionIndependentCode: false
    cpp.debugInformation: false
    cpp.generateLinkerMapFile: true

    cpp.driverLinkerFlags: [
        "--config_def", "_CSTACK_SIZE=0x80",
        "--config_def", "_HEAP_SIZE=0",
        "--merge_duplicate_sections",
        "--config", product.sourceDirectory + "/config/lnkstm8s103f3.icf",
    ]

    cpp.includePaths: [
        "scm_aux",
        "sensors",
        "pwm",
        "controller",
    ]

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
            "*.cpp",
        ]
    }

    Group {
        name: "UartComm"
        prefix: "uart_comm/"
        files: [
            "*.h",
            "*.cpp",
        ]
    }

    Group {
        name: "Sensors"
        prefix: "sensors/"
        files: [
            "*.h",
            "*.cpp",
        ]
    }

    Group {
        name: "Controller"
        prefix: "controller/"
        files: [
            "*.h",
            "*.cpp",
        ]
    }

    Group {
        name: "Pwm"
        prefix: "pwm/"
        files: [
            "*.h",
            "*.cpp",
        ]
    }

    files: ["main.cpp"]

    Rule {
        inputs: ["application"]
        outputFileTags: ["print_size"]
        prepare: {
            var reverseIndexOf = function(s, entry, position) {
                while(s[position--] !== entry)
                    ;
                return position;
            }
            var mapPath = FileInfo.joinPaths(product.buildDirectory, product.name + ".map");
            var cmd = new JavaScriptCommand();
            var file = new TextFile(mapPath);
            var content = file.readAll();
            file.close()
            var begin = reverseIndexOf(content, "\n", content.indexOf("bytes of")) + 2;
            var end = content.indexOf("\n", content.lastIndexOf("bytes of")) + 1
            cmd.description = content.slice(begin, end);
            cmd.highlight = "Sections";
            return cmd;
        }
    }

} //CppApplication

} //Project
