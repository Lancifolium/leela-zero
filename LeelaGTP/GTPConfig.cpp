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
        main_config(m_config) {
    assert(main_config != nullptr);

    this->setWindowTitle(Trans("gtpconfig_title"));
    this->setWindowIcon(QIcon(":/images/Lancifolium.ico"));

    /***
     * Elements setting.
     */
    this->label_threads.setText(Trans("threads"));
    this->label_threads.setToolTip(Trans("tip_threads"));
    this->butt_threads.setRange(0, 800);
    this->butt_threads.setValue(config.threads_num);
    this->butt_threads.setSingleStep(10);
    connect(&butt_threads, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.threads_num = val; });

    this->label_playouts.setText(Trans("playouts"));
    this->label_playouts.setToolTip(Trans("tip_playouts"));
    this->butt_playouts.setRange(0, 800);
    this->butt_playouts.setValue(config.playouts);
    this->butt_playouts.setSingleStep(10);
    connect(&butt_playouts, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.playouts = val; });

    this->label_batchsize.setText(Trans("batchsize"));
    this->label_batchsize.setToolTip(Trans("tip_batchsize"));
    this->butt_batchsize.setRange(0, 800);
    this->butt_batchsize.setValue(config.batch_size);
    this->butt_batchsize.setSingleStep(10);
    connect(&butt_batchsize, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.batch_size = val; });

    this->label_random.setText(Trans("random"));
    this->label_random.setToolTip(Trans("tip_random"));
    this->butt_random.setRange(0, 800);
    this->butt_random.setValue(config.randomcnt);
    this->butt_random.setSingleStep(10);
    connect(&butt_random, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.randomcnt = val; });

    this->label_loopvisits.setText(Trans("loop_visits"));
    this->label_loopvisits.setToolTip(Trans("tip_loop_visits"));
    this->butt_loopvisits.setRange(100, 100000);
    this->butt_loopvisits.setValue(config.loop_visits);
    this->butt_loopvisits.setSingleStep(100);
    connect(&butt_loopvisits, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.loop_visits = val; });

    this->label_resignpct.setText(Trans("resign"));
    this->label_resignpct.setToolTip(Trans("tip_resign"));
    this->show_resignpct.setText("%");
    this->butt_resignpct.setRange(1, 30);
    this->butt_resignpct.setSingleStep(1);
    connect(&butt_resignpct, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.resignation_percent = val; });

    this->butt_enablenoise.setText(Trans("noise"));
    connect(&butt_enablenoise, SIGNAL(toggled(bool)), this, SLOT(on_noise()));

    this->butt_dumbpass.setText(Trans("dumbpass"));
    connect(&butt_dumbpass, SIGNAL(toggled(bool)), this, SLOT(on_dumbpass()));

    this->butt_loaddata.setText(Trans("load_data"));
    connect(&butt_loaddata, SIGNAL(toggled(bool)), this, SLOT(on_loadtrainingdata()));

    this->butt_trainingdatapath.setText(Trans("open_train_data_path"));
    this->butt_trainingdatapath.setToolTip(Trans("tip_open_train_data_path"));
    connect(&butt_trainingdatapath, SIGNAL(clicked(bool)), this, SLOT(on_trainingdatapath()));
    this->show_trainingdatapath.setText(Trans("default_path") + config.training_data_path);


    this->label_exlzparam.setText(Trans("extral_params"));
    this->label_exlzparam.setToolTip(Trans("tip_extral_params"));
    this->edit_exlzparam.setText(config.extral_lzparam);
    connect(&edit_exlzparam, SIGNAL(editingFinished()), this, SLOT(on_exlzparam()));

    /*
     * Job Type settings.
     */
    this->view_jobtype.setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    this->view_jobtype.setSelectionMode(QAbstractItemView::SingleSelection);
    this->view_jobtype.setEditTriggers(QAbstractItemView::NoEditTriggers);
    this->model_jobtype.appendRow(new QStandardItem(Trans("type_local_production")));
    this->model_jobtype.appendRow(new QStandardItem(Trans("type_local_validation")));
    this->model_jobtype.appendRow(new QStandardItem(Trans("type_online")));
    this->model_jobtype.appendRow(new QStandardItem(Trans("type_dump_supervised")));
    this->view_jobtype.setModel(&this->model_jobtype);
    this->model_jobtype.item(0)->setToolTip(Trans("tip_job_type"));
    this->model_jobtype.item(1)->setToolTip(Trans("tip_job_type"));
    this->model_jobtype.item(2)->setToolTip(Trans("tip_job_type"));
    this->model_jobtype.item(3)->setToolTip(Trans("tip_job_type"));
    this->view_jobtype.setCurrentIndex(
                this->model_jobtype.indexFromItem(
                    this->model_jobtype.item(config.job_type)));
    connect(&this->view_jobtype, SIGNAL(clicked(QModelIndex)),
            this, SLOT(on_jobtype_clicked(QModelIndex)));

    this->butt_compnetfile.setText(Trans("open_net_weights_file"));
    this->butt_compnetfile.setToolTip(Trans("tip_open_net_weights_file"));
    connect(&butt_compnetfile, SIGNAL(clicked(bool)), this, SLOT(on_compnetfile()));
    this->show_compnetfile.setText(
                Trans("default_file") + config.net_component_filepath);
    this->show_compnetfile.setToolTip(
            Trans("default_file") + config.net_component_filepath);

    this->butt_dumpsgffile.setText(Trans("open_train_sgf_file"));
    this->butt_dumpsgffile.setToolTip(Trans("tip_open_train_sgf_file"));
    connect(&butt_dumpsgffile, SIGNAL(clicked(bool)), this, SLOT(on_dumpsgffile()));
    this->show_dumpsgffile.setText(Trans("default_file") + config.dump_sgf_file);
    this->show_dumpsgffile.setToolTip(
            Trans("default_file") + config.dump_sgf_file);

    this->butt_dumpdatafile.setText(Trans("save_net_weights_file"));
    this->butt_dumpdatafile.setToolTip(Trans("tip_save_net_weights_file"));
    connect(&butt_dumpdatafile, SIGNAL(clicked(bool)), this, SLOT(on_savedumpdatafile()));
    this->show_dumpdatafile.setText(Trans("default_file") + config.dump_data_file);
    this->show_dumpdatafile.setToolTip(
            Trans("default_file") + config.dump_data_file);

    this->butt_okays.setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
    this->butt_okays.button(QDialogButtonBox::Ok)->setText(Trans("okay"));
    this->butt_okays.button(QDialogButtonBox::Cancel)->setText(Trans("cancel"));
    connect(&butt_okays, &QDialogButtonBox::accepted, this, &GTPConfig::on_okay);
    connect(&butt_okays, &QDialogButtonBox::rejected, this, &GTPConfig::on_cancel);

    /***
     * Layout settings.
     */
    this->w = new QWidget(this);
    this->gl_window = new QGridLayout(this->w);

    this->gl_window->addWidget(&this->view_jobtype, 0, 0, 2, 1);

    this->gl_commons = new QGridLayout();
    this->set_common_elements(this->gl_commons);
    this->gl_window->addLayout(this->gl_commons, 0, 1, 1, 1);

    this->sl_jobtypes = new QStackedLayout();

    this->w_local = new QWidget(this->w);
    this->gl_local = new QGridLayout(this->w_local);
    this->sl_jobtypes->insertWidget(GTPConfigElements::JobType::LocalProduction,
                                    this->w_local);

    this->w_localmatch = new QWidget(this->w);
    this->gl_localmatch = new QGridLayout(this->w_localmatch);
    this->gl_localmatch->addWidget(&this->butt_compnetfile, 12, 0);
    this->gl_localmatch->addWidget(&this->show_compnetfile, 12, 1, 1, 2);
    this->sl_jobtypes->insertWidget(GTPConfigElements::JobType::LocalValidation,
                                    this->w_localmatch);

    this->w_online = new QWidget(this->w);
    this->gl_online = new QGridLayout(this->w_online);
    this->sl_jobtypes->insertWidget(GTPConfigElements::JobType::OnlineJob,
                                    this->w_online);

    this->w_convert = new QWidget(this->w);
    this->gl_convert = new QGridLayout(this->w_convert);
    this->gl_convert->addWidget(&this->butt_dumpsgffile, 13, 0);
    this->gl_convert->addWidget(&this->show_dumpsgffile, 13, 1, 1, 2);
    this->gl_convert->addWidget(&this->butt_dumpdatafile, 14, 0);
    this->gl_convert->addWidget(&this->show_dumpdatafile, 14, 1, 1, 2);
    this->sl_jobtypes->insertWidget(GTPConfigElements::JobType::DumpSupervised,
                                    this->w_convert);

    this->sl_jobtypes->setCurrentIndex(3);
    this->gl_window->addLayout(this->sl_jobtypes, 1, 1, 1, 1);

    this->gl_window->addWidget(&this->butt_okays, 2, 1);
    this->w->setLayout(this->gl_window);
    this->resize(this->gl_window->sizeHint());
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
    this->sl_jobtypes->setCurrentIndex(config.job_type);

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

    this->view_jobtype.setToolTip(Trans("tip_job_type"));
    this->model_jobtype.item(GTPConfigElements::JobType::LocalProduction)
            ->setText(Trans("type_local_production"));
    this->model_jobtype.item(GTPConfigElements::JobType::LocalValidation)
            ->setText(Trans("type_local_validation"));
    this->model_jobtype.item(GTPConfigElements::JobType::OnlineJob)
            ->setText(Trans("type_online"));
    this->model_jobtype.item(GTPConfigElements::JobType::DumpSupervised)
            ->setText(Trans("type_dump_supervised"));

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

    this->butt_okays.button(QDialogButtonBox::Ok)->setText(Trans("okay"));
    this->butt_okays.button(QDialogButtonBox::Cancel)->setText(Trans("cancel"));
}

void GTPConfig::set_common_elements(QGridLayout *l) {
    l->addWidget(&this->label_threads, 1, 0);
    l->addWidget(&this->butt_threads, 1, 1);
    l->addWidget(&this->label_playouts, 2, 0);
    l->addWidget(&this->butt_playouts, 2, 1);

    l->addWidget(&this->label_batchsize, 3, 0);
    l->addWidget(&this->butt_batchsize, 3, 1);
    l->addWidget(&this->label_random, 4, 0);
    l->addWidget(&this->butt_random, 4, 1);

    l->addWidget(&this->label_resignpct, 5, 0);
    l->addWidget(&this->show_resignpct, 5, 2);
    l->addWidget(&this->butt_resignpct, 5, 1);

    l->addWidget(&this->butt_enablenoise, 6, 0, 1, 3);
    l->addWidget(&this->butt_dumbpass, 7, 0, 1, 3);
    l->addWidget(&this->butt_loaddata, 8, 0, 1, 3);

    l->addWidget(&this->butt_trainingdatapath, 9, 0);
    l->addWidget(&this->show_trainingdatapath, 9, 1, 1, 2);

    l->addWidget(&this->label_exlzparam, 10, 0);
    l->addWidget(&this->edit_exlzparam, 11, 0, 1, 3);
}

void GTPConfig::on_noise() {
    if (this->butt_enablenoise.isChecked()) {
        config.enable_noise = true;
    } else {
        config.enable_noise = false;
    }
}

void GTPConfig::on_jobtype_clicked(QModelIndex index) {
    int i = index.row();
    assert(i >= GTPConfigElements::JobType::LocalProduction &&
           i <= GTPConfigElements::JobType::DumpSupervised);
    this->sl_jobtypes->setCurrentIndex(i);
    switch (i) {
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
    this->view_jobtype.setCurrentIndex(
                this->model_jobtype.indexFromItem(
                    this->model_jobtype.item(config.job_type)));
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
