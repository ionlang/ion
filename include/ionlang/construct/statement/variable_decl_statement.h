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

    struct VariableDeclStatement : Statement, ionshared::Named {
        ionshared::Ptr<Type> type;

        ionshared::Ptr<Construct> value;

        explicit VariableDeclStatement(const VariableDeclStatementOpts &opts);

        void accept(Pass &visitor) override;
    };
}
