/*
    This file is part of Leela GTP.
    Copyright (C) 2018 Fierralin
    Copyright (C) 2017-2018 Marco Calignano

    Leela GTP and Leela Zero are free softwares: you can redistribute it
    and/or modify it under the terms of the GNU General Public License as
    published by the Free Software Foundation, either version 3 of the
    License, or (at your option) any later version.

    Leela GTP is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela GTP.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef MOVLANCIFOLIUM_H
#define MOVLANCIFOLIUM_H

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cctype>
#include <stack>
#include <list>
#include <vector>

#define MAXBORDSIZE 26 // 最大棋盤規模

// for tmpbord[][]
#define TB_EMPTYPOINT 0 // 空點
#define TB_BLACKSTONE 1 // 黑子
#define TB_WHITESTONE 2 // 白子
#define TB_SAMECOLOUR 3 // 同色
#define TB_DIFFCOLOUR 4 //
#define TB_LIFTSTONES 5
#define TB_DIFFNOLIFT 6

// for lift
#define CANNOTPLACE 0 //
#define LIFTSTONES  1
#define NOLIFTS     2

struct MovLancifolium {
    int siz;
    char ston[MAXBORDSIZE][MAXBORDSIZE];
    char calcbord[MAXBORDSIZE][MAXBORDSIZE];

    int conflict; // 劫爭
    int confmove; // 劫爭熱點
    int currmove;

    void init();
    MovLancifolium();

    int findStoneBlock(int colour, int tmpcolour, int pos, int tmpsiz,
                       int &stonnum); // 落子判斷函數

    int configDropStone(int colour, int mov); // 著子判斷函數
};

#endif // MOVLANCIFOLIUM_H
