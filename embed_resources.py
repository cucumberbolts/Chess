"""
A script that embeds resouces to resources.cpp

... waiting for C++23 to be standardized so I can just use #embed...
"""

import os

# Embed text files as const char* strings
TEXT_FILES = [".glsl", ".ini"]

RESOURCE_DIR = "resources"
RESOURCE_EMBED_FILE = "src/Resources.h"

# Files that are not included in the resource file
IGNORE = ["fonts/Roboto/LICENSE.txt"]


def should_ignore(path: str) -> bool:
    """ Checks if path is in IGNORE """

    for file in IGNORE:
        if os.path.samefile(path, os.path.join(RESOURCE_DIR, file)):
            return True

    return False


def main():
    """ Main function """

    resource_size = 0
    output_str = "#pragma once\n\n"

    for folder, _, file_names in os.walk(RESOURCE_DIR):
        if not file_names:
            continue

        # Replace the slashes with "::" for C++ namespaces
        namespace = folder.replace(os.sep, "::").title()

        # Open namespace
        output_str += f"namespace {namespace} {{\n\n"

        for file in file_names:
            resource_path = os.path.join(folder, file)

            if should_ignore(resource_path):
                print(f"Ignoring {resource_path}")
                continue

            print(f"Embedding {resource_path}")

            # Gets the name for the variable
            variable = os.path.splitext(file)[0].upper()

            # Copies text files as text
            # Not strictly necessary, but more readable
            if os.path.splitext(file)[1] in TEXT_FILES:
                # Gets the data from the resource file
                data = None
                with open(resource_path, "r") as reader:
                    data = reader.read()

                resource_size += len(data)

                output_str += f"inline const char* {variable} = R\"(\n"
                output_str += data
                output_str += ")\";\n\n"
            else:
                # Gets the data from the resource file
                data = None
                with open(resource_path, "rb") as reader:
                    data = reader.read()

                resource_size += len(data)

                output_str += f"inline constexpr uint8_t {variable}[{len(data)}] = {{"

                for i in range(len(data)):
                    if i % 16 == 0:
                        output_str += f"\n    0x{data[i]:02x}, "
                    else:
                        output_str += f" 0x{data[i]:02x},"

                output_str += "\n};\n\n"

        # Close the namespace
        output_str += "}\n\n"

    print(f"Size of resources: {resource_size} bytes ({(resource_size / 1024):.2f} kilobytes)")

    with open(RESOURCE_EMBED_FILE, "w") as writer:
        writer.write(output_str)


if __name__ == "__main__":
    main()
