#ifndef QEQUINSTRUCTIONS_H
#define QEQUINSTRUCTIONS_H

// import string
#include <string>
// import library to use cout
#include <iostream>
#include <vector>
#include <map>
// library to get the stringstream
#include <sstream>

#include "fixpoint_instructions.h"

// enum of type of registers and the type they represent (src, typeSrc, dst, typeDst) after getting parsed
enum ParsedRegisterTwoOperands
{
    SRC,
    TYPEOP_SRC,
    DST,
    TYPEOP_DST
};

// unparsed vector of registers
enum InstructionPartsTwoOperands
{
    Mnemonic,
    SrcReg,
    DstReg
};

enum ParsedRegisterOneOperand
{
    REG,
    TYPEOP
};

enum InstructionPartsOneOperand
{
    MnemonicOneOperand,
    RegOneOperand
};

// base class to represent assembler generic Qinstructions; mov, add, sub, etc.

// abstract class QElement parent of QInstruction and QLabel
class QElement
{
public:
    // virtual print method
    virtual void print()
    {
        std::cout << "generic element" << std::endl;
    }

    // virtual method to return the mnemonic/type of the QElement
    virtual std::string getMnemonic()
    {
        return "generic";
    }

    // virtual method to receive a vector and fill it with the QElement's operands
    virtual void getOperands(std::vector<std::string> &operands) {}

    virtual std::unique_ptr<typeChecking::Mnemonic> createMnemonic() {}
};

class QSetType : public QElement
{
private:
    std::string type;
    std::string name;

public:
    QSetType(std::string type, std::string name) : type(type), name(name){};

    void print()
    {
        std::cout << " name: " << name << "type: " << type << std::endl;
    }

    std::string getMnemonic() override
    {
        return "settype";
    }

    void getOperands(std::vector<std::string> &operands) override
    {
        operands.push_back(type);
        operands.push_back(name);
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        // check if type string is "pointer" or "number" and set a variable to the correct type
        typeChecking::TypesOptions typeEnum;
        if (type == "pointer")
        {
            typeEnum = typeChecking::TypesOptions::POINTER;
        }
        else if (type == "number")
        {
            typeEnum = typeChecking::TypesOptions::NUMBER;
        }
        return std::make_unique<typeChecking::SetType>(name, typeEnum);
    }
};

// abstract class QInstruction

class QInstruction : public QElement
{
protected:
    // string src
    std::string src;
    // string dst
    std::string dst;

public:
    QInstruction(std::string src, std::string dst) : src(src), dst(dst){};

    // method to print the Qinstruction operands
    void print()
    {
        std::cout << "src: " << src << " dst: " << dst << std::endl;
    }

    std::string getMnemonic() override
    {
        return "instruction";
    }

    // method to receive a vector and fill it with the Qinstruction's operands
    void getOperands(std::vector<std::string> &operands) override
    {
        operands.push_back(dst);
        operands.push_back(src);
    }

    virtual std::unique_ptr<typeChecking::Mnemonic> createMnemonic() {}
};

class QAdd : public QInstruction
{
public:
    QAdd(std::string src, std::string dst) : QInstruction(src, dst){};

    void print()
    {
        std::cout << "add ";
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "add";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Add>(src, dst);
    }
};

class QSub : public QInstruction
{
public:
    QSub(std::string src, std::string dst) : QInstruction(src, dst){};

    void print()
    {
        std::cout << "sub ";
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "sub";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Sub>(src, dst);
    }
};

class QMul : public QInstruction
{
public:
    QMul(std::string src) : QInstruction(src, "r1"){};

    void print()
    {
        std::cout << "mul ";
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "mul";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Mul>(src);
    }
};

class QDiv : public QInstruction
{
public:
    QDiv(std::string src) : QInstruction(src, "r1"){};

    void print()
    {
        std::cout << "div ";
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "div";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Div>(src);
    }
};

class QLogicalOP : public QInstruction
{
public:
    QLogicalOP(std::string src, std::string dst) : QInstruction(src, dst){};

    void print()
    {
        std::cout << "logical op ";
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "logop";
    }
};

class QMov : public QInstruction
{

private:
    bool srcMemOp;
    bool dstMemOp;

public:
    QMov(std::string src, std::string srcType, std::string dst, std::string dstType) : QInstruction(src, dst)
    {
        if (srcType == "Mem")
        {
            srcMemOp = true;
        }
        else
        {
            srcMemOp = false;
        }

        if (dstType == "Mem")
        {
            dstMemOp = true;
        }
        else
        {
            dstMemOp = false;
        }
    };

    void print()
    {
        std::cout << "mov ";
        // print srcmemop and dstmemop
        std::cout << "srcMemOp: " << srcMemOp << " dstMemOp: " << dstMemOp << std::endl;
        QInstruction::print();
    }

    std::string getMnemonic() override
    {
        return "mov";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Mov>(src, srcMemOp, dst);
    }
};

class QJmp : public QInstruction
{
public:
    QJmp(std::string dst) : QInstruction("", dst){};

    void print()
    {
        std::cout << "jmp ";
        QInstruction::print();
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Nope>();
    }
};

class QLabel : public QElement
{
private:
    std::string label;

public:
    QLabel(std::string label) : label(label){};

    void print()
    {
        std::cout << "label: " << label << std::endl;
    }

    std::string getMnemonic() override
    {
        return "label";
    }

    std::unique_ptr<typeChecking::Mnemonic> createMnemonic() override
    {
        return std::make_unique<typeChecking::Nope>();
    }
};

// add declarations of handlers for each instruction

// chain of responsibility pattern for QElement abstract class -  the other cha
class QElementHandler
{
private:
    // QElementHandler next
    QElementHandler *next;
    // mnemonic
    std::string mnemonic;

public:
    // constructor
    QElementHandler(QElementHandler *next, std::string mnemonic) : next(next), mnemonic(mnemonic){};
    // method to handle the QElement, receives a vector of strings and a reference to a vector of QElements
    void handle(std::vector<std::string> tokens, std::vector<QElement *> &elements)
    {
        if (tokens[InstructionPartsTwoOperands::Mnemonic].find(mnemonic) != std::string::npos) // move to base class
        {
            // call the handler for the instruction
            handleInstruction(tokens, elements);
        }
        else if (next != nullptr) // if there is a next handler
        {
            // call the next handler
            next->handle(tokens, elements);
        }
        else
        {
            // throw an exception
            throw std::runtime_error("QElementHandler::handle() no handler found");
        }
    }

    // template method for handling the QElement
    virtual void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &elements)
    {
        // this should never be called
        std::cout << "QElementHandler::handleInstruction() should never be called" << std::endl;
        // throw an exception
        throw std::runtime_error("QElementHandler::handleInstruction() should never be called");
    }
};

class QLabelHandler : public QElementHandler
{
public:
    // constructor
    QLabelHandler(QElementHandler *next) : QElementHandler(next, "label"){};
    // method to handle the QLabel, receives a vector of strings and a reference to a vector of QElements
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &elements) override
    {
        // create a new QLabel and add it to the vector of QElements
        elements.push_back(new QLabel(tokens[1]));
    }
};

// chain of responsibility pattern for Qinstructions

class QInstructionHandler : public QElementHandler
{
private:
    // hash to keep track of registers of the given architecture and registers of QequArchitecture: (r1,r2, r3, r4 etc.)
    std::map<std::string, std::string> &registers;

    // hash to keep track of registers of the given architecture and registers of QequArchitecture: (r1,r2, r3, r4 etc.)
    // r0 is always the stack pointer

    // method to handle the registers of the given architecture and replace them with the QequArchitecture registers
    // receives a reference to a string
    // used by all the handlers

protected:
    void getRegTokens(std::string &reg, std::vector<std::string> &regTokens)
    {
        // register string is of the form {Reg/Mem}:RegName
        // split the string by the colon and save both elements in tokens vector
        std::stringstream ss(reg);
        std::string token;
        while (std::getline(ss, token, ':'))
        {
            regTokens.push_back(token);
        }
    }

    virtual void parseRegister(std::string &reg, std::vector<std::string> &regTokens)
    {
        std::vector<std::string> tokens;
        // parse the dst register
        getRegTokens(reg, tokens);
        // save it in the regTokens vector

        // convert the register into a QequArchitecture register
        reg = handle_architecture_registers(tokens[1]);
        regTokens.push_back(reg);
        regTokens.push_back(tokens[0]);
    }

    std::string handle_architecture_registers(std::string &reg)
    {
        // check if the register is in the hash
        if (registers.find(reg) != registers.end())
        {
            // if it is, replace it with the QequArchitecture register
            reg = registers[reg];
        }
        else
        {
            // if it is not, add it to the hash
            registers[reg] = "r" + std::to_string(registers.size());
            // replace the register with the QequArchitecture register
            reg = registers[reg];
        }

        return reg;
    }

public:
    // constructor
    QInstructionHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers, std::string mnemonic) : QElementHandler(next, mnemonic), registers(registers){};
    // method to handle the Qinstruction, receives a vector of strings and a reference to a vector of Qinstructions
};

/*

template <class QInstructionType>
class QOneRegisterHandler : public QInstructionHandler
{
public:
    QOneRegisterHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers, const char* mnemonic) : QInstructionHandler(next, registers, mnemonic){};

    std::unique_ptr<QInstruction> create(std::vector<std::string> &operands) override
    {
        return std::make_unique<QInstructionType>(operands[0]);
    }
};
*/

class QSetTypeHandler : public QInstructionHandler
{

public:
    // constructor gets the next handler and the registers hash
    QSetTypeHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "settype"){};
    // method handleInstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        std::string reg = handle_architecture_registers(tokens[InstructionPartsOneOperand::RegOneOperand]);
        // create a QSetType and add it to the vector of Qinstructions
        instructions.push_back(new QSetType(tokens[ParsedRegisterTwoOperands::DST], reg));
    }
};

// QAdd class

class QAddHandler : public QInstructionHandler
{
public:
    // constructor gets the next handler and the registers hash
    // instruction mnemonic por QInstructionHandler - "add"
    QAddHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "add"){};

    // method to handle the Qinstruction - handleInstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        std::cout << "add" << std::endl;
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::SrcReg], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::DstReg], regTokens);
        // print regTokens
        for (const auto &reg : regTokens)
        {
            std::cout << reg << std::endl;
        }
        // create a QAdd and add it to the vector of Qinstructions
        instructions.push_back(new QAdd(regTokens[ParsedRegisterTwoOperands::SRC], regTokens[ParsedRegisterTwoOperands::DST]));
    }
};

// QSub class

class QSubHandler : public QInstructionHandler
{
public:
    // constructor
    QSubHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "sub"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::SrcReg], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::DstReg], regTokens);
        // create a QSub and add it to the vector of Qinstructions
        instructions.push_back(new QSub(regTokens[ParsedRegisterTwoOperands::SRC], regTokens[ParsedRegisterTwoOperands::DST]));
    }
};

// QMul class

class QMulHandler : public QInstructionHandler
{
public:
    // constructor
    QMulHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "mul"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[InstructionPartsOneOperand::RegOneOperand], regTokens);
        // parseRegister(tokens[2], regTokens);

        // create a QMul and add it to the vector of Qinstructions
        instructions.push_back(new QMul(regTokens[ParsedRegisterOneOperand::REG]));
    }
};

// QDiv class

class QDivHandler : public QInstructionHandler
{
public:
    // constructor
    QDivHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "div"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[InstructionPartsOneOperand::RegOneOperand], regTokens);
        // parseRegister(tokens[2], regTokens);

        // create a QDiv and add it to the vector of Qinstructions
        instructions.push_back(new QDiv(regTokens[ParsedRegisterOneOperand::REG]));
    }
};

// QMov class

class QMovHandler : public QInstructionHandler
{
public:
    // constructor
    QMovHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "mov"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::SrcReg], regTokens);
        parseRegister(tokens[InstructionPartsTwoOperands::DstReg], regTokens);

        // create a QMov and add it to the vector of Qinstructions
        instructions.push_back(new QMov(regTokens[ParsedRegisterTwoOperands::SRC], regTokens[ParsedRegisterTwoOperands::TYPEOP_SRC], regTokens[ParsedRegisterTwoOperands::DST], regTokens[ParsedRegisterTwoOperands::TYPEOP_DST]));
    }
};

// lea instruction handler

class QLeaHandler : public QInstructionHandler
{
public:
    // constructor
    QLeaHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "lea"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[1], regTokens); // source base reg
        parseRegister(tokens[2], regTokens); // source index reg if there is a 4 token, else destination reg

        // check if there is a third register
        bool isIndexReg = false;
        if (tokens.size() == 4)
        {
            parseRegister(tokens[3], regTokens);
            isIndexReg = true;
        }

        // create the three instructions that compose a lea instruction: mul, add and mov
        if (isIndexReg)
        {
            QMul *mul = new QMul(regTokens[2]);
            instructions.push_back(mul);
        }
        // offset of regTokens is 2 if there is no index register, else it is 3
        QMov *mov = new QMov(regTokens[0], regTokens[1], regTokens[2 + isIndexReg], regTokens[2 + isIndexReg]);
        // add the instructions to the vector of Qinstructions
        instructions.push_back(mov);
    }
};

class QJmpHandler : public QInstructionHandler
{
private:
    bool checkMnemonic(std::string mnemonic)
    {
        // vector of mnemonics that are jumps
        std::vector<std::string> jumps = {"jmp", "je", "jne", "jg", "jge", "jl", "jle"};
        // check if the mnemonic is a jump
        for (std::string jump : jumps)
        {
            if (mnemonic == jump)
            {
                return true;
            }
        }
        return false;
    }

public:
    // constructor
    QJmpHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "jmp"){};
    // method to handle the Qinstruction
    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        // create a QJmp and add it to the vector of Qinstructions
        instructions.push_back(new QJmp(tokens[1]));
    }
};

// QLogicalOP class

class QLogicalOPHandler : public QInstructionHandler
{
private:
    bool checkMnemonic(std::string mnemonic)
    {
        // vector of strings mnemonics
        std::vector<std::string> mnemonics = {"and", "or", "xor", "not", "shl", "shr"};
        // iterate over the mnemonics
        for (std::string m : mnemonics)
        {
            // check if the mnemonic is equal to the current mnemonic
            if (m == mnemonic)
            {
                // return true
                return true;
            }
        }
        return false;
    }

public:
    // constructor
    QLogicalOPHandler(QInstructionHandler *next, std::map<std::string, std::string> &registers) : QInstructionHandler(next, registers, "and"){};
    // method to handle the Qinstruction

    void handleInstruction(std::vector<std::string> tokens, std::vector<QElement *> &instructions) override
    {
        std::cout << "tokens[0]: " << tokens[0] << std::endl;
        // create a QLogicalOP and add it to the vector of Qinstructions
        // vector to store the tokens of the registers
        std::vector<std::string> regTokens;
        // parse the registers
        // parseRegisters(tokens[1], tokens[2], regTokens);
        parseRegister(tokens[1], regTokens);
        parseRegister(tokens[2], regTokens);

        instructions.push_back(new QLogicalOP(regTokens[ParsedRegisterTwoOperands::SRC], regTokens[ParsedRegisterTwoOperands::DST]));
    }
};

class QProgram
{
private:
    // vector of Qinstructions or Qlabels
    std::vector<QElement *> instructions;

    // map to keep the relation between architecture registers and QequArchitecture registers
    std::map<std::string, std::string> registers = {
        {"rsp", "r0"},
        {"rax", "r1"},
        {"Imm", "r2"}};

public:
    // constructor initializing an empty vector of QInstruction
    QProgram() : instructions(){};
    // method to take a vector of vector of strings and convert it to a vector of Qinstructions
    void convertToQInstructionChain(std::vector<std::vector<std::string>> tokens)
    {
        // create the chain of responsibility starting with QAddHandler and add the registers map reference to every handler
        // QInstructionHandler *instructionshandler = new QAddHandler(new QSubHandler(new QMulHandler(new QDivHandler(new QMovHandler(new QLeaHandler (new QLogicalOPHandler(new QErrorInstructionHandler(NULL, registers), registers), registers), registers), registers), registers), registers), registers);
        QInstructionHandler *instructionshandler = new QAddHandler(new QSubHandler(new QMulHandler(new QDivHandler(new QMovHandler(new QLeaHandler(new QJmpHandler(new QLogicalOPHandler(nullptr, registers), registers), registers), registers), registers), registers), registers), registers);

        // iterate over the vector of vector of strings

        QSetTypeHandler *setTypeHandler = new QSetTypeHandler(instructionshandler, registers);

        QElementHandler *currentHandler = new QLabelHandler(setTypeHandler);

        for (auto token : tokens)
        {
            // call the handle method of the handler
            currentHandler->handle(token, instructions);
        }
    }

    void typeCheckProgram()
    {
        typeChecking::Program program;

        // have a vector of typeChecking::BasicBlock*
        std::vector<typeChecking::BasicBlock *> basicBlocks;

        // create a SetType for r2 register to be Number and a basic block for it
        typeChecking::BasicBlock *setTypeBlock = program.create_block();
        setTypeBlock->add(std::make_unique<typeChecking::SetType>("r2", typeChecking::TypesOptions::NUMBER));
        basicBlocks.push_back(setTypeBlock);

        for (QElement *instruction : instructions)
        {
            typeChecking::BasicBlock *bb = program.create_block();
            // put the newMnemonic in the vector
            bb->add(instruction->createMnemonic());
            basicBlocks.push_back(bb);
        }

        // iterate again over vector basicBlocks and call bb->add_successor with the next element of the vector
        for (int i = 0; i < basicBlocks.size() - 1; i++)
        {
            basicBlocks[i]->add_successor(basicBlocks[i + 1]);
        }

        // call program.set_entry with the first elment of the vector
        program.set_entry(basicBlocks[0]);
        // call program.set_exit with the last element of the vector
        program.set_exit(basicBlocks[basicBlocks.size() - 1]);

        auto fp = typeChecking::FixpointEngine<sparta::MonotonicFixpointIterator>(program);

        fp.run(typeChecking::AbstractEnvironment::top());

        // call get_exit_state of fp with the last basicblock of the vector
        // auto exitState = fp.get_exit_state_at(basicBlocks[basicBlocks.size() - 1]).get("r1") == typeChecking::NumberDomain(0);
        // std::cout << "exitState: " << exitState << std::endl;
    }

    void printRegisters()
    {
        for (auto register_ : registers)
        {
            std::cout << register_.first << " " << register_.second << std::endl;
        }
    }

    // print the Qinstructions
    void print()
    {
        for (auto instruction : instructions)
        {
            instruction->print();
        }
    }

    // method to get the instructions vector
    std::vector<QElement *> getInstructions()
    {
        return instructions;
    }
};

#endif
