#include <vector>
#include <string>
#include <cstdint>
#include <level.hpp>

#include "util.h"
#include "action.h"

#define BIDIRECTIONAL /* for when packet can be sent by either client or server */

struct SongInfo {
    bool custom;
    uint32_t songid;
    uint16_t offset;
};

typedef enum {
    HOLA,
    CRINGE,
    ADIOS,
    AYO_WTF,
    WELCOME,
    MEMBER_UPDATE,
    LVL_UPDATE,
    SELECTION,
    OBJ_ADD,
    OBJ_REMOVE,
    OBJ_ACTION
} PacketFormat;

typedef enum {
    WRONG_PASSWORD,
    TAKEN_USERNAME,
    BAD_SELECTION,
    PERMISSION_DENIED,
} CringeReason;

typedef struct HolaPacket {
    std::string username;
    std::string passcode;
} HolaPacket;

typedef struct WelcomePacket {
    struct SongInfo song;
    uint32_t member_count;
    bool granted_owner;
    std::string lvl_string;
} WelcomePacket;

typedef struct LvlUpdatePacket {
    struct SongInfo song;
    std::string lvl_string;
} LvlUpdatePacket;

BIDIRECTIONAL typedef struct SelectionPacket {
    selection_list blocks;
    std::string username; // for StC packets
    bool actually_select;
} SelectionPacket;

BIDIRECTIONAL typedef struct ObjAddPacket {
    block_t block; // for StC packets
    std::string block_str;
} ObjAddPacket;

BIDIRECTIONAL typedef struct ActionPacket {
    selection_list blocks;
    std::vector<ObjectAction*> actions;
} ActionPacket;

#pragma pack(push, 1)

struct _ErrPackform {
    PacketFormat id;
    char message[255];
};
struct _HolaPackform {
    PacketFormat id;
    char passcode[8];
    char username[15];
};
struct _MemberPackform {
    PacketFormat id;
    bool joined;
    char username[];
};
struct _CringePackform {
    PacketFormat id;
    CringeReason reason;
};
struct _SelectionPackform BIDIRECTIONAL {
    PacketFormat id;
    bool actually_select;
    char username[15];
    block_t blocks[];
};
struct _LvlUpdatePackform BIDIRECTIONAL {
    PacketFormat id;
    SongInfo info;
    char lvldata[];
};
struct _ObjAddPackform BIDIRECTIONAL {
    PacketFormat id;
    block_t block;
    char lvl_string[];
};

struct _ObjRemovePackform BIDIRECTIONAL {
    PacketFormat id;
    block_t block[];
};

struct _ObjActionPackform BIDIRECTIONAL {
    PacketFormat id;
    uint32_t block_count;
    char lol_gl_parsing_this[];
};

struct _WelcomePackform {
    PacketFormat id;
    uint32_t member_count;
    bool granted_owner;
    struct SongInfo info;
    char lvl_string[];
}; // design plan: if owner = true and the level is empty, client sends level update packet

#pragma pack(pop)

namespace CoPackets {
    std::string build_error(std::string reason) {
        size_t size = reason.size()+1;
        struct _ErrPackform buf;
        buf.id = AYO_WTF;
        strncpy(buf.message,reason.c_str(), 15);

        std::string output(reinterpret_cast<char*>(&buf), size);
        return output;
    }
    std::string build_cringe(CringeReason reason) {
        struct _CringePackform buf;
        buf.id = CRINGE;
        buf.reason = reason;
        std::string output(reinterpret_cast<char*>(&buf), 2);
        return output;
    }

    std::string build_member_update(std::string uname, bool joined) {
        auto buf = reinterpret_cast<struct _MemberPackform*>(malloc(sizeof(struct _MemberPackform) + uname.size()));
        buf->id = MEMBER_UPDATE;
        buf->joined = joined;
        strncpy(buf->username, uname.c_str(), uname.size());

        std::string output(reinterpret_cast<char*>(buf), sizeof(struct _MemberPackform) + uname.size());
        free(buf);
        return output;
    }

    std::string build_welcome(bool owner, GD::Level& level, uint16_t usercount, SongInfo song) {
        std::string lvlstr;
        if (level.blockCount())
            lvlstr = level;
        auto buf = reinterpret_cast<struct _WelcomePackform*>(malloc(sizeof(struct _WelcomePackform) + lvlstr.size()));
        buf->id = WELCOME;
        buf->granted_owner = owner;
        buf->info = song;
        buf->member_count = usercount;
        strncpy(buf->lvl_string, lvlstr.c_str(), lvlstr.size());

        std::string output(reinterpret_cast<char*>(buf), sizeof(struct _WelcomePackform) + lvlstr.size());
        free(buf);
        return output;
    }

    std::string build_lvlupdate(GD::Level& level, SongInfo song) {
        std::string lvlstr(level);
        auto buf = reinterpret_cast<struct _LvlUpdatePackform*>(malloc(0 + sizeof(struct _LvlUpdatePackform)));
        buf->id = LVL_UPDATE;
        buf->info = song;

        std::string output(reinterpret_cast<char*>(buf), sizeof(struct _LvlUpdatePackform));
        output += lvlstr;

        free(buf);
        return output;
    }
    std::string build_selection(selection_list objs, std::string username, bool actually_select) {
        auto buf = reinterpret_cast<struct _SelectionPackform*>(malloc(sizeof(struct _SelectionPackform) + sizeof(block_t)*objs.size()));
        buf->actually_select = actually_select;
        buf->id = SELECTION;
        strncpy(buf->username, username.c_str(), 15);
        memcpy(buf->blocks, objs.data(), sizeof(block_t)*objs.size());

        std::string output(reinterpret_cast<char*>(buf), sizeof(struct _SelectionPackform) + sizeof(block_t)*objs.size());
        free(buf);
        return output;
    }
    std::string build_addobj(block_t uuid, std::string obj_str) {
        std::string output(reinterpret_cast<char*>(&uuid), sizeof(block_t));
        output += obj_str;
        PacketFormat ok = OBJ_ADD;
        return std::string(reinterpret_cast<char*>(&ok), sizeof(PacketFormat)) + output;
    }

    std::string build_rmobj(selection_list blocks) {
        auto buf = reinterpret_cast<struct _ObjRemovePackform*>(malloc(sizeof(struct _ObjRemovePackform) + sizeof(block_t)*blocks.size()));
        buf->id = OBJ_REMOVE;
        memcpy(buf->block, &blocks[0], sizeof(block_t)*blocks.size());

        std::string output(reinterpret_cast<char*>(buf), sizeof(struct _ObjRemovePackform) + sizeof(block_t)*blocks.size());
        free(buf);
        return output;
    }

    bool parse_action(std::string packet, ActionPacket& ret) {
        // how the fUCK am i gonna do this fr
        auto buf = reinterpret_cast<struct _ObjActionPackform*>(malloc(packet.size()));

        char* ptr = &buf->lol_gl_parsing_this[0];
        for (uint32_t i = 0;i < buf->block_count; i+=4) {
            ret.blocks.push_back(*reinterpret_cast<block_t*>(ptr));
            ptr+=4;
        }

        for(;;) {
            uint8_t action_size = *ptr;
            std::string action_data(ptr+1, action_size-1);
            // deserialization doesn't care about the length byte
            ret.actions.push_back(ObjectAction::deserialize(action_data));
            ptr+=action_size;
        }

        return true;
    }
    bool parse_rmobj(std::string packet, selection_list& ret) {
    	if (packet.size()-1 % 4)
    		return false;
    	ret = selection_list(packet.data()+1, packet.data()+packet.size());
    	return true;
    }

    bool parse_addobj(std::string packet, ObjAddPacket& ret) {
        auto buf = reinterpret_cast<struct _ObjAddPackform*>(malloc(packet.size()));
        memcpy(buf, packet.c_str(), packet.size());
        ret = {
            .block = buf->block,
            .block_str = std::string(buf->lvl_string)
        };

        free(buf);
        return true;
    }
    bool parse_selection(std::string packet, SelectionPacket& ret) {
        auto buf = reinterpret_cast<struct _SelectionPackform*>(malloc(packet.size()));
        memcpy(buf, packet.c_str(), packet.size());

        ret = {
            .actually_select = buf->actually_select, 
            .username = std::string(buf->username), 
            .blocks = selection_list(buf->blocks, buf->blocks + packet.size() - sizeof(struct _SelectionPackform))
        };

        free(buf);
        return true;
    }
    bool parse_hola(std::string packet, HolaPacket& ret) {
        if (packet.size() > 268) {
            return false;
        }
        struct _HolaPackform buf;
        memcpy(&buf, packet.c_str(), sizeof(buf));

        ret = {.username = std::string(buf.username), .passcode = std::string(buf.passcode)};
        return true;
    }

    bool parse_lvlupdate(std::string packet, LvlUpdatePacket& ret) {
        struct _LvlUpdatePackform* buf = reinterpret_cast<struct _LvlUpdatePackform*>(malloc(packet.size()));
        memcpy(buf, packet.c_str(), packet.size());

        ret = {.song = buf->info, .lvl_string = std::string(buf->lvldata)};

        free(buf);
        return true;
    }

    inline PacketFormat packet_type(std::string packet) {return static_cast<PacketFormat>(*packet.c_str());}
}
