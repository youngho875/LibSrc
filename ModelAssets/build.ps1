param([int]$Parallel=3)
$ErrorActionPreference='Stop'
$source=$PSScriptRoot
$cmake=Join-Path $source 'tools\cmake-3.30.5-windows-x86_64\bin\cmake.exe'
if(!(Test-Path $cmake)){throw 'Prepare CMake 3.30.5 in tools; see README.'}
$assimp=Join-Path $source 'third_party\assimp-5.4.3'
if(!(Test-Path "$assimp\CMakeLists.txt")){throw 'Prepare Assimp 5.4.3 in third_party; see README.'}
# Native programs can print warnings to stderr; judge their exit code, not the stream.
$ErrorActionPreference='Continue'
& $cmake -S $assimp -B "$source\third_party\build-assimp" -G 'Visual Studio 16 2019' -A x64 -DBUILD_SHARED_LIBS=OFF -DASSIMP_BUILD_TESTS=OFF -DASSIMP_BUILD_ASSIMP_TOOLS=OFF -DASSIMP_INSTALL=OFF -DASSIMP_WARNINGS_AS_ERRORS=OFF -DASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT=OFF -DASSIMP_BUILD_3DS_IMPORTER=ON -DASSIMP_BUILD_GLTF_IMPORTER=ON -DASSIMP_BUILD_ALL_EXPORTERS_BY_DEFAULT=OFF -DASSIMP_NO_EXPORT=ON -DASSIMP_BUILD_DRACO=ON -DASSIMP_BUILD_ZLIB=ON
if($LASTEXITCODE){exit $LASTEXITCODE}
& $cmake --build "$source\third_party\build-assimp" --config Release --parallel $Parallel
if($LASTEXITCODE){exit $LASTEXITCODE}
& $cmake -S $source -B "$source\build" -G 'Visual Studio 16 2019' -A x64 -DCMAKE_CONFIGURATION_TYPES=Release
if($LASTEXITCODE){exit $LASTEXITCODE}
& $cmake --build "$source\build" --config Release --parallel $Parallel
exit $LASTEXITCODE
