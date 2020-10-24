#include <ionlang/passes/pass.h>

namespace ionlang {
    std::shared_ptr<Struct> Struct::make(
        const std::string& name,
        const Fields& fields
    ) noexcept {
        std::shared_ptr<Struct> result =
            std::make_shared<Struct>(name, fields);

        auto fieldsNativeMap = fields->unwrap();

        for (const auto& [name, type] : fieldsNativeMap) {
            type->parent = result;
        }

        return result;
    }

    Struct::Struct(std::string name, Fields fields) :
        ConstructWithParent<Module>(ConstructKind::Struct),
        ionshared::Named{std::move(name)},
        fields(std::move(fields)) {
        //
    }

    void Struct::accept(Pass& visitor) {
        visitor.visitStruct(this->dynamicCast<Struct>());
    }

    Ast Struct::getChildNodes() {
        Ast children = {};
        auto fieldsMap = this->fields->unwrap();

        // TODO: What about the field name?
        for (const auto &[name, type] : fieldsMap) {
            children.push_back(type);
        }

        return children;
    }
}
