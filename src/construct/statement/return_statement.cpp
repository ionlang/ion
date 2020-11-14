#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<ReturnStmt> ReturnStmt::make(
        ionshared::OptPtr<Expression<>> value
    ) noexcept {
        std::shared_ptr<ReturnStmt> result =
            std::make_shared<ReturnStmt>(value);

        if (ionshared::util::hasValue(value)) {
            value->get()->setParent(result);
        }

        return result;
    }

    ReturnStmt::ReturnStmt(ionshared::OptPtr<Expression<>> value) noexcept :
        Statement(StatementKind::Return),
        value(std::move(value)) {
        //
    }

    void ReturnStmt::accept(Pass& visitor) {
        visitor.visitReturnStmt(this->dynamicCast<ReturnStmt>());
    }

    Ast ReturnStmt::getChildNodes() {
        if (!this->hasValue()) {
            return Construct::getChildNodes();
        }

        return {
            *this->value
        };
    }

    bool ReturnStmt::hasValue() const noexcept {
        return ionshared::util::hasValue(this->value);
    }
}
