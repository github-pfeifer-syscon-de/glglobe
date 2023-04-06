in float shade;
in vec2 UV;
uniform sampler2D tex;
out vec4 outputColor;

void main() {
    outputColor = vec4(texture(tex, UV).rgb, 1.0);
    if (shade < 0.0) {
        outputColor *= 0.3;
    }
}
