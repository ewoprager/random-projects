#include "FunctionCall.hpp"

int globalVariable;

int PointerFunction(int *aPointer, int b){
	*aPointer += b;
	return *aPointer;
}

float FixedFunction(float x){
	return x * x;
}

float Linear(float gradient, float constant, float x){
	return gradient * x + constant;
}

// Cannot pass 'FunctionCall's as function pointers (they are functors); instead must be passed as 'FunctionCall' templates with appropriate return types and variable parameter types, with the fixed parameter type lists as template arguments.
// A 'FunctionCall' doesn't need to have fixed parameters (in which case it will function exactly like a normal function pointer), so normal function pointers can still be used with functions, like this one, that are designed to work with 'FunctionCalls'.
template <typename FPTL1, typename FPTL2> float NewtonRaphson(float x0, FunctionCall<float, FPTL1, TypeList<float>> funcCall1, FunctionCall<float, FPTL2, TypeList<float>> funcCall2){
	float x = x0;
	float delta = 1.0e-5f;
	float deltaHalfInverse = 5.0e4f;
	for(int i=0; i<20; i++){
		float grad = ((funcCall1(x + delta) - funcCall2(x + delta)) - (funcCall1(x - delta) - funcCall2(x - delta))) * deltaHalfInverse;
		x -= (funcCall1(x) - funcCall2(x)) / grad;
	}
	return x;
}

int main(int argc, const char * argv[]) {
	std::cout << "This program demonstrates storing a function and some parameters to call later on, without using a lambda expression (manually creating, writing to and reading from memory for the stored parameters, rather than using the built-in lambda weirdness way of storing the parameters as taken from the lambda's original scope):" << std::endl;
	
	// an example using a pointer
	FunctionCall<int, TypeList<int *>, TypeList<int>> myFuncCall = FunctionCall<int, TypeList<int *>, TypeList<int>>(&PointerFunction, &globalVariable);
	globalVariable = 10;
	int ret = myFuncCall(3);
	globalVariable += 5;
	myFuncCall(2);
	std::cout << ret << ", " << globalVariable << std::endl;
	
	// an example with no fixed parameters
	FunctionCall<float, TypeList<float, float>, TypeList<float>> line1 = FunctionCall<float, TypeList<float, float>, TypeList<float>>(&Linear, 0.5f, 1.0f);
	std::cout << NewtonRaphson(0.0f, line1, FunctionCall<float, TypeList<>, TypeList<float>>(&FixedFunction)) << std::endl;
	
	// an example with no variable parameters
	FunctionCall<float, TypeList<float>, TypeList<>> allFixedFunctionCall = FunctionCall<float, TypeList<float>, TypeList<>>(&FixedFunction, 3.0f);
	std::cout << allFixedFunctionCall() << std::endl;
	
	return 0;
}
