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

#ifndef SHOWBOARD_H
#define SHOWBOARD_H

#include <QWidget>
#include <QLabel>

#define MAXBORDSIZE 26 // Max board size.

// for tmpbord[][]
#define TB_EMPTYPOINT 0 // Empty point
#define TB_BLACKSTONE 1 // Black stone
#define TB_WHITESTONE 2 // White stone
#define TB_SAMECOLOUR 3 // In same colour
#define TB_DIFFCOLOUR 4 // In different colour
#define TB_LIFTSTONES 5 // Hath listed colours
#define TB_DIFFNOLIFT 6 // Hath

// for lift
#define CANNOTPLACE 0 // Banning point
#define LIFTSTONES  1 // Drop with lifted stones
#define NOLIFTS     2 // Drop without lifted stones

class ShowBoard : public QWidget
{
    Q_OBJECT
public:
    explicit ShowBoard(QWidget *parent = nullptr);
    ~ShowBoard();
    void refresh();
    void drop_stone(int colour, int move);
    bool eventFilter(QObject *watched, QEvent *event) override;

signals:

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    int siz;
    char ston[MAXBORDSIZE][MAXBORDSIZE];
    char calcbord[MAXBORDSIZE][MAXBORDSIZE];

    int conflict; // In conflict
    int confmove; // Confilict move
    int currmove;

    void get_board_size();
    void drawing_board();
    void drawing_stones();
    int findStoneBlock(int colour, int tmpcolour, int pos, int tmpsiz,
                       int &stonnum); // Move judgement
    int configDropStone(int colour, int mov); // Drop stone

    QWidget *show_board;
    QWidget *show_stones;

    /* Draw board */
    int win_size; /* Board size */
    int win_gap; /* = win_size / 20 */
    int win_xlb; /* Board top left corner abscissa */
    int win_ylb; /* Board top left corner ordinate */
};

#endif // SHOWBOARD_H
