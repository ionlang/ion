#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    template<typename T>
        // TODO: Require T : Construct;
    struct ChildConstructOpts {
        std::shared_ptr<T> parent;
    };

    template<class T = Construct>
        // TODO: Require T : Construct.
    struct ConstructWithParent : Construct {
        ConstructWithParent(std::shared_ptr<T> parent, ConstructKind kind) :
            Construct(kind, std::nullopt, parent) {
            //
        }

        [[nodiscard]] std::shared_ptr<T> getUnboxedParent() {
            if (!ionshared::util::hasValue(this->parent)) {
                throw std::runtime_error("Parent is nullptr");
            }

            return this->parent->get()->template dynamicCast<T>();
        }
    };
}
