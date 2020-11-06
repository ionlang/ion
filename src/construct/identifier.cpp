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

        for (const auto& scopePath : this->scopePath) {
            // TODO: Avoid having it hard-coded.
            stream << scopePath << "::";
        }

        stream << this->name;

        return stream.str();
    }

    std::string Identifier::operator*() const {
        return this->operator std::string();
    }

    void Identifier::accept(Pass& visitor) {
        visitor.visitIdentifier(this->dynamicCast<Identifier>());
    }
}
