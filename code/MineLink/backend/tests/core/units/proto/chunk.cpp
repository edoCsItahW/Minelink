// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file chunk.cpp 
 * @author edocsitahw 
 * @version 1.1
 * @date 2026/11/24 15:08
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
#include <gtest/gtest.h>

import std;
import protocol;
import world;

using namespace minelink;
using namespace minelink::world;

namespace {

/// @brief 按协议字节序（大端 / LEB128）拼测试用的字节流。
class Buf {
public:
    [[nodiscard]] std::span<const std::byte> bytes() const noexcept { return data_; }

    void u8(std::uint8_t value) { data_.push_back(static_cast<std::byte>(value)); }

    void u16(std::uint16_t value) {
        data_.push_back(static_cast<std::byte>(value >> 8));
        data_.push_back(static_cast<std::byte>(value));
    }

    void i32(std::int32_t value) {
        const auto raw = static_cast<std::uint32_t>(value);
        for (int shift = 24; shift >= 0; shift -= 8)
            data_.push_back(static_cast<std::byte>(raw >> shift));
    }

    void u64(std::uint64_t value) {
        for (int shift = 56; shift >= 0; shift -= 8)
            data_.push_back(static_cast<std::byte>(value >> shift));
    }

    void varint(std::int32_t value) {
        auto raw = static_cast<std::uint32_t>(value);
        do {
            std::uint8_t byte = static_cast<std::uint8_t>(raw & 0x7Fu);
            raw >>= 7;
            if (raw != 0) byte |= 0x80u;
            u8(byte);
        } while (raw != 0);
    }

private:
    std::vector<std::byte> data_;
};

/// @brief 追加一个单值（bits = 0）方块 section。
void putSingleValuedSection(Buf& buf, std::uint32_t state) {
    buf.u16(4096);   // block count
    buf.u8(0);       // bits per entry
    buf.varint(static_cast<std::int32_t>(state));
    buf.varint(0);   // data array length（1.20.4 仍会写）
    buf.u8(0);       // biome bits
    buf.varint(0);   // 单值 biome
    buf.varint(0);   // biome data array length
}

/// @brief 追加一个 section 的 biome 部分（单值）。
void putEmptyBiomes(Buf& buf) {
    buf.u8(0);
    buf.varint(0);
    buf.varint(0);
}

}  // namespace

TEST(SectionDecode, SingleValued) {
    Buf buf;
    putSingleValuedSection(buf, 1);

    protocol::Reader reader{buf.bytes()};
    Section section;

    ASSERT_TRUE(section.decode(reader));
    EXPECT_EQ(section.bitsPerEntry(), 0);
    EXPECT_EQ(section.blockCount(), 4096);

    for (int x : {0, 8, 15})
        for (int y : {0, 8, 15})
            for (int z : {0, 8, 15}) EXPECT_EQ(section.blockState(x, y, z), 1u);
}

TEST(SectionDecode, IndirectFourBits) {
    Buf buf;
    buf.u16(2);      // block count
    buf.u8(4);       // bits per entry
    buf.varint(2);   // palette 条目数
    buf.varint(0);   // 条目 0 → air
    buf.varint(1);   // 条目 1 → stone
    buf.varint(256); // data array length = 4096 / 16

    buf.u64(0x1u);   // index 0 (0,0,0) = 1 → stone；index 1 = 0 → air
    for (int i = 1; i < 256; ++i) buf.u64(0);

    putEmptyBiomes(buf);

    protocol::Reader reader{buf.bytes()};
    Section section;

    ASSERT_TRUE(section.decode(reader));
    EXPECT_EQ(section.bitsPerEntry(), 4);
    EXPECT_EQ(section.blockState(0, 0, 0), 1u);   // palette[1]
    EXPECT_EQ(section.blockState(1, 0, 0), 0u);   // palette[0]
    EXPECT_EQ(section.blockState(15, 15, 15), 0u);
}

TEST(SectionDecode, DirectFifteenBits) {
    constexpr std::uint32_t kState = 5;  // 不走 palette，直接是全局 state

    Buf buf;
    buf.u16(1);
    buf.u8(15);       // direct
    buf.varint(1024); // 4096 / (64 / 15) = 1024

    buf.u64(kState);  // index 0 (0,0,0)
    for (int i = 1; i < 1024; ++i) buf.u64(0);

    putEmptyBiomes(buf);

    protocol::Reader reader{buf.bytes()};
    Section section;

    ASSERT_TRUE(section.decode(reader));
    EXPECT_EQ(section.bitsPerEntry(), 15);
    EXPECT_EQ(section.blockState(0, 0, 0), kState);
    EXPECT_EQ(section.blockState(1, 0, 0), 0u);
}

TEST(SectionDecode, BiomeDirectIsSkipped) {
    Buf buf;

    putSingleValuedSection(buf, 1);  // 第一个 section 的 biome 是单值

    // 第二个 section：方块单值 stone，biome 用 direct（bits 6）
    buf.u16(4096);
    buf.u8(0);
    buf.varint(1);
    buf.varint(0);
    buf.u8(6);        // biome direct
    buf.varint(410);  // 4096 / (64 / 6) = 410
    for (int i = 0; i < 410; ++i) buf.u64(0);

    Buf column;
    column.varint(2);  // section 数
    // 把 buf 内容拼过来（Buf 没有裸拷贝接口，按字节复制）
    const auto raw = buf.bytes();
    for (auto byte : raw) column.u8(std::to_integer<std::uint8_t>(byte));

    ChunkColumn chunk;
    ASSERT_TRUE(chunk.decode(0, 0, column.bytes()));
    ASSERT_EQ(chunk.sectionCount(), 2u);
    EXPECT_EQ(chunk.blockState(0, 0, 0), 1u);    // 第一个 section
    EXPECT_EQ(chunk.blockState(0, 16, 0), 1u);   // 第二个 section（y = 16 落到 section 1）
}

TEST(MapChunk, AppliesToWorldModel) {
    // 包体：chunkX、chunkZ、heightmaps(NBT 空 compound)、dataLength、sections
    Buf packet;
    packet.i32(-1);     // chunkX = -1
    packet.i32(0);      // chunkZ = 0
    packet.u8(10);      // TAG_Compound
    packet.u16(0);      // 空名字
    packet.u8(0);       // TAG_End
    packet.varint(64);  // dataLength（本实现只做合理性检查）

    packet.varint(1);   // section 数
    putSingleValuedSection(packet, 1);  // 整列 stone

    WorldModel world{-64};
    ASSERT_TRUE(world.applyChunk(packet.bytes()));
    ASSERT_EQ(world.chunkCount(), 1u);
    EXPECT_TRUE(world.hasChunk(-1, 0));

    // (-1, -64, 0) 落在 chunk(-1,0) 的局部 (15, 0, 0)
    EXPECT_EQ(world.blockState(-1, -64, 0), 1u);
    EXPECT_EQ(world.blockName(-1, -64, 0), "stone");

    // 未加载的 chunk → 空气
    EXPECT_EQ(world.blockState(16, -64, 0), 0u);
    EXPECT_EQ(world.blockName(16, -64, 0), "air");

    // 超出 section 范围的 y → 空气
    EXPECT_EQ(world.blockState(-1, 100, 0), 0u);
}

TEST(RegistryLookup, KnownAndUnknownStates) {
    EXPECT_EQ(blockName(0), "air");
    EXPECT_EQ(blockName(1), "stone");
    EXPECT_EQ(blockName(26643), "trial_spawner");  // 1.20.4 的最后一个状态（边界，含）
    EXPECT_EQ(blockName(26644), "unknown");        // 越界走兜底
    EXPECT_TRUE(validBlockState(1));
    EXPECT_FALSE(validBlockState(26644));
}