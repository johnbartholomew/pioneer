#include "cbor.hpp"

namespace {

	void WriteInitialBytes(char initial_byte, uint64_t value) {
	}

}  // unnamed namespace

namespace cbor { namespace encode {

void Value(std::ostream &ss, int v) {
}

void Value(std::ostream &ss, unsigned int v) {
}

void Value(std::ostream &ss, int64_t v) {
}

void Value(std::ostream &ss, uint64_t v) {
}

void Value(std::ostream &ss, float v) {
}

void Value(std::ostream &ss, double v) {
}

void Value(std::ostream &ss, bool v) {
}

void Value(std::ostream &ss, cbor::NullT) {
	ss.put();
}

void Value(std::ostream &ss, cbor::UndefinedT) {
}

void Value(std::ostream &ss, cbor::BytesSlice v) {
}

void Value(std::ostream &ss, cbor::StringSlice v) {
}

void TagBegin(std::ostream &ss, cbor::Tag v) {
}

void SequenceTerminator(std::ostream &ss) {
	ss.put('\xFF');
}

void ChunkedStringBegin(std::ostream &ss) {
}

void ChunkedBytesBegin(std::ostream &ss) {
}

void MapBegin(std::ostream &ss, size_t count) {
}

void MapBegin(std::ostream &ss) {
}

void ArrayBegin(std::ostream &ss, size_t count) {
}

void ArrayBegin(std::ostream &ss) {
}

}}  // namespace cbor::encode
