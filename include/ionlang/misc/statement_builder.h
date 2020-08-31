#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/statement.h>
#include <ionlang/construct/block.h>
#include <ionlang/construct/type.h>

namespace ionlang {
    class StatementBuilder {
    private:
        ionshared::Ptr<Block> basicBlock;

    public:
        explicit StatementBuilder(ionshared::Ptr<Block> basicBlock);

        [[nodiscard]] ionshared::Ptr<Block> getSection() const noexcept;

        void insert(const ionshared::Ptr<Statement> &inst);

        template<class TInst, typename... TArgs>
        ionshared::Ptr<TInst> make(TArgs... args) {
            // TODO: Ensure T inherits from Inst or derived.

            ionshared::Ptr<TInst> inst = std::make_shared<TInst>(args...);

            this->insert(inst);

            return inst;
        }

        // TODO
//        ionshared::Ptr<ReturnInst> createReturn(ionshared::OptPtr<Value<>> value = std::nullopt);
    };
}
