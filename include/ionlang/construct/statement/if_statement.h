#pragma once

#include <ionlang/construct/statement.h>

namespace ionlang {
    class Pass;

    struct IfStatementOpts : StatementOpts {
        ionshared::Ptr<Construct> condition;

        ionshared::Ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;
    };

    class IfStatement : public Statement {
    private:
        ionshared::Ptr<Construct> condition;

        ionshared::Ptr<Block> consequentBlock;

        ionshared::OptPtr<Block> alternativeBlock;

    public:
        explicit IfStatement(const IfStatementOpts &opts);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        [[nodiscard]] ionshared::Ptr<Construct> getCondition() const noexcept;

        void setCondition(ionshared::Ptr<Construct> condition) noexcept;

        [[nodiscard]] ionshared::Ptr<Block> getConsequentBlock() const noexcept;

        void setConsequentBlock(ionshared::Ptr<Block> block) noexcept;

        [[nodiscard]] ionshared::OptPtr<Block> getAlternativeBlock() const noexcept;

        void setAlternativeBlock(ionshared::OptPtr<Block> block) noexcept;

        [[nodiscard]] bool hasAlternativeBlock() const noexcept;
    };
}
