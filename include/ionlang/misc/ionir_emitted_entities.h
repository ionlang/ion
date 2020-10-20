#pragma once

#include <ionshared/container/map.h>
#include <ionir/construct/construct.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    typedef ionshared::Map<std::shared_ptr<Construct>, std::shared_ptr<ionir::Construct>> ConstructSymbolTable;

    class IonIrEmittedEntities {
    private:
        ConstructSymbolTable entities;

    public:
        IonIrEmittedEntities();

        void set(
            const std::shared_ptr<Construct> &key,
            const std::shared_ptr<ionir::Construct> &value
        );

        [[nodiscard]] bool contains(std::shared_ptr<Construct> key) const;

        template<typename T = ionir::Construct>
            requires std::derived_from<T, ionir::Construct>
        [[nodiscard]] std::optional<std::shared_ptr<T>> find(
            const std::shared_ptr<Construct> &construct,
            bool useDynamicCast = true
        ) {
            if (this->contains(construct)) {
                std::shared_ptr<ionir::Construct> lookupResult =
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
