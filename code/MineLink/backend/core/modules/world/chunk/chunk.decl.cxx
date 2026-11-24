// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file chunk.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 18:18
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module world:chunk.decl;

import std;
import protocol;

export namespace minelink::world {

    inline constexpr int section_size = 16;

    inline constexpr std::size_t section_volume =
        static_cast<std::size_t>(section_size) * section_size * section_size;

    inline constexpr int max_sections_per_column = 32;

    inline constexpr std::size_t section_count = 24;

    class Section {
    public:
        [[nodiscard]] bool decode(protocol::Reader& reader) noexcept;

        [[nodiscard]] std::uint32_t blockState(int x, int y, int z) const noexcept;

        [[nodiscard]] std::int16_t blockCount() const noexcept;

        [[nodiscard]] std::int8_t bitsPerEntry() const noexcept;

    private:
        std::int16_t blockCount_{0};
        std::int8_t bits_{0};
        std::vector<std::uint32_t> palette_;
        std::vector<std::uint64_t> data_;
    };

    class ChunkColumn {
    public:
        [[nodiscard]] bool decode(
            std::int32_t chunkX, std::int32_t chunkZ, std::span<const std::byte> data
        ) noexcept;

        [[nodiscard]] bool decodeFrom(
            protocol::Reader& reader, std::int32_t chunkX, std::int32_t chunkZ
        ) noexcept;

        [[nodiscard]] std::uint32_t blockState(int x, int y, int z) const noexcept;

        [[nodiscard]] std::int32_t chunkX() const noexcept;

        [[nodiscard]] std::int32_t chunkZ() const noexcept;

        [[nodiscard]] std::size_t sectionCount() const noexcept;

    private:
        std::int32_t chunkX_{0};
        std::int32_t chunkZ_{0};
        std::vector<Section> sections_;
    };

    [[nodiscard]] bool decodeMapChunk(protocol::Reader& reader, ChunkColumn& column) noexcept;

}  // namespace minelink::word
