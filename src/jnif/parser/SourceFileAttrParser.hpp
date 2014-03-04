#ifndef JNIF_SOURCEFILEATTRPARSER_HPP
#define JNIF_SOURCEFILEATTRPARSER_HPP

#include "../base.hpp"
#include "../tree/ConstPool.hpp"

namespace jnif {

/**
 * SourceFile attribute parser.
 */
class SourceFileAttrParser {
public:

	static constexpr const char* AttrName = "SourceFile";

	template<typename TReader, typename TVisitor>
	inline static void parse(TReader& br, TVisitor& v, ConstPool& cp,
			u2 nameIndex) {
		u2 sourceFileIndex = br.readu2();

		v.visitSourceFile(nameIndex, sourceFileIndex);
	}

	/**
	 * Defines the writer for this attribute.
	 */
	template<typename TWriter>
	struct Writer {
		TWriter& w;

		inline Writer(TWriter& w) :
				w(w) {
		}

		inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
			w.writeu2(sourceFileIndex);
		}
	};

	template<typename TVisitor>
	struct Forward {
		TVisitor& av;

		inline Forward(TVisitor& av) :
				av(av) {
		}

		inline void visitSourceFile(u2 nameIndex, u2 sourceFileIndex) {
			av.visitSourceFile(nameIndex, sourceFileIndex);
		}
	};
};

}

#endif
