#include <ionlang/const/const.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Construct::Construct(
        ConstructKind kind,
        std::optional<ionshared::SourceLocation> sourceLocation,
        ionshared::OptPtr<Construct> parent
    ) :
        ionshared::BaseConstruct<Construct, ConstructKind>(kind, std::move(parent)),
        sourceLocation(std::move(sourceLocation)) {
        //
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

    std::optional<std::string> Construct::findConstructName() {
        return Const::getConstructKindName(this->constructKind);
    }
}
