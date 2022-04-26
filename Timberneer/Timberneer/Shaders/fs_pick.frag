#version 120

//precision mediump float;

uniform float u_waterHeight;

varying vec3 v_position;

uniform vec4 u_id;

void main() {
	vec4 id = u_id;
	if(v_position.y < u_waterHeight && all(equal(id, vec4(0, 0, 0, 0)))) id = vec4(0, 0, 0, 1);
	gl_FragColor = id;
}
