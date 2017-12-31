#include "cbor.hpp"

struct Vec3 {
	float v[3];
};

namespace tags {
	constexpr cbor::Tag pioneer_vec3{2501u};
}

namespace cbor { namespace encode {
	template <>
	void Value(std::ostream &ss, const Vec3 &v) {
		cbor::encode::TagBegin(ss, ::tags::pioneer_vec3);
		cbor::encode::ArrayBegin(ss, 3);
		cbor::encode::Value(ss, v.v[0]);
		cbor::encode::Value(ss, v.v[1]);
		cbor::encode::Value(ss, v.v[2]);
	}
}}

int main(int argc, char **argv) {
	cbor::MapWriter root(std::cout);
	root.Put("version", 1);

	root.Put("test_float", 3.14f);
	root.Put("test_double", 3.14);

	root.Put("test_null", cbor::null);
	root.Put("test_true", true);
	root.Put("test_false", false);
	root.Put("test_undef", cbor::undefined);

	root.Put("test_bytes", cbor::BytesSlice("hello"));
	root.Put("test_string", cbor::StringSlice("world"));

	{
		cbor::ArrayWriter vecs(root.Entry("dumb_matrix"));
		vecs.Put(Vec3{1.0f, 0.0f, 0.0f});
		vecs.Put(Vec3{0.0f, 1.0f, 0.0f});
		vecs.Put(Vec3{0.0f, 0.0f, 1.0f});
	}

	return 0;
}
