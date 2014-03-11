#ifndef JNIF_HPP
#define JNIF_HPP

/**
 *
 */
#include <string>
#include <vector>
#include <list>

/**
 * The jnif namespace contains all type definitions, constants, enumerations
 * and structs of the jnif framework.
 *
 * These implementations is based on the Java Virtual Machine Specification
 * chapter 4, class file format.
 *
 * http://docs.oracle.com/javase/specs/jvms/se7/html/jvms-4.html
 *
 * The most relevant types and classes of the framework are ClassParser
 * (using ClassBaseParser) and
 * ClassWriterVisitor which it allows to parser Java Class File and write back
 * them with the ability to inject transformers (class visitors in jnif
 * parlance) to modify these classes.
 *
 * @see ClassFile
 *
 * NOTE: The library uses structs instead of classes.
 */
namespace jnif {

/**
 * Represents a byte inside the Java Class File.
 * The sizeof(u1) must be equal to 1.
 */
typedef unsigned char u1;

/**
 * Represents two bytes inside the Java Class File.
 * The sizeof(u2) must be equal to 2.
 */
typedef unsigned short u2;

/**
 * Represents four bytes inside the Java Class File.
 * The sizeof(u4) must be equal to 4.
 */
typedef unsigned int u4;

/**
 * Defines the Java Class File signature. This magic value must be binary
 * serialized as a u4 value.
 */
enum Magic {

	/**
	 * The only allowed value for the signature.
	 */
	CLASSFILE_MAGIC = 0xcafebabe
};

/**
 * Constant pool enum
 */
enum ConstTag {
	CONSTANT_Class = 7,
	CONSTANT_Fieldref = 9,
	CONSTANT_Methodref = 10,
	CONSTANT_InterfaceMethodref = 11,
	CONSTANT_String = 8,
	CONSTANT_Integer = 3,
	CONSTANT_Float = 4,
	CONSTANT_Long = 5,
	CONSTANT_Double = 6,
	CONSTANT_NameAndType = 12,
	CONSTANT_Utf8 = 1,
	CONSTANT_MethodHandle = 15,
	CONSTANT_MethodType = 16,
	CONSTANT_InvokeDynamic = 18
};

/**
 *
 */
enum AccessFlags {

	/**
	 * Declared public; may be accessed from outside its package.
	 */
	ACC_PUBLIC = 0x0001,

	/**
	 * Declared private; accessible only within the defining class.
	 */
	ACC_PRIVATE = 0x0002,

	/**
	 * Declared protected; may be accessed within subclasses.
	 */
	ACC_PROTECTED = 0x0004,

	/**
	 * Declared static.
	 */
	ACC_STATIC = 0x0008,

	/**
	 * Declared final; must not be overridden (see 5.4.5).
	 */
	ACC_FINAL = 0x0010,

	/**
	 * Declared synchronized; invocation is wrapped by a monitor use.
	 */
	ACC_SYNCHRONIZED = 0x0020,

	/**
	 * A bridge method, generated by the compiler.
	 */
	ACC_BRIDGE = 0x0040,

	/**
	 * Declared with variable number of arguments.
	 */
	ACC_VARARGS = 0x0080,

	/**
	 * Declared native; implemented in a language other than Java.
	 */
	ACC_NATIVE = 0x0100,

	/**
	 * Declared abstract; no implementation is provided.
	 */
	ACC_ABSTRACT = 0x0400,

	/**
	 * Declared strictfp; floating-point mode is FP-strict.
	 */
	ACC_STRICT = 0x0800,

	/**
	 * Declared synthetic; not present in the source code.
	 */
	ACC_SYNTHETIC = 0x1000,
};

/**
 * OPCODES constants definitions
 */
enum Opcode {
	OPCODE_nop = 0x00,
	OPCODE_aconst_null = 0x01,
	OPCODE_iconst_m1 = 0x02,
	OPCODE_iconst_0 = 0x03,
	OPCODE_iconst_1 = 0x04,
	OPCODE_iconst_2 = 0x05,
	OPCODE_iconst_3 = 0x06,
	OPCODE_iconst_4 = 0x07,
	OPCODE_iconst_5 = 0x08,
	OPCODE_lconst_0 = 0x09,
	OPCODE_lconst_1 = 0x0a,
	OPCODE_fconst_0 = 0x0b,
	OPCODE_fconst_1 = 0x0c,
	OPCODE_fconst_2 = 0x0d,
	OPCODE_dconst_0 = 0x0e,
	OPCODE_dconst_1 = 0x0f,
	OPCODE_bipush = 0x10,
	OPCODE_sipush = 0x11,
	OPCODE_ldc = 0x12,
	OPCODE_ldc_w = 0x13,
	OPCODE_ldc2_w = 0x14,
	OPCODE_iload = 0x15,
	OPCODE_lload = 0x16,
	OPCODE_fload = 0x17,
	OPCODE_dload = 0x18,
	OPCODE_aload = 0x19,
	OPCODE_iload_0 = 0x1a,
	OPCODE_iload_1 = 0x1b,
	OPCODE_iload_2 = 0x1c,
	OPCODE_iload_3 = 0x1d,
	OPCODE_lload_0 = 0x1e,
	OPCODE_lload_1 = 0x1f,
	OPCODE_lload_2 = 0x20,
	OPCODE_lload_3 = 0x21,
	OPCODE_fload_0 = 0x22,
	OPCODE_fload_1 = 0x23,
	OPCODE_fload_2 = 0x24,
	OPCODE_fload_3 = 0x25,
	OPCODE_dload_0 = 0x26,
	OPCODE_dload_1 = 0x27,
	OPCODE_dload_2 = 0x28,
	OPCODE_dload_3 = 0x29,
	OPCODE_aload_0 = 0x2a,
	OPCODE_aload_1 = 0x2b,
	OPCODE_aload_2 = 0x2c,
	OPCODE_aload_3 = 0x2d,
	OPCODE_iaload = 0x2e,
	OPCODE_laload = 0x2f,
	OPCODE_faload = 0x30,
	OPCODE_daload = 0x31,
	OPCODE_aaload = 0x32,
	OPCODE_baload = 0x33,
	OPCODE_caload = 0x34,
	OPCODE_saload = 0x35,
	OPCODE_istore = 0x36,
	OPCODE_lstore = 0x37,
	OPCODE_fstore = 0x38,
	OPCODE_dstore = 0x39,
	OPCODE_astore = 0x3a,
	OPCODE_istore_0 = 0x3b,
	OPCODE_istore_1 = 0x3c,
	OPCODE_istore_2 = 0x3d,
	OPCODE_istore_3 = 0x3e,
	OPCODE_lstore_0 = 0x3f,
	OPCODE_lstore_1 = 0x40,
	OPCODE_lstore_2 = 0x41,
	OPCODE_lstore_3 = 0x42,
	OPCODE_fstore_0 = 0x43,
	OPCODE_fstore_1 = 0x44,
	OPCODE_fstore_2 = 0x45,
	OPCODE_fstore_3 = 0x46,
	OPCODE_dstore_0 = 0x47,
	OPCODE_dstore_1 = 0x48,
	OPCODE_dstore_2 = 0x49,
	OPCODE_dstore_3 = 0x4a,
	OPCODE_astore_0 = 0x4b,
	OPCODE_astore_1 = 0x4c,
	OPCODE_astore_2 = 0x4d,
	OPCODE_astore_3 = 0x4e,
	OPCODE_iastore = 0x4f,
	OPCODE_lastore = 0x50,
	OPCODE_fastore = 0x51,
	OPCODE_dastore = 0x52,
	OPCODE_aastore = 0x53,
	OPCODE_bastore = 0x54,
	OPCODE_castore = 0x55,
	OPCODE_sastore = 0x56,
	OPCODE_pop = 0x57,
	OPCODE_pop2 = 0x58,
	OPCODE_dup = 0x59,
	OPCODE_dup_x1 = 0x5a,
	OPCODE_dup_x2 = 0x5b,
	OPCODE_dup2 = 0x5c,
	OPCODE_dup2_x1 = 0x5d,
	OPCODE_dup2_x2 = 0x5e,
	OPCODE_swap = 0x5f,
	OPCODE_iadd = 0x60,
	OPCODE_ladd = 0x61,
	OPCODE_fadd = 0x62,
	OPCODE_dadd = 0x63,
	OPCODE_isub = 0x64,
	OPCODE_lsub = 0x65,
	OPCODE_fsub = 0x66,
	OPCODE_dsub = 0x67,
	OPCODE_imul = 0x68,
	OPCODE_lmul = 0x69,
	OPCODE_fmul = 0x6a,
	OPCODE_dmul = 0x6b,
	OPCODE_idiv = 0x6c,
	OPCODE_ldiv = 0x6d,
	OPCODE_fdiv = 0x6e,
	OPCODE_ddiv = 0x6f,
	OPCODE_irem = 0x70,
	OPCODE_lrem = 0x71,
	OPCODE_frem = 0x72,
	OPCODE_drem = 0x73,
	OPCODE_ineg = 0x74,
	OPCODE_lneg = 0x75,
	OPCODE_fneg = 0x76,
	OPCODE_dneg = 0x77,
	OPCODE_ishl = 0x78,
	OPCODE_lshl = 0x79,
	OPCODE_ishr = 0x7a,
	OPCODE_lshr = 0x7b,
	OPCODE_iushr = 0x7c,
	OPCODE_lushr = 0x7d,
	OPCODE_iand = 0x7e,
	OPCODE_land = 0x7f,
	OPCODE_ior = 0x80,
	OPCODE_lor = 0x81,
	OPCODE_ixor = 0x82,
	OPCODE_lxor = 0x83,
	OPCODE_iinc = 0x84,
	OPCODE_i2l = 0x85,
	OPCODE_i2f = 0x86,
	OPCODE_i2d = 0x87,
	OPCODE_l2i = 0x88,
	OPCODE_l2f = 0x89,
	OPCODE_l2d = 0x8a,
	OPCODE_f2i = 0x8b,
	OPCODE_f2l = 0x8c,
	OPCODE_f2d = 0x8d,
	OPCODE_d2i = 0x8e,
	OPCODE_d2l = 0x8f,
	OPCODE_d2f = 0x90,
	OPCODE_i2b = 0x91,
	OPCODE_i2c = 0x92,
	OPCODE_i2s = 0x93,
	OPCODE_lcmp = 0x94,
	OPCODE_fcmpl = 0x95,
	OPCODE_fcmpg = 0x96,
	OPCODE_dcmpl = 0x97,
	OPCODE_dcmpg = 0x98,
	OPCODE_ifeq = 0x99,
	OPCODE_ifne = 0x9a,
	OPCODE_iflt = 0x9b,
	OPCODE_ifge = 0x9c,
	OPCODE_ifgt = 0x9d,
	OPCODE_ifle = 0x9e,
	OPCODE_if_icmpeq = 0x9f,
	OPCODE_if_icmpne = 0xa0,
	OPCODE_if_icmplt = 0xa1,
	OPCODE_if_icmpge = 0xa2,
	OPCODE_if_icmpgt = 0xa3,
	OPCODE_if_icmple = 0xa4,
	OPCODE_if_acmpeq = 0xa5,
	OPCODE_if_acmpne = 0xa6,
	OPCODE_goto = 0xa7,
	OPCODE_jsr = 0xa8,
	OPCODE_ret = 0xa9,
	OPCODE_tableswitch = 0xaa,
	OPCODE_lookupswitch = 0xab,
	OPCODE_ireturn = 0xac,
	OPCODE_lreturn = 0xad,
	OPCODE_freturn = 0xae,
	OPCODE_dreturn = 0xaf,
	OPCODE_areturn = 0xb0,
	OPCODE_return = 0xb1,
	OPCODE_getstatic = 0xb2,
	OPCODE_putstatic = 0xb3,
	OPCODE_getfield = 0xb4,
	OPCODE_putfield = 0xb5,
	OPCODE_invokevirtual = 0xb6,
	OPCODE_invokespecial = 0xb7,
	OPCODE_invokestatic = 0xb8,
	OPCODE_invokeinterface = 0xb9,
	OPCODE_invokedynamic = 0xba,
	OPCODE_new = 0xbb,
	OPCODE_newarray = 0xbc,
	OPCODE_anewarray = 0xbd,
	OPCODE_arraylength = 0xbe,
	OPCODE_athrow = 0xbf,
	OPCODE_checkcast = 0xc0,
	OPCODE_instanceof = 0xc1,
	OPCODE_monitorenter = 0xc2,
	OPCODE_monitorexit = 0xc3,
	OPCODE_wide = 0xc4,
	OPCODE_multianewarray = 0xc5,
	OPCODE_ifnull = 0xc6,
	OPCODE_ifnonnull = 0xc7,
	OPCODE_goto_w = 0xc8,
	OPCODE_jsr_w = 0xc9,
	OPCODE_breakpoint = 0xca,
	OPCODE_impdep1 = 0xfe,
	OPCODE_impdep2 = 0xff
};

enum OpKind {
	KIND_ZERO,
	KIND_BIPUSH,
	KIND_SIPUSH,
	KIND_LDC,
	KIND_VAR,
	KIND_IINC,
	KIND_JUMP,
	KIND_TABLESWITCH,
	KIND_LOOKUPSWITCH,
	KIND_FIELD,
	KIND_INVOKE,
	KIND_INVOKEINTERFACE,
	KIND_INVOKEDYNAMIC,
	KIND_TYPE,
	KIND_NEWARRAY,
	KIND_MULTIARRAY,
	KIND_PARSE4TODO,
	KIND_RESERVED,
	KIND_LABEL,
};

/**
 *
 */
enum AType {
	T_BOOLEAN = 4,
	T_CHAR = 5,
	T_FLOAT = 6,
	T_DOUBLE = 7,
	T_BYTE = 8,
	T_SHORT = 9,
	T_INT = 10,
	T_LONG = 11
};

typedef u2 ClassIndex;

typedef u2 Label;

/**
 * Items used for the StackMapTable
 */
enum StackMapItem {
	ITEM_Top = 0,
	ITEM_Integer = 1,
	ITEM_Float = 2,
	ITEM_Long = 4,
	ITEM_Double = 3,
	ITEM_Null = 5,
	ITEM_UninitializedThis = 6,
	ITEM_Object = 7,
	ITEM_Uninitialized = 8
};

/**
 *
 */
enum AttrKind {
	ATTR_UNKNOWN,
	ATTR_SOURCEFILE,
	ATTR_CODE,
	ATTR_EXCEPTIONS,
	ATTR_LVT,
	ATTR_LNT,
	ATTR_SMT
};

using namespace std;

/**
 * Represent a bytecode instruction.
 */
struct Inst {

	Inst() {
	}

	Inst(OpKind kind) :
			kind(kind) {
	}

	Inst(Opcode opcode) :
			opcode(opcode), kind(KIND_ZERO) {
	}

	Inst(Opcode opcode, OpKind kind) :
			opcode(opcode), kind(kind) {
	}

	/**
	 * The opcode of this instruction.
	 */
	Opcode opcode;

	/**
	 * The kind of this instruction.
	 */
	OpKind kind;

	friend Inst InvokeInst(Opcode opcode, u2 index) {
		Inst inst;
		inst.kind = KIND_INVOKE;
		inst.opcode = opcode;
		inst.invoke.methodRefIndex = index;

		return inst;
	}

	union {
		struct {
			int value;
		} push;
		struct {
			u2 valueIndex;
		} ldc;
		struct {
			u1 lvindex;
		} var;
		struct {
			u1 index;
			u1 value;
		} iinc;
		struct {
			Label label;
			Inst* label2;
		} jump;
		struct {
			u2 fieldRefIndex;
		} field;
		struct {
			u2 methodRefIndex;
		} invoke;
		struct {
			u2 interMethodRefIndex;
			u1 count;
		} invokeinterface;
		struct {
			ClassIndex classIndex;
		} type;
		struct {
			u1 atype;
		} newarray;
		struct {
			ClassIndex classIndex;
			u1 dims;
		} multiarray;
	};

	struct {
		int def;
		int low;
		int high;
		vector<u4> targets;
	} ts;

	struct {
		u4 defbyte;
		u4 npairs;
		vector<u4> keys;
		vector<u4> targets;
	} ls;
};

static Inst* ZeroInst(Opcode opcode) {
	Inst* inst = new Inst();
	inst->kind = KIND_ZERO;
	inst->opcode = opcode;

	return inst;
}

/**
 * Represents the bytecode of a method.
 */
typedef std::list<Inst*> InstList;

/**
 *
 */
struct ConstPoolEntry {
	ConstTag tag;
	union {
		struct {
			u2 name_index;
		} clazz;
		struct {
			u2 class_index;
			u2 name_and_type_index;
		} memberref;
		struct {
			u2 string_index;
		} s;
		struct {
			u4 value;
		} i;
		struct {
			u4 value;
		} f;
		struct {
			u4 high_bytes;
			u4 low_bytes;
		} l;
		struct {
			u4 high_bytes;
			u4 low_bytes;
		} d;
		struct {
			u2 name_index;
			u2 descriptor_index;
		} nameandtype;
		struct {
			u1 reference_kind;
			u2 reference_index;
		} methodhandle;
		struct {
			u2 descriptor_index;
		} methodtype;
		struct {
			u2 bootstrap_method_attr_index;
			u2 name_and_type_index;
		} invokedynamic;
	};
	struct {
		string str;
	} utf8;
};

/**
 * The constant pool
 */
class ConstPool {
public:

	vector<ConstPoolEntry> entries;

	ConstPool();
	u2 addSingle(const ConstPoolEntry& entry);
	void addDouble(const ConstPoolEntry& entry);
	const char* getUtf8(int utf8Index) const;
	const char* getClazzName(int classIndex) const;
	void getNameAndType(int index, string* name, string* desc) const;
	void getMemberRef(int index, string* clazzName, string* name, string* desc,
			u1 tag) const;
	u2 addInteger(u4 value);
	u2 addUtf8(const char* str);
	u2 addClass(u2 classNameIndex);
	u2 addClass(const char* className);
	u2 addNameAndType(u2 nameIndex, u2 descIndex);
	u2 addMethodRef(u2 classIndex, u2 nameAndTypeIndex);
	u2 addMethodRef(u2 classIndex, const char* name, const char* desc);

private:
	const ConstPoolEntry* getEntry(u4 index, u1 tag, const char* message) const;
	u2 getClazzNameIndex(int classIndex) const;

};

/**
 * Defines the base class for all attributes in the class file.
 */
struct Attr {
	Attr(const Attr&) = delete;

	AttrKind kind;

	u2 nameIndex;
	u4 len;

protected:

	Attr(AttrKind kind, u2 nameIndex, u4 len = 0) :
			kind(kind), nameIndex(nameIndex), len(len) {
	}
};

/**
 * Represents a collection of attributes within a class, method or field
 * or even with another attributes, e.g., CodeAttr.
 */
struct Attrs {
	Attrs(const Attrs&) = delete;
	Attrs(Attrs&&) = default;

	inline Attrs() {
	}

	inline Attr* add(Attr* attr) {
		attrs.push_back(attr);

		return attr;
	}

	inline u2 size() const {
		return attrs.size();
	}

	inline const Attr& operator[](u2 index) const {
		return *attrs[index];
	}

	vector<Attr*>::iterator begin() {
		return attrs.begin();
	}

	vector<Attr*>::iterator end() {
		return attrs.end();
	}

	vector<Attr*> attrs;
};

/**
 * Represents an unknown, and hence opaque, attribute to jnif.
 */
struct UnknownAttr: public Attr {

	const u1 * const data;

	UnknownAttr(u2 nameIndex, u4 len, const u1* data) :
			Attr(ATTR_UNKNOWN, nameIndex, len), data(data) {
	}

};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
struct LvtAttr: Attr {

	struct LvEntry {
		u2 startPc;
		u2 len;
		u2 varNameIndex;
		u2 varDescIndex;
		u2 index;
	};

	std::vector<LvEntry> lvt;

	LvtAttr(u2 nameIndex) :
			Attr(ATTR_LVT, nameIndex) {
	}
};

/**
 * Represents the LineNumberTable attribute within the Code attribute.
 */
struct LntAttr: Attr {

	LntAttr(u2 nameIndex) :
			Attr(ATTR_LNT, nameIndex) {
	}

	struct LnEntry {
		u2 startpc;
		u2 lineno;
	};

	std::vector<LnEntry> lnt;

};

/**
 *
 */
struct SmtAttr: Attr {

	SmtAttr(u2 nameIndex) :
			Attr(ATTR_SMT, nameIndex) {
	}

};

/**
 * Represents the Exceptions attribute.
 */
struct ExceptionsAttr: Attr {

	ExceptionsAttr(u2 nameIndex, u4 len, const vector<u2>& es) :
			Attr(ATTR_EXCEPTIONS, nameIndex, len), es(es) {
	}

	std::vector<u2> es;
};

/**
 *
 */
struct CodeExceptionEntry {
	u2 startpc;
	u2 endpc;
	u2 handlerpc;
	u2 catchtype;
};

/**
 * Represent the Code attribute of a method.
 */
struct CodeAttr: Attr {

	CodeAttr(u2 nameIndex) :
			Attr(ATTR_CODE, nameIndex), maxStack(0), maxLocals(0), codeLen(0) {
	}

	u2 maxStack;
	u2 maxLocals;
	u4 codeLen;

	InstList instList;

	vector<CodeExceptionEntry> exceptions;

	Attrs attrs;
};

/**
 *
 */
struct SourceFileAttr: Attr {

	const u2 sourceFileIndex;

	SourceFileAttr(u2 nameIndex, u4 len, u2 sourceFileIndex) :
			Attr(ATTR_SOURCEFILE, nameIndex, len), sourceFileIndex(
					sourceFileIndex) {
	}

};

/**
 *
 */
struct Member: Attrs {
	friend struct Members;
	Member(const Member&) = delete;

	Member(Member&&) = default;

	AccessFlags accessFlags;
	u2 nameIndex;
	u2 descIndex;

	bool hasCode() {
		for (Attr* attr : attrs) {
			if (attr->kind == ATTR_CODE) {
				return true;
			}
		}

		return false;
	}

	InstList& instList();

	void instList(const InstList& newcode);

private:

	Member(AccessFlags accessFlags, u2 nameIndex, u2 descIndex) :
			accessFlags(accessFlags), nameIndex(nameIndex), descIndex(descIndex) {
	}
};

typedef Member Field;
typedef Member Method;

/**
 * Represents a collection of members within a class file, i.e.,
 * fields or methods.
 */
struct Members {

	friend class ClassFile;
	Members(const Members&) = delete;

	inline Member& add(AccessFlags accessFlags, u2 nameIndex, u2 descIndex) {
		Member* member = new Member(accessFlags, nameIndex, descIndex);
		members.push_back(member);
		//members.emplace(member);
		return *members.back();
	}

	inline u2 size() const {
		return members.size();
	}

	inline Member& operator[](u2 index) {
		return *members[index];
	}

	vector<Member*>::iterator begin() {
		return members.begin();
	}

	vector<Member*>::iterator end() {
		return members.end();
	}

private:

	inline Members() {
	}

	vector<Member*> members;
};

/**
 *
 */
class Version {
public:

	/**
	 *
	 */
	Version(u2 major = 51, u2 minor = 0) :
			_major(major), _minor(minor) {
	}

	/**
	 *
	 */
	u2 getMajor() const;

	/**
	 *
	 */
	u2 getMinor() const;

private:

	u2 _major;

	u2 _minor;
};

/**
 * Models a Java Class File following the specification from version 7.
 */
class ClassFile: public ConstPool, public Attrs {
public:

	/**
	 * Constructs a default class file given the class name, the super class
	 * name and the access flags.
	 */
	ClassFile(const char* className, const char* superClassName,
			u2 accessFlags = ACC_PUBLIC, Version version = Version());

	/**
	 * Constructs a class file from an in-memory representation of the java
	 * class file.
	 */
	ClassFile(const u1* classFileData, int classFileLen);

	/**
	 *
	 */
	Version getVersion() const;

	/**
	 *
	 */
	void setVersion(Version version);

	/**
	 *
	 */
	const char* getClassName() const;

	/**
	 * Gets the size in bytes of this class file of the in-memory
	 * representation.
	 */
	u4 getSize();

	/**
	 * Writes this class file in the specified buffer according to the
	 * specification.
	 */
	void write(u1* classFileData, int classFileLen);

	/**
	 * Shows the class file in a textual format, useful for debugging purposes.
	 */
	friend ostream& operator<<(std::ostream& os, ClassFile& cf);

	u2 accessFlags;
	u2 thisClassIndex;
	u2 superClassIndex;
	vector<u2> interfaces;
	Members fields;
	Members methods;

private:

	Version _version;

};

}

#endif
