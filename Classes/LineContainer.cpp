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
	_drawNode = DrawNode::create();

	glLineWidth(8.0 * CC_CONTENT_SCALE_FACTOR());

	this->reset();
	this->addChild(_drawNode);
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

	_drawNode->clear();

	switch (_lineType) {
	case LINE_NONE:
		break;

	case LINE_TEMP:
		_drawNode->drawLine(_tip, _pivot, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
		_drawNode->drawDot(_pivot, 5, Color4F(1.0f, 1.0f, 1.0f, 1.0f));
		break;

	case LINE_DASHED:
		_drawNode->drawDot(_pivot, 5, Color4F(Color3B::WHITE));
		int segments = _lineLength / (_dash + _dashSpace);
		float t = 0.0f;
		float x_;
		float y_;
		for (int i = 0; i < segments + 1; i++) {
			x_ = _pivot.x + t * (_tip.x - _pivot.x);
			y_ = _pivot.y + t * (_tip.y - _pivot.y);
			_drawNode->drawDot(Vec2(x_, y_), 5, Color4F(Color3B::WHITE));
			t += (float)1 / segments;
		}
		break;
	}

	_drawNode->drawLine(
		Vec2(_energyLineX, _screenSize.height * 0.1f),
		Vec2(_energyLineX, _screenSize.height * 0.9f),
		Color4F(0.0, 0.0, 0.0, 0.0)
	);

	_drawNode->drawLine(
		Vec2(_energyLineX, _screenSize.height * 0.1f),
		Vec2(_energyLineX, _screenSize.height *0.1f + _energy * _energyHeight),
		Color4F(1.0, 0.5, 0.0, 1.0)
	);

}

void LineContainer::setEnergyDecrement(float value) {
	_energyDecrement += value;
	if (_energyDecrement > 0.07) _energyDecrement = 0.07;
}

void LineContainer::draw(Renderer *renderer, const Mat4& transform, uint32_t flags) {


}
