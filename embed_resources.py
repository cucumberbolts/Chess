"""
A script that embeds resouces to resources.cpp

... waiting for C++23 to be standardized so I can just use #embed...
"""

import os

# Embed text files as const char* strings
TEXT_FILES = [".glsl", ".ini"]

RESOURCE_DIR = "resources"
RESOURCE_EMBED_FILE = "src/Resources.h"


def main():
    """ Main function """

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

            print(f"Embedding {resource_path}...")

            # Gets the name for the variable
            variable = os.path.splitext(file)[0].upper()

            # Copies text files as text
            # Not strictly necessary, but more readable
            if os.path.splitext(file)[1] in TEXT_FILES:
                # Gets the data from the resource file
                data = None
                with open(resource_path, "r") as reader:
                    data = reader.read()

                output_str += f"inline const char* {variable} = R\"(\n"
                output_str += data
                output_str += ")\";\n\n"
            else:
                # Gets the data from the resource file
                data = None
                with open(resource_path, "rb") as reader:
                    data = reader.read()

                output_str += f"inline constexpr uint8_t {variable}[{len(data)}] = {{"

                for i in range(len(data)):
                    if i % 16 == 0:
                        output_str += f"\n    0x{data[i]:02x}, "
                    else:
                        output_str += f" 0x{data[i]:02x},"

                output_str += "\n};\n\n"

        # Close the namespace
        output_str += "}\n\n"

    with open(RESOURCE_EMBED_FILE, "w") as writer:
        writer.write(output_str)


if __name__ == "__main__":
    main()
