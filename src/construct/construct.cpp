#include <ionlang/const/const.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Construct::Construct(ConstructKind kind) :
        ionshared::BaseConstruct<Construct, ConstructKind>(kind) {
        //
    }

    void Construct::accept(ionshared::BasePass<Construct> visitor) {
        // TODO
    }

    Ast Construct::getChildNodes() {
        // By default, construct contains no children.
        return {};
    }

    bool Construct::verify() {
        Ast children = this->getChildNodes();

        for (const auto &child : children) {
            if (!child->verify()) {
                return false;
            }
        }

        return true;
    }

    std::optional<ionshared::SourceLocation> Construct::getSourceLocation() const noexcept {
        return this->sourceLocation;
    }

    bool Construct::hasSourceLocation() const noexcept {
        return this->sourceLocation.has_value();
    }

    void Construct::setSourceLocation(std::optional<ionshared::SourceLocation> sourceLocation) noexcept {
        this->sourceLocation = sourceLocation;
    }

    std::optional<std::string> Construct::getConstructName() {
        return Const::getConstructKindName(this->getConstructKind());
    }
}
