#include <ionlang/passes/pass.h>
#include <ionlang/misc/statement_builder.h>

namespace ionlang {
    BasicBlock::BasicBlock(const BasicBlockOpts &opts)
        : ChildConstruct(opts.parent, ConstructKind::BasicBlock), ionshared::ScopeAnchor<Statement>(), kind(opts.kind), Named(opts.id), variableDecls(opts.variableDecls), statements(opts.statements) {
        //
    }

    void BasicBlock::accept(Pass &visitor) {
        visitor.visitScopeAnchor(this->dynamicCast<ionshared::ScopeAnchor<Construct>>());
        visitor.visitBasicBlock(this->dynamicCast<BasicBlock>());
    }

    Ast BasicBlock::getChildNodes() {
        return Construct::mergeChildren(
            Construct::convertChildren(this->variableDecls),
            Construct::convertChildren(this->statements)
        );
    }

    BasicBlockKind BasicBlock::getKind() const noexcept {
        return this->kind;
    }

    std::vector<ionshared::Ptr<VariableDeclaration>> &BasicBlock::getRegisters() noexcept {
        return this->variableDecls;
    }

    void BasicBlock::setVariableDecls(std::vector<ionshared::Ptr<VariableDeclaration>> variableDecls) {
        this->variableDecls = variableDecls;
    }

    std::vector<ionshared::Ptr<Statement>> &BasicBlock::getStatements() noexcept {
        return this->statements;
    }

    // TODO: SymbolTable must be re-populated after changing insts vector.
    void BasicBlock::setStatements(std::vector<ionshared::Ptr<Statement>> statements) {
        this->statements = std::move(statements);
    }

    void BasicBlock::insertStatement(const ionshared::Ptr<Statement> &statement) {
        this->statements.push_back(statement);

        // TODO
        std::optional<std::string> id = /*Util::getInstId(statement);*/std::nullopt;

        // Instruction is named. Register it in the symbol table.
        if (id.has_value()) {
            this->getSymbolTable()->insert(*id, statement);
        }
    }

    uint32_t BasicBlock::relocateStatements(BasicBlock &target, const uint32_t from) {
        uint32_t count = 0;

        for (uint32_t i = from; i < this->statements.size(); i++) {
            target.getStatements().push_back(this->statements[i]);
            this->statements.erase(this->statements.begin() + i - 1);
            count++;
        }

        return count;
    }

    std::optional<uint32_t> BasicBlock::locate(ionshared::Ptr<Statement> statement) const {
        return ionshared::Util::locateInVector<ionshared::Ptr<Statement>>(this->statements, std::move(statement));
    }

    ionshared::Ptr<StatementBuilder> BasicBlock::createBuilder() {
        return std::make_shared<StatementBuilder>(this->dynamicCast<BasicBlock>());
    }

    ionshared::OptPtr<Statement> BasicBlock::findTerminalStatement() const {
        for (const auto &statement : this->statements) {
            if (statement->isTerminal()) {
                return statement;
            }
        }

        return std::nullopt;
    }
}
