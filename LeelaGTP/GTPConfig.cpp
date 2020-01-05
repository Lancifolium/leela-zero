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
#include <QListView>
#include <QTime>
#include <QtGlobal>
#include <QTextStream>


GTPConfigElements::GTPConfigElements() :
        job_type(JobType::LocalProduction),
        keepSgf(true),
        sgf_path("./sgfs/"),
        net_filepath("./networks/weights.txt"),
        net_file("weights.txt"),
        net_component_filepath("./networks/component_weights.txt"),
        net_component_file("component_weights.txt"),
        dump_sgf_file("./sgfs/tmp.sgf"),
        dump_data_file("./train.txt"),
        extral_lzparam(""),
        training_data_path("./data/"),
        load_training_data(true),
        run_timeout(0),
        run_maxgames(0),
        random_num(0),
        enable_noise(0),
        loop_visits(3300),
        resignation_percent(1),
        heuristic(false),
        gpu_games(1),
        leelaz_path("./leelaz") {}

void GTPConfigElements::copyto(GTPConfigElements *c) {
    c->job_type = this->job_type;

    //c->keepSgf = this->keepSgf;
    //c->sgf_path = this->sgf_path;

    //c->net_path = this->net_path;
    //c->net_file = this->net_file;
    c->net_component_filepath = this->net_component_filepath;
    c->net_component_file = this->net_component_file;

    c->dump_sgf_file = this->dump_sgf_file;
    c->dump_data_file = this->dump_data_file;

    c->extral_lzparam = this->extral_lzparam;

    c->training_data_path = this->training_data_path;
    c->load_training_data = this->load_training_data;
    //c->load_kept_sgfs = this->load_kept_sgfs;

    //c->run_timeout = this->run_timeout;
    //c->run_maxgames = this->run_maxgames;

    c->random_num = this->random_num;

    c->enable_noise = this->enable_noise;

    c->loop_visits = this->loop_visits;
    c->resignation_percent = this->resignation_percent;

    c->heuristic = this->heuristic;
    //c->gpu_games = this->gpu_games;
    //c->leelaz_path = this->leelaz_path;
}


GTPConfig::GTPConfig(QWidget *parent, GTPConfigElements *m_config) :
        QDialog (parent),
        main_config(m_config) {
    assert(main_config != nullptr);

    this->setWindowTitle("Leela GTP further configurations");
    this->setFixedSize(600, 600);


    this->label_random = new QLabel("前几步更随机：", this);
    this->label_random->setGeometry(60, 66, 150, 24);
    this->butt_random = new QSpinBox(this);
    this->butt_random->setGeometry(60, 90, 84, 24);
    this->butt_random->setRange(0, 800);
    this->butt_random->setValue(config.random_num);
    this->butt_random->setSingleStep(10);
    connect(butt_random, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.random_num = val; });

    this->label_loopvisits = new QLabel("设置循环遍历步数", this);
    this->label_loopvisits->setGeometry(240, 66, 90, 24);
    this->butt_loopvisits = new QSpinBox(this);
    this->butt_loopvisits->setGeometry(240, 90, 84, 24);
    this->butt_loopvisits->setRange(100, 100000);
    this->butt_loopvisits->setValue(config.loop_visits);
    this->butt_loopvisits->setSingleStep(100);
    connect(butt_loopvisits, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.loop_visits = val; });

    this->label_resignpct = new QLabel("投子认输胜率", this);
    this->label_resignpct->setGeometry(420, 66, 90, 24);
    this->show_resignpct = new QLabel("%", this);
    this->show_resignpct->setGeometry(460, 90, 24, 24);
    this->butt_resignpct = new QSpinBox(this);
    this->butt_resignpct->setGeometry(420, 90, 36, 24);
    this->butt_resignpct->setRange(1, 30);
    this->butt_resignpct->setSingleStep(1);
    connect(butt_resignpct, QOverload<int>::of(&QSpinBox::valueChanged),
            [=](int val) { config.resignation_percent = val; });


    this->butt_enablenoise = new QCheckBox("是否允许策略网络随机", this);
    this->butt_enablenoise->setGeometry(60, 120, 150, 24);
    connect(butt_enablenoise, SIGNAL(toggled(bool)), this, SLOT(on_noise()));

    this->butt_heuristic = new QCheckBox("是否使用启发式智能弃权", this);
    this->butt_heuristic->setGeometry(60, 150, 150, 24);
    connect(butt_heuristic, SIGNAL(toggled(bool)), this, SLOT(on_heuristic()));

    this->butt_loaddata = new QCheckBox("是否加载保存数据文件", this);
    this->butt_loaddata->setGeometry(60, 180, 360, 24);
    connect(butt_loaddata, SIGNAL(toggled(bool)), this, SLOT(on_loadtrainingdata()));

    this->label_exlzparam = new QLabel("设置附加 leelaz 选项:", this);
    this->label_exlzparam->setGeometry(60, 216, 150, 24);
    this->edit_exlzparam = new QLineEdit(config.extral_lzparam, this);
    this->edit_exlzparam->setGeometry(60, 240, 400, 24);
    connect(edit_exlzparam, SIGNAL(editingFinished()), this, SLOT(on_exlzparam()));

    /*
     * Following is Job Type settings.
     */

    this->label_jobtype = new QLabel("选择任务类型:", this);
    this->label_jobtype->setGeometry(60, 276, 150, 24);
    this->butt_jobtype = new QComboBox(this);
    this->butt_jobtype->setGeometry(60, 300, 120, 24);
    this->butt_jobtype->addItem("self", GTPConfigElements::JobType::LocalProduction);
    this->butt_jobtype->addItem("match", GTPConfigElements::JobType::LocalValidation);
    this->butt_jobtype->addItem("train", GTPConfigElements::JobType::OnlineJob);
    this->butt_jobtype->addItem("sgf-training", GTPConfigElements::JobType::DumpSupervised);
    connect(butt_jobtype, SIGNAL(activated(int)), this, SLOT(on_jobtype()));

    this->butt_trainingdatapath = new QPushButton("打开训练文件目录", this);
    this->butt_trainingdatapath->setGeometry(60, 330, 100, 24);
    connect(butt_trainingdatapath, SIGNAL(clicked(bool)), this, SLOT(on_trainingdatapath()));
    this->show_trainingdatapath =
            new QLabel("默認目录：" + config.training_data_path, this);
    this->show_trainingdatapath->setGeometry(176, 330, 360, 24);


    this->butt_compnetfile = new QPushButton("打开对抗权重文件", this);
    this->butt_compnetfile->setGeometry(60, 360, 100, 24);
    connect(butt_compnetfile, SIGNAL(clicked(bool)), this, SLOT(on_compnetfile()));
    this->show_compnetfile =
        new QLabel("默認文件：" + config.net_component_filepath, this);
    this->show_compnetfile->setGeometry(176, 360, 400, 24);

    this->butt_dumpsgffile = new QPushButton("打开训练棋谱文件", this);
    this->butt_dumpsgffile->setGeometry(60, 390, 100, 24);
    connect(butt_dumpsgffile, SIGNAL(clicked(bool)), this, SLOT(on_dumpsgffile()));
    this->show_dumpsgffile =
        new QLabel("默認文件：" + config.dump_sgf_file, this);
    this->show_dumpsgffile->setGeometry(176, 390, 400, 24);

    this->butt_dumpdatafile = new QPushButton("保存训练数据文件", this);
    this->butt_dumpdatafile->setGeometry(60, 420, 100, 24);
    connect(butt_dumpdatafile, SIGNAL(clicked(bool)), this, SLOT(on_savedumpdatafile()));
    this->show_dumpdatafile =
        new QLabel("默認文件：" + config.dump_data_file, this);
    this->show_dumpdatafile->setGeometry(176, 420, 400, 24);


    this->butt_okay = new QPushButton("确定", this);
    this->butt_okay->setGeometry(60, 510, 84, 24);
    connect(butt_okay, SIGNAL(clicked(bool)), this, SLOT(on_okay()));
    this->butt_cancel = new QPushButton("取消", this);
    this->butt_cancel->setGeometry(150, 510, 84, 24);
    connect(butt_cancel, SIGNAL(clicked(bool)), this, SLOT(on_cancel()));
}

GTPConfig::~GTPConfig() {
    delete label_random;
    delete butt_random;
    delete butt_enablenoise;
    delete label_loopvisits;
    delete butt_loopvisits;
    delete label_resignpct;
    delete show_resignpct;
    delete butt_resignpct;
    delete label_exlzparam;
    delete edit_exlzparam;
    delete label_jobtype;
    delete butt_jobtype;
    delete butt_compnetfile;
    delete show_compnetfile;
    delete butt_dumpsgffile;
    delete show_dumpsgffile;
    delete butt_dumpdatafile;
    delete show_dumpdatafile;
    delete butt_trainingdatapath;
    delete show_trainingdatapath;
    delete butt_heuristic;
    delete butt_loaddata;
    delete butt_okay;
    delete butt_cancel;
}

void GTPConfig::drawwindow() {
    this->butt_random->setValue(config.random_num);
    this->butt_loopvisits->setValue(config.loop_visits);
    this->butt_resignpct->setValue(config.resignation_percent);
    this->butt_enablenoise->setChecked(config.enable_noise);
    this->butt_heuristic->setChecked(config.heuristic);
    this->butt_loaddata->setChecked(config.load_training_data);
    this->edit_exlzparam->setText(config.extral_lzparam);
    this->butt_jobtype->setCurrentIndex(config.job_type);
    on_jobtype();
    this->show_trainingdatapath->setText(config.training_data_path);
    this->show_compnetfile->setText(config.net_component_filepath);
    this->show_dumpsgffile->setText(config.dump_sgf_file);
    this->show_dumpdatafile->setText(config.dump_data_file);
}

void GTPConfig::copyfrom(GTPConfigElements *m_config) {
    if (m_config != nullptr)
        m_config->copyto(&config);
}

void GTPConfig::on_noise() {
    if (this->butt_enablenoise->isChecked()) {
        config.enable_noise = true;
    } else {
        config.enable_noise = false;
    }
}

void GTPConfig::on_jobtype() {
    switch (this->butt_jobtype->currentIndex()) {
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
        this->butt_compnetfile->setEnabled(true);
        this->show_compnetfile->setEnabled(true);
    } else {
        this->butt_compnetfile->setEnabled(false);
        this->show_compnetfile->setEnabled(false);
    }
    if (config.job_type == GTPConfigElements::JobType::DumpSupervised) {
        this->butt_dumpsgffile->setEnabled(true);
        this->show_dumpsgffile->setEnabled(true);
        this->butt_dumpdatafile->setEnabled(true);
        this->show_dumpdatafile->setEnabled(true);
    } else {
        this->butt_dumpsgffile->setEnabled(false);
        this->show_dumpsgffile->setEnabled(false);
        this->butt_dumpdatafile->setEnabled(false);
        this->show_dumpdatafile->setEnabled(false);
    }
}

void GTPConfig::on_compnetfile() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("权重文件"));
    if (!filepath.isEmpty()) {
        config.net_component_filepath = filepath;
        QFileInfo file(config.net_component_filepath);
        config.net_component_file = file.fileName();
        this->show_compnetfile->setText(config.net_component_filepath);
    }
}

void GTPConfig::on_dumpsgffile() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("棋谱文件"));
    if (!filepath.isEmpty()) {
        config.dump_sgf_file = filepath;
        this->show_dumpsgffile->setText(config.dump_sgf_file);
    }
}

void GTPConfig::on_savedumpdatafile() {
    QString filepath = QFileDialog::getOpenFileName(this, tr("保存训练数据文件"));
    if (!filepath.isEmpty()) {
        config.dump_data_file = filepath;
        this->show_dumpdatafile->setText(config.dump_data_file);
    }
}

void GTPConfig::on_trainingdatapath() {
    QString filepath = QFileDialog::getExistingDirectory(this, tr("训练文件所在目录"), ".");
    if (!filepath.isEmpty()) {
        config.training_data_path = filepath;
        show_trainingdatapath->setText(config.training_data_path);
    }
}

void GTPConfig::on_loadtrainingdata() {
    if (butt_loaddata->isChecked())
        config.load_training_data = true;
    else
        config.load_training_data = false;
}


void GTPConfig::on_heuristic() {
    if (this->butt_heuristic->isChecked()) {
        config.heuristic = true;
    } else {
        config.heuristic = false;
    }
}

void GTPConfig::on_exlzparam() {
    config.extral_lzparam = edit_exlzparam->text();
}

void GTPConfig::on_okay() {
    config.copyto(main_config);
    this->close();
}

void GTPConfig::on_cancel() {
    this->close();
}
