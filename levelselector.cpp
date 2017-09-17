#include "levelselector.h"
#include "common.h"

#include <QDir>
#include <QString>
#include <QFileInfo>
#include <QList>

const QString kDefaultLevelDir = ":/resources/levels";

LevelSelector::LevelSelector(QWidget *parent)
    : QWidget(parent), ui(new Ui::levelSelectorView)
{
    ui->setupUi(this);

    QStringList filter;
    filter << "*." + kSaveSuffix;
    QDir level_dir(kDefaultLevelDir);
    QFileInfoList file_info_list =  level_dir.entryInfoList(filter);
    for (const QFileInfo &file_info : file_info_list) {
        addListItem(file_info.baseName());
    }
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)), this, SLOT(onItemDoubleClicked(QListWidgetItem*)));
}

void LevelSelector::addListItem(const QString &item) {
    ui->listWidget->addItem(item);
}

void LevelSelector::onItemDoubleClicked(QListWidgetItem *item) {
    QString file_base_name = item->data(Qt::DisplayRole).toString();
    loadFile(QString(":/resources/levels/%1.%2").arg(file_base_name).arg(kSaveSuffix));
    hide();
}
