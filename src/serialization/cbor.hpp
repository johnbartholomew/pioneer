#ifndef CBOR_HPP
#define CBOR_HPP

#include <iostream>
#include <string>
#include <limits>
#include <cstring>
#include <cstdint>
#include <cassert>

namespace cbor {

	struct UndefinedT {};
	static constexpr UndefinedT undefined{};

	struct NullT {};
	constexpr NullT null{};

	struct Tag { Tag() = delete; const uint64_t value; };

	namespace tags {
		constexpr Tag self_describe_cbor{55799u};
		constexpr Tag shared_object_mark{28u};
		constexpr Tag shared_object_ref{29u};
		constexpr Tag stringref_namespace{256u};
		constexpr Tag stringref{25u};
	}

	struct BytesSlice {
		BytesSlice() = delete;
		explicit BytesSlice(const std::string &s): p(s.data()), size(s.size()) {}
		explicit BytesSlice(const char *s, size_t n): p(s), size(n) {}
		explicit BytesSlice(const char *s): p(s), size(::strlen(s)) {}
		const char * const p;
		const size_t size;
	};

	struct StringSlice {
		StringSlice() = delete;
		explicit StringSlice(const std::string &s): p(s.data()), size(s.size()) {}
		explicit StringSlice(const char *s, size_t n): p(s), size(n) {}
		explicit StringSlice(const char *s): p(s), size(::strlen(s)) {}
		const char * const p;
		const size_t size;
	};

	// Low-level encoding functions for individual value types.
	// You can implement overloads of this function for other types to make the
	// structured writer classes support them.
	namespace encode {

        // Specialize this template for types you want to add support for...
        template<typename T>
		void Value(std::ostream &ss, const T &v);

		void Value(std::ostream &ss, int v);
		void Value(std::ostream &ss, unsigned int v);
		void Value(std::ostream &ss, int64_t v);
		void Value(std::ostream &ss, uint64_t v);
		void Value(std::ostream &ss, float v);
		void Value(std::ostream &ss, double v);
		void Value(std::ostream &ss, bool v);
		void Value(std::ostream &ss, NullT);
		void Value(std::ostream &ss, UndefinedT);
		void Value(std::ostream &ss, BytesSlice v);
		void Value(std::ostream &ss, StringSlice v);

		void TagBegin(std::ostream &ss, Tag v);
		void SequenceTerminator(std::ostream &ss);
		void ChunkedStringBegin(std::ostream &ss);
		void ChunkedBytesBegin(std::ostream &ss);
		void MapBegin(std::ostream &ss, size_t count);
		void MapBegin(std::ostream &ss);
		void ArrayBegin(std::ostream &ss, size_t count);
		void ArrayBegin(std::ostream &ss);
	}

    class Encoder {
    public:
        Encoder(std::ostream &s): stream(s) {}

        template <typename T>
        const Encoder &operator<<(const T &v) const {
            encode::Value(stream, v);
            return *this;
        }

        std::ostream &stream;
    };

	class WriterBase;
	class SingleValueWriter;
	class MapWriter;
	class ArrayWriter;
	class ChunkedStringWriter;
	class ChunkedBytesWriter;

	namespace detail {
		// Helper class used to ensure single transfer of the stream pointer
		// from a structured value (e.g., an array or map) to a sub-value.
		class MustWriteSingleValue {
		public:
			~MustWriteSingleValue();

			MustWriteSingleValue(MustWriteSingleValue &&from):
				m_parent(from.m_parent), m_stream(from.m_stream) {
				from.m_stream = nullptr;
				from.m_parent = nullptr;
			}

			MustWriteSingleValue &operator=(MustWriteSingleValue &&from) {
				if (this != &from) {
					MustWriteSingleValue tmp(std::move(from));
					std::swap(tmp.m_stream, m_stream);
					std::swap(tmp.m_parent, m_parent);
				}
				return *this;
			}

		private:
			friend class ::cbor::WriterBase;
			MustWriteSingleValue(WriterBase *parent, std::ostream *stream):
				m_parent(parent), m_stream(stream) {}
			WriterBase *m_parent;
			std::ostream *m_stream;
		};
	}

	class WriterBase {
	public:
		// Non-copyable, non-assignable, non-move-assignable.
		WriterBase(const WriterBase&) = delete;
		WriterBase &operator=(const WriterBase&) = delete;
		WriterBase &operator=(WriterBase&&) = delete;

		// When a writer finishes, it transfers control of the stream back to its parent.
		~WriterBase() {
			if (m_parent) {
				assert(m_stream);
				m_parent->ResumeStream(m_stream);
			}
		}

		// Move-constructable.
		WriterBase(WriterBase &&from):
			m_parent(from.m_parent), m_stream(from.m_stream) {
			from.m_parent = nullptr;
			from.m_stream = nullptr;
		}

		std::ostream &stream() { assert(m_stream); return *m_stream; }

	protected:
		explicit WriterBase(std::ostream &stream): m_parent(nullptr), m_stream(&stream) {}

		explicit WriterBase(detail::MustWriteSingleValue &&from):
			m_parent(from.m_parent), m_stream(from.m_stream) {
			assert(m_parent && m_stream);
			from.m_parent = nullptr;
			from.m_stream = nullptr;
		}

        detail::MustWriteSingleValue SubValue() {
			assert(m_stream);
			std::ostream *p = m_stream;
			m_stream = nullptr;
			return detail::MustWriteSingleValue(this, p);
		}

	private:
		friend class detail::MustWriteSingleValue;

		void ResumeStream(std::ostream *stream) {
			assert(stream && !m_stream);
			m_stream = stream;
		}

		WriterBase *m_parent;
		std::ostream *m_stream;
	};

	inline detail::MustWriteSingleValue::~MustWriteSingleValue() {
		assert(bool(m_stream) == bool(m_parent));
		if (m_stream) {
			// This is probably an error.
			assert(0 && "MustWriteSingleValue was dropped on the floor.");
			encode::Value(*m_stream, undefined);
			m_parent->ResumeStream(m_stream);
		}
	}

	class SingleValueWriter : public WriterBase {
	public:
		explicit SingleValueWriter(detail::MustWriteSingleValue &&v):
            WriterBase(std::move(v)), m_written(false) {}
		explicit SingleValueWriter(std::ostream &ss):
            WriterBase(ss), m_written(false) {}
		~SingleValueWriter() {
			assert(m_written);
			if (!m_written) { encode::Value(stream(), undefined); }
		}

		template <typename ValueT>
		void Put(const ValueT &v) {
			assert(!m_written);
            using namespace cbor::encode;
			Value(stream(), v);
			m_written = true;
		}

        detail::MustWriteSingleValue Value() { return SubValue(); }

	private:
		bool m_written;
	};

	class TaggedWriter : public SingleValueWriter {
	public:
		explicit TaggedWriter(detail::MustWriteSingleValue &&v, Tag tag_value):
			SingleValueWriter(std::move(v)) { encode::TagBegin(stream(), tag_value); }
		explicit TaggedWriter(std::ostream &ss, Tag tag_value):
			SingleValueWriter(ss) { encode::TagBegin(stream(), tag_value); }
	};

	class MapWriter : public WriterBase {
	public:
		explicit MapWriter(detail::MustWriteSingleValue &&v):
			WriterBase(std::move(v)) { encode::MapBegin(stream()); }
		explicit MapWriter(std::ostream &ss):
			WriterBase(ss) { encode::MapBegin(stream()); }
		~MapWriter() { encode::SequenceTerminator(stream()); }

		template <typename ValueT>
		void Put(const char *k, const ValueT &v) {
			auto &s = stream();
            using cbor::encode::Value;
			Value(s, StringSlice(k));
			Value(s, v);
		}

		template <typename KeyT, typename ValueT>
		void Put(const KeyT &k, const ValueT &v) {
			auto &s = stream();
            using cbor::encode::Value;
			Value(s, k);
			Value(s, v);
		}

		detail::MustWriteSingleValue Entry(const char *k) {
			encode::Value(stream(), StringSlice(k));
			return SubValue();
		}

		template <typename KeyT>
		detail::MustWriteSingleValue Entry(const KeyT &k) {
            using cbor::encode::Value;
			Value(stream(), k);
			return SubValue();
		}
	};

	class ArrayWriter : public WriterBase {
	public:
		explicit ArrayWriter(detail::MustWriteSingleValue &&v):
			WriterBase(std::move(v)) { encode::ArrayBegin(stream()); }
		explicit ArrayWriter(std::ostream &ss):
			WriterBase(ss) { encode::ArrayBegin(stream()); }
		~ArrayWriter() { encode::SequenceTerminator(stream()); }

		template <typename ValueT>
		void Put(const ValueT &v) {
            using cbor::encode::Value;
            Value(stream(), v);
            //Encoder(stream()) << v;
        }

		detail::MustWriteSingleValue Entry() { return SubValue(); }
	};

	class Atom {
		public:
			enum AtomType {
				EndOfStream,
				ErrorTruncated,
				ErrorInvalid,
				ErrorOutOfRange,

				Bool,
				Null,
				Undefined,
				SequenceTerminator,
				Integer,
				Float,
				Tag,
				String,
				Bytes,
				Array,
				Map,
			};

			AtomType type;

			bool isError() const;
			bool isEOF() const;
			bool isValue() const;  // Anything except EndOfStream or an error code.

			bool isBool() const;
			bool isNull() const;
			bool isUndefined() const;
			bool isMap() const;
			bool isArray() const;
			bool isTag() const;
			bool isInteger() const;
			bool isFloat() const;
			bool isNumber() const;	// Integer or Float.
			bool isIntegerNumber() const;  // Any number with an integer value.
			bool isString() const;
			bool isBytes() const;
			bool isKnownLength() const;
			bool isSequenceTerminator() const;

		private:
			uint8_t initial;
			union {
				uint64_t i;
				char p[8];
			};
	};

	class StreamDecoder {
		public:
			explicit StreamDecoder(std::istream &stream): stream_(&stream) {}

			// Raw stream access. Dangerous!
			std::istream &stream() { return *stream_; }

			Atom ReadAtom();

		private:
			std::istream *stream_;
	};

	namespace {
		static_assert(sizeof(uint8_t) == 1, "'char' type must be 8 bits.");
		static_assert(sizeof(uint16_t) == 2, "'char' type must be 8 bits.");
		static_assert(sizeof(uint32_t) == 4, "'char' type must be 8 bits.");
		static_assert(sizeof(uint64_t) == 8, "'char' type must be 8 bits.");
		static_assert(sizeof(float) == 4, "'float' type must be 32 bits.");
		static_assert(sizeof(double) == 8, "'double' type must be 64 bits.");
		static_assert(
				std::numeric_limits<size_t>::max() <=
				std::numeric_limits<uint64_t>::max(),
				"'size_t' must be 64-bits or less.");
		static_assert(std::numeric_limits<float>::is_iec559,
				"CBOR encoder requires IEEE-754 compliant 'float' type.");
		static_assert(std::numeric_limits<double>::is_iec559,
				"CBOR encoder requires IEEE-754 compliant 'double' type.");
	}

}  // namespace cbor

#endif
