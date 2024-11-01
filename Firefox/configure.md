```txt
$ ./mach configure -- --help
 checking for vcs source checkout... no
 checking whether cross compiling... no
 Usage: configure.py [options]

 Options: [defaults in brackets after descriptions]
   Help options:
     --help                    print this message

   Options from ..\build\moz.configure\init.configure:
     --enable-application      Application to build. Same as --enable-project.
     --enable-project          Project to build [browser]
     --enable-artifact-builds  Download and use prebuilt binary artifacts.
     --host                    Define the system type performing the build
     --target                  Define the system type where the resulting executables will be used
     --with-version-file-path  Specify a custom path to app version files instead of auto-detecting
     --as-milestone={early-beta,late-beta,release}
                               Build with another milestone configuration (e.g., as release)
     --enable-update-channel   Select application update channel [default]
     --with-app-basename       Typically stays consistent for multiple branded versions of a given application (e.g. Aurora and Firefox both use "Firefox"), but may vary for full rebrandings (e.g. Iceweasel). Used for application.ini's "Name" field, which controls profile location in the absence of a "Profile" field (see below), and var
ious system integration hooks (Unix remoting, Windows MessageWindow name, etc.

   Options from ..\moz.configure:
     --enable-artifact-build-symbols={full}
                               Download symbols when artifact builds are enabled.
     --disable-compile-environment
                               Disable compiler/library checks
     --disable-tests           Do not build test libraries & programs
     --enable-debug            Enable building with developer debug info (using the given compiler flags).
     --with-debug-label        Debug DEBUG_<value> for each comma-separated value given
     --disable-release         Do not build with more conservative, release engineering-oriented options.
     --disable-unified-build   Enable building modules in non unified context
     --enable-valgrind         Enable Valgrind integration hooks
     --enable-build-backend={Clangd,ChromeMap,CompileDB,CppEclipse,FasterMake,FasterMake+RecursiveMake,RecursiveMake,StaticAnalysis,TestManifest,VisualStudio}
                               Deprecated
     --build-backends={Clangd,ChromeMap,CompileDB,CppEclipse,FasterMake,FasterMake+RecursiveMake,RecursiveMake,StaticAnalysis,TestManifest,VisualStudio}
                               Build backends to generate [RecursiveMake,FasterMake,Clangd]
     --enable-gtest-in-build   Enable building the gtest libxul during the build.
     --enable-ui-locale        Select the user interface locale (default: en-US) [en-US]
     --enable-strip            Enable stripping of libs & executables
     --disable-install-strip   Enable stripping of libs & executables when packaging
     --with-system-zlib        Use system libz

   Options from ..\build\moz.configure\bootstrap.configure:
     --enable-bootstrap        Automatically bootstrap or update some toolchains

   Options from ..\build\moz.configure\toolchain.configure:
     --disable-optimize        Disable optimizations via compiler flags
     --with-toolchain-prefix   Prefix for the target toolchain
     --with-compiler-wrapper   Enable compiling with wrappers such as distcc and ccache
     --with-ccache             Enable compiling with ccache
     --enable-gold             Deprecated
     --enable-linker           Select the linker {bfd, gold, ld64, lld, lld-*, mold}
     --disable-debug-symbols   Disable debug symbols using the given compiler flags
     --enable-address-sanitizer
                               Enable Address Sanitizer
     --enable-memory-sanitizer
                               Enable Memory Sanitizer
     --enable-thread-sanitizer
                               Enable Thread Sanitizer
     --enable-undefined-sanitizer
                               Enable UndefinedBehavior Sanitizer
     --enable-signed-overflow-sanitizer
                               Enable UndefinedBehavior Sanitizer (Signed Integer Overflow Parts)
     --enable-unsigned-overflow-sanitizer
                               Enable UndefinedBehavior Sanitizer (Unsigned Integer Overflow Parts)
     --enable-hardening        Enables security hardening compiler options
     --enable-frame-pointers   Enable frame pointers
     --enable-coverage         Enable code coverage
     --enable-clang-plugin     Enable building with the Clang plugin (gecko specific static analyzers)
     --enable-fuzzing          Enable fuzzing support
     --enable-snapshot-fuzzing
                               Enable experimental snapshot fuzzing support
     --enable-cpp-rtti         Enable C++ RTTI
     --enable-path-remapping={c,rust}
                               Enable remapping source and object paths in compiled outputs.
     --enable-dtrace           Build with dtrace support

   Options from ..\build\moz.configure\memory.configure:
     --enable-jemalloc         Replace memory allocator with jemalloc

   Options from ..\build\moz.configure\headers.configure:
     --with-linux-headers      location where the Linux kernel headers can be found

   Options from ..\build\moz.configure\warnings.configure:
     --enable-warnings-as-errors
                               Enable treating warnings as errors

   Options from ..\build\moz.configure\flags.configure:
     --disable-icf             Disable Identical Code Folding
     --disable-new-pass-manager
                               Use the legacy LLVM pass manager in clang builds

   Options from ..\build\moz.configure\lto-pgo.configure:
     --enable-profile-generate={cross}
                               Build a PGO instrumented binary
     --enable-profile-use={cross}
                               Use a generated profile during the build
     --with-pgo-profile-path   Path to the directory with unmerged profile data to use during the build, or to a merged profdata file
     --with-pgo-jarlog         Use the provided jarlog file when packaging during a profile-use build
     --enable-lto={full,thin,cross}
                               Enable LTO

   Options from ..\toolkit\moz.configure:
     --with-distribution-id    Set distribution-specific id [org.mozilla]
     --enable-jprof            Enable jprof profiling tool (needs mozilla/tools/jprof)
     --disable-gecko-profiler  Disable the Gecko profiler
     --enable-dmd              Enable Dark Matter Detector (heap profiler). Also enables jemalloc, replace-malloc and profiling
     --enable-audio-backends={aaudio,alsa,audiounit,jack,opensl,oss,pulseaudio,sndio,sunaudio,wasapi}
                               Enable various cubeb backends [pulseaudio]
     --enable-alsa             Enable ALSA audio backend.
     --enable-jack             Enable JACK audio backend.
     --enable-pulseaudio       Enable PulseAudio audio backend.
     --enable-sndio            Enable sndio audio backend.
     --with-l10n-base          Path to l10n repositories
     --enable-default-toolkit={cairo-gtk3,cairo-gtk3-wayland,cairo-gtk3-x11-wayland,cairo-gtk3-wayland-only,cairo-gtk3-x11-only}
                               Select default toolkit [cairo-gtk3]
     --with-gl-provider        Set GL provider backend type
     --disable-wmf             Disable support for Windows Media Foundation
     --disable-ffmpeg          Disable FFmpeg for fragmented H264/AAC decoding
     --disable-av1             Disable av1 video support
     --disable-jxl             Disable jxl image support
     --disable-real-time-tracing
                               Disable tracing of real-time audio callbacks
     --enable-openmax          Enable OpenMAX IL for video/audio decoding
     --enable-chrome-format={omni,jar,flat}
                               Select FORMAT of chrome files during packaging. [omni]
     --enable-minify={properties,js}
                               Select types of files to minify during packaging. [properties]
     --enable-gpsd             Enable gpsd support
     --with-mozilla-api-keyfile
                               Use the secret key contained in the given keyfile for Mozilla API requests
     --with-google-location-service-api-keyfile
                               Use the secret key contained in the given keyfile for Google Location Service API requests
     --with-google-safebrowsing-api-keyfile
                               Use the secret key contained in the given keyfile for Google Safebrowsing API requests
     --with-bing-api-keyfile   Use the client id and secret key contained in the given keyfile for Bing API requests
     --with-adjust-sdk-keyfile
                               Use the secret key contained in the given keyfile for Adjust SDK requests
     --with-leanplum-sdk-keyfile
                               Use the client id and secret key contained in the given keyfile for Leanplum SDK requests
     --with-pocket-api-keyfile
                               Use the secret key contained in the given keyfile for Pocket API requests
     --enable-webrender-debugger
                               Build the websocket debug server in WebRender
     --enable-app-system-headers
                               Use additional system headers defined in $MOZ_BUILD_APP/app-system-headers.mozbuild
     --disable-printing        Disable printing support
     --disable-synth-speechd   Disable speech-dispatcher support
     --disable-webspeech       Disable support for HTML Speech API
     --disable-webspeechtestbackend
                               Disable support for HTML Speech API Test Backend
     --enable-skia-pdf         Enable Skia PDF
     --with-system-webp        Use system libwebp (located with pkgconfig)
     --disable-webdriver       Disable support for WebDriver remote protocols
     --disable-geckodriver     Do not build geckodriver
     --enable-webrtc           Enable support for WebRTC
     --enable-raw              Enable support for RAW media
     --enable-address-sanitizer-reporter
                               Enable Address Sanitizer Reporter Extension
     --enable-proxy-bypass-protection
                               Prevent suspected or confirmed proxy bypasses
     --disable-proxy-direct-failover
                               Disable direct failover for system requests
     --disable-accessibility   Disable accessibility support
     --with-unsigned-addon-scopes={app,system}
                               Addon scopes where signature is not required
     --allow-addon-sideload    Addon sideloading is allowed
     --enable-extensions-webidl-bindings
                               Enable building experimental WebExtensions WebIDL bindings
     --enable-launcher-process
                               Enable launcher process by default
     --enable-bundled-fonts    Enable support for bundled fonts on desktop platforms
     --enable-reflow-perf      Enable reflow performance tracing
     --enable-layout-debugger  Enable layout debugger
     --with-system-libvpx      Use system libvpx (located with pkgconfig)
     --with-system-jpeg        Use system libjpeg (installed at given prefix)
     --with-system-png         Use system libpng
     --with-wasm-sandboxed-libraries={graphite,ogg,hunspell,expat,woff2,soundtouch}
                               Enable wasm sandboxing for the selected libraries [graphite,ogg,hunspell,expat,woff2,soundtouch]
     --with-wasi-sysroot       Path to wasi sysroot for wasm sandboxing
     --enable-disk-remnant-avoidance
                               Prevent persistence of auxiliary files on application close
     --enable-forkserver       Enable fork server
     --disable-backgroundtasks
                               Disable running in background task mode
     --enable-mobile-optimize  Enable mobile optimizations
     --disable-pref-extensions
                               Disable pref extensions such as autoconfig
     --disable-startupcache    Disable startup cache
     --with-crashreporter-url  Set an alternative crashreporter url [https://crash-reports.mozilla.com/]
     --with-system-libevent    Use system libevent
     --enable-crashreporter    Enable crash reporting
     --disable-dbus            Disable dbus support
     --enable-debug-js-modules
                               Enable debug mode for frontend JS libraries
     --enable-dump-painting    Enable paint debugging
     --enable-libproxy         Enable libproxy support
     --enable-logrefcnt        Enable logging of refcounts
     --disable-negotiateauth   Disable GSS-API negotiation
     --disable-parental-controls
                               Do not build parental controls
     --enable-sandbox          Enable sandboxing support
     --disable-system-extension-dirs
                               Disable searching system- and account-global directories for extensions of any kind; use only profile-specific extension directories
     --enable-system-pixman    Use system pixman (located with pkgconfig)
     --disable-universalchardet
                               Disable universal encoding detection
     --disable-zipwriter       Disable zipwriter component
     --with-user-appdir        Set user-specific appdir [.mozilla]
     --enable-uniffi-fixtures  Enable UniFFI Fixtures/Examples
     --disable-system-policies
                               Disable reading policies from Windows registry, macOS's file system attributes, and /etc/firefox
     --disable-legacy-profile-creation
                               Disable the creation a legacy profile, to be used by old versions of Firefox, when no profiles exist.

   Options from ..\js\moz.configure:
     --with-app-name           Used for e.g. the binary program file name. If not set, defaults to a lowercase form of MOZ_APP_BASENAME.
     --enable-smoosh           Enable SmooshMonkey (new JS engine frontend)
     --enable-js-shell         Build the JS shell
     --enable-record-tuple     Enable records and tuples (and disables JIT)
     --enable-decorators       Enable experimental JS Decorators support
     --enable-explicit-resource-management
                               Enable explicit resource management
     --enable-portable-baseline-interp
                               Enable the portable baseline interpreter.
     --enable-portable-baseline-interp-force
                               Enable forcing use of the portable baseline interpreter.
     --enable-jit              Enable use of the JITs
     --enable-ion              Deprecated
     --enable-simulator={arm,arm64,mips32,mips64,loong64,riscv64}
                               Enable a JIT code simulator for the specified architecture
     --enable-instruments      Enable instruments remote profiling
     --enable-callgrind        Enable callgrind profiling
     --enable-profiling        Set compile flags necessary for using sampling profilers (e.g. shark, perf)
     --enable-vtune            Enable VTune profiling
     --enable-gc-probes        Turn on probes for allocation and finalization
     --enable-gczeal           Enable zealous GCing
     --enable-oom-breakpoint   Enable a breakpoint function for artificial OOMs
     --enable-perf             Enable Linux perf integration
     --enable-jitspew          Enable the Jit spew and IONFLAGS environment variable
     --enable-masm-verbose     Enable MacroAssembler verbosity of generated code.
     --enable-arm64-fjcvtzs    Enable static use of FJCVTZS instruction on Aarch64 targets.
     --disable-ctypes          Disable js-ctypes
     --enable-pipeline-operator
                               Enable pipeline operator
     --enable-rust-simd        Enable explicit SIMD in Rust code.
     --enable-spidermonkey-telemetry
                               Enable performance telemetry for SpiderMonkey (e.g. compile and run times)
     --enable-wasm-codegen-debug
                               Enable debugging for wasm codegen
     --wasm-no-experimental    Force disable all wasm experimental features for testing.
     --enable-wasm-tail-calls  Enable WebAssembly tail-calls
     --disable-wasm-gc         Disable WebAssembly GC
     --enable-wasm-jspi        Enable WebAssembly JS PI
     --disable-wasm-js-string-builtins
                               Disable WebAssembly JS String Builtins
     --disable-shared-memory   Disable JS/WebAssembly shared memory and atomics
     --enable-wasm-simd        Enable WebAssembly SIMD
     --enable-wasm-avx         Enable AVX support for WebAssembly SIMD
     --enable-wasm-relaxed-simd
                               Enable WebAssembly relaxed SIMD
     --enable-wasm-moz-intgemm
                               Enable WebAssembly intgemm private intrinsics
     --enable-wasm-memory64    Enable WebAssembly Memory64
     --enable-wasm-memory-control
                               Enable WebAssembly fine-grained memory control instructions
     --disable-wasm-multi-memory
                               Disable WebAssembly multi-memory
     --enable-wasm-branch-hinting
                               Enable WebAssembly Branch hints
     --with-qemu-exe           Use path as an arm emulator on host platforms
     --with-cross-lib          Use dir as the location for arm libraries [/usr/unknown-unknown-unknown]
     --with-sixgill            Enable static checking of code using sixgill
     --with-jitreport-granularity={0,1,2,3}
                               Default granularity at which to report JIT code to external tools (0 - no info, 1 - code ranges for while functions only, 2 - per-line information, 3 - per-op information) [3]
     --with-system-icu         Use system ICU
     --disable-icu4x           Disable using ICU4X
     --without-intl-api        Disable ECMAScript Internationalization API
     --with-temporal-api       Enable ECMAScript Temporal API
     --enable-wasm-type-reflections
                               Enable type reflection in WASM JS-API

   Options from ..\build\moz.configure\nspr.configure:
     --with-system-nspr        Use system NSPR

   Options from ..\build\moz.configure\rust.configure:
     --enable-rust-tests       Enable building and running of Rust tests during `make check`
     --enable-rust-debug       Build Rust code with debug assertions turned on.
     --disable-cargo-incremental
                               Disable incremental rust compilation.

   Options from ..\build\moz.configure\bindgen.configure:
     --with-libclang-path      Absolute path to a directory containing Clang/LLVM libraries for bindgen (version 3.9.x or above)
     --with-clang-path         Absolute path to a Clang binary for bindgen (version 3.9.x or above)

   Options from ..\js\ffi.configure:
     --with-system-ffi         Use system libffi (located with pkgconfig)

   Options from ..\build\moz.configure\node.configure:
     --disable-nodejs          Require Node.js to build

   Options from ..\build\moz.configure\nss.configure:
     --with-system-nss         Use system NSS

   Options from ..\build\moz.configure\update-programs.configure:
     --disable-updater         Disable building the updater
     --enable-unverified-updates
                               Enable application update without verifying MAR or updater binary signatures
     --enable-default-browser-agent
                               Enable building the default browser agent

   Options from ..\build\moz.configure\old.configure:
     --cache-file              Help missing for old configure options
     --datadir                 Help missing for old configure options
     --enable-official-branding
                               Help missing for old configure options
     --includedir              Help missing for old configure options
     --libdir                  Help missing for old configure options
     --prefix                  Help missing for old configure options
     --with-branding           Help missing for old configure options
     --x-includes              Help missing for old configure options
     --x-libraries             Help missing for old configure options


 Environment variables:
   Options from ..\build\moz.configure\init.configure:
     MOZ_AUTOMATION            Enable options for automated builds
     OLD_CONFIGURE             Path to the old configure script
     MOZCONFIG                 Mozconfig location
     MOZILLABUILD              Path to Mozilla Build (Windows-only)
     CONFIG_SHELL              Path to a POSIX shell
     MOZILLA_OFFICIAL          Build an official release
     MOZBUILD_STATE_PATH       Path to a persistent state directory for the build system and related tools

   Options from ..\moz.configure:
     MOZ_BUILD_HOOK            moz.build file that will be executed as if it were appended to every moz.build in the tree.
     MOZ_COPY_PDBS             For builds that do not support symbols in the normal fashion, generate and copy them into the resulting build archive.
     MOZ_PGO                   Build with profile guided optimizations
     WGET                      Path to the wget program
     READELF                   Path to the readelf program
     OBJCOPY                   Path to the objcopy program
     AWK                       Path to the awk program
     MAKE                      Path to GNU make
     GMAKE                     Path to the gmake program
     WATCHMAN                  Path to the watchman program
     XARGS                     Path to the xargs program
     MKFSHFS                   Path to the mkfshfs program
     HFS_TOOL                  Path to the hfs_tool program
     RPMBUILD                  Path to the rpmbuild program
     STRIP_FLAGS               Flags for the strip command
     STRIP                     Path to the strip program

   Options from ..\build\moz.configure\toolchain.configure:
     HOST_CPPFLAGS             Extra flags for Preprocessing host sources. []
     HOST_CFLAGS               Extra flags for compiling host C sources. []
     HOST_CXXFLAGS             Extra flags for compiling host C++ sources. []
     HOST_LDFLAGS              Extra flags for linking host object files. []
     CPPFLAGS                  Extra flags for preprocessing sources. []
     CFLAGS                    Extra flags for compiling C sources. []
     CXXFLAGS                  Extra flags for compiling C++ sources. []
     ASFLAGS                   Extra flags for assembling sources. []
     LDFLAGS                   Extra flags for linking object files. []
     MOZ_OPTIMIZE_FLAGS        Extra optimization flags. []
     MOZ_HAZARD                Build for the GC rooting hazard analysis
     CCACHE_PREFIX             Compiler prefix to use when using ccache
     RUSTC_WRAPPER             Wrap rust compilation with given tool
     SCCACHE_VERBOSE_STATS     Print verbose sccache stats after build
     CC                        Path to the target C compiler
     LD                        Deprecated
     CXX                       Path to the target C++ compiler
     HOST_CC                   Path to the host C compiler
     HOST_LD                   Deprecated
     HOST_CXX                  Path to the host C++ compiler
     MOZ_DEBUG_FLAGS           Debug compiler flags
     AS                        Path to the assembler
     LLVM_OBJDUMP              Path to llvm-objdump
     AR                        Path to the ar program
     HOST_AR                   Path to the host_ar program

   Options from ..\build\moz.configure\pkg.configure:
     PKG_CONFIG                Path to the pkg_config program

   Options from ..\build\moz.configure\lto-pgo.configure:
     LLVM_PROFDATA             Path to the llvm_profdata program
     MOZ_LD64_KNOWN_GOOD       Indicate that ld64 is free of symbol aliasing bugs.

   Options from ..\toolkit\moz.configure:
     MOZ_STUB_INSTALLER        produce a stub installer
     MOZ_SOURCE_REPO           project source repository
     MOZ_SOURCE_CHANGESET      source changeset
     MOZ_INCLUDE_SOURCE_INFO   include build repository informations
     MOZ_INSTRUMENT_EVENT_LOOP
                               Force-enable event loop instrumentation
     USE_FC_FREETYPE           Force-enable the use of fontconfig freetype
     MOZ_SELECTABLE_PROFILES   Enable experimental and unstable profile groups
     MOZ_TELEMETRY_REPORTING   Enable telemetry reporting
     TAR                       Path to the tar program
     UNZIP                     Path to the unzip program
     MIDL_FLAGS                Extra flags to pass to MIDL
     MOZ_REQUIRE_SIGNING       Enforce that add-ons are signed by the trusted root
     WASM_CC                   Path to the wasm C compiler
     WASM_CXX                  Path to the wasm C++ compiler
     WASM_CFLAGS               Options to pass to WASM_CC
     WASM_CXXFLAGS             Options to pass to WASM_CXX
     DUMP_SYMS                 Path to the dump_syms program
     MOZ_APP_REMOTINGNAME      Used for the internal program name, which affects profile name and remoting. If not set, defaults to MOZ_APP_NAME if the update channel is release, and MOZ_APP_NAME-MOZ_UPDATE_CHANNEL otherwise.
     ANDROID_PACKAGE_NAME      Name of the Android package (default org.mozilla.$MOZ_APP_NAME)
     MOZ_WINCONSOLE            Whether we can create a console window.
     MOZ_CRASHREPORTER_MOCK    Mock the crashreporter to test native GUIs
     MOZ_PACKAGE_JSSHELL       whether the installer bundles the JS shell

   Options from ..\build\moz.configure\rust.configure:
     RUSTC                     Path to the rust compiler
     CARGO                     Path to the Cargo package manager
     RUSTDOC                   Path to the rustdoc program
     RUSTFLAGS                 Rust compiler flags
     RUSTC_OPT_LEVEL           Rust compiler optimization level (-C opt-level=%s) [2]

   Options from ..\build\moz.configure\bindgen.configure:
     CBINDGEN                  Path to cbindgen
     RUSTFMT                   Path to the rustfmt program
     BINDGEN_CFLAGS            Options bindgen should pass to the C/C++ parser

   Options from ..\build\moz.configure\node.configure:
     NODEJS                    Path to nodejs

   Options from ..\build\moz.configure\old.configure:
     M4                        Path to the m4 program
```
