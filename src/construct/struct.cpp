#include <ionlang/passes/pass.h>

namespace ionlang {
    Struct::Struct(ionshared::Ptr<Module> parent, std::string name, Fields fields) :
        ConstructWithParent<Module>(std::move(parent), ConstructKind::Struct),
        ionshared::Named{std::move(name)},
        fields(std::move(fields)) {
        //
    }

    void Struct::accept(Pass &visitor) {
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

    bool Struct::containsField(std::string name) const {
        return this->fields->contains(std::move(name));
    }

    ionshared::OptPtr<Type> Struct::lookupField(std::string name) {
        return this->fields->lookup(std::move(name));
    }

    void Struct::setField(std::string name, ionshared::Ptr<Type> field) {
        this->fields->set(std::move(name), std::move(field));
    }
}
