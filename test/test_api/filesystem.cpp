#include <fstream>
#include "filesystem.h"
#include "util.h"

namespace ionlang::test::fs {
    const std::string rootPath = "../..";

    const std::string testPath = "test";

    std::string resolvePath(std::string path) {
        return joinPaths(rootPath, path);
    }

    std::string resolveTestPath(std::string path) {
        return resolvePath(joinPaths(testPath, path));
    }

    std::string joinPaths(std::string path1, std::string path2) {
        // TODO: What if path1 contains a trailing directory separator?
        return path1 + "/" + path2;
    }

    std::optional<std::string> readFileContents(std::string filePath) {
        // Target file does not exist, do not continue.
        if (!exists(filePath)) {
            return std::nullopt;
        }

        // Open a stream to the file.
        std::ifstream stream = std::ifstream(filePath);

        // Read the file's content.
        std::string content = std::string(std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>());

        return content;
    }

    std::optional<std::string> readTestFile(std::string filePath) {
        std::string finalPath = resolveTestPath(filePath);

        return readFileContents(finalPath);
    }

    bool exists(const std::string &filePath) {
        std::ifstream file = std::ifstream(filePath.c_str());

        return (bool) file;
    }
}
