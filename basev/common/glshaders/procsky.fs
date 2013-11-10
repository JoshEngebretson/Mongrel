#version 110

uniform sampler2D SkyTex;
varying vec3 EyeV;
uniform vec3 OffsetV;

void main(void) {

    vec3 V = normalize( EyeV ) + OffsetV;
    vec2 lt = vec2( (1.0 + normalize( V.xy ).y) / 2.0, 1.0-normalize( V ).z );
    vec4 skyLight = texture2D( SkyTex, lt );

    gl_FragColor = skyLight;
}
