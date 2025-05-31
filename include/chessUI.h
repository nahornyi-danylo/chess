#include "ui.h"

struct drawInfo{
  Texture2D cellTexture;
  Texture2D piecesTexture;
  Vector2 cellSize;
  Rectangle lightCellSrcRec;
  Rectangle darkCellSrcRec;
  Rectangle piecesSrc[2][6];
  int reverseBoard;
  int *playingAs;

  Shader cellShader;
  int hoverLoc;
  int attackLoc;
  int moveLoc;
  int lastMoveLoc;
  int hovered[64];
  int canBeAttack[64];
  int canBeMove[64];
  int lastMoveArr[64];
  int checkedKingPos;
};

uiElement *chessUIGetBoard(Rectangle bounds, int reversed);
