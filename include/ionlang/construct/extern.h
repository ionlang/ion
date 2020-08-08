#pragma once

namespace ionlang {
    class Pass;

    class Extern : public Construct {
    private:
        ionshared::Ptr<Prototype> prototype;

    public:
        explicit Extern(ionshared::Ptr<Prototype> prototype);

        void accept(Pass &visitor) override;

        Ast getChildNodes() override;

        ionshared::Ptr<Prototype> getPrototype() const noexcept;

        void setPrototype(ionshared::Ptr<Prototype> prototype) noexcept;
    };
}
