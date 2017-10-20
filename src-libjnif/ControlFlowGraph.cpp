/*
 * ControlFlowGraph.cpp
 *
 *  Created on: Jun 6, 2014
 *      Author: luigi
 */
#include <jnif.hpp>

using namespace std;

namespace jnif {

    static void addBasicBlock2(InstList::Iterator eit, InstList::Iterator& beginBb,
                               int& bbid, ControlFlowGraph& cfg) {
        if (beginBb != eit) {
            std::stringstream ss;
            ss << "BB" << bbid;
            string name = ss.str();

            cfg.addBasicBlock(beginBb, eit, name);

            beginBb = eit;
            bbid++;
        }
    }

    static void buildBasicBlocks(InstList& instList, ControlFlowGraph& cfg) {
        //setBranchTargets(instList);

        int bbid = 0;
        InstList::Iterator beginBb = instList.begin();

        for (InstList::Iterator it = instList.begin(); it != instList.end(); ++it) {
            Inst* inst = *it;

            if (inst->isLabel()
                && (inst->label()->isBranchTarget || inst->label()->isTryStart)) {
                addBasicBlock2(it, beginBb, bbid, cfg);
            }

            if (inst->isBranch() || inst->isExit()) {
                InstList::Iterator eit = it;
                ++eit;
                addBasicBlock2(eit, beginBb, bbid, cfg);
            }
        }
    }

    static void addTarget2(BasicBlock* bb, const Inst* inst, ControlFlowGraph& cfg) {
        JnifError::assert(inst->isLabel(), "Expected label instruction");
        int labelId = inst->label()->id;
        BasicBlock* tbbid = cfg.findBasicBlockOfLabel(labelId);
        bb->addTarget(tbbid);
    }

    static void buildCfg(InstList& instList, ControlFlowGraph& cfg) {
        buildBasicBlocks(instList, cfg);

        for (BasicBlock* bb : cfg) {
            if (bb->start == instList.end()) {
                JnifError::assert(bb->name == ControlFlowGraph::EntryName || bb->name == ControlFlowGraph::ExitName,
                                  "");
                JnifError::assert(bb->exit == instList.end(), "");
                continue;
            }

            InstList::Iterator e = bb->exit;
            --e;
            JnifError::assert(e != instList.end(), "");

            Inst* last = *e;

            if (bb->start == instList.begin()) {
                cfg.entry->addTarget(bb);
            }

            if (last->isJump()) {
                addTarget2(bb, last->jump()->label2, cfg);

                if (last->opcode != Opcode::GOTO) {
                    JnifError::assert(bb->next != NULL, "next bb is null");
                    bb->addTarget(bb->next);
                }
                bb->last = last;
            } else if (last->isTableSwitch()) {
                addTarget2(bb, last->ts()->def, cfg);

                for (Inst* target : last->ts()->targets) {
                    addTarget2(bb, target, cfg);
                }
                bb->last = last;
            } else if (last->isLookupSwitch()) {
                addTarget2(bb, last->ls()->defbyte, cfg);

                for (Inst* target : last->ls()->targets) {
                    addTarget2(bb, target, cfg);
                }
                bb->last = last;
            } else if (last->isExit()) {
                bb->addTarget(cfg.exit);
                bb->last = last;
            } else {
                JnifError::assert(bb->next != nullptr, "next bb is null");
                bb->addTarget(bb->next);
            }
        }
    }

    ControlFlowGraph::ControlFlowGraph(InstList& instList) :
            entry(addConstBb(instList, ".Entry")),
            exit(addConstBb(instList, ".Exit")),
            instList(instList) {
        buildCfg(instList, *this);

        // std::vector<BasicBlock*>::iterator it = basicBlocks.begin();
        // JnifError::assert((*it)->name == EntryName, "Invalid entry");
        // it++;
        // JnifError::assert((*it)->name == ExitName, "Invalid entry");

        // basicBlocks.push_back(*it);
        // basicBlocks.erase(it);
    }

    ControlFlowGraph::~ControlFlowGraph() {
        for (auto bb : *this) {
            delete bb;
        }
    }

    BasicBlock* ControlFlowGraph::addBasicBlock(InstList::Iterator start,
                                                InstList::Iterator end, const string& name) {
        BasicBlock* const bb = new BasicBlock(start, end, name, this);

        if (basicBlocks.size() > 0) {
            BasicBlock* prevbb = basicBlocks.back();
            prevbb->next = bb;
        }

        basicBlocks.push_back(bb);

        return bb;
    }

    BasicBlock* ControlFlowGraph::findBasicBlockOfLabel(int labelId) const {
        for (BasicBlock* bb : *this) {
            if (bb->start == instList.end()) {
                JnifError::assert(bb->name == EntryName || bb->name == ExitName, "");
                JnifError::assert(bb->exit == instList.end(), "");
                continue;
            }

            Inst* inst = *bb->start;
            if (inst->isLabel() && inst->label()->id == labelId) {
                return bb;
            }
        }

        throw JnifException("Invalid label id: ", labelId, " for the instruction list: ",
                            ", in cfg: ", *this, instList);
    }

    ControlFlowGraph::D ControlFlowGraph::dominance(BasicBlock* start) {
        ControlFlowGraph& cfg = *this;
        map<BasicBlock*, set<BasicBlock*> > ds;

        for (BasicBlock* bb : cfg) {
            ds[bb].insert(cfg.basicBlocks.begin(), cfg.basicBlocks.end());
        }

        bool changed = true;
        while (changed) {
            changed = false;

            for (BasicBlock* bb : cfg) {
                set<BasicBlock*> ns;
                for (BasicBlock* p : bb->targets) {
                    if (ns.empty()) {
                        ns = ds[p];
                    } else {
                        for (BasicBlock* bbp : ns) {
                            if (ds[p].count(bbp) == 0) {
                                ns.erase(bbp);
                            }
                        }
                    }
                }
                ns.insert(bb);

                if (ns != ds[bb]) {
                    changed = true;
                    ds[bb] = ns;
                }
            }
        }

        return ds;
    }

}
