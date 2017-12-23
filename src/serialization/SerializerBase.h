#ifndef SERIALIZER_BASE_H
#define SERIALIZER_BASE_H

#include "vector3.h"
#include "Quaternion.h"
#include "matrix3x3.h"
#include "matrix4x4.h"
#include "Color.h"
#include <sstream>
#include <memory>

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
		void Wr(bool b);
		void Wr(Sint32 n);
		void Wr(Sint64 n);
		void Wr(Uint64 n);
		void Wr(float n);
		void Wr(double n);
		void Wr(const std::string &s);
		void Wr(const Quaternionf &q);
		void Wr(const Quaterniond &q);
		void Wr(const vector3f &v);
		void Wr(const vector3d &v);
		void Wr(const matrix3x3f &m);
		void Wr(const matrix3x3d &m);
		void Wr(const matrix4x4f &m);
		void Wr(const matrix4x4d &m);
		void Wr(const Color3ub &c);
		void Wr(const Color4ub &c);
	};

	class StructureWriter {};

	template <typename EncoderT>
	class ArrayWriter : public StructureWriter {
	public:
		~ArrayWriter();

		ObjectWriter WrObject();
		ArrayWriter WrArray();

		void WrElementNil() { Next(); m_stream->WrNil(); }
		void WrElement(bool b) { Next(); m_stream->Wr(n); }
		void WrElement(Sint32 n) { Next(); m_stream->Wr(n); }
		void WrElement(Sint64 n) { Next(); m_stream->Wr(n); }
		void WrElement(Uint64 n) { Next(); m_stream->Wr(n); }
		void WrElement(float n) { Next(); m_stream->Wr(n); }
		void WrElement(double n) { Next(); m_stream->Wr(n); }
		void WrElement(const std::string &s) { Next(); m_stream->Wr(s); }
		void WrElement(const Quaternionf &q) { Next(); m_stream->Wr(q); }
		void WrElement(const Quaterniond &q) { Next(); m_stream->Wr(q); }
		void WrElement(const vector3f &v) { Next(); m_stream->Wr(v); }
		void WrElement(const vector3d &v) { Next(); m_stream->Wr(v); }
		void WrElement(const matrix3x3f &m) { Next(); m_stream->Wr(m); }
		void WrElement(const matrix3x3d &m) { Next(); m_stream->Wr(m); }
		void WrElement(const matrix4x4f &m) { Next(); m_stream->Wr(m); }
		void WrElement(const matrix4x4d &m) { Next(); m_stream->Wr(m); }
		void WrElement(const Color3ub &c) { Next(); m_stream->Wr(c); }
		void WrElement(const Color4ub &c) { Next(); m_stream->Wr(c); }

	private:
		friend class OutputStream;
		ArrayWriter(EncoderT &stream): m_stream(&stream), m_count(0) {}
		EncoderT *m_stream;
		int m_count;

		ArrayWriter(const ArrayWriter&) = delete;
		ArrayWriter& operator=(const ArrayWriter&) = delete;
	};

	template <typename EncoderT>
	class ObjectWriter {
	public:
		~ObjectWriter();

		ObjectWriter WrObject(const char *fname);
		ArrayWriter WrArray(const char *fname);

		void WrFieldNil(const char *fname) { Next(); m_stream->WrObjectField(fname); m_stream->WrNil(); }
		void WrField(const char *fname, bool b) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(b); }
		void WrField(const char *fname, Sint32 n) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(n); }
		void WrField(const char *fname, Sint64 n) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(n); }
		void WrField(const char *fname, Uint64 n) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(n); }
		void WrField(const char *fname, float n) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(n); }
		void WrField(const char *fname, double n) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(n); }
		void WrField(const char *fname, const std::string &s) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(s); }
		void WrField(const char *fname, const Quaternionf &q) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(q); }
		void WrField(const char *fname, const Quaterniond &q) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(q); }
		void WrField(const char *fname, const vector3f &v) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(v); }
		void WrField(const char *fname, const vector3d &v) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(v); }
		void WrField(const char *fname, const matrix3x3f &m) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(m); }
		void WrField(const char *fname, const matrix3x3d &m) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(m); }
		void WrField(const char *fname, const matrix4x4f &m) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(m); }
		void WrField(const char *fname, const matrix4x4d &m) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(m); }
		void WrField(const char *fname, const Color3ub &c) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(c); }
		void WrField(const char *fname, const Color4ub &c) { Next(); m_stream->WrObjectField(fname); m_stream->Wr(c); }
	private:
		friend class OutputStream;
		ObjectWriter(OutputStream &stream): m_stream(&stream) {}
		OutputStream *m_stream;

		ObjectWriter(const ObjectWriter&) = delete;
		ObjectWriter& operator=(const ObjectWriter&) = delete;
	};

}

#endif  /* SERIALIZER_BASE_H */
