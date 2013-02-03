//
// MessagePack for C++ static resolution routine
//
// Copyright (C) 2008-2009 FURUHASHI Sadayuki
//
//    Licensed under the Apache License, Version 2.0 (the "License");
//    you may not use this file except in compliance with the License.
//    You may obtain a copy of the License at
//
//        http://www.apache.org/licenses/LICENSE-2.0
//
//    Unless required by applicable law or agreed to in writing, software
//    distributed under the License is distributed on an "AS IS" BASIS,
//    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//    See the License for the specific language governing permissions and
//    limitations under the License.
//
#ifndef MSGPACK_TYPE_CXX11_TUPLE_HPP__
#define MSGPACK_TYPE_CXX11_TUPLE_HPP__

#include "msgpack/object.hpp"

namespace msgpack {

namespace detail {

template<int Pos, typename... T>
struct tuple_helper {
	static void unpack(object o, std::tuple<T...>& v) {
		tuple_helper<Pos-1, T...>::unpack(o, v);
		o.via.array.ptr[Pos].convert<std::tuple_element<Pos, T...>::type>(&std::get<Pos>(v));
	}
	template<typename Stream>
	static void pack(packer<Stream>& o, const std::tuple<T...>& v) {
		tuple_helper<Pos-1, T...>::pack(o, v);
		o.pack(std::get<Pos>(v));
	}
	static void pack(object::with_zone& o, const std::tuple<T...>& v) {
		tuple_helper<Pos-1, T...>::pack(o, v);
		o.via.array.ptr[Pos] = object(std::get<Pos>(v), o.zone);
	}
};

template<typename... T>
struct tuple_helper<-1, T...> {
	static void unpack(object o, std::tuple<T...>& v) {}
	template<typename Stream>
	static void pack(packer<Stream>& o, const std::tuple<T...>& v) {}
	static void pack(object::with_zone& o, const std::tuple<T...>& v) {}
};

} // namespace msgpack::detail

template<typename... T>
inline std::string& operator>> (object o, std::tuple<T...>& v)
{
	if(o.type != type::ARRAY) { throw type_error(); }
	typedef std::tuple_size<std::tuple<T...>> size;
	if(o.via.array.size < size::value) { throw type_error(); }

	detail::tuple_helper<size::value-1, T...>::unpack(o, v);
	return v;
}

template <typename Stream, typename... T>
inline packer<Stream>& operator<< (packer<Stream>& o, const std::tuple<T...>& v)
{
	typedef std::tuple_size<std::tuple<T...>> size;
	o.pack_array(size::value);
	detail::tuple_helper<size::value-1, T...>::pack(o, v);
	return o;
}

template <typename... T>
inline void operator<< (object::with_zone& o, const std::tuple<T...>& v)
{
	o.type = type::ARRAY;
	typedef std::tuple_size<std::tuple<T...>> size;
	o.via.array.ptr = size::value==0 ? nullptr :
		(object *)o.zone->malloc(sizeof(object)*size::value);
	o.via.array.size = size::value;
	detail::tuple_helper<size::value-1, T...>::pack(o, v);
}

}  // namespace msgpack

#endif /* msgpack/type/c++11/tuple.hpp */
