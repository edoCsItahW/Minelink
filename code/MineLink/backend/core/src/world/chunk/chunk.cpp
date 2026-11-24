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
 * @date 2026/11/19 18:31
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module world;

namespace minelink::world {

    namespace {

        [[nodiscard]] std::size_t entryIndex(int x, int y, int z) noexcept {
            return static_cast<std::size_t>((y * section_size + z) * section_size + x);
        }

        [[nodiscard]] std::size_t expectedLongCount(std::uint8_t bits) noexcept {
            const std::size_t perLong = 64 / bits;
            return (section_volume + perLong - 1) / perLong;
        }

        [[nodiscard]] std::uint32_t readEntry(
            const std::vector<std::uint64_t>& data, std::size_t index, std::uint8_t bits
        ) noexcept {
            const std::size_t perLong   = 64 / bits;
            const std::size_t longIndex = index / perLong;

            if (longIndex >= data.size()) return 0;

            const std::size_t offset = (index % perLong) * bits;
            const std::uint64_t mask = (std::uint64_t{1} << bits) - 1;

            return static_cast<std::uint32_t>((data[longIndex] >> offset) & mask);
        }

        [[nodiscard]] bool skipBytes(protocol::Reader& reader, std::size_t bytes) noexcept {
            while (bytes >= 8) {
                if (!reader.read<std::uint64_t>()) return false;
                bytes -= 8;
            }
            while (bytes != 0) {
                if (!reader.read<std::uint8_t>()) return false;
                --bytes;
            }
            return true;
        }

        [[nodiscard]] bool skipNbtString(protocol::Reader& reader) noexcept {
            const auto length = reader.read<std::uint16_t>();
            if (!length) return false;
            return skipBytes(reader, *length);
        }

        [[nodiscard]] bool skipNbtPayload(
            protocol::Reader& reader, std::uint8_t type, int depth
        ) noexcept {
            if (depth > 64) return false;  // 防畸形数据递归爆炸

            switch (type) {
                case 0:  // TAG_End
                    return true;
                case 1:  // Byte
                    return skipBytes(reader, 1);
                case 2:  // Short
                    return skipBytes(reader, 2);
                case 3:  // Int
                    return skipBytes(reader, 4);
                case 4:  // Long
                    return skipBytes(reader, 8);
                case 5:  // Float
                    return skipBytes(reader, 4);
                case 6:  // Double
                    return skipBytes(reader, 8);
                case 7: {  // ByteArray
                    const auto count = reader.read<std::uint32_t>();   // NBT 长度 = 定长 i32
                    if (!count) return false;
                    if (static_cast<std::size_t>(*count) > reader.remaining()) return false;
                    return skipBytes(reader, static_cast<std::size_t>(*count));
                }
                case 8:  // String
                    return skipNbtString(reader);
                case 9: {  // List
                    const auto elementType = reader.read<std::uint8_t>();
                    const auto count       = reader.read<std::uint32_t>();   // 同上
                    if (!elementType || !count) return false;
                    if (*elementType == 0) return *count == 0;
                    if (static_cast<std::size_t>(*count) > reader.remaining()) return false;
                    for (std::uint32_t i = 0; i < *count; ++i)
                        if (!skipNbtPayload(reader, *elementType, depth + 1)) return false;
                    return true;
                }
                case 10: {  // Compound
                    for (;;) {
                        const auto childType = reader.read<std::uint8_t>();
                        if (!childType) return false;
                        if (*childType == 0) return true;
                        if (!skipNbtString(reader)) return false;
                        if (!skipNbtPayload(reader, *childType, depth + 1)) return false;
                    }
                }
                case 11: {  // IntArray
                    const auto count = reader.read<std::uint32_t>();
                    if (!count) return false;
                    if (static_cast<std::size_t>(*count) > reader.remaining() / 4) return false;
                    return skipBytes(reader, static_cast<std::size_t>(*count) * 4u);
                }
                case 12: {  // LongArray
                    const auto count = reader.read<std::uint32_t>();
                    if (!count) return false;
                    if (static_cast<std::size_t>(*count) > reader.remaining() / 8) return false;
                    return skipBytes(reader, static_cast<std::size_t>(*count) * 8u);
                }
                default: return false;
            }
        }

        [[nodiscard]] bool skipAnonymousNbt(protocol::Reader& reader) noexcept {
            const auto type = reader.read<std::uint8_t>();
            if (!type) return false;
            if (*type == 0) return true;
            return skipNbtPayload(reader, *type, 0);
        }

        [[nodiscard]] bool skipBiomes(protocol::Reader& reader) noexcept {
            const auto bits = reader.read<std::uint8_t>();
            if (!bits) return false;

            if (*bits == 0) {
                const auto biome  = reader.read<std::int32_t>();  // 单值
                const auto length = reader.read<std::int32_t>();  // data array length（0）
                return biome && length && *length == 0;
            }

            if (*bits <= 3) {
                const auto paletteCount = reader.read<std::int32_t>();
                if (!paletteCount || *paletteCount <= 0 || *paletteCount > 256) return false;
                for (std::int32_t i = 0; i < *paletteCount; ++i)
                    if (!reader.read<std::int32_t>()) return false;
            }

            const auto longCount = reader.read<std::int32_t>();
            if (!longCount || *longCount < 0) return false;
            return skipBytes(reader, static_cast<std::size_t>(*longCount) * 8u);
        }

    }  // namespace

    // Section

    bool Section::decode(protocol::Reader& reader) noexcept {
        const auto count = reader.read<std::uint16_t>();
        if (!count) return false;

        blockCount_ = static_cast<std::uint16_t>(*count);

        const auto bits = reader.read<std::uint8_t>();
        if (!bits) return false;

        bits_ = *bits;
        palette_.clear();

        if (bits_ == 0) {
            const auto state = reader.read<std::int32_t>();
            if (!state || *state < 0) return false;

            palette_.push_back(static_cast<std::uint32_t>(*state));

            const auto length = reader.read<std::int32_t>();
            if (!length || *length) return false;
        } else if (bits_ == 15) {
        } else if (bits_ >= 4 && bits_ <= 8) {
            const auto paletteCount = reader.read<std::int32_t>();
            if (!paletteCount || *paletteCount <= 0 || *paletteCount > (1 << bits_)) return false;

            palette_.reserve(static_cast<std::size_t>(*paletteCount));

            for (std::int32_t i = 0; i < *paletteCount; ++i) {
                const auto entry = reader.read<std::int32_t>();
                if (!entry || *entry < 0) return false;

                palette_.push_back(static_cast<std::uint32_t>(*entry));
            }
        } else
            return false;

        data_.clear();

        if (bits_ > 0) {
            const auto longCount = reader.read<std::int32_t>();
            if (!longCount || *longCount <= 0) return false;

            if (static_cast<std::size_t>(*longCount) != expectedLongCount(bits_)) return false;

            data_.reserve(static_cast<std::size_t>(*longCount));

            for (std::int32_t i = 0; i < *longCount; ++i) {
                const auto word = reader.read<std::uint64_t>();
                if (!word) return false;

                data_.push_back(*word);
            }
        }

        return skipBiomes(reader);
    }

    std::uint32_t Section::blockState(int x, int y, int z) const noexcept {
        if (x < 0 || x >= section_size || y < 0 || y >= section_size || z < 0
            || z >= section_size)
            return 0;

        if (bits_ == 0) return palette_.empty() ? 0 : palette_[0];

        const std::uint32_t value = readEntry(data_, entryIndex(x, y, z), bits_);

        if (bits_ == 15) return value;
        return value < palette_.size() ? palette_[value] : 0;
    }

    std::int16_t Section::blockCount() const noexcept { return blockCount_; }

    std::int8_t Section::bitsPerEntry() const noexcept { return bits_; }

    // ChunkColumn

    bool ChunkColumn::decode(
        std::int32_t chunkX, std::int32_t chunkZ, std::span<const std::byte> data
    ) noexcept {
        protocol::Reader reader{data};

        return decodeFrom(reader, chunkX, chunkZ);
    }

    bool ChunkColumn::decodeFrom(
        protocol::Reader& reader, std::int32_t chunkX, std::int32_t chunkZ
    ) noexcept {
        std::vector<Section> sections(section_count);
        for (auto& section : sections)
            if (!section.decode(reader)) return false;

        sections_ = std::move(sections);
        chunkX_   = chunkX;
        chunkZ_   = chunkZ;

        return true;
    }

    std::uint32_t ChunkColumn::blockState(int x, int y, int z) const noexcept {
        if (y < 0 || x < 0 || x >= section_size || z < 0 || z >= section_size) return 0;

        const auto sectionIndex  = static_cast<std::size_t>(y / section_size);
        const auto sectionLocalY = y % section_size;

        if (sectionIndex >= sections_.size()) return 0;

        return sections_[sectionIndex].blockState(x, sectionLocalY, z);
    }

    std::int32_t ChunkColumn::chunkX() const noexcept { return chunkX_; }

    std::int32_t ChunkColumn::chunkZ() const noexcept { return chunkZ_; }

    std::size_t ChunkColumn::sectionCount() const noexcept { return sections_.size(); }

    bool decodeMapChunk(protocol::Reader& reader, ChunkColumn& column) noexcept {
        constexpr std::int32_t max_chunk_data_bytes = 8 * 1024 * 1024;

        const auto rawX = reader.read<std::uint32_t>();   // 定长 4 字节大端
        const auto rawZ = reader.read<std::uint32_t>();
        if (!rawX || !rawZ) return false;

        const std::int32_t chunkX = std::bit_cast<std::int32_t>(*rawX);
        const std::int32_t chunkZ = std::bit_cast<std::int32_t>(*rawZ);

        if (!skipAnonymousNbt(reader)) return false;

        const auto dataLength = reader.read<std::int32_t>();
        if (!dataLength || *dataLength <= 0 || *dataLength > max_chunk_data_bytes) return false;

        const auto data = reader.take(static_cast<std::size_t>(*dataLength));
        if (!data) return false;

        protocol::Reader sectionReader{*data};
        if (!column.decodeFrom(sectionReader, chunkX, chunkZ)) return false;

        return sectionReader.empty();  // data 段必须正好读完，多余/不足都是畸形包
    }

}  // namespace minelink::word
