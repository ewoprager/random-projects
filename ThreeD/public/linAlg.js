export var linAlg = {
	normalise: function(mat, index, pivot){
		var newMat = Object.assign([], mat);
		var multiplier = 1 / newMat[index][pivot]/1;
		for (let i=0; i<newMat[index].length; i++){
			if(newMat[index][i] != 0){
				newMat[index][i] *= multiplier;
			}
		}
		return newMat;
	},
	change: function(mat, index, pivot){
		if((index + 1) >= mat.length){
			return;
		}
		var newMat = Object.assign([], mat);
		for(var i=(index + 1); i<newMat.length; i++){
			var multiplier = newMat[i][pivot] * -1;
			for(var j = 0; j < newMat[i].length; j++){
				newMat[i][j] = newMat[i][j] + (newMat[index][j] * multiplier);
			}
		}
		return newMat;
	},
	echolonForm: function(mat){
		var pivots = mat.length - 1;
		for (var pivot = 0; pivot < pivots; pivot++){
			mat = linAlg.normalise(mat, pivot, pivot);
			mat = linAlg.change(mat, pivot, pivot);
		}
		return mat;
	},
	backSubstitute: function(A, b){ // returns x such that Ax = b, assuming A is upper triangular
		var n = b.length;
		var x = new Array(n);
		for(var i=n-1; i>=0; i--){
			x[i] = b[i];
			for(var j=n-1; j>i; j--){
				x[i] -= A[i][j]*x[j];
			}
			if(A[i][i] === 0){ return x; console.log("Error, pivot is zero: ", A); }
			x[i] /= A[i][i]/1;
		}
		return x;
	},
	matrixVectorMultiply: function(A, v){
		var ret = [];
		A.forEach(rowA => {
			ret.push(linAlg.dot(rowA, v));
		});
		return ret;
	},
	multiply: function(A, B){
		var Bt = linAlg.transpose(B);
		var ret = [];
		A.forEach(rowA => {
			var rowR = [];
			Bt.forEach(colB => {
				rowR.push(linAlg.dot(rowA, colB));
			});
			ret.push(rowR);
		});
		return ret;
	},
	transpose: function(A){
		var ret = [];
		for(var c=0; c<A[0].length; c++){
			ret.push([]);
			for(var r=0; r<A.length; r++){
				ret[c].push(A[r][c]);
			}
		}
		return ret;
	},
	dot: function(v1, v2){
		var ret = 0;
		v1.forEach((x, i) => {
			ret += x*v2[i];
		});
		return ret;
	},
	scalarVectorMultiply: function(scalar, vector){
		var ret = [];
		vector.forEach(x => { ret.push(scalar * x); });
		return ret;
	},
	vectorNormalise: function(v){
		var i_mag = 0;
		v.forEach(x => { i_mag += x*x; });
		i_mag = 1 / Math.sqrt(i_mag)/1;
		return linAlg.scalarVectorMultiply(i_mag, v);
	},
	vectorSubtract: function(v1, v2){
		var ret = [];
		v1.forEach((x, i) => { ret.push(x - v2[i]); });
		return ret;
	},
	gramSchmidt: function(A){
		var columns = linAlg.transpose(A);
		var Q = [];
		for(var i=0; i<columns.length; i++){
			var a = columns[i];
			var aHat = a;
			Q.forEach(q => {
				aHat = linAlg.vectorSubtract(aHat, linAlg.scalarVectorMultiply(linAlg.dot(q, a), q));
			});
			Q.push(linAlg.vectorNormalise(aHat));
		}
		return Q;
	},
	leastSquares: function(A, b){ // finds least squares solution x for Ax = b
		var Qt = linAlg.gramSchmidt(A);
		return linAlg.backSubstitute(linAlg.multiply(Qt, A), linAlg.matrixVectorMultiply(Qt, b));
	},
};
