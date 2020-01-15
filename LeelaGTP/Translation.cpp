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
