// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file model.decl.cxx
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 19:04
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
export module world:model.decl;

import :chunk.decl;

export namespace minelink::world {

    struct ChunkKey {
        std::int32_t x{0};
        std::int32_t z{0};

        [[nodiscard]] bool operator==(const ChunkKey&) const noexcept = default;
    };

    struct ChunkKeyHash {
        [[nodiscard]] std::size_t operator()(const ChunkKey& key) const noexcept;
    };

    class WorldModel {
    public:
        explicit WorldModel(std::int32_t minY = -64) noexcept;

        [[nodiscard]] bool applyChunk(std::span<const std::byte> packetBody) noexcept;

        [[nodiscard]] std::uint32_t blockState(
            std::int32_t x, std::int32_t y, std::int32_t z
        ) const noexcept;

        [[nodiscard]] std::string_view blockName(
            std::int32_t x, std::int32_t y, std::int32_t z
        ) const noexcept;

        [[nodiscard]] bool hasChunk(std::int32_t chunkX, std::int32_t chunkZ) const noexcept;

        [[nodiscard]] std::size_t chunkCount() const noexcept;

        [[nodiscard]] std::int32_t minY() const noexcept;

        [[nodiscard]] std::size_t appliedChunks() const noexcept;

        [[nodiscard]] std::size_t failedChunks() const noexcept;

    private:
        std::int32_t minY_{-64};
        std::size_t applied_{0};
        std::size_t failed_{0};
        std::unordered_map<ChunkKey, ChunkColumn, ChunkKeyHash> chunks_;
    };

}  // namespace minelink::world
