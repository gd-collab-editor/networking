#include <websocketpp/config/asio_no_tls.hpp>
#include <websocketpp/server.hpp>
#include <ctime>
#include <unistd.h>
#include <level.hpp>
#include <vector>
#include <set>
#include <algorithm>
#include <functional>

#include <packets.h>
#include <action.h>

using namespace websocketpp::frame;
using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using ws_errcode = websocketpp::lib::error_code;
namespace close_status = websocketpp::close::status;

typedef websocketpp::server<websocketpp::config::asio> WSServer;


struct UserData {
    std::string username;
    selection_list selection;
};

class CoServer;
inline CoServer* g_sharedServer;

class CoServer {
 public:
    CoServer() : m_server() {
        m_server.set_message_handler(bind(&CoServer::_onMessage,this,::_1,::_2));
        m_server.set_access_channels(websocketpp::log::alevel::all);
        m_server.set_error_channels(websocketpp::log::elevel::all);

        m_owner = nullptr;
        gen_passcode(m_passcode);
        gen_passcode(m_admincode);
    }
    static CoServer* shared() {
        if (!g_sharedServer)
            g_sharedServer = new CoServer;
        return g_sharedServer;
    }

    // util 

    void gen_passcode(std::string& passcode) {
        passcode.clear();

        static const char alphanum[] =
            "0123456789"
            "abcdefghijklmnopqrstuvwxyz";
        
        srand( (unsigned) time(NULL) * getpid());
        passcode.reserve(8);

        for (int i = 0; i < 8; ++i) 
            passcode += alphanum[rand() % (sizeof(alphanum) - 1)];
    }

    void update_members(std::string username, bool added) {
        ws_errcode err;
        for (auto i : m_users) {
            m_server.send(i.first, CoPackets::build_member_update(username, added), opcode::BINARY, err);
        }
    }

    bool check_owner(websocketpp::connection_hdl h) {
        for (auto& i : m_users) {
            if (m_server.get_con_from_hdl(i.first) == m_server.get_con_from_hdl(h) && (m_owner ? (i.second == *m_owner) : false)) {
                return true;
            }
        }
        return false;
    }

    bool username_taken(std::string uname) {
        for (auto& i : m_users) {
            if (i.second == uname)
                return true;
        }
        return false;
    }

    bool bad_selection(selection_list selection, std::string username) {
        selection_list other_selections;
        for (auto& i : m_userdata) {
            if (i.first != username)
                other_selections.insert(other_selections.end(), i.second.selection.begin(), i.second.selection.end());
        }

        selection_list intersections(selection.size());
        selection_list::iterator it;

        std::sort(selection.begin(), selection.end());
        std::sort(other_selections.begin(), other_selections.end());

        it = std::set_intersection(selection.begin(), selection.end(), other_selections.begin(), other_selections.end(), intersections.begin());
        intersections.resize(it - intersections.begin());

        return intersections.size()>0; 
    }

    selection_list selection_differences(selection_list selection, std::string username) {
        selection_list old_selection = m_userdata[username].selection;

        selection_list differences(selection.size());
        selection_list::iterator it;

        std::sort(selection.begin(), selection.end());
        std::sort(old_selection.begin(), old_selection.end());

        it = std::set_difference(selection.begin(), selection.end(), old_selection.begin(), old_selection.end(), differences.begin());
        differences.resize(it - differences.begin());

        return differences;
    }

    selection_list selection_intersects(selection_list selection, std::string username) {
        selection_list old_selection = m_userdata[username].selection;

        selection_list intersects(selection.size());
        selection_list::iterator it;

        std::sort(selection.begin(), selection.end());
        std::sort(old_selection.begin(), old_selection.end());

        it = std::set_intersection(selection.begin(), selection.end(), old_selection.begin(), old_selection.end(), intersects.begin());
        intersects.resize(it - intersects.begin());

        return intersects;
    }

    inline bool has_ownership(std::string user, block_t block) {
        return m_ownership[block]==user;
    }
    bool has_ownership(std::string user, selection_list blocks) {
        for (auto i : blocks) {
            if (!has_ownership(user, i))
                return false;
        }
        return true;
    }
    //314

    void _onMessage(websocketpp::connection_hdl h, WSServer::message_ptr msg) {
        std::string yea = msg->get_payload();
        ws_errcode err;

        if (yea.size()<1){
            m_server.send(h, CoPackets::build_error("No packet data found"), opcode::BINARY, err);
        }
        switch (CoPackets::packet_type(yea)) {
            case HOLA:
                hola_case(h, yea);
                break;
            case LVL_UPDATE:
                lvlupdate_case(h, yea);
            case ADIOS:
                m_users.erase(h);
                update_members(m_users[h], false);
                m_server.close(h, close_status::normal, "adios");
            case SELECTION:
                selection_case(h, yea);
            case OBJ_ADD:
                add_case(h, yea);
            case OBJ_REMOVE:
                rm_case(h, yea);
            default:
                break;
        }
    }
    void _onClose(websocketpp::connection_hdl h) {
        m_users.erase(h);
        update_members(m_users[h], false);
    }

    void rm_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        selection_list blocks;
        if (!CoPackets::parse_rmobj(yea, blocks)) {
            m_server.send(h, CoPackets::build_error("invalid data (rm object) this should NOT happen"), opcode::BINARY, err);
        } else if(!has_ownership(m_users[h], blocks)) {
            m_server.send(h, CoPackets::build_cringe(PERMISSION_DENIED), opcode::BINARY, err);
        } else {
            auto block_objs = m_level.findByUUIDs(blocks);
            for (GD::Block* i : block_objs) {
                vec_remove_val(m_level.blocks, i);
                delete i;
            }
            for (auto& i : m_users) {
                m_server.send(i.first, CoPackets::build_rmobj(blocks), opcode::BINARY, err);
            }
        }
    }

    void action_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        ActionPacket act;
        if (!CoPackets::parse_action(yea, act)) {
            m_server.send(h, CoPackets::build_error("what the fuck?? (action"), opcode::BINARY, err);
        } else if(!has_ownership(m_users[h], act.blocks)){
            m_server.send(h, CoPackets::build_cringe(PERMISSION_DENIED), opcode::BINARY, err);
        } else {
            for (auto i : act.actions) {
                for (auto b : m_level.findByUUIDs(act.blocks)) {
                    i->apply_server(m_level, *b);
                }
                for (auto& j : m_users) {
                    m_server.send(j.first, yea, opcode::BINARY, err);
                }
                m_actionlog.push_back(ActionContainer(act.blocks, i));
            }
        }
    }

    void add_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        ObjAddPacket add;
        if (!CoPackets::parse_addobj(yea, add)) {
            m_server.send(h, CoPackets::build_error("what the fuck?? (add object)"), opcode::BINARY, err);
        } else {
            GD::Block* to_add = new GD::Block();
            (*to_add) = add.block_str;
            m_level.addBlock(to_add);
            m_ownership[to_add->get_uuid()] = m_users[h];

            for (auto& i : m_users) {
                m_server.send(i.first, CoPackets::build_addobj(to_add->get_uuid(), add.block_str), opcode::BINARY, err);
            }
        }
    }
    void selection_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        SelectionPacket selection;
        if (!CoPackets::parse_selection(yea, selection)) {
            m_server.send(h, CoPackets::build_error("what the fuck?? (selection)"), opcode::BINARY, err);
        } else if(bad_selection(selection.blocks, m_users[h])){
            m_server.send(h, CoPackets::build_cringe(BAD_SELECTION), opcode::BINARY, err);
        } else if(selection.actually_select) {
            selection_list& before_selection = m_userdata[m_users[h]].selection;

            auto diff = selection_differences(selection.blocks, m_users[h]);
            if (diff.size()) {
                for (auto i : diff) {
                    before_selection.push_back(i);
                    m_ownership[i] = m_users[h];
                }
                for (auto& i : m_users) {
                    m_server.send(i.first, CoPackets::build_selection(diff, m_users[h], true), opcode::BINARY, err);
                }
            }

        } else {
            selection_list& before_selection = m_userdata[m_users[h]].selection;
            auto contains = selection_intersects(selection.blocks, m_users[h]);
            if (contains.size()) {
                for (auto i : contains) {
                    vec_remove_val(before_selection, i);
                }
            }

            for (auto& i : m_users) {
                m_server.send(i.first, CoPackets::build_selection(contains, m_users[h], false), opcode::BINARY, err);
            }
        }
    }
    void lvlupdate_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        LvlUpdatePacket level_info;

        if (!CoPackets::parse_lvlupdate(yea, level_info)) {
            m_server.send(h, CoPackets::build_error("what the fuck?? (lvlupdate)"), opcode::BINARY, err);
        } else if (!check_owner(h)){
            m_server.send(h, CoPackets::build_cringe(PERMISSION_DENIED), opcode::BINARY, err);
        } else {
            m_level = level_info.lvl_string;
            m_song = level_info.song;

            for (auto& i : m_users) {
                m_server.send(i.first, CoPackets::build_lvlupdate(m_level, m_song), opcode::BINARY, err);
            }
        }
    }
    void hola_case(websocketpp::connection_hdl h, std::string yea) {
        ws_errcode err;
        HolaPacket entry_ticket;

        if (!CoPackets::parse_hola(yea, entry_ticket)) {
            m_server.send(h, CoPackets::build_error("Packet too large"), opcode::BINARY, err);
        } else if (entry_ticket.passcode != m_passcode && entry_ticket.passcode != m_admincode) {
            m_server.send(h, CoPackets::build_cringe(WRONG_PASSWORD), opcode::BINARY, err);
        } else if (username_taken(entry_ticket.username)) {
            m_server.send(h, CoPackets::build_cringe(TAKEN_USERNAME), opcode::BINARY, err);
        } else {
            m_users[h] = entry_ticket.username;
            struct UserData udat;
            m_userdata[entry_ticket.username] = udat;
            if (entry_ticket.passcode == m_admincode && !m_owner)
                m_owner = new std::string(entry_ticket.username);
            update_members(entry_ticket.username, true);

            bool owner = m_owner ? (entry_ticket.username == *m_owner) : false;
            m_server.send(h, CoPackets::build_welcome(owner, m_level, m_users.size(), m_song), opcode::BINARY, err);
        }
    }
 protected:
    WSServer m_server;
    std::map<websocketpp::connection_hdl, std::string, std::owner_less<websocketpp::connection_hdl>> m_users;
    std::map<std::string, struct UserData> m_userdata;
    std::map<block_t, std::string> m_ownership;
    std::vector<ActionContainer> m_actionlog;
    std::string* m_owner;
    std::string m_passcode;
    std::string m_admincode;
    GD::Level m_level;
    struct SongInfo m_song;
};
//just for test
int main() {
}