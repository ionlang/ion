#pragma once

#include <ionshared/container/lowering_symbol_table.h>
#include <ionir/construct/basic_block.h>
#include <ionlang/misc/ionir_emitted_entities.h>
#include <ionlang/passes/pass.h>

namespace ionlang {
    class IonIrLoweringPass : public Pass {
    private:
        struct IonIrBuffers {
            ionshared::Stack<std::shared_ptr<ionir::Module>> modules{};

            ionshared::Stack<std::shared_ptr<ionir::Function>> functions{};

            ionshared::Stack<std::shared_ptr<ionir::BasicBlock>> basicBlocks{};

            std::shared_ptr<ionir::InstBuilder> makeBuilder();
        };

        [[nodiscard]] static std::shared_ptr<ionir::Type> processTypeQualifiers(
            std::shared_ptr<ionir::Type> type,
            const std::shared_ptr<TypeQualifiers> &qualifiers
        );

        ionshared::PtrSymbolTable<ionir::Module> modules;

        IonIrBuffers irBuffers;

        ionshared::LoweringSymbolTable<std::shared_ptr<Construct>, std::shared_ptr<ionir::Construct>> symbolTable;

        uint32_t nameCounter;

        /**
         * Backup current buffers, invoke the callback and restore the
         * stashed buffers.
         */
        void stashBuffers(const std::function<void()>& callback);

        [[nodiscard]] uint32_t getNameCounter() noexcept;

        /**
         * Visit and emit a construct if it has not been already
         * previously visited and emitted, and return the resulting
         * lowered construct by looking it up on the local symbol table.
         * By default, buffers are stashed during visitation of the
         * construct and restored afterwards to avoid being overwritten.
         */
        template<typename T = ionir::Construct>
            requires std::derived_from<T, ionir::Construct>
        std::shared_ptr<T> safeEarlyVisitOrLookup(
            const std::shared_ptr<Construct>& construct,
            bool useDynamicCast = true,
            bool stashBuffers = true
        ) {
            /**
             * NOTE: If specified, buffers must be stashed and restored after
             * visitation, as the construct could be anything, including a block
             * or a function, which, among others, alter buffers when being lowered.
             */
            if (stashBuffers) {
                this->stashBuffers([&, this] {
                    /**
                     * NOTE: Construct is protected from being emitted more
                     * than once during this call.
                     */
                    this->visit(construct);
                });
            }
            else {
                this->visit(construct);
            }

            if (!this->symbolTable.contains(construct)) {
                throw std::runtime_error("Visiting construct did not create an entry in the local symbol table");
            }

            // TODO: Fix.
//            return *this->symbolTable.find<T>(construct, useDynamicCast);
            std::optional<std::shared_ptr<ionir::Construct>> lookupResult =
                this->symbolTable.unwrap().lookup(construct);

            if (!ionshared::util::hasValue(lookupResult)) {
                throw std::runtime_error("Value lookup failed");
            }

            return useDynamicCast
                ? lookupResult->get()->dynamicCast<T>()
                : lookupResult->get()->staticCast<T>();
        }

    public:
        explicit IonIrLoweringPass(
            std::shared_ptr<ionshared::PassContext> context,

            ionshared::PtrSymbolTable<ionir::Module> modules =
                std::make_shared<ionshared::SymbolTable<std::shared_ptr<ionir::Module>>>()
        );

        [[nodiscard]] std::shared_ptr<ionshared::SymbolTable<std::shared_ptr<ionir::Module>>> getModules() const;

        /**
         * Visit a construct for lowering. Will not visit children,
         * as they are already visited by the delegated visit methods.
         * Must be used as the default visiting method for constructs,
         * as it will prevent constructs from possibly being emitted more
         * than once (for example, when a construct is used earlier than
         * its official lowering point).
         */
        void visit(std::shared_ptr<Construct> construct) override;

        void visitModule(std::shared_ptr<Module> construct) override;

        void visitFunction(std::shared_ptr<Function> construct) override;

        void visitExtern(std::shared_ptr<Extern> construct) override;

        void visitPrototype(std::shared_ptr<Prototype> construct) override;

        void visitBlock(std::shared_ptr<Block> construct) override;

        void visitIntegerLiteral(std::shared_ptr<IntegerLiteral> construct) override;

        void visitCharLiteral(std::shared_ptr<CharLiteral> construct) override;

        void visitStringLiteral(std::shared_ptr<StringLiteral> construct) override;

        void visitBooleanLiteral(std::shared_ptr<BooleanLiteral> construct) override;

        void visitGlobal(std::shared_ptr<Global> construct) override;

        void visitIntegerType(std::shared_ptr<IntegerType> construct) override;

        void visitBooleanType(std::shared_ptr<BooleanType> construct) override;

        void visitVoidType(std::shared_ptr<VoidType> construct) override;

        void visitIfStatement(std::shared_ptr<IfStmt> construct) override;

        void visitReturnStatement(std::shared_ptr<ReturnStmt> construct) override;

        void visitAssignmentStatement(std::shared_ptr<AssignmentStmt> construct) override;

        void visitVariableDecl(std::shared_ptr<VariableDeclStmt> construct) override;

        void visitCallExpr(std::shared_ptr<CallExpr> construct) override;

        void visitOperationExpr(std::shared_ptr<OperationExpr> construct) override;

        void visitStruct(std::shared_ptr<Struct> construct) override;

        void visitStructDefinition(std::shared_ptr<StructDefinition> construct) override;

        // TODO: visitRef() if !isResolved() error, else this->visit(ref->getValue()); Is this a good idea? It may be required for example for if statement condition (if it's a ref). Investigate.
    };
}
