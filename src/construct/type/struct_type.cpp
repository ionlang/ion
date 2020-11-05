#include <ionlang/passes/pass.h>

#include <utility>

namespace ionlang {
    std::shared_ptr<StructType> StructType::make(
        const std::string& name,
        const Fields& fields
    ) noexcept {
        std::shared_ptr<StructType> result =
            std::make_shared<StructType>(name, fields);

        auto fieldsNativeMap = fields->unwrap();

        for (const auto& [name, type] : fieldsNativeMap) {
            type->parent = result;
        }

        return result;
    }

    StructType::StructType(std::string name, Fields fields) :
        ConstructWithParent<Module, Type, std::string, TypeKind>(std::move(name), TypeKind::Struct),
        fields(std::move(fields)) {
        //
    }

    void StructType::accept(Pass& visitor) {
        visitor.visitStructType(this->dynamicCast<StructType>());
    }

    Ast StructType::getChildNodes() {
        Ast children = {};
        auto fieldsMap = this->fields->unwrap();

        // TODO: What about the field name?
        for (const auto &[name, type] : fieldsMap) {
            children.push_back(type);
        }

        return children;
    }
}
