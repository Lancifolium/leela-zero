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

#ifndef LEELAGTP_H
#define LEELAGTP_H

#include <QObject>
#include <QMainWindow>
#include <QApplication>
#include <QDialog>
#include <QFileDialog>
#include <QMessageBox>

#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QLineEdit>
#include <QTextBrowser>
#include <QLabel>
#include <QResizeEvent>

#include <QString>

#include "Management.h"
#include "MovLancifolium.h"
#include "GTPConfig.h"

class LeelaGTP : public QMainWindow {
    Q_OBJECT
public:
    LeelaGTP(QApplication *app, QWidget *parent = nullptr);
    ~LeelaGTP();
    bool eventFilter(QObject *watched, QEvent *event) override;

private:
    void _enable_all_elements(bool cmd);
    int _run();

    void drawing_board();
    void drawing_stones();
    void retranslate();

private slots:
    void on_keepSgf();
    void on_sgfpathbutt();
    void on_netfilebutt();
    void on_furconfigs();
    void on_runbutt();
    void on_bossexit();
    void on_recvmove(int);
    void on_translation();

private:
    QApplication *app;
    GTPConfig config_dialog;

    // Save SGF files after each self-play game.
    QCheckBox *butt_keepSgf;
    QPushButton *butt_sgfpath;
    QLabel *show_sgfpath;

    // Set self defined network
    QPushButton *butt_netfile;
    QLabel *show_netfile;

    // Save running games after the timeout (in minutes) is passed and then exit.
    QLabel *label_timeout;
    QSpinBox *butt_timeout;

    // Play 'gpu_names' games on one GPU at the same time.
    QLabel *label_gpugames;
    QSpinBox *butt_gpugames;

    // Further configurations
    QPushButton *butt_configs;

    // Exit after the given number of games is completed.
    QLabel *label_maxgames;
    QSpinBox *butt_maxgames;

    // Run
    QPushButton *butt_run;


    // Show board and stones
    QLabel *show_board;
    QLabel *show_stones;

    // Job type chozen
    QComboBox *butt_translation;


    GTPConfigElements config;

    bool is_running;
    Management *boss;

    MovLancifolium draw_mov;

    /* 繪圖 */
    int win_size; /* 棋盤尺寸 */
    int win_gap; /* = win_size / 20 */
    int win_xlb; /* 棋盤左上角的橫坐標 */
    int win_ylb; /* 棋盤左上角的縱坐標 */
};

#endif // LEELAGTP_H
