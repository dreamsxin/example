https://github.com/microsoft/vscode

## 插件

- https://marketplace.visualstudio.com/items?itemName=yzane.markdown-pdf#usage
- GlassIt-VSC 背景透明度
https://marketplace.visualstudio.com/items?itemName=s-nlf-fh.glassit
- https://github.com/r23qw/vscode-http-client
- https://github.com/continuedev/continue

### Command Palette
1. Open the Markdown file
2. Press F1 or Ctrl+Shift+P
3. Type export and select below
  * markdown-pdf: Export (settings.json)
  * markdown-pdf: Export (pdf)
  * markdown-pdf: Export (html)
  * markdown-pdf: Export (png)
  * markdown-pdf: Export (jpeg)
  * markdown-pdf: Export (all: pdf, html, png, jpeg)

## task.json

```json
{
  // See https://go.microsoft.com/fwlink/?LinkId=733558
  // for the documentation about the tasks.json format
  "version": "2.0.0",
  "type": "shell",
  "command": "${workspaceFolder}/mach",
  "args": [
    "--log-no-times"
  ],
  "windows": {
    // Not using ${workspaceFolder} because doing so somehow runs
    // the command on a new window instead of within the vscode terminal
    "command": "./mach"
  },
  "options": {
    "env": {
      "MOZCONFIG": "mozconfig-dbg"
    }
  },
  "tasks": [
    {
      "label": "clobber",
      "args": [
        "clobber"
      ],
      "problemMatcher": []
    },
    {
      "label": "clobber-python",
      "args": [
        "clobber",
        "python"
      ],
      "problemMatcher": []
    },
    {
      "label": "configure",
      "args": [
        "configure"
      ],
      "problemMatcher": []
    },
    {
      "label": "build",
      "args": [
        "build"
      ],
      "problemMatcher": {
        "owner": "cpp",
        "fileLocation": "absolute",
        "pattern": {
          "regexp": "^.*?([^\\s]*)[:(](\\d+)[:,](\\d+)[)]?:\\s+(warning|error):\\s+(.*)$",
          "file": 1,
          "line": 2,
          "column": 3,
          "severity": 4,
          "message": 5
        }
      }
    },
    {
      "label": "build-binaries",
      "args": [
        "build",
        "binaries"
      ],
      "problemMatcher": {
        "owner": "cpp",
        "fileLocation": "absolute",
        "pattern": {
          "regexp": "^(.*)[:(](\\d+)[:,](\\d+)[)]?:\\s+(warning|error):\\s+(.*)$",
          "file": 1,
          "line": 2,
          "column": 3,
          "severity": 4,
          "message": 5
        }
      },
      "group": {
        "kind": "build",
        "isDefault": true
      }
    },
    {
      "label": "build-faster",
      "args": [
        "build",
        "faster"
      ],
      "problemMatcher": {
        "owner": "cpp",
        "fileLocation": "absolute",
        "pattern": {
          "regexp": "^(.*)[:(](\\d+)[:,](\\d+)[)]?:\\s+(warning|error):\\s+(.*)$",
          "file": 1,
          "line": 2,
          "column": 3,
          "severity": 4,
          "message": 5
        }
      }
    },
    {
      "label": "run",
      "args": [
        "run",
        "-purgecaches",
        "--browser-id Shop-xxxx"
      ],
      "problemMatcher": []
    },
    {
      "label": "lint-wo",
      "args": [
        "lint",
        "-wo"
      ],
      "problemMatcher": [
        "$eslint-stylish"
      ]
    },
    {
      "label": "eslint",
      "args": [
        "eslint",
        "${file}"
      ],
      "problemMatcher": [
        "$eslint-stylish"
      ]
    },
    {
      "label": "eslint-fix",
      "args": [
        "eslint",
        "--fix",
        "${file}"
      ],
      "problemMatcher": [
        "$eslint-stylish"
      ]
    },
    {
      "label": "test",
      "args": [
        "test",
        "${relativeFile}"
      ],
      "group": "test",
      "presentation": {
        "reveal": "always",
        "panel": "new"
      }
    },
    {
      "label": "mochitest",
      "args": [
        "mochitest",
        "${relativeFile}"
      ],
      "problemMatcher": {
        "fileLocation": [
          "relative",
          "${workspaceFolder}"
        ],
        "pattern": {
          "regexp": "^.*\\s+(TEST-UNEXPECTED-FAIL|TEST-UNEXPECTED-PASS)\\s+\\|\\s+([^\\s]*)\\s+\\|\\s+(.*)$",
          "severity": 1,
          "file": 2,
          "message": 3
        }
      },
      "presentation": {
        "reveal": "always",
        "panel": "new"
      }
    },
    {
      "label": "reftest",
      "args": [
        "reftest",
        "${relativeFile}"
      ],
      "problemMatcher": {
        "fileLocation": [
          "absolute"
        ],
        "pattern": {
          "regexp": "^.*\\s+(TEST-UNEXPECTED-FAIL|TEST-UNEXPECTED-PASS)\\s+\\|\\s+file:\/\/([^\\s]*)\\s+==\\s+[^\\s]*\\s+\\|\\s+(.*)$",
          "severity": 1,
          "file": 2,
          "message": 3
        }
      },
      "presentation": {
        "reveal": "always",
        "panel": "new"
      }
    },
    {
      "label": "xpcshell-test",
      "args": [
        "xpcshell-test",
        "${relativeFile}",
        "--sequential"
      ],
      "problemMatcher": {
        "fileLocation": [
          "relative",
          "${workspaceFolder}"
        ],
        "pattern": {
          "regexp": "^.*\\s+(FAIL|ERROR)\\s+\\[([^\\s]*)\\s+:\\s+(\\d+)\\]\\s+(.*)$",
          "severity": 1,
          "file": 2,
          "location": 3,
          "message": 4
        }
      },
      "presentation": {
        "reveal": "always",
        "panel": "new"
      }
    }
  ]
}
```
