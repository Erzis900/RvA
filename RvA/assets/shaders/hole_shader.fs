#version 330

uniform vec2 holeCenter;
uniform vec2 resolution;
uniform vec2 holeSize;

in vec2 fragTexCoord;
out vec4 fragColor;

void main() {
    vec2 center = holeCenter;
    center.y = resolution.y - center.y; // Flip Y coordinate
    vec2 delta = (gl_FragCoord.xy - center) / holeSize; // Normalize by radius
    float dist = dot(delta, delta); // Squared distance

    float smoothness = 0.8;
    float edge0 = 1.0 - smoothness;
    float edge1 = 1.0;
    float alpha = mix(0.0, 0.7, smoothstep(edge0, edge1, dist));

    fragColor = vec4(0.0, 0.0, 0.0, alpha);
}