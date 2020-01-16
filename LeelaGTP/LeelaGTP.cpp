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

#include "LeelaGTP.h"
#include "Game.h"
#include "Translation.h"

#ifdef WIN32
#include <direct.h>
#endif
#include <QPainter>
#include <QTextStream>
#include <QtCore/QTimer>

LeelaGTP::LeelaGTP(QApplication *app, QWidget *parent) :
        QMainWindow(parent),
        app(app),
        config_dialog(this, &config),
        butt_keepSgf(this),
        butt_sgfpath(this),
        butt_netfile(this),
        show_netfile(this),
        label_timeout(this),
        butt_timeout(this),
        label_maxgames(this),
        butt_maxgames(this),
        label_gpugames(this),
        butt_gpugames(this),
        butt_configs(this),
        butt_run(this),
        show_board(this),
        show_stones(this),
        butt_translation(this),
        is_running(false),
        win_size(600), win_gap(30), win_xlb(3), win_ylb(3) {
    this->config_dialog.setModal(true);


    this->setWindowTitle(Trans("leelagtp_title"));
    this->setFixedSize(900, 720);

    show_board.installEventFilter(this);
    show_board.setGeometry(QRect(win_xlb + win_gap / 2, win_ylb + win_gap / 2,
                                 win_gap * 19.5, win_gap * 19.5));
    show_board.update();

    show_stones.installEventFilter(this);
    show_stones.setGeometry(QRect(win_xlb + win_gap / 2, win_ylb + win_gap / 2,
                                 win_gap * 19.5, win_gap * 19.5));
    show_stones.update();


    this->butt_run.setText(Trans("run"));
    this->butt_run.setToolTip(Trans("tip_run"));
    this->butt_run.setGeometry(660, 240, 84, 24);
    connect(&butt_run, SIGNAL(clicked(bool)), this, SLOT(on_runbutt()));

    this->label_timeout.setText(Trans("timeout"));
    this->label_timeout.setToolTip(Trans("tip_timeout"));
    this->label_timeout.setGeometry(660, 306, 120, 24);
    this->butt_timeout.setGeometry(660, 330, 84, 24);
    this->butt_timeout.setRange(0, 1000000);
    this->butt_timeout.setValue(config.run_timeout);
    this->butt_timeout.setSingleStep(10);
    this->butt_timeout.setSpecialValueText(Trans("unlimited"));
    connect(&butt_timeout, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.run_timeout = val; });

    this->label_maxgames.setText(Trans("max_games"));
    this->label_maxgames.setToolTip(Trans("tip_max_games"));
    this->label_maxgames.setGeometry(660, 366, 120, 24);
    this->butt_maxgames.setGeometry(660, 390, 84, 24);
    this->butt_maxgames.setRange(0, 1000000);
    this->butt_maxgames.setValue(config.run_maxgames);
    this->butt_maxgames.setSingleStep(10);
    this->butt_maxgames.setSpecialValueText(Trans("unlimited"));
    connect(&butt_maxgames, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.run_maxgames = val; });

    this->label_gpugames.setText(Trans("max_gpugames"));
    this->label_gpugames.setToolTip(Trans("tip_max_gpugames"));
    this->label_gpugames.setGeometry(660, 426, 150, 24);
    this->butt_gpugames.setGeometry(660, 450, 84, 24);
    this->butt_gpugames.setRange(1, 8);
    this->butt_gpugames.setValue(config.gpu_games);
    this->butt_gpugames.setSingleStep(1);
    //connect(butt_gamesNum, static_cast<void(QSpinBox::*)(int)>(&QSpinBox::valueChanged),
    //        [=] (int val) { int_gamesNum = val; });
    connect(&butt_gpugames, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.gpu_games = val; });

    this->butt_configs.setText(Trans("more_configs"));
    this->butt_configs.setToolTip(Trans("tip_more_configs"));
    this->butt_configs.setGeometry(660, 516, 84, 24);
    connect(&butt_configs, SIGNAL(clicked(bool)), this, SLOT(on_furconfigs()));


    this->butt_keepSgf.setText(Trans("keep_sgf"));
    this->butt_keepSgf.setGeometry(60, 606, 600, 24);
    this->butt_keepSgf.setChecked(config.keepSgf);
    connect(&butt_keepSgf, SIGNAL(toggled(bool)), this, SLOT(on_keepSgf()));
    this->butt_sgfpath.setText(Trans("open_filepath"));
    this->butt_sgfpath.setToolTip(Trans("tip_open_filepath"));
    this->butt_sgfpath.setFocus();
    this->butt_sgfpath.setGeometry(60, 630, 120, 24);
    connect(&butt_sgfpath, SIGNAL(clicked(bool)), this, SLOT(on_sgfpathbutt()));
    this->butt_sgfpath.setEnabled(config.keepSgf);
    this->show_sgfpath.setText(Trans("default_path") + config.sgf_path);
    this->show_sgfpath.setToolTip(Trans("default_path") + config.sgf_path);
    this->show_sgfpath.setGeometry(196, 630, 600, 24);
    this->show_sgfpath.setEnabled(config.keepSgf);

    this->butt_netfile.setText(Trans("net_file"));
    this->butt_netfile.setToolTip(Trans("tip_net_file"));
    this->butt_netfile.setGeometry(60, 660, 120, 24);
    connect(&butt_netfile, SIGNAL(clicked(bool)), this, SLOT(on_netfilebutt()));
    this->show_netfile.setGeometry(196, 660, 600, 24);
    this->show_netfile.setText(Trans("default_file") + config.net_filepath);
    this->show_netfile.setToolTip(Trans("default_file") + config.net_filepath);

    this->butt_translation.setGeometry(660, 33, 48, 24);
    this->butt_translation.addItem(QIcon(":/images/chn.png"), "", LeelaGTPLocale::Cn);
    this->butt_translation.addItem(QIcon(":/images/eng.png"), "", LeelaGTPLocale::En);
    this->butt_translation.setToolTip(Trans("tip_translation"));
    connect(&butt_translation, SIGNAL(activated(int)), this, SLOT(on_translation()));
}

LeelaGTP::~LeelaGTP() {
    delete boss;
}

bool LeelaGTP::eventFilter(QObject *watched, QEvent *event) {
    if (event->type() == QEvent::Paint) {
        if (watched == &show_board)
            drawing_board();
        else if (watched == &show_stones)
            drawing_stones();
    }
    return QWidget::eventFilter(watched, event);
}

void LeelaGTP::on_keepSgf() {
    if (butt_keepSgf.isChecked()) {
        config.keepSgf = true;
    } else {
        config.keepSgf = false;
    }
    butt_sgfpath.setEnabled(config.keepSgf);
    show_sgfpath.setEnabled(config.keepSgf);
}

void LeelaGTP::on_sgfpathbutt() {
    QString sgf_path = QFileDialog::getExistingDirectory(this, Trans("msg_sgf_save_path"), ".");
    if (!sgf_path.isEmpty()) {
        config.sgf_path = sgf_path;
        this->show_sgfpath.setText(Trans("save_to") + config.sgf_path);
        this->show_sgfpath.setToolTip(Trans("save_to") + config.sgf_path);
    }
}

void LeelaGTP::on_netfilebutt() {
    QString filepath = QFileDialog::getOpenFileName(this, Trans("msg_net_path"));
    if (!filepath.isEmpty()) {
        config.net_filepath = filepath;
        QFileInfo file(config.net_filepath);
        config.net_file = file.fileName();
        show_netfile.setText(config.net_filepath);
        show_netfile.setToolTip(config.net_filepath);
    }
}

void LeelaGTP::on_furconfigs() {
    config_dialog.copyfrom(&config);
    config_dialog.drawwindow();
    config_dialog.show();
}

void LeelaGTP::on_runbutt() {
    is_running = !is_running;
    if (is_running) {
        // Now running
        _enable_all_elements(false);
        butt_run.setText(Trans("stop"));
        _run();
    } else {
        // Now exit
        if (boss) {
            //QTextStream(stdout) << "sendquit in on runbutt\n";
            boss->sendQuit();
        } else {
            _enable_all_elements(true);
            butt_run.setText(Trans("run"));
        }
    }
}

void LeelaGTP::on_translation() {
    if (this->butt_translation.currentIndex() == __leela_gtp_locale) {
        return;
    } else {
        __leela_gtp_locale = this->butt_translation.currentIndex() == 0 ?
                    LeelaGTPLocale::Cn : LeelaGTPLocale::En;
        this->retranslate();
    }
}

int LeelaGTP::_run() {
    if (config.keepSgf) {
        if (!QDir().mkpath(config.sgf_path)) {
            QMessageBox::information(this, Trans("msg_create_path_fail"),
                                     Trans("msg_sgf_path_fail_details"));
            on_bossexit();
            return EXIT_FAILURE;
        }
    }
    if (!QDir().mkpath(config.training_data_path)) {
        QMessageBox::information(this, Trans("msg_create_path_fail"),
                                 Trans("msg_train_path_fail_details"));
        on_bossexit();
        return EXIT_FAILURE;
    }
    //if (config.run_maxgames == 0)
    //    config.run_maxgames = -1;
    config.run_maxgames -= 1;
    QTextStream(stdout) << config.run_maxgames << " max games\n";
    boss = new Management(1, config.gpu_games, QStringList(), AUTOGTP_VERSION,
                          config.run_maxgames, false,
                          config.keepSgf ? config.sgf_path : QString(), &config,
#ifdef WIN32
                          this->app->applicationDirPath(),
#endif
                          config.training_data_path);

    QTimer *timer = new QTimer();
    Job *job = boss->giveAssignments();
    job->connect_sendmessage(this, SLOT(on_recvmove(int)));
    if (config.run_timeout > 0) {
        connect(timer, &QTimer::timeout, boss, &Management::storeGames);
        connect(timer, &QTimer::timeout, this, &LeelaGTP::on_bossexit);
        timer->start(config.run_timeout * 60000);
    } else {
        connect(boss, &Management::sendQuit, this, &LeelaGTP::on_bossexit);
    }
    return 0;
}

void LeelaGTP::on_bossexit() {
    is_running = false;
    _enable_all_elements(true);
    butt_run.setText(Trans("run"));

    if (boss->terminate_leelaz())
        QMessageBox::information(this, Trans("msg_err_stop"),
                                 Trans("msg_err_stop_details"));

    delete boss;

    boss = nullptr;
    draw_mov.init();
    show_stones.update();
}

void LeelaGTP::on_recvmove(int move) {
    int tmpmove;
    if (move == 0)
        return;
    /*
     * 210000: black pass
     * 220000: white pass
     * 310000: black resign
     * 320000: white resign
     *  10000: black move
     *  20000: white move
     */
    int cmd = move / 100000;
    switch (cmd) {
    case 0: // move
        tmpmove = (move % 10000) / 100 * 100 + 18 - (move % 100);
        draw_mov.configDropStone(move / 10000, tmpmove);
        break;
    case 2: // pass
        return;
    case 1: // init
    case 3: // resign
        draw_mov.init();
        QTextStream(stdout) << "SgfMov start a new game\n";
        break;
    default:
        return;
    }
    show_stones.update();
}

void LeelaGTP::_enable_all_elements(bool cmd) {
    butt_timeout.setEnabled(cmd);
    butt_gpugames.setEnabled(cmd);
    butt_configs.setEnabled(cmd);
    butt_maxgames.setEnabled(cmd);
    butt_keepSgf.setEnabled(cmd);
    butt_netfile.setEnabled(cmd);
    if (config.keepSgf) {
        butt_sgfpath.setEnabled(cmd);
        show_sgfpath.setEnabled(cmd);
    }
}

void LeelaGTP::drawing_board() {
    QPainter pain(&show_board);
    QRect target(win_gap / 2, win_gap / 2,
                 win_gap * 19, win_gap * 19);
    pain.drawImage(target, QImage(":/images/bord.png"));
    pain.setRenderHint(QPainter::Antialiasing, true); // 使得邊緣柔和

    int tmpi, tmpj;
    pain.setPen(Qt::black);
    //pain.setBrush(Qt::blue);
    for (tmpi = 0; tmpi < 19; tmpi++) { // 畫棋盤
        pain.drawLine(win_gap, win_gap + tmpi * win_gap,
                      win_gap + 18 * win_gap, win_gap + tmpi * win_gap);
        pain.drawLine(win_gap + tmpi * win_gap, win_gap,
                      win_gap + tmpi * win_gap, win_gap + 18 * win_gap);
    }
    int dotsize = win_gap / 15;
    for (tmpi = win_gap + 3 * win_gap - dotsize; tmpi < win_size;
         tmpi += 6 * win_gap) { // 畫星位
        for (tmpj = win_gap + 3 * win_gap - dotsize; tmpj < win_size;
             tmpj += 6 * win_gap) {
            pain.drawRect(tmpi, tmpj, dotsize * 2, dotsize * 2);
        }
    }
}

void LeelaGTP::drawing_stones() {
    QPainter pain(&show_stones);
    pain.setRenderHint(QPainter::Antialiasing, true); // 使得邊緣柔和

    int tmpi, tmpj;
    pain.setPen(Qt::black);
    for (tmpi = 0; tmpi < 19; tmpi++) {
        for (tmpj = 0; tmpj < 19; tmpj++) {
            if (draw_mov.ston[tmpi][tmpj] == 1) {
                QRect target(tmpi * win_gap + win_gap * 11 / 20,
                             tmpj * win_gap + win_gap * 11 / 20,
                             win_gap * 9 / 10,
                             win_gap * 9 / 10);
                pain.drawImage(target, QImage(":/images/movblack.png"));
            }
            else if (draw_mov.ston[tmpi][tmpj] == 2) {
                QRect target(tmpi * win_gap + win_gap * 11 / 20,
                             tmpj * win_gap + win_gap * 11 / 20,
                             win_gap * 9 / 10,
                             win_gap * 9 / 10);
                pain.drawImage(target, QImage(":/images/movwhite.png"));
            }
        }
    } // finished for

    if (draw_mov.currmove >= 0) {
        QRect target(draw_mov.currmove / 100 * win_gap + win_gap,
                     draw_mov.currmove % 100 * win_gap + win_gap,
                     win_gap / 3, win_gap / 3);
        pain.drawImage(target, QImage(":/images/cur_mov.png"));
    }
}

void LeelaGTP::retranslate() {
    this->setWindowTitle(Trans("leelagtp_title"));

    if (is_running)
        this->butt_run.setText(Trans("stop"));
    else
        this->butt_run.setText(Trans("run"));
    this->butt_run.setToolTip(Trans("tip_run"));

    this->label_timeout.setText(Trans("timeout"));
    this->label_timeout.setToolTip(Trans("tip_timeout"));
    this->butt_timeout.setSpecialValueText(Trans("unlimited"));

    this->label_maxgames.setText(Trans("max_games"));
    this->label_maxgames.setToolTip(Trans("tip_max_games"));
    this->butt_maxgames.setSpecialValueText(Trans("unlimited"));

    this->label_gpugames.setText(Trans("max_gpugames"));
    this->label_gpugames.setToolTip(Trans("tip_max_gpugames"));

    this->butt_configs.setText(Trans("more_configs"));
    this->butt_configs.setToolTip(Trans("tip_more_configs"));

    this->butt_keepSgf.setText(Trans("keep_sgf"));
    this->butt_sgfpath.setText(Trans("open_filepath"));
    this->butt_sgfpath.setToolTip(Trans("tip_open_filepath"));

    if (config.sgf_path == "./sgfs/") {
        this->show_sgfpath.setText(Trans("default_path") + config.sgf_path);
        this->show_sgfpath.setToolTip(Trans("default_path") + config.sgf_path);
    }
    this->butt_netfile.setText(Trans("net_file"));
    this->butt_netfile.setToolTip(Trans("tip_net_file"));
    if (config.net_filepath == "./networks/weights.txt") {
        this->show_netfile.setText(Trans("default_file") + config.net_filepath);
        this->show_netfile.setToolTip(Trans("default_file") + config.net_filepath);
    }

    config_dialog.retranslate();
}
