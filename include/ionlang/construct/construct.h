#pragma once

#include <ionshared/tracking/symbol_table.h>
#include <ionshared/construct/base_construct.h>

namespace ionlang {
    enum class ConstructKind {
        Type,

        FunctionBody,

        Prototype,

        Function,

        Extern,

        Global,

        BasicBlock,

        VariableRef,

        VariableDecl,

        StructDecl,

        Module,

        Ref,

        Value,

        Statement
    };

    class Construct;

    class Pass;

    typedef ionshared::Ast<Construct> Ast;

    class Construct : public ionshared::BaseConstruct<Construct, ConstructKind> {
    public:
        template<class T>
        static Ast convertChildren(std::vector<ionshared::Ptr<T>> vector) {
            // TODO: Ensure T is child of AstNode.
            Ast children = {};

            for (const auto item : vector) {
                children.push_back(item);
            }

            return children;
        }

        template<class T>
        static Ast convertChildren(ionshared::SymbolTable<T> symbolTable) {
            Ast children = {};

            for (const auto &[key, value] : symbolTable.unwrap()) {
                children.push_back(value);
            }

            return children;
        }

        template<typename TFirst, typename TSecond>
        static Ast mergeChildren(TFirst first, TSecond second) {
            Ast children = {};

            for (const auto item : first) {
                children.push_back(item);
            }

            for (const auto item : second) {
                children.push_back(item);
            }

            return children;
        }

        explicit Construct(ConstructKind kind);

        virtual void accept(Pass &visitor) = 0;

        virtual Ast getChildNodes();
    };
}
