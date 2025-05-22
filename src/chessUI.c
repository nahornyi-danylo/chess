#include "../include/ui.h"
#include "../include/chessUI.h"
#define LOG_PREFIX "chess UI"
#include "../include/common.h"
#include "../include/chess.h"
//#include <raylib.h>

// nobody will have more than one board, so I MUST hardcode on a single global instance
extern struct board board;

static struct {
  Texture2D cellTexture;
  Texture2D piecesTexture;
  Vector2 cellSize;
  Rectangle lightCellSrcRec;
  Rectangle darkCellSrcRec;
  Rectangle piecesSrc[2][6];
  int reverseBoard;

  Shader cellShader;
  int hoverLoc;
  int attackLoc;
  int moveLoc;
  int hovered[64];
  int canBeAttack[64];
  int canBeMove[64];
}boardInfo;

static const char *cellTexturePath = "../resources/default/cellsTest.png";
static const char *piecesTexturePath = "../resources/default/ChessPiecesArray.png";
static const char *cellShaderPath = "../resources/default/cellfs.glsl";

static void handler(unsigned permission, uiElement *element){
  struct move mlist[256];
  Vector2 mousepos = GetMousePosition();
  int i;
  int boardX = element->positionAbsolute.x;
  int boardY = element->positionAbsolute.y;

  int cellW = boardInfo.cellSize.x;
  int cellH = boardInfo.cellSize.y;

  // compute file (0–7) and rank (0–7)
  int file = 8 - (element->size.y-(mousepos.x - boardX)) / cellW;
  int rank = (element->size.x-(mousepos.y - boardY)) / cellH;

  i = 8*rank+file;
  if(boardInfo.reverseBoard) i = 63-i;
  for(int j = 0; j<64; j++){
    if(j == i) boardInfo.hovered[j] = 1;
    else boardInfo.hovered[j] = 0;
  }
  if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
    int k = generateLegalMoves(mlist, i);
    for(int j = 0; j<64; j++){
      boardInfo.canBeAttack[j] = 0;
      boardInfo.canBeMove[j] = 0;
    }
    for(int l = 0; l<k; l++){
      if(mlist[l].type == CAPTURE || mlist[l].type == EN_PASSANT){
        boardInfo.canBeAttack[mlist[l].to] = 1;
      }
      else{
        boardInfo.canBeMove[mlist[l].to] = 1;
      }
    }

  }

}

static void drawBoard(uiElement *element){
  int j;
  Rectangle *current = &boardInfo.lightCellSrcRec;
  for(int i = 0; i<64; i++){
    if(boardInfo.reverseBoard) j = 63-i;
    else j = i;
    int file =  j % 8;
    int rank =  (63-j) / 8;
    if((file == 0 && !boardInfo.reverseBoard) || (file == 7 && boardInfo.reverseBoard)){
        current = (current == &boardInfo.darkCellSrcRec)?&boardInfo.lightCellSrcRec:&boardInfo.darkCellSrcRec;
    }
    BeginShaderMode(boardInfo.cellShader);
    SetShaderValue(boardInfo.cellShader, boardInfo.hoverLoc, &boardInfo.hovered[i], SHADER_UNIFORM_INT);
    SetShaderValue(boardInfo.cellShader, boardInfo.attackLoc, &boardInfo.canBeAttack[i], SHADER_UNIFORM_INT);
    SetShaderValue(boardInfo.cellShader, boardInfo.moveLoc, &boardInfo.canBeMove[i], SHADER_UNIFORM_INT);
    DrawTexturePro(
        boardInfo.cellTexture,
        *current,
        (Rectangle){element->positionAbsolute.x+(file)*boardInfo.cellSize.x, element->positionAbsolute.y+(rank)*boardInfo.cellSize.y, boardInfo.cellSize.x, boardInfo.cellSize.y},
        (Vector2){0},
        0,
        WHITE
        );
    EndShaderMode();

    current = current == &boardInfo.darkCellSrcRec?&boardInfo.lightCellSrcRec:&boardInfo.darkCellSrcRec;
  }
  
  for(int i = 0; i<64; i++){
    if(boardInfo.reverseBoard) j = 63-i;
    else j = i;
    int file =  (j) % 8;
    int rank =  (63-j) / 8;
    if(board.board[i].type != NONE){
      DrawTexturePro(
          boardInfo.piecesTexture,
          boardInfo.piecesSrc[board.board[i].side][board.board[i].type-1],
          (Rectangle){element->positionAbsolute.x+(file)*boardInfo.cellSize.x, element->positionAbsolute.y+(rank)*boardInfo.cellSize.y, boardInfo.cellSize.x, boardInfo.cellSize.y},
          (Vector2){0},
          0,
          WHITE
          );
    }
  }

}

//enum pieceType{
  //NONE = 0,
  //PAWN,
  //BISHOP,
  //KNIGHT,
  //ROOK,
  //QUEEN,
  //KING
//};
uiElement *chessUIGetBoard(Rectangle bounds, int reversed){

  uiElement *result = bestow(sizeof(uiElement));
  boardInfo.cellTexture = LoadTexture(cellTexturePath);
  boardInfo.piecesTexture = LoadTexture(piecesTexturePath);
  boardInfo.cellShader = LoadShader(NULL, cellShaderPath);
  boardInfo.hoverLoc = GetShaderLocation(boardInfo.cellShader, "isHovered");
  boardInfo.attackLoc = GetShaderLocation(boardInfo.cellShader, "isAttack");
  boardInfo.moveLoc = GetShaderLocation(boardInfo.cellShader, "isMove");
  
  // I REALLY want to index the src recs with my enum and color,
  // however, in the texture it's almost completly backwards.
  // I don't think this can be done in a pretty loop

  for(int i = 0; i<2; i++){
#define map(piece, ix) boardInfo.piecesSrc[1-i][piece-1] = (Rectangle){ix*(float)boardInfo.piecesTexture.width/6, (float)i*boardInfo.piecesTexture.height/2, (float)boardInfo.piecesTexture.width/6, (float)boardInfo.piecesTexture.height/2};
    map(QUEEN, 0);
    map(KING, 1);
    map(ROOK, 2);
    map(KNIGHT, 3);
    map(BISHOP, 4);
    map(PAWN, 5)
#undef map
  }
  // condolences to whoever reads this

  boardInfo.cellSize = (Vector2){bounds.width/8, bounds.height/8};
  boardInfo.lightCellSrcRec = (Rectangle){0, 0, (float)boardInfo.cellTexture.width/2, boardInfo.cellTexture.height};
  boardInfo.darkCellSrcRec = (Rectangle){(float)boardInfo.cellTexture.width/2, 0, (float)boardInfo.cellTexture.width/2, boardInfo.cellTexture.height};

  boardInfo.reverseBoard = reversed;

  *result = (uiElement){0};
  *result = (uiElement){
    .positionAbsolute.x = bounds.x,
    .positionAbsolute.y = bounds.y,
    .size.x = bounds.width,
    .size.y = bounds.height,
    .draw = drawBoard,
    .handledState = 0b1001,
    .handlerFunction = handler,

  };

  return result;
}
