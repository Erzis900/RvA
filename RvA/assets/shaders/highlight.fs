#ifdef GL_ES
precision mediump float;
#endif

varying vec4 fragColor;
varying vec2 fragTexCoord;
uniform sampler2D texture0;
uniform vec4 highlightColor;

void main() {
    vec4 base = texture2D(texture0, fragTexCoord);
    gl_FragColor = (base * fragColor) + highlightColor * base.a;
}