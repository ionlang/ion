#include <ionlang/passes/pass.h>

namespace ionlang {
    Identifier::Identifier(
        std::string baseName,
        std::vector<std::string> scopePath
    ) noexcept :
        Construct(ConstructKind::Identifier),
        baseName(std::move(baseName)),
        scopePath(std::move(scopePath)) {
        //
    }

    Identifier::operator std::string() const {
        std::stringstream stream{};

        stream << this->baseName;

        for (const auto& scopePath : this->scopePath) {
            // TODO: Avoid having it hard-coded.
            stream << "::" << scopePath;
        }

        return stream.str();
    }

    std::string Identifier::operator*() const {
        return this->operator std::string();
    }

    void Identifier::accept(Pass& visitor) {
        visitor.visitIdentifier(this->dynamicCast<Identifier>());
    }
}
