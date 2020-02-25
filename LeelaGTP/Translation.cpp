/*
    This file is part of Leela GTP.
    Copyright (C) 2019 Fierralin
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

#include "Translation.h"
#include <QTextStream>

LeelaGTPLocale __leela_gtp_locale = LeelaGTPLocale::Default;

QString Trans(QString key) {
   QHash<QString, TranslateValue>::iterator it = __trans.find(key);
   if (it == __trans.end()) {
       return __trans["not_defined"][__leela_gtp_locale];
   }
   return it.value()[__leela_gtp_locale];
}
