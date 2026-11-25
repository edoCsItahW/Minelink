// Copyright (c) 2026. All rights reserved.
// This source code is licensed under the CC BY-NC-SA
// (Creative Commons Attribution-NonCommercial-NoDerivatives) License, By Xiao Songtao.
// This software is protected by copyright law. Reproduction, distribution, or use for commercial
// purposes is prohibited without the author's permission. If you have any questions or require
// permission, please contact the author: edocsitahw@qq.com

/**
 * @file types.ts
 * @author edocsitahw
 * @version 1.1
 * @date 2026/11/19 20:20
 * @desc
 * @copyright CC BY-NC-SA 2026. All rights reserved.
 * */

export type Integer = number;

export type Floating = number;

export type Material = `${"" | `${string};`}${string}${"" | `/${string}`}`;

export type IntegerString = `${number}`;

export interface Block {
    id: Integer;
    name: string;
    displayName: string;
    hardness: Floating;
    resistance: Floating;
    stackSize: Integer;
    diggable: boolean;
    material: Material;
    transparent: boolean;
    emitLight: Integer;
    filterLight: Integer;
    defaultState: Integer;
    minStateId: Integer;
    maxStateId: Integer;
    states: {
        name: string;
        type: string;
        num_values: Integer;
        values: IntegerString[];
    }[];
    harvestTools: { [K in IntegerString]: boolean; };
    drops: Integer[];
    boundingBox: string;
}