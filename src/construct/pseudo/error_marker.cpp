#include <utility>
#include <ionlang/passes/pass.h>

namespace ionlang {
    ErrorMarker::ErrorMarker(
        ionshared::Range range,
        std::optional<std::string> message
    ) :
        Construct(ConstructKind::ErrorMarker),
        range(range),
        message(std::move(message)) {
        //
    }

    void ErrorMarker::accept(Pass &visitor) {
        visitor.visitErrorMarker(this->dynamicCast<ErrorMarker>());
    }

    std::optional<std::string> ErrorMarker::getMessage() const noexcept {
        return this->message;
    }

    void ErrorMarker::setMessage(std::optional<std::string> message) noexcept {
        this->message = std::move(message);
    }

    bool ErrorMarker::hasMessage() const noexcept {
        return this->message.has_value();
    }

    void ErrorMarker::removeMessage() {
        this->setMessage(std::nullopt);
    }

    ionshared::Range ErrorMarker::getRange() const noexcept {
        return this->range;
    }

    void ErrorMarker::setRange(ionshared::Range range) noexcept {
        this->range = range;
    }
}
