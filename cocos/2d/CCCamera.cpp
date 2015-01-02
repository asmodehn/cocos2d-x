#include "CCCamera.h"
#include "base/CCEventListenerTouch.h"
#include "base/CCEventListenerMouse.h"

#include <algorithm>

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
	, _panMode(PAN_CENTER)
	, _touchCount(0)
{
	// Arbitrary value from Director::setProjection
	_range.set(-1024, 1024);

	// Arbitrary value
	_zoomLimit.set(0.1f, 2.f);
	setZOrder(-1000);

	// set max limit
	_panLimit.first.set(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest());
	_panLimit.second.set(std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
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

	setEnable(true);

	return true;
}

void Camera::setEnable(bool enable)
{
	if (enable)
	{
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_zoomListener, this);
		_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);
	}
	else
	{
		_eventDispatcher->removeEventListenersForTarget(this);
		_eventDispatcher->removeEventListenersForTarget(this);
		// reset in case it was zooming
		_touchCount = 0;
	}
}

void Camera::setPosition(const Vec2 &position)
{
	Vec2 newPos = clampToLimit(position);
	Node::setPosition(newPos);
}

void Camera::setPosition(float x, float y)
{
	Vec2 newPos = clampToLimit(cocos2d::Vec2(x, y));
	Node::setPosition(newPos);
}

void Camera::getProjectionMatrix(cocos2d::Mat4& projMatrix)
{
	switch (_projectionType)
	{
	case cocos2d::Director::Projection::_2D:
		//we center the camera on its position
		//-> the center of the view will be the position of the camera
		int left = _position.x - (_fov.x * _scaleX)/2;
		int right = _position.x + (_fov.x * _scaleX)/2;

		int bottom = _position.y - (_fov.y * _scaleX)/2;
		int top = _position.y + (_fov.y * _scaleX)/2;

		projMatrix.setIdentity();
		cocos2d::Mat4::createOrthographicOffCenter(left, right, bottom, top, _range.x, _range.y, &projMatrix);

		break;
	}
}

Vec2 Camera::convertCenterToWorldSpace(const Vec2& nodePoint) const
{
	Mat4 tmp = getNodeToWorldTransform();
	//because _fov is the size of the camera viewport
	//we can do the inverse transform on the nodePoint to compensate for centering camera
	Vec3 vec3(nodePoint.x - _fov.x / 2, nodePoint.y - _fov.y / 2, 0);
	Vec3 ret;
 	tmp.transformPoint(vec3, &ret);
	return Vec2(ret.x, ret.y);
}

Vec2 Camera::convertWorldSpaceToCenter(const Vec2& worldPoint) const
{
	Mat4 tmp = getWorldToNodeTransform();
	//because _fov is the size of the camera viewport
	//we can do the inverse transform on the nodePoint to compensate for centering camera
	Vec3 vec3(worldPoint.x + _fov.x * _scaleX / 2, worldPoint.y + _fov.y * _scaleX / 2, 0);
	Vec3 ret;
	tmp.transformPoint(vec3, &ret);
	return Vec2(ret.x, ret.y);
}

bool Camera::onTouchBegan(Touch *touch, Event *unused_event)
{
	return _touchCount <= 1;
}

void Camera::onTouchMoved(Touch *touch, Event *unused_event)
{
	if (_touchCount <= 1)
	{
		if (touch->getPreviousLocation().distanceSquared(touch->getLocation()) > 20.f)
		{
			_eventDispatcher->cancelEvent(_touchListener, touch);
		}
		Vec2 newPos = getPosition() - touch->getDelta() * getZoom();

		setPosition(newPos);
	}
}

void Camera::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void Camera::onTouchCancelled(Touch *touch, Event *unused_event)
{
}

void Camera::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{
	_touchCount += touches.size();
	CCLOG("onTouchesBegan touches.size() %i", _touchCount);
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

		setPosition(getPosition());
	}
}

void Camera::onTouchesEnded(const std::vector<Touch*>& touches, Event *unused_event)
{
	_touchCount -= touches.size();
	CCLOG("onTouchesEnded touches.size() %i", _touchCount);
}

void Camera::onTouchesCancelled(const std::vector<Touch*>& touches, Event *unused_event)
{
	_touchCount -= touches.size();
}

#ifdef _WINDOWS
void Camera::onMouseScroll(Event* evt)
{
	EventMouse* e = static_cast<EventMouse*>(evt);
	float zoom = e->getScrollY() * _zoomVelocity;

	addZoom(zoom);
	setPosition(getPosition());
}
#else //_WINDOWS
#endif //_WINDOWS

cocos2d::Vec2 Camera::clampToLimit(const cocos2d::Vec2& camPos)
{
	cocos2d::Vec2 clampPos = camPos;
	switch (_panMode)
	{
	case PAN_CENTER:
		clampPos.x = std::max(clampPos.x, _panLimit.first.x);
		clampPos.x = std::min(clampPos.x, _panLimit.second.x);
		clampPos.y = std::max(clampPos.y, _panLimit.first.y);
		clampPos.y = std::min(clampPos.y, _panLimit.second.y);
		break;
	case PAN_BORDER:
		clampPos.x = std::max(clampPos.x, _panLimit.first.x + (_fov.x * getZoom() - _fov.x) / 2);
		clampPos.x = std::min(clampPos.x, _panLimit.second.x - (_fov.x * getZoom() - _fov.x) / 2);
		clampPos.y = std::max(clampPos.y, _panLimit.first.y + (_fov.y  * getZoom() - _fov.y) / 2);
		clampPos.y = std::min(clampPos.y, _panLimit.second.y - (_fov.y * getZoom() - _fov.y) / 2);
		break;
	}

	return clampPos;
}
NS_CC_END
