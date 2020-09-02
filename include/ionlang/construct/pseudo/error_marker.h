#pragma once

#include <optional>
#include <string>
#include <ionshared/misc/range.h>
#include <ionlang/construct/construct.h>

namespace ionlang {
    // TODO: What if 'pass.h' is never included?
    class Pass;

    class ErrorMarker : public Construct {
    private:
        ionshared::Range range;

        std::optional<std::string> message;

    public:
        explicit ErrorMarker(
            ionshared::Range range,
            std::optional<std::string> message = std::nullopt
        );

        void accept(Pass &visitor) override;

        [[nodiscard]] std::optional<std::string> getMessage() const noexcept;

        void setMessage(std::optional<std::string> message) noexcept;

        [[nodiscard]] bool hasMessage() const noexcept;

        void removeMessage();

        [[nodiscard]] ionshared::Range getRange() const noexcept;

        void setRange(ionshared::Range range) noexcept;
    };
}
