/*
 * FrameGenerator.hpp
 *
 *  Created on: Jun 25, 2014
 *      Author: luigi
 */

#ifndef JNIF_ANALYSIS_FRAMEGENERATOR_HPP
#define JNIF_ANALYSIS_FRAMEGENERATOR_HPP

namespace jnif {

class FrameGenerator {
public:

	FrameGenerator(ClassFile& cf, IClassPath* classPath) :
			_attrIndex(ConstPool::NULLENTRY), _cf(cf), _classPath(classPath) {
	}

	void setCpIndex(Type& type, InstList& instList) {
		if (type.isObject()) {
			const string& className = type.getClassName();

      ConstPool::Index index = _cf.putClass(className.c_str());
			type.setCpIndex(index);

			if (!type.init) {
				JnifError::assert(type.uninit.newinst->isType(), "It is not type");
				LabelInst* l = instList.addLabel(type.uninit.newinst);
				type = TypeFactory::uninitType(-1, l);
			}
		}
	}

	void setCpIndex(Frame& frame, InstList& instList) {
      for (Frame::T& t : frame.lva) {
			setCpIndex(t.first, instList);
		}

      for (Frame::T& t : frame.stack) {
			setCpIndex(t.first, instList);
		}
	}

	void computeFrames(CodeAttr* code, Method* method) {
		for (auto it = code->attrs.begin(); it != code->attrs.end(); it++) {
			Attr* attr = *it;
			if (attr->kind == ATTR_SMT) {
				code->attrs.attrs.erase(it);
				break;
			}
		}

		if (_attrIndex == ConstPool::NULLENTRY) {
			_attrIndex = _cf.putUtf8("StackMapTable");
		}

		Frame initFrame;

		u4 lvindex;
		if (method->isStatic()) {
			lvindex = 0;
		} else {
			//|| method->isClassInit()
			string className = _cf.getThisClassName();
			if (method->isInit()) {
          Type u = TypeFactory::uninitThisType();
				u.init = false;
				u.typeId = Type::nextTypeId;
				u.className = className;
				Type::nextTypeId++;
				initFrame.setVar2(0, u, nullptr);
			} else {
          initFrame.setRefVar(0, className, nullptr);
			}

			lvindex = 1;
		}

		const char* methodDesc = _cf.getUtf8(method->descIndex);
		std::vector<Type> argsType;
    TypeFactory::fromMethodDesc(methodDesc, &argsType);

		for (Type t : argsType) {
        initFrame.setVar(&lvindex, t, nullptr);
		}

		ControlFlowGraph* cfgp = new ControlFlowGraph(code->instList);
		code->cfg = cfgp;

		ControlFlowGraph& cfg = *cfgp;

		initFrame.valid = true;
		BasicBlock* bbe = cfg.entry;
		bbe->in = initFrame;
		bbe->out = initFrame;

		BasicBlock* to = *cfg.entry->begin();
		ComputeFrames comp;
		comp.computeState(*to, initFrame, code->instList, _cf, code, _classPath,
				method);

    u4 maxStack = code->maxStack;
    if (!code->instList.hasBranches() && !code->hasTryCatch()) {
      for (BasicBlock* bb : cfg) {
        if (maxStack < bb->in.maxStack) {
          maxStack = bb->in.maxStack;
        }

        if (maxStack < bb->out.maxStack) {
          maxStack = bb->out.maxStack;
        }
      }

      code->maxStack = maxStack;
			return;
		}

		SmtAttr* smt = new SmtAttr(_attrIndex, &_cf);

		int totalOffset = -1;

		Frame* f = &cfg.entry->out;
		f->cleanTops();

		class Ser {
		public:

			bool isSame(Frame& current, Frame& prev) {
				return current.lva == prev.lva && current.stack.size() == 0;
			}

			bool isSameLocals1StackItem(Frame& current, Frame& prev) {
				return current.lva == prev.lva && current.stack.size() == 1;
			}

			int isChopAppend(Frame& current, Frame& prev) {
				int diff = current.lva.size() - prev.lva.size();

				for (u4 i = 0; i < std::min(current.lva.size(), prev.lva.size()); ++i) {
					if (current.lva.at(i) != prev.lva.at(i)) {
						return 0;
					}
				}

				bool emptyStack = current.stack.size() == 0;
				bool res = diff != 0 && diff >= -3 && diff <= 3 && emptyStack;
				return res ? diff : 0;
			}
		} s;

		for (BasicBlock* bb : cfg) {
      if (maxStack < bb->in.maxStack) {
        maxStack = bb->in.maxStack;
      }

      if (maxStack < bb->out.maxStack) {
        maxStack = bb->out.maxStack;
      }

			if (bb->start != code->instList.end()) {
				Inst* start = *bb->start;
				if (start->isLabel() && (start->label()->isBranchTarget
								|| start->label()->isCatchHandler)) {
					Frame& current = bb->in;

					current.cleanTops();

					setCpIndex(current, code->instList);

					SmtAttr::Entry e;

					e.label = start;

					totalOffset += 1;
					int offsetDelta = start->label()->offset - totalOffset;

					int diff;

					if (s.isSame(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = offsetDelta;
						} else {
							e.frameType = 251;
							e.same_frame_extended.offset_delta = offsetDelta;
						}
					} else if (s.isSameLocals1StackItem(current, *f)) {
						if (offsetDelta <= 63) {
							e.frameType = 64 + offsetDelta;
							const Type& t = current.stack.front().first;
							e.sameLocals_1_stack_item_frame.stack.push_back(t);
						} else {
							e.frameType = 247;
							const Type& t = current.stack.front().first;
							e.same_locals_1_stack_item_frame_extended.stack.push_back(
									t);
							e.same_locals_1_stack_item_frame_extended.offset_delta =
									offsetDelta;
						}
					} else if ((diff = s.isChopAppend(current, *f)) != 0) {
						JnifError::assert(diff != 0 && diff >= -3 && diff <= 3);

						e.frameType = 251 + diff;
						if (diff > 0) {
							e.append_frame.offset_delta = offsetDelta;
							int size = current.lva.size();
							//list<Type> ts;
							for (int i = 0; i < diff; i++) {
								Type t = current.lva[size - diff + i].first;
								//ts.push_front(t);
								e.append_frame.locals.push_back(t);
							}
							//for (const Type& t : ts) {
							//e.append_frame.locals.push_back(t);
							//}
							//e.append_frame.locals.re
						} else {
							e.chop_frame.offset_delta = offsetDelta;
						}
					} else {
						e.frameType = 255;
						e.full_frame.offset_delta = offsetDelta;
            std::vector<Type> lva(current.lva.size(), TypeFactory::topType());
            for (size_t i = 0; i < current.lva.size(); i++) {
                lva[i] = current.lva[i].first;
            }

						e.full_frame.locals = lva;

						std::list<Frame::T> rs = current.stack;
						rs.reverse();
						for (const Frame::T& t : rs) {
							e.full_frame.stack.push_back(t.first);
							//e.full_frame.stack.push_front(t);
						}
					}

					totalOffset += offsetDelta;
					smt->entries.push_back(e);
					f = &bb->in;
				}
			}
		}

		//code->cfg = cfgp;

		code->attrs.add(smt);

    code->maxStack = maxStack;
	}

private:

    ConstPool::Index _attrIndex;
	ClassFile& _cf;
	IClassPath* _classPath;

};

}

#endif