#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    template<class T = Construct>
        // TODO: Require T : Construct.
    struct ConstructWithParent : Construct {
        explicit ConstructWithParent(ConstructKind kind) :
            Construct(kind) {
            //
        }

        [[nodiscard]] std::shared_ptr<T> forceGetUnboxedParent() {
            if (!ionshared::util::hasValue(this->parent)) {
                throw std::runtime_error("Parent is nullptr");
            }

            return this->parent->get()->template dynamicCast<T>();
        }
    };
}
