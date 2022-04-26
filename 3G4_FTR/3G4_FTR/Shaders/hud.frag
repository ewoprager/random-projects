#version 120

uniform sampler3D u_tex;
uniform mat4 u_viewMatrix;

varying vec2 v_texCoord;

void main() {
	gl_FragColor = texture3D(u_tex, (u_viewMatrix * vec4(vec2(v_texCoord.x - 0.5, 0.5 - v_texCoord.y), 0.0, 1.0)).xyz + vec3(0.5, 0.5, 0.5));
}
