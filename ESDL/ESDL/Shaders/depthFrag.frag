float near = 0.1;
float far  = 100.0;
  
float LinearizeDepth(float depth){
	float z = depth * 2.0 - 1.0; // back to NDC
	return (2.0 * near * far) / (far + near - z * (far - near));
}

void main() {
	float depth = LinearizeDepth(gl_FragCoord.z) / far; // divide by far for demonstration
	gl_FragColor = vec4(vec3(depth), 1.0);
	//gl_FragColor = vec4(vec3(gl_FragCoord.z), 1.0);
}
