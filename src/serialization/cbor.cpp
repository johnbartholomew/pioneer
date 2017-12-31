#include "cbor.hpp"
#include <SDL_endian.h>

namespace {

static inline void WriteFloat32BE(std::ostream &stream, float v) {
	union { float f; uint32_t i; } convert;
	convert.f = v;
	// Assume float and int have the same endianness on the target platform.
	uint32_t vbe = SDL_SwapBE32(convert.i);
	stream.write(reinterpret_cast<char*>(&vbe), sizeof(vbe));
}

static inline void WriteFloat64BE(std::ostream &stream, double v) {
	union { double f; uint64_t i; } convert;
	convert.f = v;
	// Assume float and int have the same endianness on the target platform.
	uint64_t vbe = SDL_SwapBE64(convert.i);
	stream.write(reinterpret_cast<char*>(&vbe), sizeof(vbe));
}

static inline void WriteInitial(std::ostream &stream, char initial_byte, uint64_t value) {
	if (value < 24u) {
		stream.put(initial_byte | static_cast<char>(value));
	} else if (value < 0x100u) {
		stream.put(initial_byte | '\x18');
		uint8_t bits = value;
		stream.write(reinterpret_cast<char*>(&bits), sizeof(bits));
	} else if (value < 0x10000u) {
		stream.put(initial_byte | '\x19');
		uint16_t bits = SDL_SwapBE16(value);
		stream.write(reinterpret_cast<char*>(&bits), sizeof(bits));
	} else if (value < 0x100000000u) {
		stream.put(initial_byte | '\x1A');
		uint32_t bits = SDL_SwapBE32(value);
		stream.write(reinterpret_cast<char*>(&bits), sizeof(bits));
	} else {
		stream.put(initial_byte | '\x1B');
		uint64_t bits = SDL_SwapBE64(value);
		stream.write(reinterpret_cast<char*>(&bits), sizeof(bits));
	}
}

}  // unnamed namespace

namespace cbor { namespace encode {

void Value(std::ostream &ss, int v) {
	Value(ss, static_cast<int64_t>(v));
}

void Value(std::ostream &ss, unsigned int v) {
	Value(ss, static_cast<uint64_t>(v));
}

void Value(std::ostream &ss, int64_t v) {
	if (v < 0) {
		WriteInitial(ss, '\x20', ~static_cast<uint64_t>(v));
	} else {
		WriteInitial(ss, '\x00', v);
	}
}

void Value(std::ostream &ss, uint64_t v) {
	WriteInitial(ss, '\x00', v);
}

void Value(std::ostream &ss, float v) {
	ss.put('\xFA');
	WriteFloat32BE(ss, v);
}

void Value(std::ostream &ss, double v) {
	ss.put('\xFB');
	WriteFloat64BE(ss, v);
}

void Value(std::ostream &ss, bool v) {
	ss.put('\xF4' | (v ? '\x01' : '\x00'));
}

void Value(std::ostream &ss, cbor::NullT) {
	ss.put('\xF6');
}

void Value(std::ostream &ss, cbor::UndefinedT) {
	ss.put('\xF7');
}

void Value(std::ostream &ss, cbor::BytesSlice v) {
	WriteInitial(ss, '\x40', v.size);
	ss.write(v.p, v.size);
}

void Value(std::ostream &ss, cbor::StringSlice v) {
	WriteInitial(ss, '\x60', v.size);
	ss.write(v.p, v.size);
}

void TagBegin(std::ostream &ss, cbor::Tag v) {
	WriteInitial(ss, '\xC0', v.value);
}

void SequenceTerminator(std::ostream &ss) {
	ss.put('\xFF');
}

void ChunkedStringBegin(std::ostream &ss) {
	ss.put('\x7A');
}

void ChunkedBytesBegin(std::ostream &ss) {
	ss.put('\x5A');
}

void MapBegin(std::ostream &ss, size_t count) {
	WriteInitial(ss, '\xA0', count);
}

void MapBegin(std::ostream &ss) {
	ss.put('\xBF');
}

void ArrayBegin(std::ostream &ss, size_t count) {
	WriteInitial(ss, '\x80', count);
}

void ArrayBegin(std::ostream &ss) {
	ss.put('\x9F');
}

}}  // namespace cbor::encode
