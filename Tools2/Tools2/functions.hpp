#ifndef functions_h
#define functions_h

#include <cmath>

template <typename T=float> T Add			(T a, T b){ return a + b; 		}
template <typename T=float> T Multiply		(T a, T b){ return a * b; 		}
template <typename T=float> T Subtract		(T a, T b){ return a - b; 		}
template <typename T=float> T Divide		(T a, T b){ return a / b; 		}
template <typename T=float> T Power			(T a, T b){ return pow(a, b); 	}
template <typename T=float> bool LessThan	(T a, T b){ return (a < b);		}
template <typename T=float> bool MoreThan	(T a, T b){ return (a > b);		}
template <typename T=float> double ATanRatio(T a, T b){
	if(b == 0){
		return ( (a < 0) ? -PI/2 : PI/2 );
	} else {
		return atan(a/b) + ( (b < 0) ? PI : 0 );
	}
}

#endif /* functions_h */
