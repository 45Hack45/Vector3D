import os
import argparse
from pathlib import Path
from collections import defaultdict
import fnmatch

source_extensions = {".cpp", ".c"}
header_extensions = {".h", ".hpp"}
project_name = "vector_3d"
AUTO_SECTION_END = "# ## === AUTO-GENERATED SECTION ENDS HERE ==="

file_occurrences = defaultdict(list)
ignore_patterns = []


def load_ignore_file(ignore_path: Path):
    if ignore_path.exists():
        with open(ignore_path, "r") as f:
            for line in f:
                pattern = line.strip()
                if pattern and not pattern.startswith("#"):
                    ignore_patterns.append(pattern)


def should_ignore(rel_path: Path) -> bool:
    path_str = rel_path.as_posix()
    return any(fnmatch.fnmatch(path_str, pattern) for pattern in ignore_patterns)


def gather_files(base_dir: Path):
    grouped_files = defaultdict(lambda: {"sources": [], "headers": []})

    for root, _, files in os.walk(base_dir):
        rel_root = Path(root).relative_to(base_dir)
        group = rel_root.parts[0] if rel_root.parts else "core"

        for file in files:
            ext = Path(file).suffix
            if ext not in source_extensions | header_extensions:
                continue

            full_path = Path(root) / file
            rel_path = full_path.relative_to(base_dir)

            file_occurrences[file].append(rel_path)
            cmake_path = f"${{CMAKE_CURRENT_SOURCE_DIR}}/{rel_path.as_posix()}"

            if should_ignore(rel_path):
                cmake_path = f"# {cmake_path}"

            if ext in source_extensions:
                grouped_files[group]["sources"].append((rel_path.as_posix(), cmake_path))
            else:
                grouped_files[group]["headers"].append((rel_path.as_posix(), cmake_path))

    for group_data in grouped_files.values():
        group_data["sources"].sort()
        group_data["headers"].sort()

    return grouped_files


def read_existing_manual_tail(output_path: Path) -> str:
    if not output_path.exists():
        return ""
    with open(output_path, "r") as f:
        content = f.read()
        if AUTO_SECTION_END in content:
            return content.split(AUTO_SECTION_END, 1)[1].lstrip()
    return ""


def write_cmakelists(grouped_files, output_path: Path):
    manual_tail = read_existing_manual_tail(output_path)

    with open(output_path, "w") as f:
        f.write(f'message(STATUS "-- Generating Core Engine")\n\n')
        all_group_vars = []

        for group, files in grouped_files.items():
            sources_var = f"{project_name}_{group}_SOURCES"
            headers_var = f"{project_name}_{group}_HEADERS"
            all_var = f"{project_name}_{group}_FILES"
            all_group_vars.append(all_var)

            f.write(f'message(STATUS "-- {group.capitalize()}")\n\n')

            f.write(f"set({sources_var}\n")
            for _, src in files["sources"]:
                f.write(f"    {src}\n")
            f.write(")\n\n")

            f.write(f"set({headers_var}\n")
            for _, hdr in files["headers"]:
                f.write(f"    {hdr}\n")
            f.write(")\n\n")

            f.write(f"set({all_var}\n")
            f.write(f"    ${{{sources_var}}}\n")
            f.write(f"    ${{{headers_var}}}\n")
            f.write(")\n\n")

        f.write('message(STATUS "-- --")\n\n')
        f.write(f"set({project_name}_FILES\n")
        for var in all_group_vars:
            f.write(f"    ${{{var}}}\n")
        f.write(")\n\n")

        f.write(f"{AUTO_SECTION_END}\n\n")

        if manual_tail:
            f.write(manual_tail)
        else:
            # Provide default tail if nothing existed
            f.write(f'message(STATUS "-- Adding Core Engine Library")\n\n')
            f.write(f"add_library({project_name} ${{{project_name}_FILES}})\n")
            f.write("target_include_directories(vector_3d PUBLIC\n")
            f.write("    ${ENGINE_INCLUDE}\n")
            f.write(")\n\n")
            f.write('target_compile_definitions(vector_3d PUBLIC "CHRONO_DATA_DIR=\\"${CHRONO_DATA_DIR}\\"")\n\n')
            f.write("if(MSVC)\n")
            f.write('    set_target_properties(vector_3d PROPERTIES MSVC_RUNTIME_LIBRARY "${CHRONO_MSVC_RUNTIME_LIBRARY}")\n')
            f.write("endif()\n\n")
            f.write("target_link_libraries(vector_3d PUBLIC ${vector_3d_LIBS})\n")


def main():
    parser = argparse.ArgumentParser(description="Generate a CMakeLists.txt for the Vector 3D engine.")
    parser.add_argument("--dir", type=str, default=".", help="Base directory for source files.")
    parser.add_argument("--output", type=str, default="CMakeLists.txt", help="Output path for CMakeLists.txt")
    parser.add_argument("--ignore-file", type=str, default=".cmakeignore", help="Ignore file with glob patterns.")
    args = parser.parse_args()

    base_path = Path(args.dir).resolve()
    output_path = Path(args.output).resolve()
    ignore_path = base_path / args.ignore_file

    print(f"📁 Base directory: {base_path}")
    print(f"📄 Generating CMakeLists at: {output_path}")
    load_ignore_file(ignore_path)

    grouped_files = gather_files(base_path)
    write_cmakelists(grouped_files, output_path)
    print("✅ Generation complete.")


if __name__ == "__main__":
    main()
