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

	_eventDispatcher->addEventListenerWithSceneGraphPriority(_zoomListener, this);
	_eventDispatcher->addEventListenerWithSceneGraphPriority(_touchListener, this);

	return true;
}

void Camera::getProjectionMatrix(cocos2d::Mat4& projMatrix)
{
	switch (_projectionType)
	{
	case cocos2d::Director::Projection::_2D:
		//we center the camera on its position
		//-> the center of the view will be the position of the camera
		int left = _position.x - (_fov.x * _scaleX)/2 ;
		int right = _position.x + (_fov.x * _scaleX)/2 ;

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
	
	return true;
}

void Camera::onTouchMoved(Touch *touch, Event *unused_event)
{
	if (touch->getPreviousLocation().distanceSquared(touch->getLocation()) > 20.f)
	{
		_eventDispatcher->cancelEvent(_touchListener, touch);
	}
	Vec2 newPos = getPosition() - touch->getDelta();
	
	switch (_panMode)
	{
	case PAN_CENTER:
		newPos.x = std::max(newPos.x, _panLimit.first.x);
		newPos.x = std::min(newPos.x, _panLimit.second.x);
		newPos.y = std::max(newPos.y, _panLimit.first.y);
		newPos.y = std::min(newPos.y, _panLimit.second.y);
		break;
	case PAN_BORDER:
		//newPos.x = std::max(newPos.x, _panLimit.first.x *(2.08f - getZoom()));
		//newPos.x = std::min(newPos.x, _panLimit.second.x *(1 - getZoom() + 1));
		//newPos.y = std::max(newPos.y, _panLimit.first.y *(2.3f - getZoom()));
		//newPos.y = std::min(newPos.y, _panLimit.second.y *(1 - getZoom() + 1));
		newPos.x = std::max(newPos.x, _panLimit.first.x + (_fov.x * getZoom() - _fov.x) / 2);
		newPos.x = std::min(newPos.x, _panLimit.second.x - (_fov.x * getZoom() - _fov.x) / 2);
		newPos.y = std::max(newPos.y, _panLimit.first.y + (_fov.y  * getZoom() - _fov.y) / 2);
		newPos.y = std::min(newPos.y, _panLimit.second.y - (_fov.y * getZoom() - _fov.y) / 2);


		printf("zoom is %f and %f and pan is %f", getZoom(), Director::getInstance()->getWinSize().height, _panLimit.first.y);
		break;
	}
	
	setPosition(newPos);
}

void Camera::onTouchEnded(Touch *touch, Event *unused_event)
{
}

void Camera::onTouchCancelled(Touch *touch, Event *unused_event)
{
}

void Camera::onTouchesBegan(const std::vector<Touch*>& touches, Event *unused_event)
{

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
