#ifndef JNIF_DOMINATOR_HPP
#define JNIF_DOMINATOR_HPP

namespace jnif {

    typedef std::map<BasicBlock*, std::set<BasicBlock*> > DomMap;

    template <class TDir>
    struct Dom : DomMap {

        Dom(const ControlFlowGraph& cfg) {
            for (BasicBlock* bb : cfg) {
                (*this)[bb].insert(cfg.basicBlocks.begin(), cfg.basicBlocks.end());
            }

            bool changed = true;
            while (changed) {
                changed = false;

                for (BasicBlock* bb : cfg) {
                    std::set<BasicBlock*> ns;
                    for (BasicBlock* p : TDir::dir(bb)) {
                        if (ns.empty()) {
                            ns = (*this)[p];
                        } else {
                            for (BasicBlock* bbp : ns) {
                                if ((*this)[p].count(bbp) == 0) {
                                    ns.erase(bbp);
                                }
                            }
                        }
                    }
                    ns.insert(bb);

                    if (ns != (*this)[bb]) {
                        changed = true;
                        (*this)[bb] = ns;
                    }
                }
            }

        }
    };

    template <class TDir>
    struct SDom : Dom<TDir> {
        SDom(const ControlFlowGraph& cfg) : Dom<TDir>(cfg) {
            int count = this->erase(TDir::start(cfg));
            JnifError::assert(count == 1, "count");
            for (auto& d : *this) {
                d.second.erase(d.first);
            }
        }
    };

    template <class TDir>
    struct IDom : DomMap {
        IDom(SDom<TDir>& ds) {
            for (const std::pair<BasicBlock*, std::set<BasicBlock*> >& d : ds) {
                std::set<BasicBlock*> sdomBy = d.second;
                for (BasicBlock* bb : sdomBy) {
                    for (BasicBlock* dd : ds[bb]) {
                        sdomBy.erase(dd);
                    }
                }

                JnifError::assert(sdomBy.size() == 1, "dom size: ", sdomBy.size());
                (*this)[*sdomBy.begin()].insert(d.first);
            }
        }
    };

    struct Forward {
        static std::vector<BasicBlock*>& dir(BasicBlock* bb) { return bb->ins; }
        static BasicBlock* start(const ControlFlowGraph& cfg) { return cfg.entry; }
    };

    struct Backward {
        static std::vector<BasicBlock*>& dir(BasicBlock* bb) {return bb->targets; }
        static BasicBlock* start(const ControlFlowGraph& cfg) { return cfg.exit; }
    };

    std::ostream& operator<<(std::ostream& os, const DomMap& ds) {
        for (const std::pair<BasicBlock*, std::set<BasicBlock*> >& d : ds) {
            os << d.first->name << ": ";
            for (const BasicBlock* dp : d.second) {
                os << "  " << dp->name << " ";
            }
            os << std::endl;
        }

        return os;
    }

}

#endif
