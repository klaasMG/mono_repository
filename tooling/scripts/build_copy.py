import shutil
from pathlib import Path
from sys import argv
from hashlib import sha256
import re

def get_highest_number_diff_file(directory: str) -> int | None:
    path = Path(directory)
    numbers = []
    for file in path.iterdir():
        if file.is_file():
            name = file.name
            i = len(name)
            while i > 0 and name[i-1].isdigit():
                i -= 1
            if i < len(name):  # Found digits at end
                numbers.append(int(name[i:]))
    return max(numbers) if numbers else None

def diff_list_builder(path: Path):
    file_data: list[str] = []
    for path_loop in path.iterdir():
        if path_loop.is_dir():
            sub_dir_list: list[str] = diff_list_builder(path_loop)
            file_data.extend(sub_dir_list)
        elif path_loop.is_file():
            file_data.append(f"-N {str(Path(path_loop).relative_to(Path.cwd()))}")
    return file_data

def diff_creation_func(is_new_diff_directory: bool):
    if is_new_diff_directory:
        path: Path = Path(".build/.diff")
        if path.exists() and (path.is_dir() and not any(path.iterdir())):
            raise Exception(f"Directory {path} already exists and has content")
        if path.exists() and (not path.is_dir()):
            raise Exception(f"Directory {path} is a file")
        file_name: str = "diff_1"
        file_data: list[str] = diff_list_builder(Path.cwd())
    else:
        pass
    with open(file_name, "a") as diff_file:
        for line in file_data:
            diff_file.write(f"{line}\n")

if __name__ == "__main__":
    args = argv[1:]
    if len(args) > 2:
        raise Exception("Too many arguments")
    if len(args) < 1:
        raise Exception("Too little arguments")
    base_dir: Path = Path(args[0])
    move_to_dir: Path = Path(args[1])
    for entry in base_dir.iterdir():
        if Path(entry) != move_to_dir or entry.name.startswith("cmake-build-"):
            shutil.copytree(src=entry, dst=move_to_dir, dirs_exist_ok=True)