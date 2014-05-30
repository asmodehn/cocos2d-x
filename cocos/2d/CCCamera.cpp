#include "CCCamera.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerMouse.h"


NS_CC_BEGIN

Camera::Camera() :
	_projectionType(Director::Projection::_2D),
	_touchListener(nullptr),
	_zoomListener(nullptr)
#ifdef _WINDOWS
	, _mouseListener(nullptr)
	, _zoomVelocity(0.2f)
#else
	,_zoomVelocity(0.0000008f)
#endif //_WINDOWS
{
	// Arbitrary value from Director::setProjection
	_range.set(-1024, 1024);

	// Arbitrary value
	_zoomLimit.set(0.1f, 2.f);
	setZOrder(-1000);
}
	
Camera::~Camera()
{}

bool Camera::init()
{
	if (!cocos2d::Node::init())
	{
		return false;
	}
	_touchListener = EventListenerTouchOneByOne::create();
	_touchListener->setSwallowTouches(false);

	_touchListener->onTouchBegan = CC_CALLBACK_2(Camera::onTouchBegan, this);
	_touchListener->onTouchMoved = CC_CALLBACK_2(Camera::onTouchMoved, this);
	_touchListener->onTouchEnded = CC_CALLBACK_2(Camera::onTouchEnded, this);
	_touchListener->onTouchCancelled = CC_CALLBACK_2(Camera::onTouchCancelled, this);

#ifdef _WINDOWS
	_mouseListener = EventListenerMouse::create();
	_mouseListener->onMouseScroll = CC_CALLBACK_1(Camera::onMouseScroll, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_mouseListener, this);
#endif

	_zoomListener = EventListenerTouchAllAtOnce::create();
	_zoomListener->onTouchesBegan = CC_CALLBACK_2(Camera::onTouchesBegan, this);
	_zoomListener->onTouchesMoved = CC_CALLBACK_2(Camera::onTouchesMoved, this);
	_zoomListener->onTouchesEnded = CC_CALLBACK_2(Camera::onTouchesEnded, this);
	_zoomListener->onTouchesCancelled = CC_CALLBACK_2(Camera::onTouchesCancelled, this);

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_zoomListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	return true;
}

void Camera::getProjectionMatrix(cocos2d::Mat4& projMatrix)
{
	switch (_projectionType)
	{
	case cocos2d::Director::Projection::_2D:
		int left = _position.x;
		int right = _position.x + (_fov.x * _scaleX);

		int top = _position.y;
		int bottom = _position.y + (_fov.y * _scaleX);

		projMatrix.setIdentity();
		cocos2d::Mat4::createOrthographicOffCenter(left, right, top, bottom, _range.x, _range.y, &projMatrix);

		break;
	}
}

bool Camera::onTouchBegan(Touch *touch, Event *unused_event)
{
	return true;
}

void Camera::onTouchMoved(Touch *touch, Event *unused_event)
{
	setPosition(getPosition() - touch->getDelta());
}

void Camera::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void Camera::onTouchCancelled(Touch *touch, Event *unused_event)
{
}

bool Camera::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	//if (touches.size() == 2)
	{
		return true;
	}
}

void Camera::onTouchesMoved(const std::vector<Touch*>& touches, Event *unused_event)
{
	if (touches.size() == 2)
	{
		cocos2d::Vec2 currentOffset = touches[0]->getLocation() - touches[1]->getLocation();
		cocos2d::Vec2 previousOffset = touches[0]->getPreviousLocation() - touches[1]->getPreviousLocation();

		float zoomValue = (touches[0]->getPreviousLocation().getDistanceSq(touches[1]->getPreviousLocation()) - touches[0]->getLocation().getDistanceSq(touches[1]->getLocation())) * _zoomVelocity;
		//CCLOG("Zoom value is %f", zoomValue);
		addZoom(zoomValue);
	}
}

void Camera::onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{}

void Camera::onTouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
{}

#ifdef _WINDOWS
void Camera::onMouseScroll(Event* evt)
{
	EventMouse* e = static_cast<EventMouse*>(evt);
	float zoom = e->getScrollY() * _zoomVelocity;

	addZoom(zoom);
}
#else //_WINDOWS
#endif //_WINDOWS

NS_CC_END
