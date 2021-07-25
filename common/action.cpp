#include "action.h"

#define ACASE(cls, swch) \
		case swch: \
			ret = new Action::cls(); \
			ret->pack_size = sizeof(Action::cls)-12; \
			memcpy(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(ret)+12), data.data(), ret->pack_size); \
			break;

ObjectAction* ObjectAction::deserialize(std::string data) {
	auto packet_type = static_cast<enum ActionType>(data[0]);
	ObjectAction* ret;
	switch (packet_type) {
		ACASE(Move, MOVE)
		ACASE(Rotate, ROTATE)
		ACASE(Scale, SCALE)
		ACASE(FlipX, FLIP_X)
		ACASE(FlipY, FLIP_Y)
		ACASE(EditGroup, EDIT_GROUP)
		ACASE(Hsv, HSV)
		ACASE(EditorLayer, EDITOR_LAYER)
		ACASE(ColorID, COLOR_ID)
		ACASE(ZLayer, Z_LAYER)
		ACASE(ZOrder, Z_ORDER)
		ACASE(ObjProps, OBJ_PROPS)
		ACASE(ObjSpecial, OBJ_SPECIAL)
		ACASE(PortalCheck, PORTAL_CHECK)
		ACASE(StartPos, START_POS)
		ACASE(CollisionBlock, COLLISION_BLOCK)
		ACASE(Trigger, TRIGGER)
		ACASE(TriggerColor, TRIGGER_COLOR)
		ACASE(TriggerRotate, TRIGGER_ROTATE)
		ACASE(TriggerMove, TRIGGER_MOVE)
		ACASE(TriggerStop, TRIGGER_STOP)
		ACASE(TriggerPulse, TRIGGER_PULSE)
		ACASE(TriggerFollowY, TRIGGER_FOLLOW_Y)
		ACASE(TriggerAlpha, TRIGGER_ALPHA)
		ACASE(TriggerToggle, TRIGGER_TOGGLE)
		ACASE(TriggerSpawn, TRIGGER_SPAWN)
		ACASE(TriggerFollow, TRIGGER_FOLLOW)
		ACASE(TriggerShake, TRIGGER_SHAKE)
		ACASE(TriggerAnimate, TRIGGER_ANIMATE)
		ACASE(TriggerTouch, TRIGGER_TOUCH)
		ACASE(TriggerCount, TRIGGER_COUNT)
		ACASE(TriggerInstantCount, TRIGGER_ICOUNT)
		ACASE(TriggerPickup, TRIGGER_PICKUP)
		ACASE(TriggerCollision, TRIGGER_COLLISION)
		ACASE(TriggerOnDeath, TRIGGER_ONDEATH)
	}
	return ret;
}
