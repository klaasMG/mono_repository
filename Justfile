#!/usr/bin/env just --justfile

set shell := ["powershell.exe", "-Command"]
run_ui:
    cd {{justfile_directory()}}
    Remove-Item -Recurse -Force build
    mkdir build
    cd build; cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DTESTING=OFF -DBUILD_CODE=OFF -DBUILD_PCL=OFF -DBUILD_GMAKE=OFF -DBUILD_ASSET_MANAGER=OFF -DBUILD_AUDIO_ENGINE=OFF -DBUILD_LOGGER=OFF -DBUILD_STRING_FORMATTER=OFF -DUI_BUILD=ON
    cmake --build build --clean-first
    cd the_ui_tree_build; ../.venv/Scripts/python.exe ui_manger.py

build option:
    cd {{justfile_directory()}}; $opt = "{{option}}"; $defined = ((Select-String -Path CMakeLists.txt -Pattern '^\s*option\(([A-Za-z0-9_]+)' | ForEach-Object { $_.Matches[0].Groups[1].Value }) | Select-Object -Unique); if ($defined -notcontains $opt) { Write-Host "WARNING: option '$opt' is not defined in CMakeLists.txt. Building it requires adding 'option($opt ...)' to the cmake config." }; $flags = $defined | ForEach-Object { "-D$_=$(if ($_ -eq $opt) { 'ON' } else { 'OFF' })" }; cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ @flags; if ($?) { cmake --build build --clean-first }