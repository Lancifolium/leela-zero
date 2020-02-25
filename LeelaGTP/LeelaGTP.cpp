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
#include <QScrollArea>
#include <QTextStream>
#include <QtCore/QTimer>
#include <QDialogButtonBox>

LeelaGTP::LeelaGTP(QApplication *app, QWidget *parent) :
        QMainWindow(parent),
#ifdef WIN32
        app(app),
#endif
        config_dialog(nullptr, &config),
        is_running(false),
        boss(nullptr) {
    this->config_dialog.setModal(true);

    this->setWindowTitle(Trans("leelagtp_title"));
    this->setWindowIcon(QIcon(":/images/Lancifolium.ico"));
    this->resize(900, 700);


    /***
     * Functionalities and Actions.
     */
    this->butt_run.setText(Trans("run"));
    this->butt_run.setToolTip(Trans("tip_run"));
    connect(&butt_run, SIGNAL(clicked(bool)), this, SLOT(on_runbutt()));
    connect(&config_dialog.butt_okays, &QDialogButtonBox::accepted, this, &LeelaGTP::on_runbutt);

    this->butt_createtask.setText(Trans("new_task"));
    this->butt_createtask.setToolTip(Trans("tip_new_task"));
    connect(&butt_createtask, SIGNAL(clicked(bool)), this, SLOT(on_createtask()));

    this->label_timeout.setText(Trans("timeout"));
    this->label_timeout.setToolTip(Trans("tip_timeout"));
    this->butt_timeout.setRange(0, 1000000);
    this->butt_timeout.setValue(config.run_timeout);
    this->butt_timeout.setSingleStep(10);
    this->butt_timeout.setSpecialValueText(Trans("unlimited"));
    connect(&butt_timeout, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.run_timeout = val; });

    this->label_maxgames.setText(Trans("max_games"));
    this->label_maxgames.setToolTip(Trans("tip_max_games"));
    this->butt_maxgames.setRange(0, 1000000);
    this->butt_maxgames.setValue(config.run_maxgames);
    this->butt_maxgames.setSingleStep(10);
    this->butt_maxgames.setSpecialValueText(Trans("unlimited"));
    connect(&butt_maxgames, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.run_maxgames = val; });

    this->label_gpugames.setText(Trans("max_gpugames"));
    this->label_gpugames.setToolTip(Trans("tip_max_gpugames"));
    this->butt_gpugames.setRange(1, 8);
    this->butt_gpugames.setValue(config.gpu_games);
    this->butt_gpugames.setSingleStep(1);
    connect(&butt_gpugames, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.gpu_games = val; });


    this->butt_keepSgf.setText(Trans("keep_sgf"));
    this->butt_keepSgf.setChecked(config.keepSgf);
    connect(&butt_keepSgf, SIGNAL(toggled(bool)), this, SLOT(on_keepSgf()));
    this->butt_sgfpath.setText(Trans("open_filepath"));
    this->butt_sgfpath.setToolTip(Trans("tip_open_filepath"));
    this->butt_sgfpath.setFocus();
    connect(&butt_sgfpath, SIGNAL(clicked(bool)), this, SLOT(on_sgfpathbutt()));
    this->butt_sgfpath.setEnabled(config.keepSgf);
    this->show_sgfpath.setText(Trans("default_path") + config.sgf_path);
    this->show_sgfpath.setToolTip(Trans("default_path") + config.sgf_path);
    this->show_sgfpath.setEnabled(config.keepSgf);

    this->butt_netfile.setText(Trans("net_file"));
    this->butt_netfile.setToolTip(Trans("tip_net_file"));
    connect(&butt_netfile, SIGNAL(clicked(bool)), this, SLOT(on_netfilebutt()));
    this->show_netfile.setText(Trans("default_file") + config.net_filepath);
    this->show_netfile.setToolTip(Trans("default_file") + config.net_filepath);

    this->butt_translation.addItem(QIcon(":/images/chn.png"), "中文",
                                   LeelaGTPLocale::Cn);
    this->butt_translation.addItem(QIcon(":/images/eng.png"), "English",
                                   LeelaGTPLocale::En);
    this->butt_translation.setToolTip(Trans("tip_translation"));
    connect(&this->butt_translation, SIGNAL(activated(int)), this,
            SLOT(on_translation()));


    /***
     * Menu & Tool bar settings.
     */

    /*
     * Actions.
     */
    this->act_newfile.setText(Trans("new_file"));
    this->act_newfile.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileIcon));
    this->act_newfile.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));

    this->act_openfile.setText(Trans("open_file"));
    this->act_openfile.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogStart));
    this->act_openfile.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));

    this->act_savefile.setText(Trans("save_file"));
    this->act_savefile.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    this->act_savefile.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));

    this->act_exit.setText(Trans("exit"));
    this->act_exit.setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Q));
    connect(&this->act_exit, &QAction::triggered, this, &LeelaGTP::on_exit);

    this->act_newtask.setText(Trans("new_task"));
    this->act_newtask.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogDetailedView));
    this->act_newtask.setShortcut(QKeySequence(Qt::Key_F5));
    connect(&this->act_newtask, &QAction::triggered, this, &LeelaGTP::on_createtask);

    this->act_opentask.setText(Trans("open_task"));
    this->act_opentask.setShortcut(QKeySequence(Qt::ALT + Qt::Key_F5));
    this->act_opentask.setIcon(QApplication::style()->standardIcon(QStyle::SP_FileDialogEnd));
    this->act_opentask.setEnabled(false);

    this->act_savetask.setText(Trans("save_task"));
    this->act_savetask.setShortcut(QKeySequence(Qt::ALT + Qt::Key_S));
    this->act_savetask.setIcon(QApplication::style()->standardIcon(QStyle::SP_DialogSaveButton));
    this->act_savetask.setEnabled(false);

    this->act_about.setText(Trans("about"));
    this->act_about.setShortcut(QKeySequence(Qt::Key_F1));
    this->act_about.setIcon(QApplication::style()->standardIcon(QStyle::SP_MessageBoxQuestion));
    connect(&this->act_about, &QAction::triggered, this, &LeelaGTP::on_about);


    /*
     * Menu bar settings:
     *   |File| => |New File|, |Open File|, |Save File|, |Exit|
     *   |Task| => |New Task|, |Open Task|, |Save Task|
     *   |Help| => |About|
     */
    QMenuBar *menubar = menuBar();
    /*
    this->menu_file = menubar->addMenu(Trans("file"));
    this->menu_file->addAction(&this->act_newfile);
    this->menu_file->addAction(&this->act_openfile);
    this->menu_file->addAction(&this->act_savefile);
    this->menu_file->addSeparator();
    this->menu_file->addAction(&this->act_exit);
    */

    this->menu_task = menubar->addMenu(Trans("task"));
    this->menu_task->addAction(&this->act_newtask);
    this->menu_task->addAction(&this->act_opentask);
    this->menu_task->addAction(&this->act_savetask);
    this->menu_task->addSeparator();
    this->menu_task->addAction(&this->act_exit);

    this->menu_help = menubar->addMenu(Trans("help"));
    this->menu_help->addAction(&this->act_about);


    /*
     * Tool bar settings:
     */
    this->toolbar = this->addToolBar(Trans("task"));
    //this->toolbar->addAction(&this->act_newfile);
    //this->toolbar->addAction(&this->act_openfile);
    //this->toolbar->addAction(&this->act_savefile);
    //this->toolbar->addSeparator();
    this->toolbar->addAction(&this->act_newtask);
    this->toolbar->addAction(&this->act_opentask);
    this->toolbar->addAction(&this->act_savetask);
    this->toolbar->addSeparator();
    this->toolbar->addAction(&this->act_about);
    this->toolbar->addWidget(&this->butt_translation);

    /*
     * Status bar settings:
     */


    /***
     * Main window.
     */
    this->main_splitter = new QSplitter(this);
    this->main_splitter->setOrientation(Qt::Horizontal);
    this->main_splitter->setMinimumSize(30, 300);
    this->main_splitter->addWidget(&this->show_board);
    this->show_board.refresh();

    QWidget *w = new QWidget(this->main_splitter);
    QVBoxLayout *vl = new QVBoxLayout(w);
    vl->addWidget(&this->view_panel);
    vl->addWidget(&this->butt_run);
    vl->addWidget(&this->label_timeout);
    vl->addWidget(&this->butt_timeout);
    vl->addWidget(&this->label_maxgames);
    vl->addWidget(&this->butt_maxgames);
    vl->addWidget(&this->label_gpugames);
    vl->addWidget(&this->butt_gpugames);
    vl->addWidget(&this->butt_createtask);
    vl->addWidget(&this->butt_keepSgf);
    vl->addWidget(&this->butt_sgfpath);
    vl->addWidget(&this->show_sgfpath);
    vl->addWidget(&this->butt_netfile);
    vl->addWidget(&this->show_netfile);

    w->setLayout(vl);
    this->setCentralWidget(this->main_splitter);
}

LeelaGTP::~LeelaGTP() {
}

void LeelaGTP::resizeEvent(QResizeEvent *event) {
}

bool LeelaGTP::eventFilter(QObject *watched, QEvent *event) {
    //if (event->type() == QEvent::Paint) {
        if (watched == &show_board) {
            show_board.eventFilter(&show_board, event);
        }
    //}
    return QMainWindow::eventFilter(watched, event);
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

void LeelaGTP::on_createtask() {
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
        __leela_gtp_locale = (this->butt_translation.currentIndex() == 0) ?
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
    this->show_board.refresh();
}

void LeelaGTP::on_recvmove(int move) {
    int tmpmove;
    if (move == 0)
        return;
    /*
     * 0x01XXXX: black move
     * 0x02XXXX: white move
     * 0x210000: black pass
     * 0x220000: white pass
     * 0x310000: black resign
     * 0x320000: white resign
     */
    int cmd = (move & 0xF00000) >> 20;
    switch (cmd) {
    case 0: // move
        tmpmove = (move & 0xFF00) + 18 - (move & 0xFF);
        this->show_board.drop_stone((move & 0xFF0000) >> 16, tmpmove);
        break;
    case 2: // pass
        return;
    case 1: // init
    case 3: // resign
        this->show_board.refresh();
        QTextStream(stdout) << "SgfMov start a new game\n";
        break;
    default:
        return;
    }
}

void LeelaGTP::_enable_all_elements(bool cmd) {
    butt_timeout.setEnabled(cmd);
    butt_gpugames.setEnabled(cmd);
    butt_createtask.setEnabled(cmd);
    butt_maxgames.setEnabled(cmd);
    butt_keepSgf.setEnabled(cmd);
    butt_netfile.setEnabled(cmd);
    if (config.keepSgf) {
        butt_sgfpath.setEnabled(cmd);
        show_sgfpath.setEnabled(cmd);
    }
}

void LeelaGTP::retranslate() {
    this->setWindowTitle(Trans("leelagtp_title"));

    this->act_newfile.setText(Trans("new_file"));
    this->act_openfile.setText(Trans("open_file"));
    this->act_savefile.setText(Trans("save_file"));
    this->act_exit.setText(Trans("exit"));
    this->act_newtask.setText(Trans("new_task"));
    this->act_opentask.setText(Trans("open_task"));
    this->act_savetask.setText(Trans("save_task"));
    this->act_about.setText(Trans("about"));

    //this->menu_file->setTitle(Trans("file"));
    this->menu_task->setTitle(Trans("task"));
    this->menu_help->setTitle(Trans("help"));

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

    this->butt_createtask.setText(Trans("new_task"));
    this->butt_createtask.setToolTip(Trans("tip_new_task"));

    this->butt_keepSgf.setText(Trans("keep_sgf"));
    this->butt_sgfpath.setText(Trans("open_filepath"));
    this->butt_sgfpath.setToolTip(Trans("tip_open_filepath"));

    if (config.sgf_path == "./sgfs/") {
        this->show_sgfpath.setText(Trans("default_path") + config.sgf_path);
        this->show_sgfpath.setToolTip(Trans("default_path") + config.sgf_path);
    } else {
        this->show_sgfpath.setText(Trans("save_to") + config.sgf_path);
        this->show_sgfpath.setToolTip(Trans("save_to") + config.sgf_path);
    }
    this->butt_netfile.setText(Trans("net_file"));
    this->butt_netfile.setToolTip(Trans("tip_net_file"));
    if (config.net_filepath == "./networks/weights.txt") {
        this->show_netfile.setText(Trans("default_file") + config.net_filepath);
        this->show_netfile.setToolTip(Trans("default_file") + config.net_filepath);
    }

    config_dialog.retranslate();
}

void LeelaGTP::on_exit() {
    this->close();
}

void LeelaGTP::on_about() {
    QMessageBox::about(this, Trans("msg_about"),
                             Trans("msg_about_details"));
}
