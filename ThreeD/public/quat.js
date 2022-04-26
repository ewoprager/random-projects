export var quat = {
	identity: [1, 0, 0, 0],
	normalise: function(q){
		var i_mag = 1/Math.sqrt(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3])/1;
		return [q[0]*i_mag, q[1]*i_mag, q[2]*i_mag, q[3]*i_mag];
	},
	product: function(q1, q2){
		var w = q1[0]*q2[0] - q1[1]*q2[1] - q1[2]*q2[2] - q1[3]*q2[3];
		var x = q1[0]*q2[1] + q1[1]*q2[0] + q1[2]*q2[3] - q1[3]*q2[2];
		var y = q1[0]*q2[2] + q1[2]*q2[0] + q1[3]*q2[1] - q1[1]*q2[3];
		var z = q1[0]*q2[3] + q1[3]*q2[0] + q1[1]*q2[2] - q1[2]*q2[1];
		return [w, x, y, z];
	},
	inverse: function(q){
		var i_sqMag = 1/(q[0]*q[0] + q[1]*q[1] + q[2]*q[2] + q[3]*q[3])/1;
		return [q[0]*i_sqMag, -q[1]*i_sqMag, -q[2]*i_sqMag, -q[3]*i_sqMag];
	},
	rotate: function(q, p){
		return quat.product(quat.product(q, p), quat.inverse(q));
	},
	makeRotation: function(v3, angle, unit=false){
		angle *= 0.5;
		var s = Math.sin(angle);
		var i_mag = 1;
		if(!unit) i_mag = 1/Math.sqrt(v3[0]*v3[0] + v3[1]*v3[1] + v3[2]*v3[2])/1;
		return [Math.cos(angle), s*v3[0]*i_mag, s*v3[1]*i_mag, s*v3[2]*i_mag];
	},
	dot: function(q1, q2){
		return q1[0]*q2[0] + q1[1]*q2[1] + q1[2]*q2[2] + q1[3]*q2[3];
	},
	slerp: function(q1, q2, frac){
		var theta = Math.acos(quat.dot(quat.normalise(q1), quat.normalise(q2)));
		var i_s = 1/Math.sin(theta)/1;
		var f1 = Math.sin((1 - frac) * theta) * i_s;
		var f2 = Math.sin(frac * theta) * i_s;
		return [f1*q1[0] + f2*q2[0],
				f1*q1[1] + f2*q2[1],
				f1*q1[2] + f2*q2[2],
				f1*q1[3] + f2*q2[3]];
	},
	matrix: function(q){
		return [1 - 2*(q[2]*q[2] + q[3]*q[3]), 2*(q[1]*q[2] + q[0]*q[3]), 2*(q[1]*q[3] - q[0]*q[2]), 0,
				2*(q[1]*q[2] - q[0]*q[3]), 1 - 2*(q[1]*q[1] + q[3]*q[3]), 2*(q[2]*q[3] + q[0]*q[1]), 0,
				2*(q[1]*q[3] + q[0]*q[2]), 2*(q[2]*q[3] - q[0]*q[1]), 1 - 2*(q[1]*q[1] + q[2]*q[2]), 0,
				0, 0, 0, 1];
	},
};
