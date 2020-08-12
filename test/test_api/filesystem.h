#pragma once

#include <optional>
#include <string>

namespace ionlang::test::fs {
    /**
     * Resolves a path relative to the root directory
     * of the project.
     */
    std::string resolvePath(std::string path);

    /**
     * Joins two paths with a directory separator.
     */
    std::string joinPaths(std::string path1, std::string path2);

    /**
     * Reads the contents of a file if it
     * exists, otherwise returns null.
     */
    std::optional<std::string> readFileContents(std::string path);

    std::optional<std::string> readTestFile(std::string filePath);

    /**
     * Determines whether a file path exists.
     */
    bool exists(const std::string &filename);
}
