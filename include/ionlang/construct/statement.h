#pragma once

#include <ionshared/misc/helpers.h>
#include <ionlang/construct/pseudo/child_construct.h>

namespace ionlang {
    class Pass;

    class BasicBlock;

    typedef ChildConstructOpts<BasicBlock> InstOpts;

    enum class StatementKind {
        Return
    };

    class Statement : public ChildConstruct<BasicBlock> {
    private:
        StatementKind kind;

        ionshared::OptPtr<Statement> yields;

    public:
        Statement(ionshared::Ptr<BasicBlock> parent, StatementKind kind, ionshared::OptPtr<Statement> yields = std::nullopt);

        virtual void accept(Pass &visitor) = 0;

        StatementKind getStatementKind() const noexcept;

        ionshared::OptPtr<Statement> getYields() const noexcept;

        bool isTerminal() const noexcept;
    };
}
