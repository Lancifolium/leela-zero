/*
    This file is part of Leela Zero.
    Copyright (C) 2017 Marco Calignano

    Leela Zero is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Leela Zero is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Leela Zero.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QUuid>
#include <QFile>
#include <QTextStream>
#include <QRegularExpression>
#include "Game.h"

Game::Game(const QString& weights, const QString& opt, const QString& binary) :
    QProcess(),
    m_cmdLine(""),
    m_binary(binary),
    m_timeSettings("time_settings 0 1 0"),
    m_resignation(false),
    m_blackToMove(false), // WHITE first
    m_blackResigned(false),
    m_passes(0),
    m_moveNum(0)
{
#ifdef WIN32
    m_binary.append(".exe");
#endif
    m_cmdLine = m_binary + " " + opt + " " + weights;
    m_fileName = QUuid::createUuid().toRfc4122().toHex();
}

bool Game::checkGameEnd() {
    return (m_resignation ||
            m_passes > 1 ||
            m_moveNum > (19 * 19 * 2));
}

void Game::error(int errnum) {
    QTextStream(stdout) << "*ERROR*: ";
    switch(errnum) {
        case Game::NO_LEELAZ:
            QTextStream(stdout)
                << "No 'leelaz' binary found." << endl;
            break;
        case Game::PROCESS_DIED:
            QTextStream(stdout)
                << "The 'leelaz' process died unexpected." << endl;
            break;
        case Game::WRONG_GTP:
            QTextStream(stdout)
                << "Error in GTP response." << endl;
            break;
        case Game::LAUNCH_FAILURE:
            QTextStream(stdout)
                << "Could not talk to engine after launching." << endl;
            break;
        default:
            QTextStream(stdout)
                << "Unexpected error." << endl;
            break;
    }
}

bool Game::eatNewLine() {
    char readBuffer[256];
    // Eat double newline from GTP protocol
    if (!waitReady()) {
        error(Game::PROCESS_DIED);
        return false;
    }
    auto readCount = readLine(readBuffer, 256);
    if(readCount < 0) {
        error(Game::WRONG_GTP);
        return false;
    }
    return true;
}

bool Game::sendGtpCommand(QString cmd) {
    write(qPrintable(cmd.append("\n")));
    waitForBytesWritten(-1);
    if (!waitReady()) {
        error(Game::PROCESS_DIED);
        return false;
    }
    char readBuffer[256];
    int readCount = readLine(readBuffer, 256);
    if (readCount <= 0 || readBuffer[0] != '=') {
        QTextStream(stdout) << "GTP: " << readBuffer << endl;
        error(Game::WRONG_GTP);
        return false;
    }
    if (!eatNewLine()) {
        error(Game::PROCESS_DIED);
        return false;
    }
    return true;
}

void Game::checkVersion(const VersionTuple &min_version) {
    write(qPrintable("version\n"));
    waitForBytesWritten(-1);
    if (!waitReady()) {
        error(Game::LAUNCH_FAILURE);
        exit(EXIT_FAILURE);
    }
    char readBuffer[256];
    int readCount = readLine(readBuffer, 256);
    //If it is a GTP comment just print it and wait for the real answer
    //this happens with the winogard tuning
    if (readBuffer[0] == '#') {
        readBuffer[readCount-1] = 0;
        QTextStream(stdout) << readBuffer << endl;
        if (!waitReady()) {
            error(Game::PROCESS_DIED);
            exit(EXIT_FAILURE);
        }
        readCount = readLine(readBuffer, 256);
    }
    // We expect to read at last "=, space, something"
    if (readCount <= 3 || readBuffer[0] != '=') {
        QTextStream(stdout) << "GTP: " << readBuffer << endl;
        error(Game::WRONG_GTP);
        exit(EXIT_FAILURE);
    }
    QString version_buff(&readBuffer[2]);
    version_buff = version_buff.simplified();
    QStringList version_list = version_buff.split(".");
    if (version_list.size() < 2) {
        QTextStream(stdout)
            << "Unexpected Leela Zero version: " << version_buff << endl;
        exit(EXIT_FAILURE);
    }
    if (version_list.size() < 3) {
        version_list.append("0");
    }
    int versionCount = (version_list[0].toInt() - std::get<0>(min_version)) * 10000;
    versionCount += (version_list[1].toInt() - std::get<1>(min_version)) * 100;
    versionCount += version_list[2].toInt() - std::get<2>(min_version);
    if (versionCount < 0) {
        QTextStream(stdout)
            << "Leela version is too old, saw " << version_buff
            << " but expected "
            << std::get<0>(min_version) << "."
            << std::get<1>(min_version) << "."
            << std::get<2>(min_version)  << endl;
        QTextStream(stdout)
            << "Check https://github.com/gcp/leela-zero for updates." << endl;
        exit(EXIT_FAILURE);
    }
    if (!eatNewLine()) {
        error(Game::WRONG_GTP);
        exit(EXIT_FAILURE);
    }
}

bool Game::gameStart(const VersionTuple &min_version) {
    start(m_cmdLine);
    if (!waitForStarted()) {
        error(Game::NO_LEELAZ);
        return false;
    }
    // This either succeeds or we exit immediately, so no need to
    // check any return values.
    checkVersion(min_version);
    QTextStream(stdout) << "Engine has started." << endl;
    sendGtpCommand(m_timeSettings);
    QString files[] = {
        "trains/e27e8b9722714f98868c3f43f22090f1.txt.0.gz",
        "trains/e93619065b3c4918b251520ee95857b3.txt.0.gz",
        "trains/a13fa9be0951447e9005aec93084b0f3.txt.0.gz",
        "trains/3cb8d206f7c74fcfb504909df34376c5.txt.0.gz",
        "trains/b0f6cf0224dc475693b358f86f2a0fca.txt.0.gz",
        "trains/0ae171df81ef4dfd93ddc712a4e42de9.txt.0.gz",
        "trains/671d23c89c7b44609e91092f040baac0.txt.0.gz",
        "trains/eb5f28d466dd471cbf76e26b0d41cd0c.txt.0.gz",
        "trains/b7566489c46f438e8860cc6beb302ac9.txt.0.gz",
        "trains/6f37c4364dbf459fb8ab812b08c2a3ac.txt.0.gz",
        "trains/1fd8dcba952943f7951bd308b06d5018.txt.0.gz",
        "trains/3d85f06789f24cce887e6f2d2fbd0a42.txt.0.gz",
        "trains/02a0af6baced496696101fe2482cc0b3.txt.0.gz",
        "trains/fd1cfd83afcf4857a18d8afc947e4b21.txt.0.gz",
        "trains/e9cac84010f64634bce44c0389019cbc.txt.0.gz",
        "trains/3076c5bf6035427b857df098f81aca33.txt.0.gz",
        "trains/6b1b4681d4204bd6ac9963be1b263ec6.txt.0.gz",
        "trains/ab8a1debcd244b6d8a1b99b869bdefda.txt.0.gz",
        "trains/1aafc87a1c1e4d00839b8b44ee8f0975.txt.0.gz",
        "trains/d1d976d66128421e8955029cd6b15a99.txt.0.gz",

        "trains1/fcd5e3dd6c034803b0a39cf21457cf46.txt.0.gz",
        "trains1/cf3996fd53f449cbab534b5b9a54ee8a.txt.0.gz",
        "trains1/536ec473a97246e8a018059dcaecd8f8.txt.0.gz",
        "trains1/96f7825d60ff4919b4999f1e82b15ccc.txt.0.gz",
        "trains1/66fa05f0c1db483195bda5d688b9e221.txt.0.gz",
        "trains1/2b993880c59f499cbe802530e10ec19c.txt.0.gz",
        "trains1/bf5dc5efc9a34f41a63ec82ce29f1e44.txt.0.gz",
        "trains1/d6c9d4277be24f4e99107585f7be68b3.txt.0.gz",
        "trains1/bcb3be0e21dd4882b44b1ed934eb61e6.txt.0.gz",
        "trains1/0a71dc06d5fc4b5885c2f99df9e5b084.txt.0.gz",

        "trains2/c22104a6b5354aa5a39893976a31257b.txt.0.gz",
        "trains2/3b9e2622d6044be99d9c48d4630a0b8c.txt.0.gz",
        "trains2/d47a074aa1af43dca467626194699dbe.txt.0.gz",
        "trains2/cd8801df46014e41a366fc4300b424f3.txt.0.gz",
        "trains2/195d54a1e66f44beb893eaff78a7f11e.txt.0.gz",
        "trains2/a926c4dc61f5473a85efb4ee9162b1a8.txt.0.gz",
        "trains2/ab2e07dcb96141e980c9318760a06fc6.txt.0.gz",
        "trains2/b5d5aeef430f4a6295581c24dff2d1cb.txt.0.gz",
        "trains2/88532c1c4dee4c0aa1faf33d054d3597.txt.0.gz",
        "trains2/268b884bbecb4c0bb2613a316ca2b12a.txt.0.gz",

        "trains3/8f6442d16a464ae8b84f0d537a05890a.txt.0.gz",
        "trains3/75c68f6fd70c479c8e7ae6343aa41c17.txt.0.gz",
        "trains3/b9564bc49e3a40bc958730d580d901c4.txt.0.gz",
        "trains3/e0839a5825314932b89c5ad9a47e03af.txt.0.gz",
        "trains3/4027521b752544bb98e174b13d439ce9.txt.0.gz",
        "trains3/949ec52318d34e4b9137f44c5742f87e.txt.0.gz",
        "trains3/f771063cb0844d58aa62edf2592fb7d5.txt.0.gz",
        "trains3/523d2fb590f3476d8f23ec9c4df39849.txt.0.gz",
        "trains3/634e59c6b6e948a3bfa2aed7316fba6b.txt.0.gz",
        "trains3/dd02caeceebd4272a5f9b814f10dcbba.txt.0.gz",

        "trains4/0c84bd1e2a3a4081a1d9fa6b797e9c62.txt.0.gz",
        "trains4/835321a2640642539cc84792ca9d3415.txt.0.gz",
        "trains4/cc4eed5c9cc5468abf88a513a5136374.txt.0.gz",
        "trains4/9d27b40298144efeb743b72ace691abb.txt.0.gz",
        "trains4/3245e7710acd4e3cb09e05623fd88a67.txt.0.gz",
        "trains4/b6d006fbcacd48db93e895e5bb9f1e46.txt.0.gz",
        "trains4/8ca7a5e597684f8c975740d8ccd90ce1.txt.0.gz",
        "trains4/16c34e13226048459b8ba45e5c0ce548.txt.0.gz",
        "trains4/596d580f29884ec69fb3da531310a12d.txt.0.gz",
        "trains4/55bfc74b829d4fb39eca01feb03c2419.txt.0.gz",

        "trains5/e0bf2615342c4cc29f5101117616822c.txt.0.gz",
        "trains5/13f63266acbe400095fc9e5e10a44df1.txt.0.gz",
        "trains5/240c7677bc184202b9d50e2f2ccffdea.txt.0.gz",
        "trains5/814914d2fdd743d99d873e73c19be057.txt.0.gz",
        "trains5/4a63661022c74fe596c63f0296fb2338.txt.0.gz",
        "trains5/9af96bbb1f1c401bb6b7db837d6629a2.txt.0.gz",
        "trains5/ab252fbdce114094830a9acc1d71bd5b.txt.0.gz",
        "trains5/4f7ff9967c9e446ba407c0dc0de53599.txt.0.gz",
        "trains5/5199ba27101a442aa745cf32271c3933.txt.0.gz",
        "trains5/09c48fbb5d8b46ed9146961c37d8c55a.txt.0.gz",
        "trains5/ca384a6009a7496ebea49d15d92d4a33.txt.0.gz",
        "trains5/1616c442fb3643af9136984130a26f61.txt.0.gz",
        "trains5/4385ea4e5b714f86b3534453b6a88829.txt.0.gz",
        "trains5/37dc136c226841c5ac41c76228c31a6a.txt.0.gz",
        "trains5/f23c3a0f608644929fcb7e3199aa5535.txt.0.gz",
        "trains5/d0c82210b4d44cffb0750631c0891850.txt.0.gz",

        "trains6/bc8345e43ef3444782a8a161d6ce4e12.txt.0.gz",
        "trains6/875b268d02064b6597491f9c4542b2d1.txt.0.gz",
        "trains6/4476ab6af6694d209a849a7dffd8ee5d.txt.0.gz",
        "trains6/945e0329bf764605b8017c9f850cf28e.txt.0.gz",
        "trains6/bff5c0a37fd64f2d8497fab5186b1ac3.txt.0.gz",
        "trains6/84072ea7c3b543019b3a0338a89bcda2.txt.0.gz",
        "trains6/5284eaefcbfa455982f89e1cc456076a.txt.0.gz",

        "trains7/5c982d16da3d4e4ca38c11809d764670.txt.0.gz",
        "trains7/96fbd58d792244d393728bda50fa338e.txt.0.gz",
        "trains7/82b625724882438b8faebc64a2e11ac3.txt.0.gz",
        "trains7/46e9e7b904c24cdeb0eb5603cc1b0d95.txt.0.gz",
        "trains7/6172e132428649f3a8be63bebd3b8b83.txt.0.gz",
        "trains7/8f60ac403b0b429093257b9b552000e7.txt.0.gz",
        "trains7/4b3cc81a989d408d9d11359d706becb7.txt.0.gz",
        "trains7/f50339fb04eb4f0ea61aba19469768e1.txt.0.gz",
        "trains7/dc173dea074241a486dd5cf25aa826a9.txt.0.gz",
        "trains7/daec2eb3e03b4f69bf14a4c215bedffb.txt.0.gz",
        "trains7/218e5a31b95d4328a353ed8c8afff24a.txt.0.gz",

        "trains8/67e4fb67ed7d40dba9ba438e0390fd30.txt.0.gz",
        "trains8/ce3187a1331d4efa8d1b4a672a2662b8.txt.0.gz",
        "trains8/8c89e89d6a2b4643aba7321d65cb2de8.txt.0.gz",
        "trains8/94e3c2d1ce8d46419e67fa44303e6116.txt.0.gz",
        "trains8/3046bf0751584aecbcf37b4920418ea1.txt.0.gz",
        "trains8/4db8b6ad98894d4699637f2736cb4f94.txt.0.gz",
        "trains8/35ef64399dbe4aeab806ec952e0a7446.txt.0.gz",
        "trains8/6980d66847b04bf5aed8fcafdf9098a4.txt.0.gz",
        "trains8/06fc184e5a6546f29e5a5cd2123d860b.txt.0.gz",
        "trains8/bbfce7f11b6f4467bc20a49d3b8c2078.txt.0.gz",
        "trains8/e6157fbd9ce24945adbb256b4a25bbb8.txt.0.gz",
        "trains8/00025ccff63c4b149b797058f7d6ac7d.txt.0.gz",

        "trains9/12f7c42a9ce04ed39df8d1e7e9034f25.txt.0.gz",
        "trains9/79b510cff96c4809a5b142130a54f172.txt.0.gz",
        "trains9/17f7601457ca49ddbdd521f2f33b49cb.txt.0.gz",
        "trains9/9cc12cbcead440868cbdfb349469acba.txt.0.gz",
        "trains9/333fc50d1b0c4ba6acdd820a4b7b898f.txt.0.gz",
        "trains9/ae0c7ab404214fd58b1f995f3ee3c523.txt.0.gz",
        "trains9/7baba74dba774803bf1482061da89a89.txt.0.gz",
        "trains9/55be7f2206f84b819791d9b5f81a2234.txt.0.gz",
        "trains9/d29fe614ec2346199d30290ec1cb8816.txt.0.gz",
        "trains9/a9c6d57f14f141ee93c5016d8e5460ad.txt.0.gz",
        "trains9/469a8a5257334328b24b18caa2ff65a8.txt.0.gz",
        "trains9/43a02e6b69be406d948164a61917415e.txt.0.gz",
        "trains9/6935c7f4668f42f090dd3eeb96b0bd06.txt.0.gz",
        "trains9/a10f2f597785454a8d008bacc94b0eca.txt.0.gz",
        "trains9/0319bc91b8ca46158461b48c2d3c051f.txt.0.gz",
        "trains9/1281aa51317149ec83feb5694196aa69.txt.0.gz",
        "trains9/ec60a173d08849699418863efaba3ba9.txt.0.gz",
        "trains9/d5b08657440d4d939e8e2289460ca6ed.txt.0.gz",
        "trains9/5ea2f13eabc249269eb024292949b786.txt.0.gz",
        "trains9/31f45a39696c4bdba64cac068ebba25d.txt.0.gz",
        "trains9/812d96a70fd84e7aae8d73a9a12207e2.txt.0.gz",
        "trains9/71bea893ff5646369aab3560bd494bf5.txt.0.gz",
        "trains9/1f57fa007074472e815117027d6e2654.txt.0.gz",
        "trains9/4e427e5782d54982866c4f92c1988971.txt.0.gz",
        "trains9/2ac34c4eadf649ae84156868c7c2adeb.txt.0.gz",
        "trains9/9eef86593c7044f79b5cc9c779a1ddef.txt.0.gz",
        "trains9/83fdf3972ac24ae6b8c86076366a0b3b.txt.0.gz",
        "trains9/a430e6b3bc79478dadd9e2cf4544cfd7.txt.0.gz",
        "trains9/00ffc75c7e654e1cb2228ccbc04b5427.txt.0.gz",
        "trains9/6d4704fde552442790d9bf78f586b125.txt.0.gz",
        "trains9/90642ccd9a304ab18d2074462f2f64f6.txt.0.gz",
        "trains9/294dc87e17cd49609dd9f2d27572b8a5.txt.0.gz",
        "trains9/b5a0dc9c4ad8431b8ca7d7c79e04e603.txt.0.gz",
        "trains9/15c6876d01f34e67a41c320ca961dc4a.txt.0.gz",
        "trains9/389fd761ee5c410e9c7a9428ca944ec8.txt.0.gz",
        "trains9/ad553791c7de4714b2e708c56a4fa960.txt.0.gz",
        "trains9/2ef26ce5a34c4b1facb3e41ebf436784.txt.0.gz",
        "trains9/8088ca5b14f54e169f537914a969ada8.txt.0.gz",
        "trains9/0724c46696954c789cdcc534202203e8.txt.0.gz",
        "trains9/fe36a59becdb4c1fb7e2508543df376c.txt.0.gz",
        "trains9/4e454b41689a4b4c9698d29f5e2ec111.txt.0.gz",
        "trains9/dc811db01c684dc591b824ad03060b4a.txt.0.gz",
        "trains9/15949d9268274d4fb2845785f149d486.txt.0.gz",
        "trains9/8ba0c62a8ab94a18a8f102ba4a0e20b9.txt.0.gz",
        "trains9/08c7e4df00b642498420030ead7f638b.txt.0.gz",
        "trains9/b8ae02c79d0046fcb2187055acfbf930.txt.0.gz",
        "trains9/6b70de58810840c88799b564a5038c2e.txt.0.gz",
        "trains9/b9a3015696cc4cbeb53b9b5b2de54c67.txt.0.gz",
        "trains9/2efb5618ef3a4fb9834cdd0ae5655975.txt.0.gz",
        "trains9/f27ad1072ffd4dd1ba68e4d9bda83a46.txt.0.gz",
        "trains9/8089f31de82b43ebae6df484c6e4cb02.txt.0.gz",
        "trains9/e474d130cef84623ba577fd0fcc6116b.txt.0.gz",
        "trains9/4ed91771dcfc466a9e50f2e90da87987.txt.0.gz",
        "trains9/5b0b478105a54e8cb50e6ef3c901e1e0.txt.0.gz",
        "trains9/5e658f10ff3d4926bab936379a024cab.txt.0.gz",
        "trains9/0cc4104121cb479aa8f9cb3371916a36.txt.0.gz",
    };
    int len = sizeof(files) / sizeof(files[0]);
    for (int tmpi = 0; tmpi < len; tmpi++) {
        sendGtpCommand(qPrintable("load_training " + files[tmpi]));
        QTextStream(stdout) << "load_training: " << files[tmpi] << endl;
    }
    QTextStream(stdout) << "Infinite thinking time set." << endl;
    return true;
}

void Game::move() {
    m_moveNum++;
    QString moveCmd;
    if (m_blackToMove) {
        moveCmd = "genmove b\n";
    } else {
        moveCmd = "genmove w\n";
    }
    write(qPrintable(moveCmd));
    waitForBytesWritten(-1);
}

void Game::setMovesCount(int moves) {
    m_moveNum = moves;
    m_blackToMove = (moves % 2) == 0;
}

bool Game::waitReady() {
    while (!canReadLine() && state() == QProcess::Running) {
        waitForReadyRead(-1);
    }
    // somebody crashed
    if (state() != QProcess::Running) {
        return false;
    }
    return true;
}

bool Game::readMove() {
    char readBuffer[256];
    int readCount = readLine(readBuffer, 256);
    if (readCount <= 3 || readBuffer[0] != '=') {
        error(Game::WRONG_GTP);
        QTextStream(stdout) << "Error read " << readCount << " '";
        QTextStream(stdout) << readBuffer << "'" << endl;
        terminate();
        return false;
    }
    // Skip "= "
    m_moveDone = readBuffer;
    m_moveDone.remove(0, 2);
    m_moveDone = m_moveDone.simplified();
    if (!eatNewLine()) {
        error(Game::PROCESS_DIED);
        return false;
    }
    if(readCount == 0) {
        error(Game::WRONG_GTP);
    }
    QTextStream(stdout) << m_moveNum << " (";
    QTextStream(stdout) << (m_blackToMove ? "B " : "W ") << m_moveDone << ") ";
    QTextStream(stdout).flush();
    if (m_moveDone.compare(QStringLiteral("pass"),
                          Qt::CaseInsensitive) == 0) {
        m_passes++;
    } else if (m_moveDone.compare(QStringLiteral("resign"),
                                 Qt::CaseInsensitive) == 0) {
        m_resignation = true;
        m_blackResigned = m_blackToMove;
    } else {
        m_passes = 0;
    }
    return true;
}

bool Game::setMove(const QString& m) {
    if (!sendGtpCommand(m)) {
        return false;
    }
    m_moveNum++;
    QStringList moves = m.split(" ");
    if (moves.at(2)
        .compare(QStringLiteral("pass"), Qt::CaseInsensitive) == 0) {
        m_passes++;
    } else if (moves.at(2)
               .compare(QStringLiteral("resign"), Qt::CaseInsensitive) == 0) {
        m_resignation = true;
        m_blackResigned = (moves.at(1).compare(QStringLiteral("black"), Qt::CaseInsensitive) == 0);
    } else {
        m_passes = 0;
    }
    m_blackToMove = !m_blackToMove;
    return true;
}

bool Game::nextMove() {
    if(checkGameEnd()) {
        return false;
    }
    m_blackToMove = !m_blackToMove;
    return true;
}

bool Game::getScore() {
    if(m_resignation) {
        if (m_blackResigned) {
            m_winner = QString(QStringLiteral("white"));
            m_result = "W+Resign ";
            QTextStream(stdout) << "Score: " << m_result << endl;
        } else {
            m_winner = QString(QStringLiteral("black"));
            m_result = "B+Resign ";
            QTextStream(stdout) << "Score: " << m_result << endl;
        }
    } else{
        write("final_score\n");
        waitForBytesWritten(-1);
        if (!waitReady()) {
            error(Game::PROCESS_DIED);
            return false;
        }
        char readBuffer[256];
        readLine(readBuffer, 256);
        m_result = readBuffer;
        m_result.remove(0, 2);
        if (readBuffer[2] == 'W') {
            m_winner = QString(QStringLiteral("white"));
        } else if (readBuffer[2] == 'B') {
            m_winner = QString(QStringLiteral("black"));
        }
        if (!eatNewLine()) {
            error(Game::PROCESS_DIED);
            return false;
        }
        QTextStream(stdout) << "Score: " << m_result;
    }
    if (m_winner.isNull()) {
        QTextStream(stdout) << "No winner found" << endl;
        return false;
    }
    QTextStream(stdout) << "Winner: " << m_winner << endl;
    return true;
}

int Game::getWinner() {
    if(m_winner.compare(QStringLiteral("white"), Qt::CaseInsensitive) == 0)
        return Game::WHITE;
    else
        return Game::BLACK;
}

bool Game::writeSgf() {
    return sendGtpCommand(qPrintable("printsgf " + m_fileName + ".sgf"));
}

bool Game::loadTraining(const QString &fileName) {
    QTextStream(stdout) << "Loading " << fileName + ".train" << endl;
    return sendGtpCommand(qPrintable("load_training " + fileName + ".train"));

}

bool Game::saveTraining() {
     QTextStream(stdout) << "Saving " << m_fileName + ".train" << endl;
     return sendGtpCommand(qPrintable("save_training " + m_fileName + ".train"));
}


bool Game::loadSgf(const QString &fileName) {
    QTextStream(stdout) << "Loading " << fileName + ".sgf" << endl;
    return sendGtpCommand(qPrintable("loadsgf " + fileName + ".sgf"));
}

bool Game::fixSgf(QString& weightFile, bool resignation) {
    QFile sgfFile(m_fileName + ".sgf");
    if (!sgfFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return false;
    }
    QString sgfData = sgfFile.readAll();
    QRegularExpression re("PW\\[Human\\]");
    QString playerName("PB[Leela Zero ");
    QRegularExpression le("PB\\[Leela Zero \\S+ ");
    QRegularExpressionMatch match = le.match(sgfData);
    if (match.hasMatch()) {
        playerName = match.captured(0);
    }
    playerName = "PW" + playerName.remove(0, 2);
    playerName += weightFile.left(8);
    playerName += "]";
    sgfData.replace(re, playerName);

    if(resignation) {
        QRegularExpression oldResult("RE\\[B\\+.*\\]");
        QString newResult("RE[B+Resign] ");
        sgfData.replace(oldResult, newResult);
        if(!sgfData.contains(newResult, Qt::CaseInsensitive)) {
            QRegularExpression oldwResult("RE\\[W\\+.*\\]");
            sgfData.replace(oldwResult, newResult);
        }
        QRegularExpression lastpass(";W\\[tt\\]\\)");
        QString noPass(")");
        sgfData.replace(lastpass, noPass);
    }

    sgfFile.close();
    if(sgfFile.open(QFile::WriteOnly | QFile::Truncate)) {
        QTextStream out(&sgfFile);
        out << sgfData;
    }
    sgfFile.close();

    return true;
}

bool Game::dumpTraining() {
    return sendGtpCommand(
        qPrintable("dump_training " + m_winner + " " + m_fileName + ".txt"));
}

bool Game::dumpDebug() {
    return sendGtpCommand(
        qPrintable("dump_debug " + m_fileName + ".debug.txt"));
}

void Game::gameQuit() {
    write(qPrintable("quit\n"));
    waitForFinished(-1);
}
