#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    ErrorMarker::ErrorMarker(std::string message)
        : Construct(ConstructKind::ErrorMarker), message(std::move(message)) {
        //
    }

    void ErrorMarker::accept(Pass &visitor) {
        visitor.visitErrorMarker(this->dynamicCast<ErrorMarker>());
    }

    std::string ErrorMarker::getMessage() const noexcept {
        return this->message;
    }

    void ErrorMarker::setMessage(std::string message) noexcept {
        this->message = std::move(message);
    }
}
