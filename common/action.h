#pragma once

#include <level.hpp>
#include "util.h"

class GameObject;

#ifdef _CoClient_
#define ACTION(cls, enm, ...) \
    class cls : public ObjectAction { \
     public: \
         cls() : ObjectAction(enm) {} \
         void apply_client(GameObject* b) override; \
        __VA_ARGS__ \
};
#else
#define ACTION(cls, enm, ...) \
    class cls : public ObjectAction { \
     public: \
         cls() : ObjectAction(enm) {} \
         void apply_server(GD::Level& lvl, GD::Block& b) override; \
        __VA_ARGS__ \
};
#endif

enum ActionType {
    MOVE,
    ROTATE,
    SCALE,
    FLIP_X,
    FLIP_Y,
    EDIT_GROUP,
    HSV,
    EDITOR_LAYER,
    COLOR_ID,
    Z_LAYER,
    Z_ORDER,
    OBJ_PROPS,
    OBJ_SPECIAL,
    PORTAL_CHECK,
    START_POS,
    COLLISION_BLOCK,

    // triggers
    TRIGGER,

    TRIGGER_COLOR,
    TRIGGER_PULSE,
    TRIGGER_MOVE,
    TRIGGER_ROTATE,
    TRIGGER_FOLLOW_Y,
    TRIGGER_FOLLOW,
    TRIGGER_STOP,
    TRIGGER_ONDEATH,
    TRIGGER_COLLISION,
    TRIGGER_ALPHA,
    TRIGGER_TOGGLE,
    TRIGGER_SPAWN,
    TRIGGER_SHAKE,
    TRIGGER_ANIMATE,
    TRIGGER_TOUCH,
    TRIGGER_COUNT,
    TRIGGER_ICOUNT,
    TRIGGER_PICKUP,
    //314
};

typedef enum {
    B4 = -3,
    B3 = -1,
    B2 = 1,
    B1 = 3,
    T1 = 5,
    T2 = 7,
    T3 = 9
} ZLayerType;

typedef enum {
    SLOW,
    NORMAL,
    X2,
    X3,
    X4
} Speed;

typedef enum {
    CUBE,
    SHIP,
    BALL,
    UFO,
    WAVE,
    ROBOT,
    SPIDER
} Gamemode;

typedef enum {
    NONE,
    EASE_IN_OUT,
    EASE_IN,
    EASE_OUT,
    ELASTIC_IN_OUT,
    ELASTIC_IN,
    ELASTIC_OUT,
    BOUNCE_IN_OUT,
    BOUNCE_IN,
    BOUNCE_OUT,
    EXPONENTIAL_IN_OUT,
    EXPONENTIAL_IN,
    EXPONENTIAL_OUT,
    SINE_IN_OUT,
    SINE_IN,
    SINE_OUT,
    BACK_IN_OUT,
    BACK_IN,
    BACK_OUT
} Easing;

typedef enum {
    EQUALS,
    LARGER,
    SMALLER
} Comparison;


#pragma pack(push, 4)

class ObjectAction {
 
 public:
    #ifdef _CoClient_
    virtual void apply_client(GameObject* b) = 0;
    #else 
    virtual void apply_server(GD::Level& lvl, GD::Block& b) = 0;
    #endif

    char pad[4];

    #if BITS_32
        char pad2[4];
    #endif

    uint8_t pack_size;
    enum ActionType type;
    ObjectAction(enum ActionType t) : type(t) {}
    virtual ~ObjectAction() {}

    // inside action.cpp
    static ObjectAction* deserialize(std::string data);
    std::string serialize();
    //314
};

class ActionContainer {
 public:
    std::vector<block_t> uuid;
    ObjectAction* action;
    ActionContainer(std::vector<block_t> uid, ObjectAction* act) : uuid(uid), action(act) {}
    ~ActionContainer() {
        delete action;
    }
};

// ACTIONS HERE
namespace Action {
ACTION(Move, MOVE,
    float x;
    float y;
);
ACTION(Rotate, ROTATE, 
    int16_t degrees;
);
ACTION(Scale, SCALE,
    float amount;
);
ACTION(FlipX, FLIP_X, 
    bool flipped;
);
ACTION(FlipY, FLIP_Y,
    bool flipped;
);
ACTION(EditGroup, EDIT_GROUP,
    bool add;
    uint16_t group;
);
ACTION(Hsv, HSV,
    bool sat_absolute;
    bool bright_absolute;
    bool glow;
    bool detail;
    int16_t hue;
    float sat;
    float bright;
);
ACTION(EditorLayer, EDITOR_LAYER,
    bool two;
    uint16_t layer;
);
ACTION(ColorID, COLOR_ID,
    bool detail;
    int16_t id;
);
ACTION(ZLayer, Z_LAYER,
    ZLayerType type;
);
ACTION(ZOrder, Z_ORDER,
    int16_t order;
);
ACTION(ObjProps, OBJ_PROPS,
    bool dont_fade;
    bool dont_enter;
    bool parent;
    bool high_detail;
);
ACTION(ObjSpecial, OBJ_SPECIAL,
    bool multi_activate;
    bool target_group;
    bool activate_group;
    bool is_pickup;
    bool subtract;
    bool is_toggle;
    bool randomize_start;
    bool rotate_disable;
    int16_t rotate_degrees;
    uint16_t counter_id;
    float pulse_speed;
);
ACTION(PortalCheck, PORTAL_CHECK,
    bool checked;
);
ACTION(StartPos, START_POS,
    Speed speed;
    Gamemode gamemode;
    bool mini;
    bool dual;
    bool upsidedown;
);
ACTION(CollisionBlock, COLLISION_BLOCK,
    bool dynamic;
    uint16_t block_id;
);
ACTION(Trigger, TRIGGER,
    bool touch;
    bool spawn;
    bool multi_activate;
);
ACTION(TriggerColor, TRIGGER_COLOR,
    bool pcol_1;
    bool pcol_2;
    bool blend;
    bool sat_absolute;
    bool bright_absolute;
    bool copy_opacity;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint16_t colorid;
    uint16_t copy_colorid;
    int16_t hue;
    float fade;
    float opacity;
    float sat;
    float bright;
);
ACTION(TriggerRotate, TRIGGER_ROTATE,
    bool lock_rotation;
    Easing ease;
    uint16_t target_group;
    uint16_t center_group;
    uint16_t degrees;
    uint32_t times_360;
    float move_time;
);
ACTION(TriggerMove, TRIGGER_MOVE,
    Easing ease;
    bool x_only;
    bool y_only;
    bool lock_x;
    bool lock_y;
    bool use_target;
    uint16_t target_group;
    uint16_t followed_group;
    float move_time;
    float move_x;
    float move_y;
);
ACTION(TriggerStop, TRIGGER_STOP,
    uint16_t target_group;
);
ACTION(TriggerPulse, TRIGGER_PULSE,
    bool exclusive;
    bool use_hsv;
    bool target_is_group;
    bool main_only;
    bool detail_only;
    bool sat_absolute;
    bool bright_absolute;
    uint8_t red;
    uint8_t green;
    uint8_t blue;
    uint16_t something_id;
    uint16_t hsv_color_id;
    int16_t hue;
    float fade_in;
    float fade_out;
    float hold;
    float opacity;
    float sat;
    float bright;
);
ACTION(TriggerFollowY, TRIGGER_FOLLOW_Y,
    uint16_t offset;
    uint16_t target_group;
    float speed;
    float delay;
    float max_speed;
    float move_time;
);
ACTION(TriggerAlpha, TRIGGER_ALPHA,
    uint16_t target_group;
    float fadetime;
    float opacity;
);
ACTION(TriggerToggle, TRIGGER_TOGGLE,
    bool activate_group;
    uint16_t target_group;
);
ACTION(TriggerSpawn, TRIGGER_SPAWN,
    bool editor_disable;
    uint16_t target_group;
    float delay;
);
ACTION(TriggerFollow, TRIGGER_FOLLOW,
    uint16_t target_group;
    uint16_t follow_group;
    float move_time;
    float x_mod;
    float y_mod;
);
ACTION(TriggerShake, TRIGGER_SHAKE,
    float strength;
    float interval;
    float duration;
);
ACTION(TriggerAnimate, TRIGGER_ANIMATE,
    uint16_t target_group;
    uint16_t animate_id;
);
ACTION(TriggerTouch, TRIGGER_TOUCH,
    uint8_t toggle_mode;
    bool hold_mode;
    bool dual_mode;
    uint16_t target_group;
);
ACTION(TriggerCount, TRIGGER_COUNT,
    bool activate_group;
    bool multi_activate;
    uint16_t item_id;
    uint16_t target_id;
    int32_t target_count;
);
ACTION(TriggerInstantCount, TRIGGER_ICOUNT,
    bool activate_group;
    Comparison cmp;
    uint16_t item_id;
    uint16_t target_id;
    int32_t target_count;
);
ACTION(TriggerPickup, TRIGGER_PICKUP,
    uint16_t item_id;
    int32_t count;
);
ACTION(TriggerCollision, TRIGGER_COLLISION,
    bool activate_group;
    bool on_exit;
    uint16_t block_a;
    uint16_t block_b;
    uint16_t target_group;
);
ACTION(TriggerOnDeath, TRIGGER_ONDEATH,
    bool activate_group;
    uint16_t target_group;
);
}
#pragma pack(pop)