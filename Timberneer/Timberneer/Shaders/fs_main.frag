#version 120

// It was expressed that some drivers required this next line to function properly
//precision mediump float;

const float piByTwo = 1.570796327;
uniform float u_waterHeight;
uniform int u_portWidth;
uniform int u_portHeight;
const float normalMapInvSize = 1.0 / 8.0;
const float waterSpecularFactor = 1.0;
uniform float u_halfOverTanHalfAngleOfView;

varying vec2 v_texCoord;
varying vec3 v_normal;
varying vec3 v_surfaceToLight;

varying vec3 v_position;
varying vec3 v_camPos;
varying vec3 v_lightWorldPos;

uniform sampler2D u_texture;
uniform vec4 u_lightColour;
uniform vec4 u_colourMult;
uniform vec4 u_specular;
uniform float u_shininess;
uniform float u_specularFactor;

varying vec4 gl_FragCoord;
uniform bool u_water;
uniform sampler2D u_reflectionTexture;
uniform sampler2D u_riverNormalMapTexture;
uniform vec2 u_riverNormalMapOffset;
uniform mat4 u_viewProjection;
const float riverNormalMapValueScale = 0.03;

vec4 lit(float l, float h, float m) {
	return vec4(1.0,
				abs(l),
				(l > 0.0) ? pow(max(0.0, h), m) : 0.0,
				1.0);
}

//float inverseInvertibleS(float x){
//	const float invHarshness = 1.0 / 2.5;
//	return 1.0 - 1.0/(1.0 + pow(1.0/(1.0 - x) - 1.0, invHarshness));
//}
//vec3 inverseInvertibleSV(vec3 v){
//	return vec3(inverseInvertibleS(v.x), inverseInvertibleS(v.y), inverseInvertibleS(v.z));
//}

void main() {
	vec3 positionToCamera = v_camPos - v_position;
	float positionToCameraLength = length(positionToCamera);
	vec3 surfaceToView = positionToCamera / positionToCameraLength; // normalize(waterSurfaceToView) would be the same
	
	if(u_water && v_position.y < u_waterHeight){
		// water normals
		vec2 viewportCoord = vec2(gl_FragCoord.x / u_portWidth, gl_FragCoord.y / u_portHeight);
		float frac = (u_waterHeight - v_position.y) / positionToCamera.y;
		float positionToWaterPositionLength = frac*positionToCameraLength;
		vec3 waterPosition = v_position + frac*positionToCamera;
		vec2 normalMapCoord = (vec2(waterPosition.x, waterPosition.z) + u_riverNormalMapOffset)*normalMapInvSize;
		normalMapCoord.x -= float(int(normalMapCoord.x));
		normalMapCoord.y -= float(int(normalMapCoord.y));
		vec3 waterNormal = 2.0*riverNormalMapValueScale*(texture2D(u_riverNormalMapTexture, normalMapCoord).xyz - vec3(0.5, 0.5, 0.5));
		waterNormal.y = 1.0 - waterNormal.y;
		
		// normal colour
		vec4 diffuseColour = texture2D(u_texture, v_texCoord);
		vec3 a_normal = normalize(v_normal);
		vec3 surfaceToLight = normalize(v_surfaceToLight);
		vec3 halfVector = normalize(surfaceToLight + surfaceToView);
		vec4 litR = lit(dot(a_normal, surfaceToLight), dot(a_normal, halfVector), u_shininess);
		vec4 mainColour = vec4((u_lightColour * (diffuseColour * litR.y * u_colourMult + u_specular * litR.z * u_specularFactor)).rgb, diffuseColour.a);
		
		// water specular reflection and internal attentuation
		float attenuation = exp(-0.3*positionToWaterPositionLength);
		vec4 waterAttenuation = vec4(attenuation, (0.3*positionToWaterPositionLength + 1.0)*attenuation, (positionToWaterPositionLength + 1.0)*attenuation, 1.0f);
		mainColour *= waterAttenuation;
		vec3 waterSurfaceToLight = normalize(v_lightWorldPos - waterPosition);
		vec3 waterHalfVector = normalize(waterSurfaceToLight + surfaceToView);
		vec4 waterLitR = lit(dot(waterNormal, waterSurfaceToLight), dot(waterNormal, waterHalfVector), 5);
		vec4 underwaterColour = vec4((u_lightColour * (mainColour * waterLitR.y * u_colourMult + u_specular * waterLitR.z * u_specularFactor)).rgb, mainColour.a);
		
		// water reflection
		float tanHalfthetaX = waterNormal.x / waterNormal.y;
		float tanThetaX = 2.0*tanHalfthetaX / (1.0 - tanHalfthetaX*tanHalfthetaX);
		float tanHalfthetaY = waterNormal.z / waterNormal.y;
		float tanThetaY = 2.0*tanHalfthetaY / (1.0 - tanHalfthetaY*tanHalfthetaY);
		vec2 delta = ((1.0 - frac)*positionToCameraLength - u_halfOverTanHalfAngleOfView) * vec2(tanThetaX, tanThetaY);
		// this uses the approximation that the distance from the object in the new image is the same as the distance from the object in the original, undistorted image:
		delta *= positionToWaterPositionLength*u_halfOverTanHalfAngleOfView/(positionToCameraLength*(u_halfOverTanHalfAngleOfView - positionToCameraLength + positionToWaterPositionLength));
		vec2 reflectionViewportCoord = clamp(viewportCoord + delta, vec2(0.0, 0.0), vec2(1.0, 1.0));
		vec4 reflectionColour = vec4((u_lightColour * (texture2D(u_reflectionTexture, reflectionViewportCoord) * waterLitR.y + u_specular * waterLitR.z * waterSpecularFactor)).rgb, 1.0);
		float refractiveFactor = pow(dot(surfaceToView, waterNormal), 0.5);
		vec4 waterColour = mix(reflectionColour, underwaterColour, refractiveFactor);
		gl_FragColor = waterColour;
	} else {
		vec4 diffuseColour = texture2D(u_texture, v_texCoord);
		vec3 a_normal = normalize(v_normal);
		vec3 surfaceToLight = normalize(v_surfaceToLight);
		vec3 halfVector = normalize(surfaceToLight + surfaceToView);
		vec4 litR = lit(dot(a_normal, surfaceToLight), dot(a_normal, halfVector), u_shininess);
		vec4 mainColour = vec4((u_lightColour * (diffuseColour * litR.y * u_colourMult + u_specular * litR.z * u_specularFactor)).rgb, diffuseColour.a);
		gl_FragColor = mainColour;
	}
}
