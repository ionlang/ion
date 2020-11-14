#include <ionlang/const/const.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    Construct::Construct(
        ConstructKind kind,
        std::optional<ionshared::SourceLocation> sourceLocation,
        ionshared::OptPtr<Construct> parent
    ) :
        ionshared::BaseConstruct<Construct, ConstructKind>(
            kind,
            sourceLocation,
            std::move(parent)
        ) {
        //
    }

    Ast Construct::getChildNodes() {
        // NOTE: By default, constructs contain no children.
        return {};
    }

    bool Construct::verify() {
        Ast children = this->getChildNodes();

        for (const auto& child : children) {
            if (!child->verify()) {
                return false;
            }
        }

        return true;
    }

    std::optional<std::string> Construct::findConstructName() {
        return Const::findConstructKindName(this->constructKind);
    }

    void ScopedConstruct::setParent(std::optional<std::shared_ptr<Construct>> parent) noexcept {
        BaseConstruct::setParent(parent);

        if (!ionshared::util::hasValue(parent)) {
            return;
        }

        this->traverseParents([&](auto parent) -> bool {
            /**
             * NOTE: Optional wrapper is used for compatibility, in this case
             * it should always contain a value if the cast succeeds.
             */
            if (std::optional<std::shared_ptr<Scoped>> scopedParent =
                std::dynamic_pointer_cast<Scoped>(parent)) {
                this->parentScope = *scopedParent;

                return false;
            }

            return true;
        });
    }
}
