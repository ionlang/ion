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

        void set(const ionshared::Ptr<Construct> &key, const ionshared::Ptr<ionir::Construct> &value);

        [[nodiscard]] bool contains(ionshared::Ptr<Construct> key) const;

        template<typename T = ionir::Construct>
        [[nodiscard]] std::optional<ionshared::Ptr<T>> find(const ionshared::Ptr<Construct> &construct) {
            if (this->contains(construct)) {
                ionshared::Ptr<T> castResult = this->entities.lookup(construct)->get()->dynamicCast<T>();

                if (castResult != nullptr) {
                    return castResult;
                }

                throw std::runtime_error("LLVM value cast failed");
            }

            return std::nullopt;
        }
    };
}
