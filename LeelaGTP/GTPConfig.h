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

#ifndef GTPCONFIG_H
#define GTPCONFIG_H

#include <QCheckBox>
#include <QComboBox>
#include <QDialog>
#include <QFileDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QWidget>

class LeelaGTP;

struct GTPConfigElements {
    enum JobType {
        LocalProduction = 0,
        LocalValidation = 1,
        OnlineJob = 2,
        DumpSupervised = 3,
    };

    // Job type
    JobType job_type;

    // Keep Sgf file
    bool keepSgf;
    QString sgf_path;

    // Weight file path and file name
    QString net_filepath;
    QString net_file;
    QString net_component_filepath;
    QString net_component_file;

    // Dump supervised sgf file
    QString dump_sgf_file;
    QString dump_data_file;

    QString extral_lzparam;

    // load training data from directory
    QString training_data_path;

    // load training data or kept sgf files
    bool load_training_data;
    //bool load_kept_sgfs;

    // Save running games after the timeout (in minutes) is passed and then exit.
    int run_timeout;

    // Exit after the given number of games is completed.
    int run_maxgames;

    // Play more randomly the first x moves (-m =0).
    int random_num;

    // Enable policy network randomization (-n).
    bool enable_noise;

    // Set loop number of visits (-v [3200])
    int loop_visits;

    // Resign when winrate is less than x%. -1 uses 10% but scales for handicap.
    int resignation_percent;

    // Don's use heuristic for smarter pass (-d)
    bool heuristic;

    // Play 'gpu_names' games on one GPU at the same time.
    int gpu_games;

    // Specify 'leelaz' path
    QString leelaz_path;

    GTPConfigElements();

    void copyto(GTPConfigElements *c);
};


class GTPConfig : public QDialog {
    Q_OBJECT
public:
    GTPConfig(QWidget *parent, GTPConfigElements *m_config);
    ~GTPConfig();
    void drawwindow();
    void copyfrom(GTPConfigElements *m_config);

private:
    friend class LeelaGTP;
    void retranslate();

private slots:
    void on_noise();
    void on_jobtype();
    void on_compnetfile();
    void on_dumpsgffile();
    void on_savedumpdatafile();
    void on_trainingdatapath();
    void on_loadtrainingdata();
    void on_heuristic();
    void on_exlzparam();

    void on_okay();
    void on_cancel();

private:

    // Play more randomly the first x moves. (-m =0)
    QLabel *label_random;
    QSpinBox *butt_random;

    // Enable policy network randomization
    QCheckBox *butt_enablenoise;

    // Set loop number of visits (-v [3200])
    QLabel *label_loopvisits;
    QSpinBox *butt_loopvisits;

    // Set resign winrate (-r 1)
    QLabel *label_resignpct, *show_resignpct;
    QSpinBox *butt_resignpct;

    // Set extral leelaz parameters
    QLabel *label_exlzparam;
    QLineEdit *edit_exlzparam;

    // Job type chozen
    QLabel *label_jobtype;
    QComboBox *butt_jobtype;

    // Matches
    QPushButton *butt_compnetfile;
    QLabel *show_compnetfile;

    // Dump supervised
    QPushButton *butt_dumpsgffile;
    QLabel *show_dumpsgffile;
    QPushButton *butt_dumpdatafile;
    QLabel *show_dumpdatafile;

    // load training data from directory
    QPushButton *butt_trainingdatapath;
    QLabel *show_trainingdatapath;

    // Don's use heuristic for smarter pass (-d)
    QCheckBox *butt_heuristic;

    // Load training data
    QCheckBox *butt_loaddata;

    // OK, Cancel
    QPushButton *butt_okay;
    QPushButton *butt_cancel;


    struct GTPConfigElements config;
    struct GTPConfigElements *main_config;
};


#endif // GTPCONFIG_H
