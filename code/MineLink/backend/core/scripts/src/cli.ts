// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file cli.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/21 12:30
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */
import {collectRanges, renderBlockTable} from "./genBlockStates.ts";
import {mkdir, readFile, writeFile} from "node:fs/promises";
import { dirname, resolve } from "node:path";
import type {Block} from "./types.js";
//import { parseArgs } from "node:util";

const USAGE = "usage: gen-block-states <blocks.json> <out.cxx>";

async function main(argv: readonly string[]) {
//    const { values: opts, positionals: args } = parseArgs({
//        options: {
//            out: { type: "string", short: "o" }
//        },
//        allowPositionals: true
//    });
    if (argv.length < 2) throw {msg: USAGE, code: 2};

    const [blocksPath, outPath] = argv as [string, string];

    const blocks = await readFile(blocksPath, "utf-8")
        .catch((err: Error) => {
            throw {msg: `failed to read ${blocksPath}: ${err.message}`, code: 1};
        })
        .then(
            text => JSON.parse(text) as Block[]
        ).catch((err: Error) => {
            throw {msg: `failed to parse ${blocksPath}: ${err.message}`, code: 1};
        });

    const ranges = collectRanges(blocks);
    if (ranges.length === 0)
        throw {msg: "No blocks with states found", code: 1};

    ranges.sort((a, b) => a.min - b.min);

    let next = 0;
    for (const r of ranges) {
        if (r.min !== next)
            throw { msg: `state ids not contiguous at ${r.name}: expected ${next}, got ${r.min}`, code: 1 };
        next = r.max + 1;
    }

    const content = renderBlockTable(ranges);

    const out = resolve(outPath);
    await mkdir(dirname(out), { recursive: true });
    await writeFile(out, content, "utf8");
}


main(process.argv.slice(2)).catch((err: Error | { msg: string; code: number; }) => {
    if (err instanceof Error) {
        console.error(err.message);
        process.exit(1);
    } else {
        console.error(err.msg);
        process.exit(err.code);
    }
});
