#pragma once

#include <concepts>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    template<
        typename TParent,
        typename TBase = Construct,
        typename ...TArgs
    >
        requires std::derived_from<TParent, Construct>
            && std::derived_from<TBase, Construct>
    struct ConstructWithParent : TBase {
        explicit ConstructWithParent(TArgs... args) :
            TBase(args...) {
            //
        }

        [[nodiscard]] std::shared_ptr<TParent> forceGetUnboxedParent() {
            if (!ionshared::util::hasValue(this->parent)) {
                throw std::runtime_error("Parent is nullptr");
            }

            return this->parent->get()->template dynamicCast<TParent>();
        }
    };
}
