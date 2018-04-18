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
        "trains14/738ebc0910db4e6da830f8dac6acc865.txt.0.gz",
        "trains14/99aad1871d5348f689e3149d724b9368.txt.0.gz",
        "trains14/5f384abb6b5545158196fbd1e384c0c6.txt.0.gz",
        "trains14/ffaf250b7f844f1e95f65165a939b967.txt.0.gz",
        "trains14/152a4178dd4441db8eb04bd95c06efa5.txt.0.gz",
        "trains14/c9f5230bd06040d68b8555134adf96bc.txt.0.gz",
        "trains14/5f17371576624fff854025dddec1f267.txt.0.gz",
        "trains14/8845ee18734841d9b55a43426d9fd27f.txt.0.gz",
        "trains14/d6cfa2d3e8e942aaa9531fefe3864fea.txt.0.gz",
        "trains14/c8d4e459d9ff4dad904be8952baae227.txt.0.gz",
        "trains14/260c62417ac640ed84ad81e527259a6c.txt.0.gz",
        "trains14/9f28108de0f34fe0be5f953aed48d20e.txt.0.gz",
        "trains14/b60276bc13e64ecb9bea326e6f16bee4.txt.0.gz",
        "trains14/b609609af9ce4b0b831f1679cbc489d6.txt.0.gz",
        "trains14/b7166f96858b4e8486203ddfe0c265f8.txt.0.gz",
        "trains14/86c3583c81af408d8640d1786820f966.txt.0.gz",
        "trains14/4dc56e2c3f814ad4b76792a29eff13f9.txt.0.gz",
        "trains14/49102b1f08ce4f429e803293db432eb3.txt.0.gz",
        "trains14/fdfcabb666a747959c4cc5ca7f5efe3c.txt.0.gz",
        "trains14/10bd6c73691a4a88a8f2f0e29c96c60c.txt.0.gz",
        "trains14/108847e2f6e84e25b023dd3c08f2db76.txt.0.gz",
        "trains14/d6b297115c1043f4bb7ae97ee175b27f.txt.0.gz",
        "trains14/0eb58817950049358f4ee3054b4f2513.txt.0.gz",
        "trains14/441136c4d1914a5480ec146afb7bd988.txt.0.gz",
        "trains14/fd1fdb1466e04388865e7ee12a04b8e9.txt.0.gz",
        "trains14/b0b199079a2d4c35a01f356451119bb7.txt.0.gz",
        "trains14/bd28613fc6c648cebce0fce0de2152ae.txt.0.gz",
        "trains14/7e403dee43a343dfb8abf939635e2234.txt.0.gz",
        "trains14/c131a3aab44e4ce39e81a9bb158a13cd.txt.0.gz",
        "trains14/557acb7591204ee5a1695dcc055a81eb.txt.0.gz",
        "trains14/a211d4bef0b3494ca84ff36e18f8c8b9.txt.0.gz",
        "trains14/e8ef1fdb919a4ede8eecb2c8dde2eb02.txt.0.gz",
        "trains14/a81c376e500b4e58b2dff360619377b1.txt.0.gz",
        "trains14/795d212bdeda4cab8deedcfa757456c8.txt.0.gz",
        "trains14/008f83fc142543c1ac87ac48fd76e646.txt.0.gz",
        "trains14/6282d48193d24fdfbeda0e65a3aab695.txt.0.gz",
        "trains14/0f114de1182b4656a57cfe84e868dd92.txt.0.gz",
        "trains14/137527bfca30440f937aa74f41cc0fda.txt.0.gz",
        "trains14/a57ba69cfa3c45d9b8d4270a55dd9232.txt.0.gz",
        "trains14/1d53325813074246a13b94cb8308b8cf.txt.0.gz",
        "trains14/968bddb003ef40d89db9fd431f2bc58c.txt.0.gz",
        "trains14/5b372a25c757470bab7872d98d1a56b7.txt.0.gz",
        "trains14/a12a08087c4b4b89b81b24eb57fccb0e.txt.0.gz",
        "trains14/94c512b8279d47e787ccddaea733a370.txt.0.gz",
        "trains14/8d0868ec0285415796f7ecf9a1a65846.txt.0.gz",
        "trains14/e069286a23224ade86d042d8e13f9273.txt.0.gz",
        "trains14/5ed898da6783499ea694026564344cd4.txt.0.gz",
        "trains14/94f2abb371ae47aa9cb7f6fe03114af9.txt.0.gz",
        "trains14/f6b9673d1d1c41ef9453d1c7a1711867.txt.0.gz",
        "trains14/2a8ad1b6e176448795c0eeeff7d3842f.txt.0.gz",
        "trains14/2d895b7e56b14b97ae53f39668af2bb6.txt.0.gz",
        "trains14/ff58dfa237bc41b0ac7191c84393d17f.txt.0.gz",
        "trains14/c57216dcc24d4e1891c987953425b9f2.txt.0.gz",
        "trains14/4bc6cff0f7a048b8898e45919e154201.txt.0.gz",
        "trains14/a5a699573fe142ceac06a5b5651a1732.txt.0.gz",
        "trains14/358afa8149e64a308d8ac03fdc98f3e4.txt.0.gz",
        "trains14/383456e597f4400caa3be6309fbc864f.txt.0.gz",
        "trains14/74bacd3073d14367b5fe7cccc6b34eae.txt.0.gz",
        "trains14/af6e6374b1764e1abdfc0a86b96cab54.txt.0.gz",
        "trains14/115b6ed9045b4fb9bf9c2b65e13aeee4.txt.0.gz",
        "trains14/c2ba3053c48e4fe08da924545e867f1d.txt.0.gz",
        "trains14/a2f71c2ae67e4822b47eb1cea40bc564.txt.0.gz",
        "trains14/fa2c5abea1004ea89f8a84863b74ff29.txt.0.gz",
        "trains14/d99b169f76be43308ecc82a5a49e4e26.txt.0.gz",
        "trains14/605aaa1186714bd1aa699a6f2a100d47.txt.0.gz",
        "trains14/3267275e2610447293192852dc1a2309.txt.0.gz",
        "trains14/84ba4a92030f423580f5ad5058389845.txt.0.gz",
        "trains14/45c79c4e665441e3a01c611db7830c58.txt.0.gz",
        "trains14/ae1a0247dbd745afa96fc6d30ee877c6.txt.0.gz",
        "trains14/89a96b47e80d4317a8fdf9ccf11ff3c6.txt.0.gz",
        "trains14/a811019a966b4c6e98f7f368798777d4.txt.0.gz",
        "trains14/db7f850c1d9d453394bc966432df6193.txt.0.gz",
        "trains14/3380a81dc30f400e89c7cb60892a5631.txt.0.gz",
        "trains14/fb8afa9e013d40d7aa73be22a8cecf93.txt.0.gz",
        "trains14/cd7c05af6a354c8b9123a441afee7f03.txt.0.gz",
        "trains14/8092d3d10fd2411d8e7ee48f097bc3f5.txt.0.gz",
        "trains14/fadcf66b021c4dc495338c4e57d6e9e3.txt.0.gz",
        "trains14/ba1380c2dfc046f7a58418ea0f3d4b8a.txt.0.gz",
        "trains14/387a5254a70b4acdafdca56640a8d47f.txt.0.gz",
        "trains14/54c5512c0b9041619929c0d13d14abeb.txt.0.gz",
        "trains14/5cde3762d60445419aff0b4c031c717c.txt.0.gz",
        "trains14/e4633b00b8954773bf669ff896789cca.txt.0.gz",
        "trains14/d6ae5ff8d3094f21a9757ea9fc21de0b.txt.0.gz",
        "trains14/e06809681d644436aed918315f511123.txt.0.gz",
        "trains14/90bd43c368184e6b93299ce8821ddb2a.txt.0.gz",
        "trains14/06157066b95d4bcc87f0cf6f2d485c23.txt.0.gz",
        "trains14/7d429a9ecc254fa8ac77053c50af1424.txt.0.gz",
        "trains14/e8ae4d6bbd464f10bac02f5a4563237f.txt.0.gz",
        "trains14/c98b824f9956482abf5c1e91fa06c167.txt.0.gz",
        "trains14/180d11d3e1bc405f90f851978dd1dd11.txt.0.gz",
        "trains14/4c99294a153e447ab76eb9b4f826402c.txt.0.gz",
        "trains14/71748840818748dca63c48bcc426c6f5.txt.0.gz",
        "trains14/9b53d57781bf4869b41f8e7465f4d9df.txt.0.gz",
        "trains14/7ca1b17dd7c54b718ab74d9e1125f59f.txt.0.gz",
        "trains14/3133c36261bc4d01b973842c6b70c4f1.txt.0.gz",
        "trains14/9752c9e89d3149a6ba686384265386e3.txt.0.gz",
        "trains14/94e8d3cffe704c9a9787df152cda9205.txt.0.gz",
        "trains14/c9d15bad6009438bb37b367bda74a216.txt.0.gz",
        "trains14/54135cb74a6b454bbecfbc5abd2f58fe.txt.0.gz",
        "trains14/bc573b1dded74a8baf4e6979d487620f.txt.0.gz",
        "trains14/ac847a8b6b084d4782cf2e302b8747d6.txt.0.gz",
        "trains14/272703f53c104dceaf3135b980fb8012.txt.0.gz",
        "trains14/342d46af65bf44bbb2f1e1680e248c23.txt.0.gz",
        "trains14/36ff62f60dcd4daeb40cd827807073e9.txt.0.gz",
        "trains14/1662d94f24404bd5ad6ec942d501eb70.txt.0.gz",
        "trains14/d71092228780439b96542bf3331a3459.txt.0.gz",
        "trains14/b2e33179004a4464b1333f0e1b60830a.txt.0.gz",
        "trains14/fd04d4d041554226969df19e7c62be68.txt.0.gz",
        "trains14/27f914a462ff465bab94db61427708b4.txt.0.gz",
        "trains14/aa2382184e9c4adc8a8296c0cd6f065f.txt.0.gz",
        "trains14/18d9bcbbfe1b4126a8f1afa34b514059.txt.0.gz",
        "trains14/3f5565e6ec1048ca9f3e252d22bcb470.txt.0.gz",
        "trains14/c1c3bb0064e84499a2a9fdf215651a60.txt.0.gz",
        "trains14/f8945e74242747db95c82aa2e2183d5e.txt.0.gz",
        "trains15/afe0bfe090284ea1be86f716aa98b732.txt.0.gz",
        "trains15/a1f9228b55184ccfbe4e2640cb71432d.txt.0.gz",
        "trains15/61f7b9674b7049598dd0c8f467f38660.txt.0.gz",
        "trains15/34695737aeeb4022941809b37c38ade2.txt.0.gz",
        "trains15/c98c6a7fdb0b4bfcb87b78a3c2da96ed.txt.0.gz",
        "trains15/013453f0258e42d9abcf93012d5b721d.txt.0.gz",
        "trains15/e47b97f5dad44c388da7cac2e3728d71.txt.0.gz",
        "trains15/39fb6411b1334bbe83336227cb33f370.txt.0.gz",
        "trains15/5e957a8723aa4cc8a440b23608975ac4.txt.0.gz",
        "trains15/af5ed66f86ce43198b582f53865dbf2e.txt.0.gz",
        "trains15/3b25970619244c478e31a1dafacaa152.txt.0.gz",
        "trains15/429906197999413a89331cc43b31c4ad.txt.0.gz",
        "trains15/b32ab0b361ec48c694e14c38992e4b34.txt.0.gz",
        "trains15/83521d1c09b0474d89f51130ac9af421.txt.0.gz",
        "trains15/4b68ffcf140e41c6b84a0e2675e2ed5d.txt.0.gz",
        "trains15/a56fe278b58f48e69f5f4de804ba6ab7.txt.0.gz",
        "trains15/cab9436898684b3aa558ae542e53ca16.txt.0.gz",
        "trains15/8e04eae210264876b0d41df5bbb1eee8.txt.0.gz",
        "trains15/a7fc70be700949f3a3859eb45a8e19c5.txt.0.gz",
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
