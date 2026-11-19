// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file ids.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/18 13:38
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module protocol:ids;

import std;

export namespace minelink::protocol {

    namespace v1_20_4 {

        inline constexpr std::int32_t protocol_version = 765;

        // handshaking / to_server
        inline constexpr std::int32_t handshake_c2s_set_protocol = 0x00;

        // status
        inline constexpr std::int32_t status_c2s_ping_start  = 0x00;
        inline constexpr std::int32_t status_s2c_server_info = 0x00;

        // login / to_server
        inline constexpr std::int32_t login_c2s_login_start        = 0x00;
        inline constexpr std::int32_t login_c2s_login_acknowledged = 0x03;

        // login / to_client
        inline constexpr std::int32_t login_s2c_disconnect = 0x00;
        inline constexpr std::int32_t login_s2c_success    = 0x02;
        inline constexpr std::int32_t login_s2c_compress   = 0x03;

        // configuration / to_server
        inline constexpr std::int32_t config_c2s_settings             = 0x00;
        inline constexpr std::int32_t config_c2s_finish_configuration = 0x02;
        inline constexpr std::int32_t config_c2s_keep_alive           = 0x03;

        // configuration / to_client
        inline constexpr std::int32_t config_s2c_disconnect           = 0x01;
        inline constexpr std::int32_t config_s2c_finish_configuration = 0x02;
        inline constexpr std::int32_t config_s2c_keep_alive           = 0x03;

        // play / to_server
        inline constexpr std::int32_t play_c2s_teleport_confirm = 0x00;
        inline constexpr std::int32_t play_c2s_keep_alive       = 0x15;
        inline constexpr std::int32_t play_c2s_position         = 0x17;
        inline constexpr std::int32_t play_c2s_position_look    = 0x18;

        // play / to_client（只列出 Step 2 关心的）
        inline constexpr std::int32_t play_s2c_kick_disconnect = 0x1b;
        inline constexpr std::int32_t play_s2c_keep_alive      = 0x24;
        inline constexpr std::int32_t play_s2c_login           = 0x29;
        inline constexpr std::int32_t play_s2c_position        = 0x3e;
        inline constexpr std::int32_t play_s2c_update_health   = 0x5b;

    }  // namespace v1_20_4

}  // namespace minelink::protocol
