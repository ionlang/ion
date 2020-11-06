#pragma once

#include <string>
#include <vector>
#include "construct.h"

namespace ionlang {
    struct Pass;

    struct Identifier : Construct {
        std::string baseName;

        std::vector<std::string> scopePath;

        explicit Identifier(
            std::string baseName,
            std::vector<std::string> scopePath = {}
        ) noexcept;

        [[nodiscard]] explicit operator std::string() const;

        [[nodiscard]] std::string operator*() const;

        void accept(Pass& visitor) override;
    };
}
