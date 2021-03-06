/*
 * Copyright (C) 2008-2014 TrinityCore <http://www.trinitycore.org/>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "ScriptMgr.h"
#include "Channel.h"
#include "Guild.h"
#include "Group.h"

class ChatLogScript : public PlayerScript
{
    public:
        ChatLogScript() : PlayerScript("ChatLogScript") { }

        void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg) OVERRIDE
        {
            switch (type)
            {
                case CHAT_MSG_SAY:
                    TC_LOG_DEBUG("chat.log.say", "[SAY] Player %s says (language %u): %s",
                        player->GetName().c_str(), lang, msg.c_str());
                    break;

                case CHAT_MSG_EMOTE:
                    TC_LOG_DEBUG("chat.log.emote", "[EMOTE] Player %s emotes: %s",
                        player->GetName().c_str(), msg.c_str());
                    break;

                case CHAT_MSG_YELL:
                    TC_LOG_DEBUG("chat.log.yell", "[SAY] Player %s yells (language %u): %s",
                        player->GetName().c_str(), lang, msg.c_str());
                    break;
            }
        }

        void OnChat(Player* player, uint32 /*type*/, uint32 lang, std::string& msg, Player* receiver) OVERRIDE
        {
            if (lang != LANG_ADDON)
                TC_LOG_DEBUG("chat.log.whisper", "[WHISPER] Player %s tells %s: %s",
                    player->GetName().c_str(), receiver ? receiver->GetName().c_str() : "<unknown>", msg.c_str());
            else
                TC_LOG_DEBUG("chat.log.addon.whisper", "[WHISPER] [ADDON] Player %s tells %s: %s",
                    player->GetName().c_str(), receiver ? receiver->GetName().c_str() : "<unknown>", msg.c_str());
        }

        void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Group* group) OVERRIDE
        {
            //! NOTE:
            //! LANG_ADDON can only be sent by client in "PARTY", "RAID", "GUILD", "BATTLEGROUND", "WHISPER"
            switch (type)
            {
                case CHAT_MSG_PARTY:
                    if (lang != LANG_ADDON)
                        TC_LOG_DEBUG("chat.log.party", "[GROUP] Player %s tells group with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    else
                        TC_LOG_DEBUG("chat.log.addon.party", "[GROUP] [ADDON] Player %s tells group with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    break;

                case CHAT_MSG_PARTY_LEADER:
                    TC_LOG_DEBUG("chat.log.party", "[GROUP] [LEADER]Leader %s tells group: %s",
                        player->GetName().c_str(), msg.c_str());
                    break;

                case CHAT_MSG_RAID:
                    if (lang != LANG_ADDON)
                        TC_LOG_DEBUG("chat.log.raid", "[RAID] Player %s tells raid with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    else
                        TC_LOG_DEBUG("chat.log.addon.raid", "[RAID] [ADDON] Player %s tells raid with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    break;

                case CHAT_MSG_RAID_LEADER:
                    TC_LOG_DEBUG("chat.log.raid", "[RAID] [LEADER] Leader player %s tells raid: %s",
                        player->GetName().c_str(), msg.c_str());
                    break;

                case CHAT_MSG_RAID_WARNING:
                    TC_LOG_DEBUG("chat.log.raid", "[RAID] Leader player %s warns raid with: %s",
                        player->GetName().c_str(), msg.c_str());
                    break;

                case CHAT_MSG_BATTLEGROUND:
                    if (lang != LANG_ADDON)
                        TC_LOG_DEBUG("chat.log.bg", "[BATTLEGROUND] Player %s tells battleground with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    else
                        TC_LOG_DEBUG("chat.log.addon.bg", "[BATTLEGROUND] [ADDON] Player %s tells battleground with leader %s: %s",
                            player->GetName().c_str(), group ? group->GetLeaderName() : "<unknown>", msg.c_str());
                    break;

                case CHAT_MSG_BATTLEGROUND_LEADER:
                    TC_LOG_DEBUG("chat.log.bg", "[BATTLEGROUND] [LEADER] Leader player %s tells battleground: %s",
                        player->GetName().c_str(), msg.c_str());
                    break;
            }
        }

        void OnChat(Player* player, uint32 type, uint32 lang, std::string& msg, Guild* guild) OVERRIDE
        {
            switch (type)
            {
                case CHAT_MSG_GUILD:
                    if (lang != LANG_ADDON)
                        TC_LOG_DEBUG("chat.log.guild", "[GUILD] Player %s tells guild %s: %s",
                            player->GetName().c_str(), guild ? guild->GetName().c_str() : "<unknown>", msg.c_str());
                    else
                        TC_LOG_DEBUG("chat.log.addon.guild", "[GUILD] [ADDON] Player %s sends to guild %s: %s",
                            player->GetName().c_str(), guild ? guild->GetName().c_str() : "<unknown>", msg.c_str());
                    break;

                case CHAT_MSG_OFFICER:
                    TC_LOG_DEBUG("chat.log.guild.officer", "[GUILD] [OFFICER] Player %s tells guild %s officers: %s",
                        player->GetName().c_str(), guild ? guild->GetName().c_str() : "<unknown>", msg.c_str());
                    break;
            }
        }

        void OnChat(Player* player, uint32 /*type*/, uint32 /*lang*/, std::string& msg, Channel* channel) OVERRIDE
        {
            bool isSystem = channel &&
                            (channel->HasFlag(CHANNEL_FLAG_TRADE) ||
                             channel->HasFlag(CHANNEL_FLAG_GENERAL) ||
                             channel->HasFlag(CHANNEL_FLAG_CITY) ||
                             channel->HasFlag(CHANNEL_FLAG_LFG));

            if (isSystem)
                TC_LOG_DEBUG("chat.log.system", "[SYSTEM] Player %s tells channel %s: %s",
                    player->GetName().c_str(), channel->GetName().c_str(), msg.c_str());
            else
            {
                std::string channelName = channel ? channel->GetName() : "<unknown>";
                TC_LOG_DEBUG("chat.log.channel." + channelName, "[CHANNEL] Player %s tells channel %s: %s",
                    player->GetName().c_str(), channelName.c_str(), msg.c_str());
            }
        }
};

void AddSC_chat_log()
{
    new ChatLogScript();
}
