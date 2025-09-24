#!/usr/bin/env python3

import os
import sys
import fnmatch
import argparse
from pathlib import Path
from collections import defaultdict
from io import StringIO

AUTO_GENERATED_END = "# ## === AUTO-GENERATED SECTION ENDS HERE ==="
DEFAULT_EXTENSIONS = [".cpp", ".c", ".h", ".hpp"]

def log_error(msg):
    print(f"[ERROR] {msg}", file=sys.stderr)

def relative_cmake_path(path: Path):
    return "${CMAKE_SOURCE_DIR}/" + str(path.as_posix())

def read_ignore_file(path: Path):
    if not path.exists():
        return []
    with open(path, "r", encoding="utf-8") as f:
        return [line.strip() for line in f if line.strip() and not line.startswith("#")]

def should_ignore(path: Path, patterns):
    path_str = str(path.as_posix())
    return any(fnmatch.fnmatch(path_str, pattern) for pattern in patterns)

def collect_files(base_path: Path, ignore_patterns, allowed_exts):
    groups = defaultdict(lambda: {'sources': [], 'headers': []})
    for path in base_path.rglob("*"):
        if not path.is_file():
            continue
        rel_path = path.relative_to(base_path)

        if should_ignore(rel_path, ignore_patterns):
            continue

        ext = path.suffix.lower()
        if ext not in allowed_exts:
            continue

        if len(rel_path.parts) == 1:
            group = "core"
        else:
            group = rel_path.parts[0]

        if ext in [".cpp", ".c"]:
            groups[group]['sources'].append(path)
        elif ext in [".h", ".hpp"]:
            groups[group]['headers'].append(path)

    for group in groups.values():
        group['sources'].sort()
        group['headers'].sort()

    return groups

def write_group(f, group_name, sources, headers):
    f.write(f'message(STATUS "-- {group_name.upper()}")\n\n')

    f.write(f"set(vector_3d_{group_name}_SOURCES\n")
    for src in sources:
        f.write(f"    {relative_cmake_path(src)}\n")
    f.write(")\n\n")

    f.write(f"set(vector_3d_{group_name}_HEADERS\n")
    for hdr in headers:
        f.write(f"    {relative_cmake_path(hdr)}\n")
    f.write(")\n\n")

    f.write(f"set(vector_3d_{group_name}_FILES\n")
    f.write(f"    ${{vector_3d_{group_name}_SOURCES}}\n")
    f.write(f"    ${{vector_3d_{group_name}_HEADERS}}\n")
    f.write(")\n\n")

def parse_args():
    parser = argparse.ArgumentParser(description="Generate a CMakeLists.txt for the Vector 3D engine.")
    parser.add_argument("--dir", type=str, default=".", help="Base directory for source files.")
    parser.add_argument("--output", type=str, default="CMakeLists.txt", help="Output path for CMakeLists.txt")
    parser.add_argument("--ignore-file", type=str, default=".cmakeignore", help="Ignore file with glob patterns.")
    return parser.parse_args()

def main():
    args = parse_args()
    base_path = Path(args.dir)
    output_file = Path(args.output)
    ignore_file = Path(args.ignore_file)

    if not base_path.exists() or not base_path.is_dir():
        log_error(f"Directory '{base_path}' not found or is not a directory.")
        sys.exit(1)

    ignore_patterns = read_ignore_file(ignore_file)

    manual_part = ""
    if output_file.exists():
        with open(output_file, "r", encoding="utf-8") as f:
            content = f.read()
        if AUTO_GENERATED_END in content:
            manual_part = content.split(AUTO_GENERATED_END, 1)[1]
        else:
            manual_part = ""

    cmake_output = StringIO()
    cmake_output.write('message(STATUS "-- Generating Core Engine")\n\n')

    groups = collect_files(base_path, ignore_patterns, DEFAULT_EXTENSIONS)
    all_files = []

    print("[INFO] File grouping summary:")
    for group_name in sorted(groups.keys()):
        g = groups[group_name]
        write_group(cmake_output, group_name, g['sources'], g['headers'])
        print(f"[SUMMARY] Group '{group_name}': {len(g['sources'])} source(s), {len(g['headers'])} header(s)")
        all_files.append(f"${{vector_3d_{group_name}_FILES}}")

    cmake_output.write('message(STATUS "-- --")\n\n')
    cmake_output.write("set(vector_3d_FILES\n")
    for line in all_files:
        cmake_output.write(f"    {line}\n")
    cmake_output.write(")\n\n")

    cmake_output.write(f"{AUTO_GENERATED_END}")
    final_output = cmake_output.getvalue() + manual_part

    with open(output_file, "w", encoding="utf-8") as f:
        f.write(final_output)

    print(f"[INFO] Generated {output_file.resolve()} successfully.")

if __name__ == "__main__":
    main()
