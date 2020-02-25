/*
    This file is part of Leela GTP.
    Copyright (C) 2019 Fierralin
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

#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QString>
#include <QHash>

enum LeelaGTPLocale {
    Default = 0,
    Cn = 0,
    En = 1,
};

extern LeelaGTPLocale __leela_gtp_locale;

struct TranslateValue {
    QString value[2];

    const QString& operator[](int index) const {
        return value[index];
    }
};


static QHash<QString, TranslateValue> __trans = {
    {
        // LeelaGTP
        {"leelagtp_title", {"里拉訓練GTP程序", "Leela AutoGTP (Local Version)"}},

        {"run", {"開始", "Run"}},
        {"stop", {"中止", "Stop"}},
        {"tip_run", {"開始执行任务或训练", "Start job."}},

        {"timeout", {"訓練時間（分鍾）：", "Training Time (m):"}},
        {"tip_timeout",
         {"訓練的总時間，单位分鍾，程序将在超时时强制结束",
          "Save running games after the timeout (in minutes) is passed and then"
          " exit."}},

        {"unlimited", {"不限", "Unlimited"}},
        {"max_games", {"最多訓練局數：", "Most Traning Games:"}},
        {"tip_max_games",
         {"最多訓練局數",
          "Exit after the given number of games is completed."}},

        {"max_gpugames", {"單GPU上同時下棋局數：", "Max Games per GPU(s):"}},
        {"tip_max_gpugames",
         {"單GPU上同時下棋局數",
          "Play given number of games on one device (GPU/CPU) at the same time."}},

        // Tool bar and menu bar
        {"file", {"文件", "File"}},
        {"new_file", {"新建文件", "New file"}},
        {"open_file", {"打开文件", "Open File"}},
        {"save_file", {"保存文件", "Save File"}},
        {"exit", {"退出", "Exit"}},
        {"task", {"任务", "Task"}},
        {"new_task", {"新建任务", "New Task"}},
        {"tip_new_task", {"新建任务", "New Task"}},
        {"open_task", {"打开任务", "Open Task"}},
        {"save_task", {"保存任务", "Save Task"}},
        {"help", {"帮助", "Help"}},
        {"about", {"关于", "About"}},

        {"keep_sgf",
         {"是否保存棋譜文件",
          "Save SGF files after each self-play game."}},
        {"open_filepath", {"打開路徑...", "Open File Path..."}},
        {"tip_open_filepath", {"打開路徑...", "Open File Path..."}},

        {"net_file", {"打開自定义权重文件", "Open Weights File"}},
        {"tip_net_file", {"打開自定义权重文件", "Open self defined weights file."}},
        {"tip_translation",
         {"选择语言/Choose Translation", "选择语言/Choose Translation"}},

        // GTPConfig
        {"gtpconfig_title", {"里拉GTP详细设置",
                             "Leela GTP further configurations"}},

        {"threads", {"线程数:", "Number of threads:"}},
        {"tip_threads",
         {"使用线程数，0表示让leela-zero自动选择",
          "Number of threads to use. Select 0 to let leela-zero pick a "
          "reasonable default."}},

        {"playouts", {"限制手数:", "Limit playouts:"}},
        {"tip_playouts",
         {"限制落子手数削弱引擎，需要--noponder参数配合",
          "Weaken engine by limiting the number of playouts. Requires "
          "--noponder."}},

        {"batchsize", {"最大批量:", "Max batch size:"}},
        {"tip_batchsize",
         {"最大批量大小，0表示让leela-zero自动选择",
          "Max batch size.  Select 0 to let leela-zero pick a reasonable default."}},

        {"random", {"前几步更随机：", "Randomly first steps:"}},
        {"tip_random", {"前几步更随机:", " Play more randomly the first x moves."}},
        {"loop_visits", {"设置循环遍历步数:", "Limit loop visits:"}},
        {"tip_loop_visits",
         {"设置循环遍历步数",
          "Weaken engine by limiting the number of visits."}},

        {"resign", {"投子认输胜率:", "Resign winrate:"}},
        {"tip_resign", {"投子认输胜率",
                        "Resign if winrate is lover than given rate."}},

        {"noise", {"是否允许策略网络随机", "Enable policy network randomization"}},
        {"dumbpass", {"是否使用启发式智能弃权", "Enable dumbpass resign"}},
        {"load_data", {"是否加载保存数据文件", "Load training data"}},
        {"open_train_data_path", {"打开训练文件目录", "Training Data"}},
        {"tip_open_train_data_path",
         {"打开训练文件目录", "Open Training Data Path"}},

        {"extral_params", {"设置附加leelaz选项:", "Set extra leelaz parameters:"}},
        {"tip_extral_params", {"设置附加的leelaz选项", "Set extra leelaz parameters."}},

        {"job_type", {"选择任务类型:", "Choose job type:"}},
        {"tip_job_type",
         {"选择任务类型:\n  线下自战任务： 线下自战\n  线下对抗任务： 线下两张不同权重对战"
          "\n  线上任务： 接受线上任务（目前不支持）\n  训练转换任务： 将棋谱转换成训练数据",
          "Choose job type:\n  Local  Production job\n  Local Validation job"
          "\n  Online job (not support)"
          "\n  Dump Supervised: dump SGF to training data"}},
        {"type_local_production", {"线下自战", "Local Production"}},
        {"type_local_validation", {"线下对抗", "Local Validation"}},
        {"type_online", {"线上训练", "Online Job"}},
        {"type_dump_supervised", {"训练转换", "Dump Supervised"}},

        {"open_net_weights_file", {"打开对抗权重文件", "Component Weights"}},
        {"tip_open_net_weights_file",
         {"打开对抗权重文件", "Open Component Training Weights File"}},
        {"open_train_sgf_file", {"打开训练棋谱文件", "Dumped SGF"}},
        {"tip_open_train_sgf_file", {"打开训练棋谱文件", "Open Dumped SGF File"}},
        {"save_net_weights_file", {"保存训练数据文件", "Dump Traning"}},
        {"tip_save_net_weights_file", {"保存训练数据文件", "Save Dumped Traning Data"}},

        {"okay", {"确定", "Okay"}},
        {"cancel", {"取消", "Cancel"}},

        // Message boxes
        {"msg_sgf_save_path", {"棋谱保存路徑", "Save SGF Files"}},
        {"msg_net_path", {"权重文件", "Open Net File"}},
        {"msg_sgf_path", {"棋谱文件", "Open SGF File"}},
        {"msg_create_path_fail", {"目錄創建失败", "Failed to create target Path"}},
        {"msg_sgf_path_fail_details",
         {"创建棋譜保存目錄失敗！", "Failed to create path for saving SGF files"}},
        {"msg_train_path_fail_details",
         {"创建訓練數據保存目錄失敗！", "Failed to create path for saving training data"}},
        {"msg_err_stop",
         {"错误，中止训练", "Error, Stop Running"}},
        {"msg_err_stop_details",
         {"没有找到leelaz执行文件或权重文件\n请您确认设置是否正确",
          "Cannot find leelaz execution file or net weights file.\n"
          "Please check your configurations."}},

        // Common
        {"default_path", {"默認路徑： ", "Default Path: "}},
        {"default_file", {"默認文件： ", "Default File: "}},
        {"save_to", {"另存为： ", "Save To: "}},
        {"not_defined", {"未定义", "Not Defined"}},
        {"msg_about", {"关于LeelaGTP", "About LeelaGTP"}},
        {"msg_about_details",
         {"LeelaGTP是一个里拉训练程序",
          "LeelaGTP used for leela-zero training"}},
    }
};


QString Trans(QString key);

#endif // TRANSLATION_H
