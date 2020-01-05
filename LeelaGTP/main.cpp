/*
    This file is part of Leela Zero.
    Copyright (C) 2018 Fierralin
    Copyright (C) 2017-2018 Gian-Carlo Pascutto
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
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "LeelaGTP.h"
int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    LeelaGTP w(&a);
    w.show();
    return a.exec();
}
