// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file model.cpp
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 19:10
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
module world;

namespace minelink::world {

    namespace {

        [[nodiscard]] constexpr std::int32_t floorDiv(
            std::int32_t value, std::int32_t divisor
        ) noexcept pre(divisor > 0) {
            const std::int32_t q = value / divisor;
            return q - (value % divisor < 0 ? 1 : 0);
        }

    }  // namespace

    std::size_t ChunkKeyHash::operator()(const ChunkKey& key) const noexcept {
        const auto x = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.x));
        const auto z = static_cast<std::uint64_t>(static_cast<std::uint32_t>(key.z));
        return (x << 32) ^ z;
    }

    WorldModel::WorldModel(std::int32_t minY) noexcept
        : minY_(minY) {}

    bool WorldModel::applyChunk(std::span<const std::byte> packetBody) noexcept {
        protocol::Reader reader{packetBody};

        ChunkColumn column;
        if (!decodeMapChunk(reader, column)) {
            ++failed_;
            return false;
        }

        chunks_.insert_or_assign(ChunkKey{column.chunkX(), column.chunkZ()}, std::move(column));

        ++applied_;

        return true;
    }

    std::uint32_t WorldModel::blockState(
        std::int32_t x, std::int32_t y, std::int32_t z
    ) const noexcept {
        const std::int32_t chunkX = floorDiv(x, section_size);
        const std::int32_t chunkZ = floorDiv(z, section_size);

        const auto it = chunks_.find(ChunkKey{chunkX, chunkZ});
        if (it == chunks_.end()) return 0;

        const std::int32_t localY = y - minY_;
        if (localY < 0) return 0;

        return it->second.blockState(x - chunkX * section_size, localY, z - chunkZ * section_size);
    }

    std::string_view WorldModel::blockName(
        std::int32_t x, std::int32_t y, std::int32_t z
    ) const noexcept {
        return world::blockName(blockState(x, y, z));
    }

    bool WorldModel::hasChunk(std::int32_t chunkX, std::int32_t chunkZ) const noexcept {
        return chunks_.contains(ChunkKey{chunkX, chunkZ});
    }

    std::size_t WorldModel::chunkCount() const noexcept { return chunks_.size(); }

    std::int32_t WorldModel::minY() const noexcept { return minY_; }

    std::size_t WorldModel::appliedChunks() const noexcept { return applied_; }

    std::size_t WorldModel::failedChunks() const noexcept { return failed_; }

}  // namespace minelink::world