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

#include <QEvent>
#include <QPainter>
#include "ShowBoard.h"

#include <stack>

ShowBoard::ShowBoard(QWidget *parent) :
    QWidget(parent)
{
    get_board_size();
    show_board = new QWidget(this);
    show_stones = new QWidget(this);
    show_board->installEventFilter(this);
    show_stones->installEventFilter(this);
    refresh();
}

ShowBoard::~ShowBoard() {
}

void ShowBoard::refresh() {
    siz = 19;
    conflict = confmove = 0;
    memset(ston, 0, sizeof(char) * 676);
    memset(calcbord, 0, sizeof(char) * 676);
#if defined (ANCIENT_CHINESE_RULE_ENABLED)
    ston[3][3] = ston[15][15] = 2;
    ston[3][15] = ston[15][3] = 1;
#endif
    currmove = -1;
    show_board->update();
    show_stones->update();
}

void ShowBoard::drop_stone(int colour, int move) {
    configDropStone(colour, move);
    show_stones->update();
}

bool ShowBoard::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Paint) {
        if (watched == show_stones) {
            drawing_stones();
        } else if (watched == show_board) {
            drawing_board();
        }
    }
    return QWidget::eventFilter(watched, event);
}

void ShowBoard::resizeEvent(QResizeEvent *event) {
    get_board_size();
    show_board->setGeometry(win_xlb, win_ylb, win_size, win_size);
    this->show_board->setGeometry(QRect(win_xlb, win_ylb,
                                        win_gap * 19, win_gap * 19));
    this->show_stones->setGeometry(QRect(win_xlb, win_ylb,
                                         win_gap * 19, win_gap * 19));
    show_board->update();
    show_stones->update();
}

void ShowBoard::get_board_size() {
    int w = this->width() > 100 ? this->width() : 100;
    int h = this->height() > 100 ? this->height() : 100;

    int board_size = std::min(w, h);

    win_gap = board_size / 19;
    win_size = win_gap * 19;
    win_xlb = (w - win_size) / 2;
    win_ylb = (h - win_size) / 2;
}

void ShowBoard::drawing_board() {
    QPainter pain(show_board);
    QRect target(0, 0, win_gap * 19, win_gap * 19);
    pain.drawImage(target, QImage(":/images/bord.png"));

    int line_width = win_gap / 15;
    if (line_width < 2)
        line_width = 2;
    if (line_width % 2 == 1)
        line_width += 1;
    QPen pen(Qt::black);
    pen.setWidth(line_width);
    int tmpi, tmpj;
    pain.setPen(pen);
    int border = win_gap / 2 - (line_width / 2);
    for (tmpi = 0; tmpi < 19; tmpi++) { // Draw lines
        pain.drawLine(border, border + tmpi * win_gap,
                      border + 18 * win_gap, border + tmpi * win_gap);
        pain.drawLine(border + tmpi * win_gap, border,
                      border + tmpi * win_gap, border + 18 * win_gap);
    }
    int dotsize = line_width + 1;
    pain.setPen(Qt::PenStyle::NoPen);
    pain.setBrush(Qt::black);
    for (tmpi = border + 3 * win_gap - dotsize; tmpi < win_size;
         tmpi += 6 * win_gap) { // Draw stars
        for (tmpj = border + 3 * win_gap - dotsize; tmpj < win_size;
             tmpj += 6 * win_gap) {
            pain.drawRect(tmpi, tmpj, dotsize * 2, dotsize * 2);
        }
    }
}

void ShowBoard::drawing_stones() {
    QPainter pain(show_stones);
    pain.setRenderHint(QPainter::Antialiasing, true);

    int tmpi, tmpj;
    pain.setPen(Qt::black);
    for (tmpi = 0; tmpi < 19; tmpi++) {
        for (tmpj = 0; tmpj < 19; tmpj++) {
            if (ston[tmpi][tmpj] == 1) {
                QRect target(tmpi * win_gap + win_gap / 20,
                             tmpj * win_gap + win_gap / 20,
                             win_gap * 9 / 10,
                             win_gap * 9 / 10);
                pain.drawImage(target, QImage(":/images/movblack.png"));
            }
            else if (ston[tmpi][tmpj] == 2) {
                QRect target(tmpi * win_gap + win_gap / 20,
                             tmpj * win_gap + win_gap / 20,
                             win_gap * 9 / 10,
                             win_gap * 9 / 10);
                pain.drawImage(target, QImage(":/images/movwhite.png"));
            }
        }
    } // finished for

    if (currmove >= 0) {
        tmpi = ((currmove & 0xFF00) >> 8);
        tmpj = currmove & 0xFF;
        QRect target(tmpi * win_gap + win_gap / 2,
                     tmpj * win_gap + win_gap / 2,
                     win_gap / 3, win_gap / 3);
        pain.drawImage(target, QImage(":/images/cur_mov.png"));
    }
}

int ShowBoard::findStoneBlock(int colour, int tmpcolour, int pos,
                                   int tmpsiz, int &stonnum) {
    // Find a block of stones and return its breaths.
    // |colour| is target colour.
    // |tmpcolour| is colour this block of stones on |calcbord|.
    int tmpmov, breath;
    int tmpx, tmpy;
    std::stack<int> movStack;
    movStack.push(pos);
    breath = 0;
    stonnum = 1;
    while (!movStack.empty()) {
        tmpmov = movStack.top(); movStack.pop();
        tmpx = (tmpmov & 0xFF00) >> 8; tmpy = tmpmov & 0xFF;
        if ((tmpx > 0) && (calcbord[tmpx - 1][tmpy] != tmpcolour)) {
            if (ston[tmpx - 1][tmpy] == colour) {
                movStack.push(tmpmov - 0x100);
                calcbord[tmpx - 1][tmpy] = tmpcolour;
                stonnum++;
            }
            else if (ston[tmpx - 1][tmpy] == 0) breath++;
        }
        if ((tmpx < tmpsiz) && (calcbord[tmpx + 1][tmpy] != tmpcolour)) {
            if (ston[tmpx + 1][tmpy] == colour) {
                movStack.push(tmpmov + 0x100);
                calcbord[tmpx + 1][tmpy] = tmpcolour;
                stonnum++;
            }
            else if (ston[tmpx + 1][tmpy] == 0) breath++;
        }
        if ((tmpy > 0) && (calcbord[tmpx][tmpy - 1] != tmpcolour)) {
            if (ston[tmpx][tmpy - 1] == colour) {
                movStack.push(tmpmov - 1);
                calcbord[tmpx][tmpy - 1] = tmpcolour;
                stonnum++;
            }
            else if (ston[tmpx][tmpy - 1] == 0) breath++;
        }
        if ((tmpy < tmpsiz) && (calcbord[tmpx][tmpy + 1] != tmpcolour)) {
            if (ston[tmpx][tmpy + 1] == colour) {
                movStack.push(tmpmov + 1);
                calcbord[tmpx][tmpy + 1] = tmpcolour;
                stonnum++;
            }
            else if (ston[tmpx][tmpy + 1] == 0) breath++;
        }
    } // finished while loop
    return breath;
} // finished findStoneBlock

int ShowBoard::configDropStone(int colour, int mov) {
    // Place judgement: 0 cannot move; 1 should lift stones, 2 no lifted stones.
    // |calcbord| marking rule: 3 in same colour, 5 in different colour, 5 lift
    // stones, 6 in different colour but no lifted stones.
    // Return: 0 banning place (including conflict place), 1 lift and start
    //conflict, 2 lift stones, 3 no lifted stones.

    if (mov < 0) return 0;
    if (ston[(mov & 0xFF00) >> 8][mov & 0xFF]) return 0;
    if (conflict && (mov == confmove)) return 0;

    int opcolour;
    int tmpx, tmpy;
    int tmpi, tmpj;
    int tmpsiz, lift, bnum;
    tmpsiz = siz - 1;
    opcolour = (colour == 1) ? 2 : 1;
    memset(calcbord, 0, sizeof(char) * 676);
    tmpx = (mov & 0xFF00) >> 8; tmpy = mov & 0xFF;
    calcbord[tmpx][tmpy] = TB_SAMECOLOUR;
    ston[tmpx][tmpy] = colour;

    //int liftcolour;
    std::vector<int> liftsave;

    // Should firstly judge whether there are lifted stones.
    lift = 0;
    if ((tmpx > 0) && (ston[tmpx - 1][tmpy] == opcolour)) { // Upper
        calcbord[tmpx - 1][tmpy] = TB_DIFFCOLOUR;
        if (findStoneBlock(opcolour, TB_DIFFCOLOUR, mov - 0x100, tmpsiz, tmpi)
                == 0) { // No breath, should lift stones.
            lift += tmpi;
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_LIFTSTONES;
                }
            }
        }
        else { // Hath breath, no lifted stones.
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_DIFFNOLIFT;
                }
            }
        }
    }
    if ((tmpx < tmpsiz) && (ston[tmpx + 1][tmpy] == opcolour) &&
        (calcbord[tmpx + 1][tmpy] != TB_LIFTSTONES) &&
            (calcbord[tmpx + 1][tmpy] != TB_DIFFNOLIFT)) { // Down
        calcbord[tmpx + 1][tmpy] = TB_DIFFCOLOUR;
        if (findStoneBlock(opcolour, TB_DIFFCOLOUR, mov + 0x100, tmpsiz, tmpi)
                == 0) { // No breath, should lift stones.
            lift += tmpi;
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_LIFTSTONES;
                }
            }
        }
        else { // Hath breath, no lifted stones.
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_DIFFNOLIFT;
                }
            }
        }
    }
    if ((tmpy > 0) && (ston[tmpx][tmpy - 1] == opcolour) &&
        (calcbord[tmpx][tmpy - 1] != TB_LIFTSTONES) &&
            (calcbord[tmpx][tmpy - 1] != TB_DIFFNOLIFT)) { // Left
        calcbord[tmpx][tmpy - 1] = TB_DIFFCOLOUR;
        if (findStoneBlock(opcolour, TB_DIFFCOLOUR, mov - 1, tmpsiz, tmpi)
                == 0) { // No breath, should lift stones.
            lift += tmpi;
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_LIFTSTONES;
                }
            }
        }
        else { // Hath breath, no lifted stones.
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_DIFFNOLIFT;
                }
            }
        }
    }
    if ((tmpy < tmpsiz) && (ston[tmpx][tmpy + 1] == opcolour) &&
        (calcbord[tmpx][tmpy + 1] != TB_LIFTSTONES) &&
            (calcbord[tmpx][tmpy + 1] != TB_DIFFNOLIFT)) { // Right
        calcbord[tmpx][tmpy + 1] = TB_DIFFCOLOUR;
        if (findStoneBlock(opcolour, TB_DIFFCOLOUR, mov + 1, tmpsiz, tmpi)
                == 0) { // No breath, should lift stones.
            lift += tmpi;
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_LIFTSTONES;
                }
            }
        }
        else { // Hath breath, no lifted stones.
            for (tmpi = 0; tmpi <= tmpsiz; tmpi++) {
                for (tmpj = 0; tmpj <= tmpsiz; tmpj++) {
                    if (calcbord[tmpi][tmpj] == TB_DIFFCOLOUR)
                        calcbord[tmpi][tmpj] = TB_DIFFNOLIFT;
                }
            }
        }
    }
    // Lifted stones judgement finished.

    if ((findStoneBlock(colour, TB_SAMECOLOUR, mov, tmpsiz, bnum) == 0)
            && (lift == 0)) { // Commit suicide
        ston[tmpx][tmpy] = 0; // Cancel move
        printf("No breath No kill: (%d, %d) %d", tmpx, tmpy, lift);
        return 0; // Cannot drop stone
    }
    currmove = mov;
    if (lift == 1 && bnum == 1) { // Lift stone and step into confliction
        //liftcolour = (colour == 1 ? 2 : 1); // Opponent colour
        if (tmpx > 0 && calcbord[tmpx - 1][tmpy] == TB_LIFTSTONES) {
            ston[tmpx - 1][tmpy] = 0;
            confmove = ((tmpx - 1) << 8) + tmpy;
            liftsave.push_back(confmove); // Save lifted stones
        }
        if (tmpx < tmpsiz && calcbord[tmpx + 1][tmpy] == TB_LIFTSTONES) {
            ston[tmpx + 1][tmpy] = 0;
            confmove = ((tmpx + 1) << 8) + tmpy;
            liftsave.push_back(confmove); // Save lifted stones
        }
        if (tmpy > 0 && calcbord[tmpx][tmpy - 1] == TB_LIFTSTONES) {
            ston[tmpx][tmpy - 1] = 0;
            confmove = (tmpx << 8) + tmpy - 1;
            liftsave.push_back(confmove); // Save lifted stones
        }
        if (tmpy < tmpsiz && calcbord[tmpx][tmpy + 1] == TB_LIFTSTONES) {
            ston[tmpx][tmpy + 1] = 0;
            confmove = (tmpx << 8) + tmpy + 1;
            liftsave.push_back(confmove); // Save lifted stones
        }
        conflict = 1;
        //printf("New confliction: %d, %d\n", conflict, confmove);
        return 1;
    }
    if (lift > 0) { // Lift stones
        //liftcolour = (colour == 1 ? 2 : 1); // Opponent colour
        conflict = 0; // Step out confliction
        for (tmpi = 0; tmpi < siz; tmpi++) { // Lift
            for (tmpj = 0; tmpj < siz; tmpj++) {
                if (calcbord[tmpi][tmpj] == TB_LIFTSTONES) { // Lift
                    ston[tmpi][tmpj] = 0;
                    liftsave.push_back((tmpi << 8) + tmpj); // Save Lifted stones
                }
            }
        }
        return 2; // Hath lifted stones
    }
    // default case: Attention, need to step out confiliction!
    conflict = 0;
    return 3; // No need to lift stones
} // finished configDropStone
