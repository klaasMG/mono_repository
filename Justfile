#!/usr/bin/env just --justfile

set shell := ["powershell.exe", "-Command"]


build_setup:
    

run_ui:
    cd {{justfile_directory()}}
    Remove-Item -Recurse -Force build
    mkdir build
    just build UI_BUILD
    cd the_ui_tree_build; ../.venv/Scripts/python.exe ui_manger.py

build option:
    cd {{justfile_directory()}}; $opt = "{{option}}"; $defined = ((Select-String -Path CMakeLists.txt -Pattern '^\s*option\(([A-Za-z0-9_]+)' | ForEach-Object { $_.Matches[0].Groups[1].Value }) | Select-Object -Unique); if ($defined -notcontains $opt) { Write-Host "WARNING: option '$opt' is not defined in CMakeLists.txt. Building it requires adding 'option($opt ...)' to the cmake config." }; $flags = $defined | ForEach-Object { "-D$_=$(if ($_ -eq $opt) { 'ON' } else { 'OFF' })" }; cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_COMPILER=clang -DCMAKE_CXX_COMPILER=clang++ @flags; if ($?) { cmake --build build --clean-first }

ruff:
    .venv/Scripts/ruff check .

mypy verbose:
    {{ if verbose == "verbose" {
        ".venv/Scripts/ruff check --verbose ."
    } else if verbose == "true" {
        ".venv/Scripts/ruff check --output-format=concise ."
    } else if verbose == "false" {
        ".venv/Scripts/ruff check --quiet ."
    } else if verbose == "" {
        ".venv/Scripts/ruff check ."
    } else {
        error("not a valid input")
    } }}

ci:
    just ruff
    just mypy