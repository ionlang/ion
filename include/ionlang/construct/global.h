#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/helpers.h>
#include "value.h"
#include "construct.h"
#include "type.h"

namespace ionlang {
    class Pass;

    class Global : public Construct, public ionshared::Named {
    private:
        ionshared::Ptr<Type> type;

        std::string id;

        ionshared::OptPtr<Value<>> value;

    public:
        Global(ionshared::Ptr<Type> type, std::string id, ionshared::OptPtr<Value<>> value = std::nullopt);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::Ptr<Type> getType() const noexcept;

        void setType(ionshared::Ptr<Type> type) noexcept;

        ionshared::OptPtr<Value<>> getValue() const noexcept;

        void setValue(ionshared::OptPtr<Value<>> value) noexcept;
    };
}
