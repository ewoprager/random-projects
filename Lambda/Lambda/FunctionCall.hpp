#ifndef MyLambda_h
#define MyLambda_h

#include <cstdint>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <string.h> // for 'sizeof()'

// for debugging - don't understand most of it; copied from the internet obviously
template <typename CharT> void dump(const CharT c) {
	const uint8_t* data {reinterpret_cast<const uint8_t*>(&c)};
	for (auto i {0U}; i != sizeof(CharT); ++i){
		std::cout << std::setw(2) << static_cast<unsigned>(data[i]) << ' ';
	}
}

// Function to find the size, in bytes, of a parameter pack
// 'sizeof...' finds the NUMBER of parameters in the pack, not the size in bytes of the whole pack

// this is my way, problem is I can't figure out how to make it work when given an empty type pack (and so return 0)
/*template <typename T> constexpr unsigned SizeofPack(){
	return sizeof(T);
}
template <typename T, typename T2, typename... Ts> constexpr unsigned SizeofPack(){
	return sizeof(T) + SizeofPack<T2, Ts...>();
}*/
// this I copied from the internet and is much more ugly but it can cope with empty type packs.
template <int index, typename... Ts> struct pack_size_index;
template <int index, typename T, typename... Ts> struct pack_size_index <index, T, Ts...> {
	static const int value = (index > 0) ? (sizeof(T) + pack_size_index<index - 1, Ts...>::value) : 0;
};
template <int index> struct pack_size_index<index> {
	static const int value = 0;
};

// A simple template to allow other template structs to take multiple paramter packs, through the use of template specialisation
template <typename... types> struct TypeList {};

// Struct for unpacking a byte-wise stored parameter pack and calling the appropriate function (this is the template to be specialised later). They are all pure functors, but can't (I don't think) be implemented as functions because they use template specialisation.
template <typename variableParameterTypeList, unsigned size, typename returnType, typename unpackedTypeList, typename... packedTypes> struct Unpacker;
// Specialisation to deal with the case where there are no fixed parameters
template <typename... variableParameterTypes, unsigned size, typename returnType> struct Unpacker<TypeList<variableParameterTypes...>, size, returnType, TypeList<>> {
	returnType operator()(returnType (*function)(variableParameterTypes...), unsigned char bytes[size], int index, variableParameterTypes... variableParameters){
		return (*function)(variableParameters...);
	}
};
// Specialisation of the original struct using TypeLists so that it can have multiple parameter packs. This specialisation is the 'base' one - the last one to appear in the recursive behaviour
template <typename... variableParameterTypes, unsigned size, typename returnType, typename... unpackedTypes, typename finalPackedType> struct Unpacker<TypeList<variableParameterTypes...>, size, returnType, TypeList<unpackedTypes...>, finalPackedType> {
	returnType operator()(returnType (*function)(unpackedTypes..., finalPackedType, variableParameterTypes...), unsigned char bytes[size], int index, unpackedTypes... unpacked, variableParameterTypes... variableParameters){
		finalPackedType theseBytes;
		memcpy(&theseBytes, &bytes[index], sizeof(finalPackedType));
		index += sizeof(finalPackedType);
		return (*function)(unpacked..., theseBytes, variableParameters...);
	}
};
// Another specialisation, this is the one that appears in all but the final instance in the recursive behaviour
template <typename... variableParameterTypes, unsigned size, typename returnType, typename... unpackedTypes, typename firstPackedType, typename... packedTypes> struct Unpacker<TypeList<variableParameterTypes...>, size, returnType, TypeList<unpackedTypes...>, firstPackedType, packedTypes...> {
	returnType operator()(returnType (*function)(unpackedTypes..., firstPackedType, packedTypes..., variableParameterTypes...), unsigned char bytes[size], int index, unpackedTypes... unpacked, variableParameterTypes... variableParameters){
		firstPackedType theseBytes;
		memcpy(&theseBytes, &bytes[index], sizeof(firstPackedType));
		index += sizeof(firstPackedType);
		Unpacker<TypeList<variableParameterTypes...>, size, returnType, TypeList<unpackedTypes..., firstPackedType>, packedTypes...> unpacker;
		return unpacker(function, bytes, index, unpacked..., theseBytes, variableParameters...);
	}
};

// The final, user-friendly, function-calling struct; template specialisation is used to allow multiple parameter packs, using 'TypeList's
template <typename returnType, typename fixedParameterTypeList, typename variableParameterTypeList> struct FunctionCall;
template <typename returnType, typename... fixedParameterTypes, typename... variableParameterTypes> struct FunctionCall<returnType, TypeList<fixedParameterTypes...>, TypeList<variableParameterTypes...>> {
	FunctionCall(returnType (*_function)(fixedParameterTypes..., variableParameterTypes...), fixedParameterTypes... fixedParameters) {
		function = _function;
		Store(0, fixedParameters...);
	}
	returnType operator()(variableParameterTypes... variableParameters){
		Unpacker<TypeList<variableParameterTypes...>, size, returnType, TypeList<>, fixedParameterTypes...> unpacker;
		return unpacker(function, bytes, 0, variableParameters...);
	}
private:
	//static constexpr unsigned size = SizeofPack<fixedParameterTypes...>(); // less ugly, but can't cope with empty type packs
	static constexpr int size = pack_size_index<sizeof...(fixedParameterTypes), fixedParameterTypes...>::value; // very ugly, but at least it copes with empty type packs
	returnType (*function)(fixedParameterTypes..., variableParameterTypes...);
	unsigned char bytes[size];
	// recursive function to byte-wise store the fixed parameter pack
	void Store(unsigned index){} // handles the empty pack case
	template <typename T> void Store(unsigned index, T val){
		memcpy(&bytes[index], &val, sizeof(T));
	}
	template <typename T, typename... Ts> void Store(unsigned index, T val, Ts... vals){
		memcpy(&bytes[index], &val, sizeof(T));
		index += sizeof(T);
		Store(index, vals...);
	}
};


/*
// this is the hacky way using a lambda expression; (also uses 'TypeList's)
template <typename returnType, typename fixedParameterTypeList, typename variableParameterTypeList> struct HackyFunctionCall;
template <typename returnType, typename... fixedParameterTypes, typename... variableParameterTypes> struct HackyFunctionCall<returnType, TypeList<fixedParameterTypes...>, TypeList<variableParameterTypes...>> {
public:
	HackyFunctionCall(returnType (*_function)(fixedParameterTypes..., variableParameterTypes...), fixedParameterTypes... fixedParameters){
		function = [_function, fixedParameters...](variableParameterTypes... variableParameters) -> returnType {
			return (*_function)(fixedParameters..., variableParameters...);
		};
	}
	returnType operator()(variableParameterTypes... variableParameters){
		return function(variableParameters...);
	}
	
private:
	std::function<returnType(variableParameterTypes...)> function;
};
*/

#endif /* MyLambda_h */
