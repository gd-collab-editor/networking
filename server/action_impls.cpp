#include <action.h>
#include <level.hpp>
using namespace GD::BlockAttrs;

#define IMPL(name) void name::apply_server(GD::Level& l, GD::Block& b)
namespace Action {
	IMPL(Move) {
		b[X_POSITION] = x;
		b[Y_POSITION] = y;
	}
	IMPL(Rotate) {
		b[ROTATION] = degrees;
	}
	IMPL(Scale) {
		b[SIZE] = std::to_string(amount);
	}
	IMPL(FlipX) {
		b[IS_FLIPPED_VERTICAL] = flipped;
	}
	IMPL(FlipY) {
		b[IS_FLIPPED_HORIZONTAL] = flipped;
	}
	IMPL(EditGroup) {
		if (b[GROUP_IDS].size()>0)
			b[GROUP_IDS] += "." + std::to_string(group);
		else
			b[GROUP_IDS] = group;
	}
	IMPL(Hsv) {
		b[GLOW_DISABLED] = !glow;
		if (!detail) {
			b[HSV_ENABLED_1] = "1";
			b[HSV_VALUES_1] = std::to_string(hue) + "a" +
								  std::to_string(sat) + "a" +
								  std::to_string(bright) + "a" +
								  std::to_string(sat_absolute) + "a" +
								  std::to_string(bright_absolute);
		} else {
			b[HSV_ENABLED_2] = "1";
			b[HSV_VALUES_2] = std::to_string(hue) + "a" +
								  std::to_string(sat) + "a" +
								  std::to_string(bright) + "a" +
								  std::to_string(sat_absolute) + "a" +
								  std::to_string(bright_absolute);
		}
	}
	IMPL(EditorLayer) {
		if (two)
			b[EDITOR_LAYER_2] = layer;
		else
			b[EDITOR_LAYER_2] = layer;
	}
	IMPL(ColorID) {
		if (!detail)
			b[COLOR1] = id;
		else
			b[COLOR2] = id;
	}
	IMPL(ZLayer) {
		b[GD::BlockAttrs::Z_LAYER] = type;
	}
	IMPL(ZOrder) {
		b[GD::BlockAttrs::Z_ORDER] = order;
	}
	IMPL(ObjProps) {
		b[DONT_FADE] = dont_fade;
		b[DONT_ENTER] = dont_enter;
		b[GROUP_PARENT] = parent;
		b[HIGH_DETAIL] = high_detail;
	}
	IMPL(ObjSpecial) {
		b[COUNT_MULTI_ACTIVATE] = multi_activate;
		b[TARGET_ID] = target_group;
		b[ACTIVATE_GROUP] = activate_group;
		b[PICKUP_MODE] = is_pickup ? 1 : 2;
		b[SUBSTRACT_COUNT] = subtract;
		b[ITEM_ID] = counter_id;
		b[PULSING_ANIMATION_RANDOMIZE] = randomize_start;
		b[DISABLE_ROTATION] = rotate_disable;
		b[CUSTOM_ROTATION_SPEED] = rotate_degrees;
		b[PULSING_ANIMATION_SPEED] = pulse_speed;
	}
	IMPL(PortalCheck) {
		b[PORTAL_CHECKED] = checked;
	}
	IMPL(StartPos) {
		//TODO: add this
	}
	IMPL(CollisionBlock) {
		b[DYNAMIC_BLOCK] = dynamic;
		b[BLOCK_ID] = block_id;
	}
	IMPL(Trigger) {
		b[TOUCH_TRIGGERED] = touch;
		b[SPAWN_TRIGGERED] = spawn;
		b[MULTI_TRIGGER] = multi_activate;
	}
	IMPL(TriggerColor) {
		b[RED] = red;
		b[GREEN] = green;
		b[BLUE] = blue;
		b[PLAYER_COLOR_1] = pcol_1;
		b[PLAYER_COLOR_2] = pcol_2;
		b[BLENDING] = blend;
		b[COPIED_COLOR_ID] = copy_colorid;
		b[OPACITY] = copy_opacity;
		b[COPIED_COLOR_HSV_VALUES] = std::to_string(hue) + "a" +
								     std::to_string(sat) + "a" +
						    		 std::to_string(bright) + "a" +
								     std::to_string(sat_absolute) + "a" +
								     std::to_string(bright_absolute);
		b[DURATION] = fade;
		b[OPACITY] = opacity;
	}
	IMPL(TriggerRotate) {
		b[EASING] = ease;
		b[ROTATION_LOCK] = lock_rotation;
		b[ROTATION_DEGREES] = degrees;
		b[ROTATION_TIMES_360] = times_360;
		b[TARGET_ID] = target_group;
		b[DURATION] = move_time;
		b[FOLLOW_GROUP_ID] = center_group;
	}
	IMPL(TriggerMove) {
		b[EASING] = ease;
		b[DURATION] = move_time;
		b[TARGET_ID] = target_group;
		b[FOLLOW_GROUP_ID] = followed_group;
		b[MOVE_TARGET_ENABLED] = use_target;
		b[MOVE_TRIGGER_X_UNITS] = move_x;
		b[MOVE_TRIGGER_9_UNITS] = move_y;
		b[MOVE_LOCK_TO_PLAYER_X] = lock_x;
		b[MOVE_LOCK_TO_PLAYER_Y] = lock_y;
	}
	IMPL(TriggerStop) {
		b[TARGET_ID] = target_group;
	}
	IMPL(TriggerPulse) {
		b[PULSE_EXCLUSIVE] = exclusive;
		b[PULSE_FADE_IN] = fade_in;
		b[PULSE_FADE_OUT] = fade_out;
		b[PULSE_HOLD] = hold;
		b[PULSE_DETAIL_ONLY] = detail_only;
		b[PULSE_MAIN_ONLY] = main_only;
		b[PULSE_MODE] = use_hsv;
		b[RED] = red;
		b[GREEN] = green;
		b[BLUE] = blue;
		b[COPIED_COLOR_ID] = hsv_color_id;
		b[PULSE_TARGET_TYPE] = target_is_group;
		b[TARGET_ID] = something_id;
		b[COPIED_COLOR_HSV_VALUES] = std::to_string(hue) + "a" +
								     std::to_string(sat) + "a" +
						    		 std::to_string(bright) + "a" +
								     std::to_string(sat_absolute) + "a" +
								     std::to_string(bright_absolute);
	}
	IMPL(TriggerFollowY) {
		b[FOLLOW_PLAYER_Y_SPEED] = speed;
		b[FOLLOW_PLAYER_Y_DELAY] = delay;
		b[FOLLOW_PLAYER_Y_MAX_SPEED] = max_speed;
		b[FOLLOW_PLAYER_Y_OFFSET] = offset;
		b[TARGET_ID] = target_group;
		b[DURATION] = move_time;
	}
	IMPL(TriggerAlpha) {
		b[DURATION] = fadetime;
		b[TARGET_ID] = target_group;
		b[OPACITY] = opacity;
	}
	IMPL(TriggerToggle) {
		b[ACTIVATE_GROUP] = activate_group;
		b[TARGET_ID] = target_group;
	}
	IMPL(TriggerSpawn) {
		b[SPAWN_EDITOR_DISABLE] = editor_disable;
		b[TARGET_ID] = target_group;
		b[SPAWN_DELAY] = delay;
	}
	IMPL(TriggerFollow) {
		b[FOLLOW_X_MOD] = x_mod;
		b[FOLLOW_Y_MOD] = y_mod;
		b[DURATION] = move_time;
		b[FOLLOW_GROUP_ID] = follow_group;
		b[TARGET_ID] = target_group;
	}
	IMPL(TriggerShake) {
		b[SHAKE_STRENGTH] = strength;
		b[SHAKE_INTERVAL] = interval;
		b[DURATION] = duration;
	}
	IMPL(TriggerAnimate) {
		b[TARGET_ID] = target_group;
		b[ANIMATION_ID] = animate_id;
	}
	IMPL(TriggerTouch) {
		b[TOUCH_DUAL_MODE] = dual_mode;
		b[TARGET_ID] = target_group;
		b[HOLD_MODE] = hold_mode;
		b[TOUCH_TOGGLE] = toggle_mode;
	}
	IMPL(TriggerCount) {
		b[ACTIVATE_GROUP] = activate_group;
		b[COUNT_MULTI_ACTIVATE] = multi_activate;
		b[ITEM_ID] = item_id;
		b[TARGET_ID] = target_id;
		b[COUNT] = target_count;
	}
	IMPL(TriggerInstantCount) {
		b[ACTIVATE_GROUP] = activate_group;
		b[ITEM_ID] = item_id;
		b[TARGET_ID] = target_id;
		b[COUNT] = target_count;
		b[INSTANT_COUNT_COMPARISON] = cmp;
	}
	IMPL(TriggerPickup) {
		b[ITEM_ID] = item_id;
		b[COUNT] = count;
	}
	IMPL(TriggerCollision) {
		b[ACTIVATE_GROUP] = activate_group;
		b[ON_EXIT] = on_exit;
		b[ITEM_ID] = block_a;
		b[COLLISION_BLOCK_B_ID] = block_b;
		b[TARGET_ID] = target_group;
	}
	IMPL(TriggerOnDeath) {
		b[ACTIVATE_GROUP] = activate_group;
		b[TARGET_ID] = target_group;
	}
}
