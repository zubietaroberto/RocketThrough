#include "GridContainer.hpp"

GridContainer::GridContainer(){}

void GridContainer::reset(Size pScreenSize, std::vector<GameSprite*> planets){

  //create grid
  float gridFrame = pScreenSize.width * 0.1f;
  int tile = 32;
  int rows = (pScreenSize.height - 4 * gridFrame)/tile;
  int cols = (pScreenSize.width  - 2 * gridFrame)/tile;

  int count = planets.size();
  GameSprite * planet;
  Point cell;
  bool overlaps;
  for (int r = 0; r < rows; r++) {
      for (int c = 0; c < cols; c++) {
          cell = Vec2(gridFrame + c * tile, 2 * gridFrame + r * tile);
          overlaps = false;
          for (int j = 0; j < count; j++) {
              planet = planets.at(j);
              if (pow(planet->getPositionX() - cell.x, 2) + pow(planet->getPositionY() - cell.y, 2) <= pow(planet->getRadius() * 1.2f, 2)) {
                  overlaps = true;
              }
          }
          if (!overlaps) _grid.push_back(cell);
      }
  }

  CCLOG("POSSIBLE STARS: %i", _grid.size());

  std::shuffle(std::begin(_grid), std::end(_grid), randomEngine);
  _gridIndex = 0;

  //Extra calculations
  mGridWidth = pScreenSize.width/cols;
  mGridHeight = pScreenSize.height/rows;
}

Vec2 GridContainer::getNewCell(){

    Point position = _grid[_gridIndex];
    _gridIndex++;

    if ( _gridIndex == _grid.size() ) _gridIndex = 0;

    return position;

}

Vec2 GridContainer::getNewStarPosition(Vec2 pRocketPosition){

  Point position = getNewCell();

  // Check if the rocket is within 10 grid cells
  int gridXDistance = mGridWidth*10;
  int gridYDistance = mGridHeight*10;

  if ( position.x+gridXDistance > pRocketPosition.x
    && position.x-gridXDistance < pRocketPosition.x
    && position.y+gridYDistance > pRocketPosition.y
    && position.y-gridYDistance < pRocketPosition.y
  ){

    // Rocket is too close to the star. Search a new position.
    return getNewStarPosition(pRocketPosition);

  } else {

    return position;
  }
}
