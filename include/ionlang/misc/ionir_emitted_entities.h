#pragma once

#include <ionshared/container/map.h>
#include <ionir/construct/construct.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    typedef ionshared::Map<ionshared::Ptr<Construct>, ionshared::Ptr<ionir::Construct>> ConstructSymbolTable;

    class IonIrEmittedEntities {
    private:
        ConstructSymbolTable entities;

    public:
        IonIrEmittedEntities();

        void set(
            const ionshared::Ptr<Construct> &key,
            const ionshared::Ptr<ionir::Construct> &value
        );

        [[nodiscard]] bool contains(ionshared::Ptr<Construct> key) const;

        template<typename T = ionir::Construct>
            requires std::derived_from<T, ionir::Construct>
        [[nodiscard]] std::optional<ionshared::Ptr<T>> find(
            const ionshared::Ptr<Construct> &construct,
            bool useDynamicCast = true
        ) {
            if (this->contains(construct)) {
                ionshared::Ptr<ionir::Construct> lookupResult =
                    *this->entities.lookup(construct);

                /**
                 * NOTE: Casting methods already check that casting
                 * result isn't nullptr.
                 */
                return useDynamicCast
                    ? lookupResult->dynamicCast<T>()
                    : lookupResult->staticCast<T>();
            }

            return std::nullopt;
        }
    };
}
