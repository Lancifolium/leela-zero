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

#include "GTPConfig.h"
#include "Translation.h"
#include <QListView>
#include <QTime>
#include <QtGlobal>
#include <QTextStream>


GTPConfigElements::GTPConfigElements() :
        job_type(JobType::LocalProduction),
        // LeelaGTP main configurations:
        run_timeout(0),
        gpu_games(1),
        run_maxgames(0),
        keepSgf(true),
        // Paths:
        sgf_path("./sgfs/"),
        net_filepath("./networks/weights.txt"),
        net_file("weights.txt"),
        net_component_filepath("./networks/component_weights.txt"),
        net_component_file("component_weights.txt"),
        dump_sgf_file("./sgfs/tmp.sgf"),
        dump_data_file("./train.txt"),
        training_data_path("./data/"),
        // GTPConfig configurations: (Add GTP commands...)
        extral_lzparam(""),
        load_training_data(true),
        // Options:
        threads_num(0),
        playouts(0),
        batch_size(0),
        randomcnt(0),
        loop_visits(3300),
        resignation_percent(1),
        enable_noise(0),
        dumbpass(false),
        // leelaz path
        leelaz_path("./leelaz") {}

void GTPConfigElements::copyto(GTPConfigElements *c) {
    c->job_type = this->job_type;


    // LeelaGTP main configurations:
    //c->run_timeout = this->run_timeout;
    //c->gpu_games = this->gpu_games;
    //c->run_maxgames = this->run_maxgames;

    //c->keepSgf = this->keepSgf;


    // Paths:
    //c->sgf_path = this->sgf_path;

    //c->net_filepath = this->net_filepath;
    //c->net_file = this->net_file;
    c->net_component_filepath = this->net_component_filepath;
    c->net_component_file = this->net_component_file;

    c->dump_sgf_file = this->dump_sgf_file;
    c->dump_data_file = this->dump_data_file;

    c->training_data_path = this->training_data_path;


    // GTPConfig configurations: (Add GTP commands...)
    c->extral_lzparam = this->extral_lzparam;

    c->load_training_data = this->load_training_data;


    // Options:
    c->threads_num = this->threads_num;
    c->playouts = this->playouts;
    c->batch_size = this->batch_size;
    c->randomcnt = this->randomcnt;
    c->loop_visits = this->loop_visits;
    c->resignation_percent = this->resignation_percent;
    c->enable_noise = this->enable_noise;
    c->dumbpass = this->dumbpass;


    // leelaz path
    //c->leelaz_path = this->leelaz_path;
}


GTPConfig::GTPConfig(QWidget *parent, GTPConfigElements *m_config) :
        QDialog (parent),
        label_threads(this),
        butt_threads(this),
        label_playouts(this),
        butt_playouts(this),
        label_batchsize(this),
        butt_batchsize(this),
        label_random(this),
        butt_random(this),
        butt_enablenoise(this),
        label_loopvisits(this),
        butt_loopvisits(this),
        label_resignpct(this),
        show_resignpct(this),
        butt_resignpct(this),
        label_exlzparam(this),
        edit_exlzparam(this),
        label_jobtype(this),
        butt_jobtype(this),
        butt_compnetfile(this),
        show_compnetfile(this),
        butt_dumpsgffile(this),
        show_dumpsgffile(this),
        butt_dumpdatafile(this),
        show_dumpdatafile(this),
        butt_trainingdatapath(this),
        show_trainingdatapath(this),
        butt_dumbpass(this),
        butt_loaddata(this),
        butt_okay(this),
        butt_cancel(this),
        main_config(m_config) {
    assert(main_config != nullptr);

    this->setWindowTitle(Trans("gtpconfig_title"));
    this->setFixedSize(600, 600);

    this->label_threads.setText(Trans("threads"));
    this->label_threads.setToolTip(Trans("tip_threads"));
    this->label_threads.setGeometry(60, 6, 150, 24);
    this->butt_threads.setGeometry(60, 30, 84, 24);
    this->butt_threads.setRange(0, 800);
    this->butt_threads.setValue(config.threads_num);
    this->butt_threads.setSingleStep(10);
    connect(&butt_threads, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.threads_num = val; });

    this->label_playouts.setText(Trans("playouts"));
    this->label_playouts.setToolTip(Trans("tip_playouts"));
    this->label_playouts.setGeometry(240, 6, 150, 24);
    this->butt_playouts.setGeometry(240, 30, 84, 24);
    this->butt_playouts.setRange(0, 800);
    this->butt_playouts.setValue(config.playouts);
    this->butt_playouts.setSingleStep(10);
    connect(&butt_playouts, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.playouts = val; });

    this->label_batchsize.setText(Trans("batchsize"));
    this->label_batchsize.setToolTip(Trans("tip_batchsize"));
    this->label_batchsize.setGeometry(420, 6, 150, 24);
    this->butt_batchsize.setGeometry(420, 30, 84, 24);
    this->butt_batchsize.setRange(0, 800);
    this->butt_batchsize.setValue(config.batch_size);
    this->butt_batchsize.setSingleStep(10);
    connect(&butt_batchsize, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.batch_size = val; });

    this->label_random.setText(Trans("random"));
    this->label_random.setToolTip(Trans("tip_random"));
    this->label_random.setGeometry(60, 66, 150, 24);
    this->butt_random.setGeometry(60, 90, 84, 24);
    this->butt_random.setRange(0, 800);
    this->butt_random.setValue(config.randomcnt);
    this->butt_random.setSingleStep(10);
    connect(&butt_random, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.randomcnt = val; });

    this->label_loopvisits.setText(Trans("loop_visits"));
    this->label_loopvisits.setToolTip(Trans("tip_loop_visits"));
    this->label_loopvisits.setGeometry(240, 66, 150, 24);
    this->butt_loopvisits.setGeometry(240, 90, 84, 24);
    this->butt_loopvisits.setRange(100, 100000);
    this->butt_loopvisits.setValue(config.loop_visits);
    this->butt_loopvisits.setSingleStep(100);
    connect(&butt_loopvisits, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.loop_visits = val; });

    this->label_resignpct.setText(Trans("resign"));
    this->label_resignpct.setToolTip(Trans("tip_resign"));
    this->label_resignpct.setGeometry(420, 66, 150, 24);
    this->show_resignpct.setText("%");
    this->show_resignpct.setGeometry(460, 90, 24, 24);
    this->butt_resignpct.setGeometry(420, 90, 36, 24);
    this->butt_resignpct.setRange(1, 30);
    this->butt_resignpct.setSingleStep(1);
    connect(&butt_resignpct, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.resignation_percent = val; });


    this->butt_enablenoise.setText(Trans("noise"));
    this->butt_enablenoise.setGeometry(60, 120, 150, 24);
    connect(&butt_enablenoise, SIGNAL(toggled(bool)), this, SLOT(on_noise()));

    this->butt_dumbpass.setText(Trans("dumbpass"));
    this->butt_dumbpass.setGeometry(60, 150, 150, 24);
    connect(&butt_dumbpass, SIGNAL(toggled(bool)), this, SLOT(on_dumbpass()));

    this->butt_loaddata.setText(Trans("load_data"));
    this->butt_loaddata.setGeometry(60, 180, 360, 24);
    connect(&butt_loaddata, SIGNAL(toggled(bool)), this, SLOT(on_loadtrainingdata()));

    this->butt_trainingdatapath.setText(Trans("open_train_data_path"));
    this->butt_trainingdatapath.setToolTip(Trans("tip_open_train_data_path"));
    this->butt_trainingdatapath.setGeometry(60, 210, 108, 24);
    connect(&butt_trainingdatapath, SIGNAL(clicked(bool)), this, SLOT(on_trainingdatapath()));
    this->show_trainingdatapath.setText(Trans("default_path") + config.training_data_path);
    this->show_trainingdatapath.setGeometry(176, 210, 360, 24);


    this->label_exlzparam.setText(Trans("extral_params"));
    this->label_exlzparam.setToolTip(Trans("tip_extral_params"));
    this->label_exlzparam.setGeometry(60, 246, 150, 24);
    this->edit_exlzparam.setText(config.extral_lzparam);
    this->edit_exlzparam.setGeometry(60, 270, 400, 24);
    connect(&edit_exlzparam, SIGNAL(editingFinished()), this, SLOT(on_exlzparam()));

    /*
     * Following is Job Type settings.
     */

    this->label_jobtype.setText(Trans("job_type"));
    this->label_jobtype.setToolTip(Trans("tip_job_type"));
    this->label_jobtype.setGeometry(60, 306, 150, 24);
    this->butt_jobtype.setToolTip(Trans("tip_job_type"));
    this->butt_jobtype.setGeometry(60, 330, 120, 24);
    this->butt_jobtype.addItem(Trans("type_local_production"),
                               GTPConfigElements::JobType::LocalProduction);
    this->butt_jobtype.addItem(Trans("type_local_validation"),
                               GTPConfigElements::JobType::LocalValidation);
    this->butt_jobtype.addItem(Trans("type_online"),
                               GTPConfigElements::JobType::OnlineJob);
    this->butt_jobtype.addItem(Trans("type_dump_supervised"),
                               GTPConfigElements::JobType::DumpSupervised);
    connect(&butt_jobtype, SIGNAL(activated(int)), this, SLOT(on_jobtype()));


    this->butt_compnetfile.setText(Trans("open_net_weights_file"));
    this->butt_compnetfile.setToolTip(Trans("tip_open_net_weights_file"));
    this->butt_compnetfile.setGeometry(60, 360, 108, 24);
    connect(&butt_compnetfile, SIGNAL(clicked(bool)), this, SLOT(on_compnetfile()));
    this->show_compnetfile.setText(
                Trans("default_file") + config.net_component_filepath);
    this->show_compnetfile.setToolTip(
            Trans("default_file") + config.net_component_filepath);
    this->show_compnetfile.setGeometry(176, 360, 400, 24);

    this->butt_dumpsgffile.setText(Trans("open_train_sgf_file"));
    this->butt_dumpsgffile.setToolTip(Trans("tip_open_train_sgf_file"));
    this->butt_dumpsgffile.setGeometry(60, 390, 108, 24);
    connect(&butt_dumpsgffile, SIGNAL(clicked(bool)), this, SLOT(on_dumpsgffile()));
    this->show_dumpsgffile.setText(Trans("default_file") + config.dump_sgf_file);
    this->show_dumpsgffile.setToolTip(
            Trans("default_file") + config.dump_sgf_file);
    this->show_dumpsgffile.setGeometry(176, 390, 400, 24);

    this->butt_dumpdatafile.setText(Trans("save_net_weights_file"));
    this->butt_dumpdatafile.setToolTip(Trans("tip_save_net_weights_file"));
    this->butt_dumpdatafile.setGeometry(60, 420, 108, 24);
    connect(&butt_dumpdatafile, SIGNAL(clicked(bool)), this, SLOT(on_savedumpdatafile()));
    this->show_dumpdatafile.setText(Trans("default_file") + config.dump_data_file);
    this->show_dumpdatafile.setToolTip(
            Trans("default_file") + config.dump_data_file);
    this->show_dumpdatafile.setGeometry(176, 420, 400, 24);


    this->butt_okay.setText(Trans("okay"));
    this->butt_okay.setGeometry(60, 510, 84, 24);
    connect(&butt_okay, SIGNAL(clicked(bool)), this, SLOT(on_okay()));
    this->butt_cancel.setText(Trans("cancel"));
    this->butt_cancel.setGeometry(150, 510, 84, 24);
    connect(&butt_cancel, SIGNAL(clicked(bool)), this, SLOT(on_cancel()));
}

GTPConfig::~GTPConfig() {
}

void GTPConfig::drawwindow() {
    this->butt_random.setValue(config.randomcnt);
    this->butt_loopvisits.setValue(config.loop_visits);
    this->butt_resignpct.setValue(config.resignation_percent);
    this->butt_enablenoise.setChecked(config.enable_noise);
    this->butt_dumbpass.setChecked(config.dumbpass);
    this->butt_loaddata.setChecked(config.load_training_data);
    if (config.training_data_path == "./data/") {
        this->show_trainingdatapath.setText(
                Trans("default_path") + config.training_data_path);
        this->show_trainingdatapath.setToolTip(
                Trans("default_path") + config.training_data_path);
    } else {
        this->show_trainingdatapath.setText(config.training_data_path);
        this->show_trainingdatapath.setToolTip(config.training_data_path);
    }

    this->edit_exlzparam.setText(config.extral_lzparam);
    this->butt_jobtype.setCurrentIndex(config.job_type);

    on_jobtype();

    if (config.net_component_filepath == "./networks/component_weights.txt") {
        this->show_compnetfile.setText(
                Trans("default_file") + config.net_component_filepath);
        this->show_compnetfile.setToolTip(
                Trans("default_file") + config.net_component_filepath);
    } else {
        this->show_compnetfile.setText(config.net_component_filepath);
        this->show_compnetfile.setToolTip(config.net_component_filepath);
    }

    if (config.dump_sgf_file == "./sgfs/tmp.sgf") {
        this->show_dumpsgffile.setText(
                Trans("default_file") + config.dump_sgf_file);
        this->show_dumpsgffile.setToolTip(
                Trans("default_file") + config.dump_sgf_file);
    } else {
        this->show_dumpsgffile.setText(config.dump_sgf_file);
        this->show_dumpsgffile.setToolTip(config.dump_sgf_file);
    }

    if (config.dump_data_file == "./train.txt") {
        this->show_dumpdatafile.setText(
                Trans("default_file") + config.dump_data_file);
        this->show_dumpdatafile.setToolTip(
                Trans("default_file") + config.dump_data_file);
    } else {
        this->show_dumpdatafile.setText(config.dump_data_file);
        this->show_dumpdatafile.setToolTip(config.dump_data_file);
    }
}

void GTPConfig::copyfrom(GTPConfigElements *m_config) {
    if (m_config != nullptr)
        m_config->copyto(&config);
}

void GTPConfig::retranslate() {
    this->setWindowTitle(Trans("gtpconfig_title"));
    this->label_threads.setText(Trans("threads"));
    this->label_threads.setToolTip(Trans("tip_threads"));
    this->label_playouts.setText(Trans("playouts"));
    this->label_playouts.setToolTip(Trans("tip_playouts"));
    this->label_batchsize.setText(Trans("batchsize"));
    this->label_batchsize.setToolTip(Trans("tip_batchsize"));
    this->label_random.setText(Trans("random"));
    this->label_random.setToolTip(Trans("tip_random"));
    this->label_loopvisits.setText(Trans("loop_visits"));
    this->label_loopvisits.setToolTip(Trans("tip_loop_visits"));
    this->label_resignpct.setText(Trans("resign"));
    this->label_resignpct.setToolTip(Trans("tip_resign"));
    this->butt_enablenoise.setText(Trans("noise"));
    this->butt_dumbpass.setText(Trans("dumbpass"));

    this->butt_loaddata.setText(Trans("load_data"));
    this->butt_trainingdatapath.setText(Trans("open_train_data_path"));
    this->butt_trainingdatapath.setToolTip(Trans("tip_open_train_data_path"));
    if (config.training_data_path == "./data/") {
        this->show_trainingdatapath.setText(
                Trans("default_path") + config.training_data_path);
        this->show_trainingdatapath.setToolTip(
                Trans("default_path") + config.training_data_path);
    }

    this->label_exlzparam.setText(Trans("extral_params"));
    this->label_exlzparam.setToolTip(Trans("tip_extral_params"));

    this->label_jobtype.setText(Trans("job_type"));
    this->label_jobtype.setToolTip(Trans("tip_job_type"));
    this->butt_jobtype.setToolTip(Trans("tip_job_type"));
    this->butt_jobtype.setItemText(GTPConfigElements::JobType::LocalProduction,
                                   Trans("type_local_production"));
    this->butt_jobtype.setItemText(GTPConfigElements::JobType::LocalValidation,
                                   Trans("type_local_validation"));
    this->butt_jobtype.setItemText(GTPConfigElements::JobType::OnlineJob,
                                   Trans("type_online"));
    this->butt_jobtype.setItemText(GTPConfigElements::JobType::DumpSupervised,
                                   Trans("type_dump_supervised"));

    this->butt_compnetfile.setText(Trans("open_net_weights_file"));
    this->butt_compnetfile.setToolTip(Trans("tip_open_net_weights_file"));
    if (config.net_component_filepath == "./networks/component_weights.txt") {
        this->show_compnetfile.setText(
                Trans("default_file") + config.net_component_filepath);
        this->show_compnetfile.setToolTip(
                Trans("default_file") + config.net_component_filepath);
    }

    this->butt_dumpsgffile.setText(Trans("open_train_sgf_file"));
    this->butt_dumpsgffile.setToolTip(Trans("tip_open_train_sgf_file"));
    if (config.dump_sgf_file == "./sgfs/tmp.sgf") {
        this->show_dumpsgffile.setText(
                Trans("default_file") + config.dump_sgf_file);
        this->show_dumpsgffile.setToolTip(
                Trans("default_file") + config.dump_sgf_file);
    }

    this->butt_dumpdatafile.setText(Trans("save_net_weights_file"));
    this->butt_dumpdatafile.setToolTip(Trans("tip_save_net_weights_file"));
    if (config.dump_data_file == "./train.txt") {
        this->show_dumpdatafile.setText(
                Trans("default_file") + config.dump_data_file);
        this->show_dumpdatafile.setToolTip(
                Trans("default_file") + config.dump_data_file);
    }

    this->butt_okay.setText(Trans("okay"));
    this->butt_cancel.setText(Trans("cancel"));
}

void GTPConfig::on_noise() {
    if (this->butt_enablenoise.isChecked()) {
        config.enable_noise = true;
    } else {
        config.enable_noise = false;
    }
}

void GTPConfig::on_jobtype() {
    switch (this->butt_jobtype.currentIndex()) {
    case GTPConfigElements::JobType::LocalProduction:
        config.job_type = GTPConfigElements::JobType::LocalProduction;
        break;
    case GTPConfigElements::JobType::LocalValidation:
        config.job_type = GTPConfigElements::JobType::LocalValidation;
        break;
    case GTPConfigElements::JobType::OnlineJob:
        config.job_type = GTPConfigElements::JobType::OnlineJob;
        break;
    case GTPConfigElements::JobType::DumpSupervised:
        config.job_type = GTPConfigElements::JobType::DumpSupervised;
        break;
    default:
        config.job_type = GTPConfigElements::JobType::LocalProduction;
        break;
    }

    if (config.job_type == GTPConfigElements::JobType::LocalValidation) {
        this->butt_compnetfile.setEnabled(true);
        this->show_compnetfile.setEnabled(true);
    } else {
        this->butt_compnetfile.setEnabled(false);
        this->show_compnetfile.setEnabled(false);
    }
    if (config.job_type == GTPConfigElements::JobType::DumpSupervised) {
        this->butt_dumpsgffile.setEnabled(true);
        this->show_dumpsgffile.setEnabled(true);
        this->butt_dumpdatafile.setEnabled(true);
        this->show_dumpdatafile.setEnabled(true);
    } else {
        this->butt_dumpsgffile.setEnabled(false);
        this->show_dumpsgffile.setEnabled(false);
        this->butt_dumpdatafile.setEnabled(false);
        this->show_dumpdatafile.setEnabled(false);
    }
}

void GTPConfig::on_compnetfile() {
    QString filepath = QFileDialog::getOpenFileName(this, Trans("msg_net_path"));
    if (!filepath.isEmpty()) {
        config.net_component_filepath = filepath;
        QFileInfo file(config.net_component_filepath);
        config.net_component_file = file.fileName();
        this->show_compnetfile.setText(config.net_component_filepath);
        this->show_compnetfile.setToolTip(config.net_component_filepath);
    }
}

void GTPConfig::on_dumpsgffile() {
    QString filepath = QFileDialog::getOpenFileName(this, Trans("msg_sgf_path"));
    if (!filepath.isEmpty()) {
        config.dump_sgf_file = filepath;
        this->show_dumpsgffile.setText(config.dump_sgf_file);
        this->show_dumpsgffile.setToolTip(config.dump_sgf_file);
    }
}

void GTPConfig::on_savedumpdatafile() {
    QString filepath =
            QFileDialog::getOpenFileName(this, Trans("save_net_weights_file"));
    if (!filepath.isEmpty()) {
        config.dump_data_file = filepath;
        this->show_dumpdatafile.setText(config.dump_data_file);
        this->show_dumpdatafile.setToolTip(config.dump_data_file);
    }
}

void GTPConfig::on_trainingdatapath() {
    QString filepath = QFileDialog::getExistingDirectory(this, Trans("open_train_data_path"), ".");
    if (!filepath.isEmpty()) {
        config.training_data_path = filepath;
        this->show_trainingdatapath.setText(config.training_data_path);
        this->show_trainingdatapath.setToolTip(config.training_data_path);
    }
}

void GTPConfig::on_loadtrainingdata() {
    if (butt_loaddata.isChecked())
        config.load_training_data = true;
    else
        config.load_training_data = false;
}


void GTPConfig::on_dumbpass() {
    if (this->butt_dumbpass.isChecked()) {
        config.dumbpass = true;
    } else {
        config.dumbpass = false;
    }
}

void GTPConfig::on_exlzparam() {
    config.extral_lzparam = edit_exlzparam.text();
}

void GTPConfig::on_okay() {
    config.copyto(main_config);
    this->close();
}

void GTPConfig::on_cancel() {
    this->close();
}
