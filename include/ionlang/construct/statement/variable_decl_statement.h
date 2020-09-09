#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/value.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    class Pass;

    struct VariableDeclStatementOpts : StatementOpts {
        ionshared::Ptr<Type> type;

        std::string id;

        ionshared::Ptr<Construct> value;
    };

    class VariableDeclStatement : public Statement, public ionshared::Named {
    private:
        ionshared::Ptr<Type> type;

        ionshared::Ptr<Construct> value;

    public:
        explicit VariableDeclStatement(const VariableDeclStatementOpts &opts);

        void accept(Pass &visitor) override;

        [[nodiscard]] ionshared::Ptr<Type> getType() const noexcept;

        void setType(ionshared::Ptr<Type> type) noexcept;

        [[nodiscard]] ionshared::Ptr<Construct> getValue() const noexcept;

        void setValue(ionshared::Ptr<Construct> value) noexcept;
    };
}
