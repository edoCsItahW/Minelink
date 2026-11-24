// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file genBlockStates.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 20:20
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import type { Block } from "./types.ts";

type Optional<T> = T | undefined;

export interface StateRange {
    name: string;
    min: number;
    max: number;
}

const copyright = `
// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com
`
const pad = (n: number) => String(n).padStart(2, '0');

const commentHeader = (date: Date = new Date) =>`
/**
 * @file block_table.cxx
 * @author edocsitahw 
 * @version 1.1
 * @date ${date.getFullYear()}/${pad(date.getMonth() + 1)}/${pad(date.getDate())} ${pad(date.getHours())}:${pad(date.getMinutes())}
 * @brief
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
`;

export function collectRanges(blocks: Block[]): StateRange[] {
    const ranges: StateRange[] = [];

    for (const block of blocks) {
        const { minStateId: min, maxStateId: max } = block;
        if (!Number.isInteger(min) || !Number.isInteger(max) || min > max)
            throw new Error(`block ${block.name} has invalid range ${min}..${max}`);

        const combos = (block.states ?? []).reduce((n, prop) => n * prop.num_values, 1);
        const expected = max - min + 1;
        if (combos !== expected)
            throw new Error(
                `${block.name}: ${combos} state combinations for ${expected} ids ` +
                `(min=${min}, max=${max})`
            );

        ranges.push({ name: block.name, min, max });
    }

    return ranges;
}


export function renderBlockTable(ranges: readonly StateRange[]): string {
    if (ranges.length === 0)
        throw { msg: "cannot render an empty block table", code: 1 };

    const first = ranges[0]!;
    const last = ranges[ranges.length - 1]!;

    return `${copyright}${commentHeader()}
export module world:block_table;

import std;
import :registry.decl;

export namespace minelink::world {

    inline constexpr std::array<BlockInfo, ${ranges.length}> block_table{{
        ${ranges.map(r => `{"${r.name}", ${r.min}, ${r.max}}`).join(', \n\t\t')}
    }};

}  // namespace minelink::world
`;
}
