#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform bool isHovered;
uniform bool isAttack;
uniform bool isMove;

uniform sampler2D tex;

void main(){
  vec4 color = texture(tex, fragTexCoord);
  if(isHovered){
    color = mix(color, vec4(1.0, 1.0, 1.0, color.a), 0.5);
  }
  if(isAttack){
    color = mix(color, vec4(1.0, 0.0, 0.0, color.a), 0.5);
  }
  if(isMove){
    color = mix(color, vec4(0.0, 1.0, 0.0, color.a), 0.5);
  }

  finalColor = color;
}
