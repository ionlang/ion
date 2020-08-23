#pragma once

#include <string>
#include <ionshared/misc/helpers.h>
#include <ionlang/construct/value.h>
#include "ionlang/construct/statement.h"

namespace ionlang {
    class Pass;

    class VariableDecl : public Statement, public ionshared::Named {
    private:
        ionshared::Ptr<Type> type;

        ionshared::Ptr<Value<>> value;

    public:
        VariableDecl(
            ionshared::Ptr<Block> parent,
            ionshared::Ptr<Type> type,
            std::string id,
            ionshared::Ptr<Value<>> value
        );

        void accept(Pass &visitor) override;

        ionshared::Ptr<Type> getType() const noexcept;

        void setType(ionshared::Ptr<Type> type) noexcept;

        ionshared::Ptr<Value<>> getValue() const noexcept;

        void setValue(ionshared::Ptr<Value<>> value) noexcept;
    };
}
