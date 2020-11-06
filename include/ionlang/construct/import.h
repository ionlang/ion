#pragma once

namespace ionlang {
    struct Pass;

    struct Import : Construct {
        static std::shared_ptr<Import> make(
            const std::shared_ptr<Identifier>& id
        ) noexcept;

        std::shared_ptr<Identifier> id;

        explicit Import(std::shared_ptr<Identifier> id);

        void accept(Pass& visitor) override;

        [[nodiscard]] Ast getChildNodes() override;
    };
}
