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
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListView>
#include <QPushButton>
#include <QRadioButton>
#include <QSpinBox>
#include <QStandardItemModel>
#include <QStackedLayout>
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

    // Save running games after the timeout (in minutes) is passed and then exit.
    int run_timeout;

    // Play 'gpu_names' games on one GPU at the same time.
    int gpu_games;

    // Exit after the given number of games is completed.
    int run_maxgames;

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

    // load training data from directory
    QString training_data_path;

    QString extral_lzparam;

    // load training data or kept sgf files
    bool load_training_data;

    // Number of threads to use. Select 0 to let leela-zero pick a reasonable
    // default.
    int threads_num;

    // Weaken engine by limiting the number of playouts. Requires --noponder.
    int playouts;

    // Max batch size. Select 0 to let leela-zero pick a reasonable default.
    int batch_size;

    // Play more randomly the first x moves (-m =0).
    int randomcnt;

    // Weaken engine by limiting the number of visits (-v [3200]).
    int loop_visits;

    // Resign when winrate is less than x% (-r).
    // -1 uses 10% but scales for handicap.
    int resignation_percent;

    // Enable policy network randomization (-n).
    bool enable_noise;

    // Don't use heuristics for smarter passing (-d).
    bool dumbpass;

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
    void set_common_elements(QGridLayout* l);

private slots:
    void on_noise();
    void on_jobtype_clicked(QModelIndex index);
    void on_compnetfile();
    void on_dumpsgffile();
    void on_savedumpdatafile();
    void on_trainingdatapath();
    void on_loadtrainingdata();
    void on_dumbpass();
    void on_exlzparam();

    void on_okay();
    void on_cancel();

private:

    // Number of threads to use. (-t =0)
    QLabel label_threads;
    QSpinBox butt_threads;

    // Weaken engine by limiting the number of playouts.
    QLabel label_playouts;
    QSpinBox butt_playouts;

    // Max batch size. (--batchsize =0)
    QLabel label_batchsize;
    QSpinBox butt_batchsize;

    // Play more randomly the first x moves. (-m =0)
    QLabel label_random;
    QSpinBox butt_random;

    // Enable policy network randomization
    QCheckBox butt_enablenoise;

    // Set loop number of visits (-v [3200])
    QLabel label_loopvisits;
    QSpinBox butt_loopvisits;

    // Set resign winrate (-r 1)
    QLabel label_resignpct, show_resignpct;
    QSpinBox butt_resignpct;

    // Set extral leelaz parameters
    QLabel label_exlzparam;
    QLineEdit edit_exlzparam;

    // Job type chozen
    QListView view_jobtype;
    QStandardItemModel model_jobtype;

    // Matches
    QPushButton butt_compnetfile;
    QLabel show_compnetfile;

    // Dump supervised
    QPushButton butt_dumpsgffile;
    QLabel show_dumpsgffile;
    QPushButton butt_dumpdatafile;
    QLabel show_dumpdatafile;

    // load training data from directory
    QPushButton butt_trainingdatapath;
    QLabel show_trainingdatapath;

    // Don't use heuristics for smarter passing (-d).
    QCheckBox butt_dumbpass;

    // Load training data
    QCheckBox butt_loaddata;

    // OK, Cancel
    QDialogButtonBox butt_okays;

    //
    QWidget *w;

    QGridLayout *gl_window;
    QStackedLayout *sl_jobtypes;
    QGridLayout *gl_commons;

    QWidget *w_local;
    QGridLayout *gl_local;
    QWidget *w_localmatch;
    QGridLayout *gl_localmatch;
    QWidget *w_online;
    QGridLayout *gl_online;
    QWidget *w_convert;
    QGridLayout *gl_convert;

    struct GTPConfigElements config;
    struct GTPConfigElements *main_config;
};


#endif // GTPCONFIG_H
