/*
 * tests.hpp
 *
 *  Created on: Jun 2, 2014
 *      Author: luigi
 */

#include <fstream>
#include <jnif.hpp>

#include "tests.hpp"

using namespace std;
using namespace jnif;

template<typename T>
void assertEquals(const T* lhsData, int lhsLen, const T* rhsData, int rhsLen) {
	JnifError::assertEquals(lhsLen, rhsLen, "Invalid len for arrays");

	for (int i = 0; i < lhsLen; i++) {
		JnifError::assertEquals(lhsData[i], rhsData[i], "Data difers at pos:", i);
	}
}

class UnitTestClassPath: public IClassPath {
public:

	String getCommonSuperClass(const String&, const String&) {
		return "java/lang/Object";
	}

};

class NopAdderInstr {
public:

	int diff;

	NopAdderInstr(ClassFile& cf) :
			diff(0) {
		int methodsWithCode = 0;
		for (Method* m : cf.methods) {
			if (m->hasCode()) {
				InstList& instList = m->instList();

				// If there is a tableswitch or a lookupswitch instruction
				// bytes added to the instruction flow must be a multiple
				// of four to keep the padding in this instructions.
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);
				instList.addZero(OPCODE_nop);

				methodsWithCode++;
			}
		}

		diff = methodsWithCode * 4;
	}
};

void testPrinter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);
	ofstream os;
	os << cf;
}

void testSize(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	int newlen = cf.computeSize();

	JnifError::assertEquals(newlen, jf.len);
}

void testWriter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	int newlen = cf.computeSize();

	JnifError::assertEquals(newlen, jf.len);

	u1* newdata = new u1[newlen];

	cf.write(newdata, newlen);

	assertEquals(jf.data, jf.len, newdata, newlen);

	delete[] newdata;
}

void testAnalysis(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	UnitTestClassPath cp;
	cf.computeFrames(&cp);
}

void testAnalysisPrinter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	ofstream os;
	os << cf;
}

void testAnalysisWriter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	int newlen = cf.computeSize();
	u1* newdata = new u1[newlen];
	cf.write(newdata, newlen);

	delete[] newdata;
}

void testNopAdderInstrPrinter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	NopAdderInstr instr(cf);
	fstream os;
	os << cf;
}

void testNopAdderInstrSize(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	NopAdderInstr instr(cf);
	int newlen = cf.computeSize();

	JnifError::assertEquals(jf.len + instr.diff, newlen);
}

void testNopAdderInstrWriter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	NopAdderInstr instr(cf);

	int newlen = cf.computeSize();

	JnifError::assertEquals(jf.len + instr.diff, newlen);

	u1* newdata = new u1[newlen];
	cf.write(newdata, newlen);

	ClassFile newcf(newdata, newlen);

	int newlen2 = cf.computeSize();

	JnifError::assertEquals(newlen2, newlen);

	u1* newdata2 = new u1[newlen2];
	cf.write(newdata2, newlen2);

	assertEquals(newdata, newlen, newdata2, newlen2);

	delete[] newdata;
}

void testNopAdderInstrAnalysisPrinter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	NopAdderInstr instr(cf);

	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	ofstream os;
	os << cf;
}

void testNopAdderInstrAnalysisWriter(const JavaFile& jf) {
	ClassFile cf(jf.data, jf.len);

	NopAdderInstr instr(cf);

	UnitTestClassPath cp;
	cf.computeFrames(&cp);

	int newlen = cf.computeSize();
	u1* newdata = new u1[newlen];
	cf.write(newdata, newlen);

	delete[] newdata;
}