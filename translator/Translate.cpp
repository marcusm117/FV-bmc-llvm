#include <iostream>
#include <sstream>
#include <string>
#include <llvm/Support/MemoryBuffer.h>
#include <llvm/Support/ErrorOr.h>
#include "llvm/IR/CFG.h"
#include <llvm/IR/Module.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/Bitcode/BitcodeWriter.h>
#include <llvm/Support/raw_ostream.h>
#include <json/json.h>

using namespace llvm;

// TODO: either make a class or output interoperable format for Python
int main(int argc, char *argv[]) {
    // Error and produce help message if user did not provide a filename to translate.
    if (argc != 2) {
      std::cerr << "Usage: " << argv[0] << "bitcode_filename" << std::endl;
      return 1;
    }

    StringRef filename = argv[1];
    LLVMContext context;

    ErrorOr<std::unique_ptr<MemoryBuffer>> fileOrErr =
      MemoryBuffer::getFileOrSTDIN(filename);
    // TODO: fix error handling
    /*
    if (std::error_code ec = fileOrErr.getError()) {
      std::cerr << " Error opening input file: " + ec.message() << std::endl;
      return 2;
    }
    */
    Expected<std::unique_ptr<Module>> moduleOrErr =
        parseBitcodeFile(fileOrErr.get()->getMemBufferRef(), context);
    /*
    if (std::error_code ec = fileOrErr.getError()) {
      std::cerr << "Error reading Moduule: " + ec.message() << std::endl;
      return 3;
    }
    */
    std::unique_ptr<Module> m (std::move(moduleOrErr.get()));
    std::cerr << "Successfully read Module:" << std::endl;
    std::cerr << " Name: " << m->getName().str() << std::endl;
    //std::cout << " Target triple: " << m->getTargetTriple() << std::endl;

    //  bb refers to the basic block number
    // globalVarState[std:make_pair("body", "y")] = Add(globalVarState[("l0","x")], 1)
    std::map< std::pair<std::string, std::string>, std::string> varState;
    // succ_bb[body] = { 2, 3, 5 }
    std::map<std::string, std::set<std::string>> succ_bb;
    std::map<std::string, std::set<std::string>> pred_bb;

    Json::Value data;
    Json::Value atoms(Json::arrayValue);
    std::cerr << "Global Variables: " << std::endl;
    for (Module::global_iterator iter = m->global_begin();
         iter != m->global_end(); iter++) {
        std::cerr << "   Name: " << iter->getName().str() << std::endl;
        atoms.append(Json::Value(iter->getName().str()));
    }
    data["atoms"] = atoms;

    int tmp_index = 0;
    int bb_index = 0;
    std::ostringstream sbuf;
    sbuf.str("");
    sbuf.clear();
    std::cerr << " Function: main" << std::endl;
    Function *mainFunc = m->getFunction("main");

    // Give each basic block a name.
    for (BasicBlock &BB : *mainFunc) {
        if (!BB.hasName()) {
            sbuf << "l" << bb_index++;
            BB.setName(sbuf.str());
            sbuf.str("");
            sbuf.clear();
        }
    }

    // Get the predecessors and successors of each basic block.
    for (BasicBlock &BB : *mainFunc) {
        std::string bb_name = BB.getName().str();
        Json::Value json_succs(Json::arrayValue);
        Json::Value json_preds(Json::arrayValue);
        for (BasicBlock *Succ : successors(&BB)) {
            std::string succ_name = Succ->getName().str();
            succ_bb[bb_name].insert(succ_name);
            json_succs.append(Json::Value(succ_name));
        }
        for (BasicBlock *Pred : predecessors(&BB)) {
            std::string pred_name = Pred->getName().str();
            pred_bb[bb_name].insert(pred_name);
            json_preds.append(Json::Value(pred_name));
        }
        data["succ_bb"][bb_name] = json_succs;
        data["pred_bb"][bb_name] = json_preds;

        // This code is now redundant
        /*
        if (I.getOpcode() == BinaryOperator::Br) {
            if (I.getNumOperands() == 1) {
                // unconditional jump
                std::string dest = I.getOperand(0)->getName().str();
                succ_bb[bb_name].insert(dest);
                pred_bb[dest].insert(bb_name);
            } else if (I.getNumOperands() == 3) {
                // conditional jump
                std::string left = I.getOperand(1)->getName().str();
                std::string right = I.getOperand(2)->getName().str();
                next_bb[bb_name].insert(left);
                next_bb[bb_name].insert(right);
                pred_bb[left].insert(bb_name);
                pred_bb[right].insert(bb_name);
            }
        }
        */
    }

    // Parse instructions to understand transition relations.
    for (BasicBlock &BB : *mainFunc) {
        std::string bb_name = BB.getName().str();
        std::cerr << bb_name << ":" << std::endl;

        // Loop over instructions in a basic block
        for (Instruction &I : BB) {
            auto opcode = I.getOpcode();

            // Assign names to tmp variables.
            if (opcode != BinaryOperator::Store &&
                opcode != BinaryOperator::Br) {
                if (!I.hasName()) {
                    sbuf << "t" << tmp_index++;
                    I.setName(sbuf.str());
                    sbuf.str("");
                    sbuf.clear();
                }

                for (int i=0; i < I.getNumOperands(); i++) {
                    auto operand = I.getOperand(i);
                    if (!operand->hasName()) {
                        sbuf << "t" << tmp_index++;
                        operand->setName(sbuf.str());
                        sbuf.str("");
                        sbuf.clear();
                    }
                }
            }
 
            // Process Binops.
            if (opcode == BinaryOperator::Add ||
                opcode == BinaryOperator::Mul ||
                opcode == BinaryOperator::Sub) {
                auto left_val = I.getOperand(0);
                auto right_val = I.getOperand(1);

                std::string dest = I.getName().str();
                std::string left;
                std::string right;

                if (ConstantInt* ci = dyn_cast<ConstantInt>(left_val)) {
                    left = std::to_string(ci->getSExtValue());
                } else {
                    left = left_val->getName().str();
                }
                if (ConstantInt* ci = dyn_cast<ConstantInt>(right_val)) {
                    right = std::to_string(ci->getSExtValue());
                } else {
                    right = right_val->getName().str();
                }
                
                data["tmp"][dest]["dest"] = dest;
                data["tmp"][dest]["left"] = left;
                data["tmp"][dest]["right"] = right;
                if (opcode == BinaryOperator::Add) {
                    data["tmp"][dest]["op"] = "Add";
                } else if (opcode == BinaryOperator::Mul) {
                    data["tmp"][dest]["op"] = "Mult";
                } else if (opcode == BinaryOperator::Sub) {
                    data["tmp"][dest]["op"] = "Sub";
                }
            } else if (opcode == BinaryOperator::Load) {
                auto val = I.getOperand(0);
                std::string dest = I.getName().str();
                std::string global;
                // loop over all possible pred_bb's to get an OR of all possible transitions we could be doing (i.e. all possible previous states of the global variable)
                data["tmp"][dest]["op"] = "Load";
                data["tmp"][dest]["dest"] = dest;
                data["tmp"][dest]["source"] = val->getName().str();
                data["tmp"][dest]["bb"] = data["pred_bb"][bb_name];
                for (auto iter : pred_bb[bb_name]) {
                    sbuf << "state[" << iter << "][" << val->getName().str() << "]";
                    global = sbuf.str();
                    sbuf.str("");
                    sbuf.clear();
                    std::cerr << dest << " == " << global << std::endl;
                }
            } else if (opcode == BinaryOperator::Store) {
                auto val = I.getOperand(0);
                auto dest_val = I.getOperand(1);
                std::string dest = dest_val->getName().str();
                std::string val_str;
                if (ConstantInt* ci = dyn_cast<ConstantInt>(val)) {
                    val_str = std::to_string(ci->getSExtValue());
                } else {
                    val_str = val->getName().str();
                }
                data["state"][dest][bb_name]["dest"] = dest;
                data["state"][dest][bb_name]["value"] = val_str;
                data["state"][dest][bb_name]["bb"] = bb_name;
                std::cerr << "state[" << bb_name << "][" << dest << "] == " << val_str << std::endl;
            } else {
                // ignore instruction
                // other opcodes:
                // https://llvm.org/doxygen/group__LLVMCCoreTypes.html
                // https://github.com/llvm-mirror/llvm/blob/master/include/llvm/IR/Instruction.def
            }
        } // endfor
    } //endfor

    std::cout << data << std::endl;
}

/*
int badmain(int argc, char *argv[]) {

  for (auto iter1 = m->getFunctionList().begin();
       iter1 != m->getFunctionList().end(); iter1++) {
    Function &f = *iter1;
    std::cout << " Function: " << f.getName().str() << std::endl;
    for (auto iter2 = f.getBasicBlockList().begin();
         iter2 != f.getBasicBlockList().end(); iter2++) {
      BasicBlock &bb = *iter2;
      std::cout << "  BasicBlock: " << bb.getName().str() << std::endl;
      for (auto iter3 = bb.begin(); iter3 != bb.end(); iter3++) {
        Instruction &inst = *iter3;
        std::cout << "   Instruction " << &inst << " : " << inst.getOpcodeName();

    unsigned int  i = 0;
    unsigned int opnt_cnt = inst.getNumOperands();
        for(; i < opnt_cnt; ++i)
        {
          Value *opnd = inst.getOperand(i);
          std::string o;
          //          raw_string_ostream os(o);
          //         opnd->print(os);
          //opnd->printAsOperand(os, true, m);
          if (opnd->hasName()) {
            o = opnd->getName();
            std::cout << " " << o << "," ;
          } else {
            std::cout << " ptr" << opnd << ",";
          }
        }
        std:: cout << std::endl;
      }
    }
  }
  return 0;
}
*/
