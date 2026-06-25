#!/usr/bin/env just --justfile

set shell := ["powershell.exe", "-Command"]
run_ui:
    cd {{justfile_directory()}}
    Remove-Item -Recurse -Force build
    mkdir build
    cd build; cmake .. -G "MinGW Makefiles" -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ -DTESTING=OFF -DBUILD_CODE=OFF -DBUILD_PCL=OFF -DBUILD_GMAKE=OFF -DBUILD_ASSET_MANAGER=OFF -DBUILD_AUDIO_ENGINE=OFF -DBUILD_LOGGER=OFF -DBUILD_STRING_FORMATTER=OFF -DUI_BUILD=ON
    cmake --build build --clean-first
    cd the_ui_tree_build; ../.venv/Scripts/python.exe ui_manger.py