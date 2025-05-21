#version 330

in vec2 fragTexCoord;
uniform sampler2D texture0;
uniform vec4 highlightColor;

out vec4 finalColor;

void main() {
    vec4 base = texture(texture0, fragTexCoord);
    finalColor = base + highlightColor * base.a;
}