#ifndef SERIALIZER_BASE_H
#define SERIALIZER_BASE_H

#include "vector3.h"
#include "Quaternion.h"
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "Color.h"
#include <sstream>
#include <memory>
#include <cassert>

namespace Serializer {

	class OutputStream {
	public:
		OutputStream();
	protected:
		std::ostringstream m_ss;
	};

	class OutputStreamJson : public OutputStream {
		void WrObjectStart();
		void WrObjectField(const char *fname);
		void WrObjectEnd();
		void WrObjectNext();

		void WrArrayStart();
		void WrArrayEnd();
		void WrArrayNext();

		void WrNil();
		void WrBool(bool b);
		void WrInt(Sint32 n);
		void WrInt(Uint32 n);
		void WrInt(Sint64 n);
		void WrInt(Uint64 n);
		void WrReal(float n);
		void WrReal(double n);
		void WrString(const std::string &s);
		void WrQuaternion(const Quaternionf &q);
		void WrQuaternion(const Quaterniond &q);
		void WrVector(const vector3f &v);
		void WrVector(const vector3d &v);
		void WrMatrix(const matrix3x3f &m);
		void WrMatrix(const matrix3x3d &m);
		void WrMatrix(const matrix4x4f &m);
		void WrMatrix(const matrix4x4d &m);
		void WrColor(const Color3ub &c);
		void WrColor(const Color4ub &c);
	};

	class StructureWriter {};

	template <typename EncoderT>
	class ArrayWriter : public StructureWriter {
	public:
		ArrayWriter(EncoderT *stream): m_stream(stream), m_count(0) { assert(m_stream); m_stream->WrArrayStart(); }
		~ArrayWriter() { assert(m_stream); m_stream->WrArrayEnd(); }

		ObjectWriter<EncoderT> WrObject() { assert(m_stream); Next(); return ObjectWriter<EncoderT>(m_stream, this); }
		ArrayWriter<EncoderT> WrArray() { assert(m_stream); Next(); return ArrayWriter<EncoderT>(m_stream, this); }

		void WrNil() { assert(m_stream); Next(); m_stream->WrNil(); }
		void WrBool(bool b) { assert(m_stream); Next(); m_stream->WrBool(b); }
		void WrInt(Sint32 n) { assert(m_stream); Next(); m_stream->WrInt(n); }
		void WrInt(Uint32 n) { assert(m_stream); Next(); m_stream->WrInt(n); }
		void WrInt(Sint64 n) { assert(m_stream); Next(); m_stream->WrInt(n); }
		void WrInt(Uint64 n) { assert(m_stream); Next(); m_stream->WrInt(n); }
		void WrReal(float n) { assert(m_stream); Next(); m_stream->WrReal(n); }
		void WrReal(double n) { assert(m_stream); Next(); m_stream->WrReal(n); }
		void WrString(const std::string &s) { assert(m_stream); Next(); m_stream->WrString(s); }
		void WrQuaternion(const Quaternionf &q) { assert(m_stream); Next(); m_stream->WrQuaternion(q); }
		void WrQuaternion(const Quaterniond &q) { assert(m_stream); Next(); m_stream->WrQuaternion(q); }
		void WrVector(const vector3f &v) { assert(m_stream); Next(); m_stream->WrVector(v); }
		void WrVector(const vector3d &v) { assert(m_stream); Next(); m_stream->WrVector(v); }
		void WrMatrix(const matrix3x3f &m) { assert(m_stream); Next(); m_stream->WrMatrix(m); }
		void WrMatrix(const matrix3x3d &m) { assert(m_stream); Next(); m_stream->WrMatrix(m); }
		void WrMatrix(const matrix4x4f &m) { assert(m_stream); Next(); m_stream->WrMatrix(m); }
		void WrMatrix(const matrix4x4d &m) { assert(m_stream); Next(); m_stream->WrMatrix(m); }
		void WrColor(const Color3ub &c) { assert(m_stream); Next(); m_stream->WrColor(c); }
		void WrColor(const Color4ub &c) { assert(m_stream); Next(); m_stream->WrColor(c); }

	private:
		void Next() {
			if (m_count) { m_stream->WrArrayNext(); }
			++m_count;
		}

		EncoderT *m_stream;
		int m_count;

		// non-copyable, non-assignable, non-moveable
		ArrayWriter(const ArrayWriter<EncoderT>&) = delete;
		ArrayWriter<EncoderT>& operator=(const ArrayWriter<EncoderT>&) = delete;
		ArrayWriter(ArrayWriter<EncoderT> &&from) = delete;
		ArrayWriter<EncoderT>& operator=(ArrayWriter<EncoderT>&&) = delete;
	};

	template <typename EncoderT>
	class ObjectWriter {
	public:
		ObjectWriter(EncoderT *stream): m_stream(stream), m_count(0) { assert(m_stream); m_stream->WrObjectStart(); }
		~ObjectWriter() { assert(m_stream); m_stream->WrObjectEnd(); }

		ObjectWriter<EncoderT> WrObject(const char *fname);
		ArrayWriter<EncoderT> WrArray(const char *fname);

		void WrNil(const char *field) { assert(m_stream); Next(field); m_stream->WrNil(); }
		void WrBool(const char *field, bool b) { assert(m_stream); Next(field); m_stream->WrBool(b); }
		void WrInt(const char *field, Sint32 n) { assert(m_stream); Next(field); m_stream->WrInt(n); }
		void WrInt(const char *field, Uint32 n) { assert(m_stream); Next(field); m_stream->WrInt(n); }
		void WrInt(const char *field, Sint64 n) { assert(m_stream); Next(field); m_stream->WrInt(n); }
		void WrInt(const char *field, Uint64 n) { assert(m_stream); Next(field); m_stream->WrInt(n); }
		void WrReal(const char *field, float n) { assert(m_stream); Next(field); m_stream->WrReal(n); }
		void WrReal(const char *field, double n) { assert(m_stream); Next(field); m_stream->WrReal(n); }
		void WrString(const char *field, const std::string &s) { assert(m_stream); Next(field); m_stream->WrString(s); }
		void WrQuaternion(const char *field, const Quaternionf &q) { assert(m_stream); Next(field); m_stream->WrQuaternion(q); }
		void WrQuaternion(const char *field, const Quaterniond &q) { assert(m_stream); Next(field); m_stream->WrQuaternion(q); }
		void WrVector(const char *field, const vector3f &v) { assert(m_stream); Next(field); m_stream->WrVector(v); }
		void WrVector(const char *field, const vector3d &v) { assert(m_stream); Next(field); m_stream->WrVector(v); }
		void WrMatrix(const char *field, const matrix3x3f &m) { assert(m_stream); Next(field); m_stream->WrMatrix(m); }
		void WrMatrix(const char *field, const matrix3x3d &m) { assert(m_stream); Next(field); m_stream->WrMatrix(m); }
		void WrMatrix(const char *field, const matrix4x4f &m) { assert(m_stream); Next(field); m_stream->WrMatrix(m); }
		void WrMatrix(const char *field, const matrix4x4d &m) { assert(m_stream); Next(field); m_stream->WrMatrix(m); }
		void WrColor(const char *field, const Color3ub &c) { assert(m_stream); Next(field); m_stream->WrColor(c); }
		void WrColor(const char *field, const Color4ub &c) { assert(m_stream); Next(field); m_stream->WrColor(c); }

	private:
		void Next(const char *field) {
			if (m_count) { m_stream->WrObjectNext(); }
			++m_count;
			m_stream->WrObjectField(field);
		}

		EncoderT *m_stream;
		int m_count;

		// non-copyable, non-assignable, non-moveable
		ObjectWriter(const ObjectWriter<EncoderT>&) = delete;
		ObjectWriter<EncoderT>& operator=(const ObjectWriter<EncoderT>&) = delete;
		ObjectWriter(ObjectWriter<EncoderT> &&from) = delete;
		ObjectWriter<EncoderT>& operator=(ObjectWriter<EncoderT>&&) = delete;
	};

}

#endif  /* SERIALIZER_BASE_H */
