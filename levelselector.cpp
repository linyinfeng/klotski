#include "levelselector.h"
#include "common.h"

#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QList>
#include <QRegExp>
#include <algorithm>
#include <QDebug>

LevelSelector::LevelSelector(QWidget *parent)
    : QWidget(parent), ui(new Ui::levelSelectorView)
{
    ui->setupUi(this);

    setWindowModality(Qt::ApplicationModal);

    QStringList filter;
    filter << "*." + kSaveSuffix;
    QDir level_dir(QCoreApplication::applicationDirPath() + kDefaultLevelDir);
    QFileInfoList file_info_list =  level_dir.entryInfoList(filter);

    QRegularExpression re(R"parttern(.*\((\d*)\).*)parttern");
    std::sort(file_info_list.begin(), file_info_list.end(), [&re](const QFileInfo &info1, const QFileInfo &info2) {
        QRegularExpressionMatch match1 = re.match(info1.baseName());
        QRegularExpressionMatch match2 = re.match(info2.baseName());
        if (match1.hasMatch() && match2.hasMatch()) {
            return match1.captured(1).toInt() < match2.captured(1).toInt();
        } else if (!match1.hasMatch())
            return false;
        else // if (!match2.hasMatch())
            return true;
    });

    for (const QFileInfo &file_info : file_info_list) {
        addListItem(file_info.baseName());
    }
    connect(ui->listWidget,&QListWidget::itemDoubleClicked, this, &LevelSelector::onItemDoubleClicked);
}

void LevelSelector::addListItem(const QString &item) {
    ui->listWidget->addItem(item);
}

void LevelSelector::onItemDoubleClicked(QListWidgetItem *item) {
    QString file_base_name = item->data(Qt::DisplayRole).toString();
    loadFile(QCoreApplication::applicationDirPath() + kDefaultLevelDir + QString("/%1.%2").arg(file_base_name).arg(kSaveSuffix));
    hide();
}
