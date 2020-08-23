#include <ionlang/syntax/parser.h>

namespace ionlang {
    bool Parser::is(TokenKind tokenKind) noexcept {
        return this->stream.get().getKind() == tokenKind;
    }

    bool Parser::isNext(TokenKind tokenKind) {
        return this->stream.peek()->getKind() == tokenKind;
    }

    bool Parser::expect(TokenKind tokenKind) {
        if (!this->is(tokenKind)) {
            this->makeNotice("Expected token kind: " + std::to_string((int)tokenKind) + ", but got: " +
                std::to_string((int)this->stream.get().getKind()));

            return false;
        }

        return true;
    }

    bool Parser::skipOver(TokenKind tokenKind) {
        if (!this->expect(tokenKind)) {
            return false;
        }

        this->stream.skip();

        return true;
    }

    ionshared::NoticeFactory Parser::createNoticeFactory() noexcept {
        // Abstract current Token for easier access.
        Token token = this->stream.get();

        return ionshared::NoticeFactory(ionshared::NoticeContext{
            this->filePath,
            token.getLineNumber(),
            token.getStartPosition()
        });
    }

    std::nullopt_t Parser::makeNotice(std::string message, ionshared::NoticeType type) {
        this->noticeStack->push(this->createNoticeFactory().make(type, std::move(message)));

        return std::nullopt;
    }

    Parser::Parser(TokenStream stream, const ionshared::Ptr<ionshared::NoticeStack> &noticeStack, std::string filePath)
        : stream(std::move(stream)), noticeStack(noticeStack), noticeSentinel(std::make_shared<NoticeSentinel>(noticeStack)), filePath(std::move(filePath)) {
        //
    }

    ionshared::Ptr<NoticeSentinel> Parser::getNoticeSentinel() const {
        return this->noticeSentinel;
    }

    std::string Parser::getFilePath() const {
        return this->filePath;
    }

    ionshared::OptPtr<Construct> Parser::parseTopLevel(const ionshared::Ptr<Module> &parent) {
        switch (this->stream.get().getKind()) {
            case TokenKind::KeywordFunction: {
                return this->parseFunction(parent);
            }

            case TokenKind::KeywordGlobal: {
                return this->parseGlobal();
            }

            case TokenKind::KeywordExtern: {
                return this->parseExtern(parent);
            }

            default: {
                return this->makeNotice("Unknown top-level construct");
            }
        }
    }

    ionshared::OptPtr<Global> Parser::parseGlobal() {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::KeywordGlobal))

        ionshared::OptPtr<Type> type = this->parseType();

        IONIR_PARSER_ASSURE(type)

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)

        // TODO: Handle in-line initialization & pass std::optional<Value> into Global constructor.

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolSemiColon))

        return std::make_shared<Global>(*type, *id);
    }

    ionshared::OptPtr<Block> Parser::parseBasicBlock(ionshared::Ptr<FunctionBody> parent) {
        // TODO: NO AT (@)!!!! CRITICAL!
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolAt))

        std::optional<std::string> id = this->parseId();

        IONIR_PARSER_ASSURE(id)

        // TODO: NO COLONS!!! CRITICAL!
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolColon))

        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        ionshared::Ptr<Block> basicBlock = std::make_shared<Block>(BasicBlockOpts{
            std::move(parent),
            *id
        });

        std::vector<ionshared::Ptr<RegisterAssign>> registers = {};
        std::vector<ionshared::Ptr<Inst>> statements = {};
        ionshared::PtrSymbolTable<Inst> symbolTable = basicBlock->getSymbolTable();

        while (!this->is(TokenKind::SymbolBraceR) && !this->is(TokenKind::SymbolAt)) {
            AstPtrResult<Inst> inst;

            // TODO: This means that allocas without register assigns are possible (lonely, redundant allocas).
            // Register assignment. This includes an instruction.
            if (this->is(TokenKind::OperatorModulo)) {
                AstPtrResult<RegisterAssign> registerAssignResult = this->parseRegisterAssign(basicBlock);

                IONIR_PARSER_ASSERT_RESULT(registerAssignResult, BasicBlock)

                ionshared::Ptr<RegisterAssign> registerAssign = Util::getResultValue(registerAssignResult);

                inst = registerAssign->getValue()->dynamicCast<Inst>();

                /**
                 * Register the instruction on the resulting block's symbol
                 * table.
                 */
                symbolTable->insert(registerAssign->getId(), Util::getResultValue(inst));

                /**
                 * Add the register to the block's registers to be processed
                 * later for code-gen.
                 */
                registers.push_back(registerAssign);
            }
                // Otherwise, it must be just an instruction.
            else {
                inst = this->parseInst(basicBlock);
            }

            IONIR_PARSER_ASSURE(inst)
            statements.push_back(Util::getResultValue(inst));
        }

        this->stream.skip();
        basicBlock->setRegisters(registers);
        basicBlock->setInsts(statements);

        return basicBlock;
    }

    ionshared::OptPtr<FunctionBody> Parser::parseFunctionBody(ionshared::Ptr<Function> parent) {
        IONIR_PARSER_ASSERT(this->skipOver(TokenKind::SymbolBraceL))

        ionshared::Ptr<FunctionBody> functionBody = std::make_shared<FunctionBody>(parent);

        ionshared::PtrSymbolTable<Block> basicBlocks =
            std::make_shared<ionshared::SymbolTable<ionshared::Ptr<Block>>>();

        while (!this->is(TokenKind::SymbolBraceR)) {
            ionshared::OptPtr<Block> basicBlockResult = this->parseBasicBlock(functionBody);

            IONIR_PARSER_ASSURE(basicBlockResult)

            ionshared::Ptr<Block> basicBlock = *basicBlockResult;

            basicBlocks->insert(basicBlock->getId(), basicBlock);
        }

        functionBody->setSymbolTable(basicBlocks);

        // Skip over right brace token.
        this->stream.skip();

        return functionBody;
    }
}
