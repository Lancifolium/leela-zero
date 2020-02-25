/*
    This file is part of Leela Zero.
    Copyright (C) 2017-2018 Marco Calignano

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
#include <QFileInfo>
#if defined(LEELA_GTP)
#include <QDir>
#include "Management.h"
#endif
#include "Game.h"

Game::Game(const Engine& engine) :
    QProcess(),
    m_engine(engine),
    m_isHandicap(false),
    m_resignation(false),
#if defined(ANCIENT_CHINESE_RULE_ENABLED)
    m_blackToMove(false),
#else
    m_blackToMove(true),
#endif
    m_blackResigned(false),
    m_passes(0),
    m_moveNum(0)
{
    m_fileName = QUuid::createUuid().toRfc4122().toHex();
}

bool Game::checkGameEnd() {
    return (m_resignation ||
            m_passes > 1 ||
            m_moveNum > (19 * 19 * 2));
}

void Game::error(int errnum) {
    QTextStream(stdout) << "*ERROR*: ";
    switch (errnum) {
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
    if (readCount < 0) {
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
#if defined(LEELA_GTP)
        return;
#else
        exit(EXIT_FAILURE);
#endif
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
#if defined(LEELA_GTP)
            return;
#else
            exit(EXIT_FAILURE);
#endif
        }
        readCount = readLine(readBuffer, 256);
    }
    // We expect to read at last "=, space, something"
    if (readCount <= 3 || readBuffer[0] != '=') {
        QTextStream(stdout) << "GTP: " << readBuffer << endl;
        error(Game::WRONG_GTP);
#if defined(LEELA_GTP)
        return;
#else
        exit(EXIT_FAILURE);
#endif
    }
    QString version_buff(&readBuffer[2]);
    version_buff = version_buff.simplified();
    QStringList version_list = version_buff.split(".");
    if (version_list.size() < 2) {
        QTextStream(stdout)
            << "Unexpected Leela Zero version: " << version_buff << endl;
#if defined(LEELA_GTP)
        return;
#else
        exit(EXIT_FAILURE);
#endif
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
#if defined(LEELA_GTP)
        return;
#else
        exit(EXIT_FAILURE);
#endif
    }
    if (!eatNewLine()) {
        error(Game::WRONG_GTP);
#if defined(LEELA_GTP)
        return;
#else
        exit(EXIT_FAILURE);
#endif
    }
}

bool Game::gameStart(const VersionTuple &min_version,
#if defined(LEELA_GTP)
                     Management *boss,
#endif
                     const QString &sgf,
                     const int moves) {
    start(m_engine.getCmdLine());
    if (!waitForStarted()) {
        error(Game::NO_LEELAZ);
        return false;
    }
    // This either succeeds or we exit immediately, so no need to
    // check any return values.
    checkVersion(min_version);
    QTextStream(stdout) << "Engine has started." << endl;
#if defined(LEELA_GTP)
    if (boss->gtp_config()->load_training_data) {
        QDir dir(boss->gtp_config()->training_data_path);
        QStringList trainfilter;
        trainfilter << "*.0.gz" << "*.train";
        QStringList trainfiles = dir.entryList(trainfilter, QDir::Files | QDir::Readable, QDir::Name);
        for (int tmpi = 0; tmpi < trainfiles.size(); tmpi++) {
            QString load_training("load_training " +
                                  boss->gtp_config()->training_data_path +
                                  trainfiles[tmpi]);
            sendGtpCommand(qPrintable(load_training));
        }
        QTextStream(stdout) << "load " << trainfiles.size() << " training files\n";
    }
#endif
    //If there is an sgf file to start playing from then it will contain
    //whether there is handicap in use. If there is no sgf file then instead,
    //check whether there are any handicap commands to send (these fail
    //if the board is not empty).
    //Then send the rest of the GTP commands after any SGF has been loaded so
    //that they can override any settings loaded from the SGF.
    if (!sgf.isEmpty()) {
        QFile sgfFile(sgf + ".sgf");
        if (!sgfFile.exists()) {
            QTextStream(stdout) << "Cannot find sgf file " << sgf << endl;
#if defined(LEELA_GTP)
            return false;
#else
            exit(EXIT_FAILURE);
#endif
        }
        sgfFile.open(QIODevice::Text | QIODevice::ReadOnly);
        const auto sgfData = QTextStream(&sgfFile).readAll();
        const auto re = QRegularExpression("HA\\[\\d+\\]");
        const auto match = re.match(sgfData);
        m_isHandicap = match.hasMatch();
        sgfFile.close();
        if (moves == 0) {
            loadSgf(sgf);
        } else {
            loadSgf(sgf, moves);
        }
        setMovesCount(moves);
    } else {
        for (auto command : m_engine.m_commands.filter("handicap")) {
            QTextStream(stdout) << command << endl;
            if (!sendGtpCommand(command))
            {
                QTextStream(stdout) << "GTP failed on: " << command << endl;
#if defined(LEELA_GTP)
                return false;
#else
                exit(EXIT_FAILURE);
#endif
            }
            m_isHandicap = true;
            m_blackToMove = false;
        }
    }
    const auto re = QRegularExpression("^((?!handicap).)*$");
    for (auto command : m_engine.m_commands.filter(re)) {
        QTextStream(stdout) << command << endl;
        if (!sendGtpCommand(command))
        {
            QTextStream(stdout) << "GTP failed on: " << command << endl;
#if defined(LEELA_GTP)
            return false;
#else
            exit(EXIT_FAILURE);
#endif
        }
    }
    QTextStream(stdout) << "Starting GTP commands sent." << endl;
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
    //The game always starts at move 0 (GTP states that handicap stones are not part
    //of the move history), so if there is no handicap then black moves on even
    //numbered turns but if there is handicap then black moves on odd numbered turns.
    m_blackToMove = (moves % 2) == (m_isHandicap ? 1 : 0);
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

#if defined(LEELA_GTP)
int Game::readMove() {
#else
bool Game::readMove() {
#endif
    char readBuffer[256];
    int readCount = readLine(readBuffer, 256);
    if (readCount <= 3 || readBuffer[0] != '=') {
        error(Game::WRONG_GTP);
        QTextStream(stdout) << "Error read " << readCount << " '";
        QTextStream(stdout) << readBuffer << "'" << endl;
        terminate();
#if defined(LEELA_GTP)
        return 0;
#else
        return false;
#endif
    }
    // Skip "= "
    m_moveDone = readBuffer;
    m_moveDone.remove(0, 2);
    m_moveDone = m_moveDone.simplified();
    if (!eatNewLine()) {
        error(Game::PROCESS_DIED);
#if defined(LEELA_GTP)
        return 0;
#else
        return false;
#endif
    }
    if (readCount == 0) {
        error(Game::WRONG_GTP);
    }
    QTextStream(stdout) << m_moveNum << " (";
    QTextStream(stdout) << (m_blackToMove ? "B " : "W ") << m_moveDone << ") ";
    QTextStream(stdout).flush();
#if defined(LEELA_GTP)
    int ret = 0;
#endif
    if (m_moveDone.compare(QStringLiteral("pass"),
                          Qt::CaseInsensitive) == 0) {
        m_passes++;
#if defined(LEELA_GTP)
        ret = 0x200000; // means pass
        if (m_blackToMove)
            ret += 0x10000;
        else
            ret += 0x20000;
#endif
    } else if (m_moveDone.compare(QStringLiteral("resign"),
                                 Qt::CaseInsensitive) == 0) {
        m_resignation = true;
        m_blackResigned = m_blackToMove;
#if defined(LEELA_GTP)
        ret = 0x300000;
        if (m_blackResigned)
            ret += 0x10000;
        else
            ret += 0x20000;
#endif
    } else {
        m_passes = 0;
#if defined(LEELA_GTP)
        if (m_blackToMove)
            ret = 0x10000;
        else
            ret = 0x20000;
        ret += (m_moveDone.toLatin1().data()[0] - 'A') << 8;
        if (m_moveDone.toLatin1().data()[0] > 'I')
            ret -= 0x100;
        if (m_moveDone.length() == 3)
            ret += (m_moveDone.toLatin1().data()[1] - '0') * 10
                    + m_moveDone.toLatin1().data()[2] - '1';
        else
            ret += m_moveDone.toLatin1().data()[1] - '1';
#endif
    }
#if defined(LEELA_GTP)
    return ret;
#else
    return true;
#endif
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
    if (checkGameEnd()) {
        return false;
    }
    m_blackToMove = !m_blackToMove;
    return true;
}

bool Game::getScore() {
    if (m_resignation) {
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
    if (m_winner.compare(QStringLiteral("white"), Qt::CaseInsensitive) == 0)
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

#if defined(LEELA_GTP)
bool Game::dumpSupervised(const QString &sgf, const QString &train) {
    QTextStream(stdout) << "Dumping supervised " << sgf + " as " << train << endl;
    return sendGtpCommand(qPrintable("dump_supervised " + sgf + " " + train));
}
#endif


bool Game::loadSgf(const QString &fileName) {
    QTextStream(stdout) << "Loading " << fileName + ".sgf" << endl;
    return sendGtpCommand(qPrintable("loadsgf " + fileName + ".sgf"));
}

bool Game::loadSgf(const QString &fileName, const int moves) {
    QTextStream(stdout) << "Loading " << fileName + ".sgf with " << moves << " moves" << endl;
    return sendGtpCommand(qPrintable("loadsgf " + fileName + ".sgf " + QString::number(moves + 1)));
}

void Game::fixSgfPlayer(QString& sgfData, const Engine& whiteEngine) {
    QRegularExpression oldPlayer("PW\\[Human\\]");
    QString playerName("PB[Leela Zero ");
    QRegularExpression le("PB\\[Leela Zero \\S+ ");
    QRegularExpressionMatch match = le.match(sgfData);
    if (match.hasMatch()) {
        playerName = match.captured(0);
    }
    playerName = "PW" + playerName.remove(0, 2);
    playerName += whiteEngine.getNetworkFile().left(8);
    playerName += "]";
    sgfData.replace(oldPlayer, playerName);
}

void Game::fixSgfComment(QString& sgfData, const Engine& whiteEngine,
    const bool isSelfPlay) {
    QRegularExpression oldComment("(C\\[Leela Zero)( options:.*)\\]");
    QString comment("\\1");
    if (!isSelfPlay) {
        comment += " Black";
    }
    comment += "\\2 Starting GTP commands:";
    for (const auto command : m_engine.m_commands) {
        comment += " " + command;
    }
    if (!isSelfPlay) {
        comment += " White options:";
        comment += whiteEngine.m_options + " " + whiteEngine.m_network;
        comment += " Starting GTP commands:";
        for (const auto command : whiteEngine.m_commands) {
            comment += " " + command;
        }
    }
    comment += "]";
    comment.replace(QRegularExpression("\\s\\s+"), " ");
    sgfData.replace(oldComment, comment);
}

void Game::fixSgfResult(QString& sgfData, const bool resignation) {
    if (resignation) {
        QRegularExpression oldResult("RE\\[B\\+.*\\]");
        QString newResult("RE[B+Resign] ");
        sgfData.replace(oldResult, newResult);
        if (!sgfData.contains(newResult, Qt::CaseInsensitive)) {
            QRegularExpression oldwResult("RE\\[W\\+.*\\]");
            sgfData.replace(oldwResult, newResult);
        }
        QRegularExpression lastpass(";W\\[tt\\]\\)");
        QString noPass(")");
        sgfData.replace(lastpass, noPass);
    }
}

bool Game::fixSgf(const Engine& whiteEngine, const bool resignation,
    const bool isSelfPlay) {
    QFile sgfFile(m_fileName + ".sgf");
    if (!sgfFile.open(QIODevice::Text | QIODevice::ReadOnly)) {
        return false;
    }
    QString sgfData = sgfFile.readAll();
    fixSgfPlayer(sgfData, whiteEngine);
    fixSgfComment(sgfData, whiteEngine, isSelfPlay);
    fixSgfResult(sgfData, resignation);
    sgfFile.close();
    if (sgfFile.open(QFile::WriteOnly | QFile::Truncate)) {
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
