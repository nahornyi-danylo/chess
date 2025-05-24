#version 330

in vec2 fragTexCoord;
out vec4 finalColor;

uniform bool isHovered;
uniform bool isAttack;
uniform bool isMove;
uniform bool isLastMove;

uniform sampler2D tex;

float borderThickness = 0.05;

vec2 localCoord = vec2(
  mod(fragTexCoord.x * 2.0, 1.0), // scale and wrap
  fragTexCoord.y                 // full height
);

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
  if(isLastMove){
    if(localCoord.x < borderThickness || localCoord.x > 1.0 -
        borderThickness || localCoord.y < borderThickness || localCoord.y >
        1.0 - borderThickness){

      color = vec4(1.0, 1.0, 0.0, 1.0);
    }
  }

  finalColor = color;
}
