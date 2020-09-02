#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/value.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    class Pass;

    // TODO: Use StatementOpts struct for constructor options.

    class VariableDecl : public Statement, public ionshared::Named {
    private:
        ionshared::Ptr<Type> type;

        ionshared::Ptr<Construct> value;

    public:
        VariableDecl(
            ionshared::Ptr<Block> parent,
            ionshared::Ptr<Type> type,
            std::string id,
            ionshared::Ptr<Construct> value
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] ionshared::Ptr<Type> getType() const noexcept;

        void setType(ionshared::Ptr<Type> type) noexcept;

        [[nodiscard]] ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
