#include "LineContainer.hpp"

#include "LineContainer.hpp"

USING_NS_CC;

LineContainer::LineContainer() {

	_dash = 10;
	_dashSpace = 10;
	_lineType = LINE_NONE;
	_screenSize = Director::getInstance()->getWinSize();
	_energyLineX = _screenSize.width * 0.96f;
	_energyHeight = _screenSize.height * 0.8f;

	glLineWidth(8.0 * CC_CONTENT_SCALE_FACTOR());

	this->reset();
}

LineContainer* LineContainer::create() {
	LineContainer * node = new LineContainer();
	if (node) {
		node->autorelease();
		return node;
	}
	CC_SAFE_DELETE(node);
	return NULL;
}

void LineContainer::reset() {
	_energy = 1.0;
	_energyDecrement = 0.015f;
}

void LineContainer::update(float dt) {
	_energy -= dt * _energyDecrement;
	if (_energy < 0) _energy = 0;
}

void LineContainer::setEnergyDecrement(float value) {
	_energyDecrement += value;
	if (_energyDecrement > 0.07) _energyDecrement = 0.07;
}

void LineContainer::draw(Renderer *renderer, const Mat4& transform, uint32_t flags) {


}
